/*
 * main.cpp
 *
 *  Created on: May 15, 2018
 *      Author: robin
 */

#define USBCON
#define SERIAL_BUFFER_SIZE 140

#include <Arduino.h>
#include <EEPROM.h>
#include <USBAPI.h>
#include "Sprite.h"
#include "XYSprite.h"

#include <SPI.h>

#include <Arduboy2.h>

#include <avr/pgmspace.h>

#include "ShmupSprites.h"
#include "MaskedXYSprite.h"
#include "ShmupSfx.h"

constexpr uint8_t kNumPlayerBullets = 4;
constexpr uint8_t kNumPlayerWaves = 4;
constexpr uint8_t kPlayerWaveDuration = 64;
constexpr uint8_t kEnemyWaveDuration = 128;
constexpr uint8_t kNumEnemies = 15;
constexpr uint8_t kNumBulletsPerEnemy = 2;
constexpr uint8_t kNumEnemyBullets = kNumEnemies * kNumBulletsPerEnemy;
constexpr int kNumNumberSprites = (128 / 4) - 1;
constexpr int kEnemyExplosionFrames = 12;
constexpr int kPlayerImpactFrames = 16;

constexpr char kEepromMagic[] = "ArduSHMUP";
constexpr int kEepromMagicOffset = EEPROM_STORAGE_SPACE_START;
constexpr int kEepromScoreOffset = kEepromMagicOffset + sizeof(kEepromMagic);
constexpr int kEepromFramerateOffset = kEepromScoreOffset + 4;
constexpr int kEepromInvertOffset = kEepromFramerateOffset + 1;
constexpr int kEepromPauseOffset = kEepromInvertOffset + 1;
constexpr int kEepromHelpOffset = kEepromPauseOffset + 1;
constexpr int kEepromTimeOffset = kEepromHelpOffset + 1;

constexpr uint8_t kXOffset = XYSprite::kXOffset;
constexpr uint8_t kYOffset = XYSprite::kYOffset;

struct State {
	MaskedXYSprite player_;
	MaskedXYSprite player_bullets_[kNumPlayerBullets];
	MaskedXYSprite player_waves_[kNumPlayerWaves];
	uint8_t player_wave_ends_[kNumPlayerWaves];
	MaskedXYSprite player_beam_;

	MaskedXYSprite enemy_[kNumEnemies];
	uint8_t enemy_exploding_[kNumEnemies];
	uint8_t enemy_frame_[kNumEnemies];
	int8_t enemy_ydelta_[kNumEnemies];
	uint8_t enemy_xdelta_[kNumEnemies];
	MaskedXYSprite enemy_bullets_[kNumEnemyBullets];
	int8_t enemy_bullet_ydelta_[kNumEnemyBullets];
	MaskedXYSprite enemy_waves_[kNumEnemies];
	int8_t enemy_wave_ends[kNumEnemies];

	MaskedXYSprite number_sprites_[kNumNumberSprites];

	uint8_t frame_ = 0;
	uint32_t frame_ts_ = 0;

	uint32_t millis_start_ = 0;

	uint8_t wave_countdown_ = 0;

	int16_t health_ = 0;
	uint16_t score_ = 0;
	int8_t player_impacting_ = 0;
};

void print(const char *s, int x, int y, uint8_t *buf) {
	for(char c = *s; c; c = *++s) {
		XYSprite sprite(x + kXOffset, y + kYOffset, Sprite::kOr, Sprite(c));
		for (uint8_t n = 0; n < 8; ++n) {
			sprite.render(n, buf + 128 * n);
		}
		x += 6;
	}
}

uint8_t base_framerate_;
bool write_display_ = false;
bool inverted_ = false;
bool play_inverted_ = false;
bool enable_pause_ = false;
bool enable_help_ = false;
bool has_magic_ = false;

void invert(bool b) {
	inverted_ = b;
}

uint8_t buttonWait() {
	uint8_t b;
	while (Arduboy2Core::buttonsState()) Arduboy2Core::idle();
	while (!(b = Arduboy2Core::buttonsState())) Arduboy2Core::idle();
	while (Arduboy2Core::buttonsState()) Arduboy2Core::idle();
	return b;
}

inline void render(const MaskedXYSprite* s, uint8_t len, uint8_t page_num, uint8_t *page) {
	while(len--) {
		(*s++).render(page_num, page);
	}
}

void display(const State& state) {
	uint8_t page[128];
	for (uint8_t n = 0; n < 8; ++n) {
		memset(page, inverted_ ? 0 : 255, 128);

		render(state.number_sprites_, kNumNumberSprites, n, page);
		render(state.player_bullets_, kNumPlayerBullets, n, page);
		render(state.enemy_bullets_, kNumEnemyBullets, n, page);
		render(state.player_waves_, kNumPlayerWaves, n, page);
		render(state.enemy_waves_, kNumEnemies, n, page);
		render(state.enemy_, kNumEnemies, n, page);
		render(&state.player_beam_, 1, n, page);
		render(&state.player_, 1, n, page);

		if (write_display_) {
			Serial.write(page, 128);
		}
		SPI.transfer(page, 128);
	}
}

void reset(State& state) {
	state.player_ = MaskedXYSprite(0 + kXOffset, 28 + kYOffset, ShmupSprites::player, ShmupSprites::playerMask, true);

	for (uint8_t i = 0; i < kNumPlayerBullets; ++i)
		state.player_bullets_[i] = MaskedXYSprite(ShmupSprites::bullet, ShmupSprites::bulletMask);
	for (uint8_t i = 0; i < kNumPlayerWaves; ++i)
		state.player_waves_[i] = MaskedXYSprite(ShmupSprites::wave, ShmupSprites::waveMask);
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		state.enemy_[i] = MaskedXYSprite(ShmupSprites::enemy, ShmupSprites::enemyMask);
		state.enemy_exploding_[i] = 0;
		state.enemy_waves_[i] = MaskedXYSprite(ShmupSprites::enemyWave, ShmupSprites::enemyWaveMask);
	}
	for (uint8_t i = 0; i < kNumEnemyBullets; ++i)
		state.enemy_bullets_[i] = MaskedXYSprite(ShmupSprites::enemyBullet, ShmupSprites::enemyBulletMask);

	state.player_beam_ = {ShmupSprites::beam, ShmupSprites::beamMask};

	for (uint8_t i = 0; i < kNumNumberSprites; ++i) {
		state.number_sprites_[i].setX(4 * i + 2 + kXOffset);
		state.number_sprites_[i].setY(55 + kYOffset);
	}

	state.health_ = 1500;
	state.score_ = 0;
	state.player_impacting_ = 0;

	state.millis_start_ = millis();
}

void setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10 // RGB, all the pretty colors
	// inversion is necessary because these are common annode LEDs
	analogWrite(RED_LED, 255 - red);
	analogWrite(GREEN_LED, 255 - green);
	analogWrite(BLUE_LED, 255 - blue);
#elif defined(AB_DEVKIT)
	// only blue on devkit
	digitalWrite(BLUE_LED, ~blue);
#endif
}

uint16_t getHighTime() {
	uint32_t time = 0;
	time |= EEPROM.read(kEepromTimeOffset + 0); time <<= 8;
	time |= EEPROM.read(kEepromTimeOffset + 1); time <<= 8;
	time |= EEPROM.read(kEepromTimeOffset + 2); time <<= 8;
	time |= EEPROM.read(kEepromTimeOffset + 3);
	return time;
}

void setHighTime(uint16_t time) {
	EEPROM.write(kEepromTimeOffset + 0, time >> 24);
	EEPROM.write(kEepromTimeOffset + 1, time >> 16);
	EEPROM.write(kEepromTimeOffset + 2, time >> 8);
	EEPROM.write(kEepromTimeOffset + 3, time);
}

uint16_t getHighScore() {
	uint32_t score = 0;
	score |= EEPROM.read(kEepromScoreOffset + 0); score <<= 8;
	score |= EEPROM.read(kEepromScoreOffset + 1); score <<= 8;
	score |= EEPROM.read(kEepromScoreOffset + 2); score <<= 8;
	score |= EEPROM.read(kEepromScoreOffset + 3);
	return score;
}

void setHighScore(uint16_t score) {
	EEPROM.write(kEepromScoreOffset + 0, score >> 24);
	EEPROM.write(kEepromScoreOffset + 1, score >> 16);
	EEPROM.write(kEepromScoreOffset + 2, score >> 8);
	EEPROM.write(kEepromScoreOffset + 3, score);
}


void setup(State& state) {
	reset(state);
	invert(false);
}

bool loop(State& state) {
	ShmupSfx::tick();

	++state.frame_;

	MaskedXYSprite& player = state.player_;
	uint8_t b = Arduboy2Core::buttonsState();

	// Pause check
	if (enable_pause_ && (b & (A_BUTTON | B_BUTTON)) == (A_BUTTON | B_BUTTON)) {
		MaskedXYSprite pause(ShmupSprites::pause, ShmupSprites::none);
		pause.setX(54 + kXOffset);
		pause.setY(28 + kYOffset);
		pause.setActive(true);
		uint8_t page[128];
		invert(true);
		for (uint8_t n = 0; n < 8; ++n) {
			memset(page, inverted_ ? 0 : 255, 128);
			pause.render(n, page);
			SPI.transfer(page, 128);
		}
		ShmupSfx::reset();
		setRGBled(0,0,0);
		state.millis_start_ -= millis();
		while (Arduboy2Core::buttonsState()) Arduboy2Core::idle();
		while (Arduboy2Core::buttonsState() != (A_BUTTON | B_BUTTON)) Arduboy2Core::idle();
		while (Arduboy2Core::buttonsState()) Arduboy2Core::idle();
		ShmupSfx::beginGame();
		state.millis_start_ += millis();
		return true;
	}

	// Player movement
	if ((b & UP_BUTTON) && player.y() > 0 + kYOffset)
		player.setY(player.y() - 1);
	if ((b & DOWN_BUTTON) && player.y() < 56 + kYOffset)
		player.setY(player.y() + 1);
	if ((b & LEFT_BUTTON) && player.x() > 0 + kXOffset)
		player.setX(player.x() - 1);
	if ((b & RIGHT_BUTTON) && player.x() < 128 - 8 - 1 + kXOffset)
		player.setX(player.x() + 1);

	// Bullet movement
	for (uint8_t i = 0; i < kNumPlayerBullets; ++i) {
		MaskedXYSprite& bullet = state.player_bullets_[i];
		if (!bullet.active()) continue;
		bullet.setX(bullet.x() + 2);
		if (bullet.x() >= 128 + kXOffset)
			bullet.setActive(false);
	}
	for (uint8_t i = 0; i < kNumEnemyBullets; ++i) {
		MaskedXYSprite& bullet = state.enemy_bullets_[i];
		if (!bullet.active()) continue;
		uint8_t enemy = i / kNumBulletsPerEnemy;
		bullet.setX(bullet.x() - (1 + state.enemy_xdelta_[enemy]));
		if (bullet.x() < 0 + kXOffset)
			bullet.setActive(false);
		if (state.enemy_bullet_ydelta_[i] < 0) {
			++state.enemy_bullet_ydelta_[i];
			bullet.setY(bullet.y() - 1);
		} else if (state.enemy_bullet_ydelta_[i] > 0) {
			--state.enemy_bullet_ydelta_[i];
			bullet.setY(bullet.y() + 1);
		}
		if (bullet.y() < 0 + kYOffset || bullet.y() > 64 + kYOffset)
			bullet.setActive(false);
	}

	// Wave movement
	for (uint8_t i = 0; i < kNumPlayerWaves; ++i) {
		MaskedXYSprite& wave = state.player_waves_[i];
		if (!wave.active()) continue;
		wave.setX(wave.x() + 3);
		if (wave.x() >= 128 + kXOffset || wave.x() >= state.player_wave_ends_[i])
			wave.setActive(false);
	}
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		MaskedXYSprite& wave = state.enemy_waves_[i];
		if (!wave.active()) continue;
		wave.setX(wave.x() - 1);
		if (wave.x() < 0 + kXOffset || wave.x() <= state.enemy_wave_ends[i])
			wave.setActive(false);
	}

	// Enemy movement
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		MaskedXYSprite& enemy = state.enemy_[i];
		if (!enemy.active()) continue;
		if ((state.enemy_frame_[i] + state.frame_) % 0x3) continue;
		enemy.setX(enemy.x() - state.enemy_xdelta_[i]);
		enemy.setY(enemy.y() + state.enemy_ydelta_[i]);
		if (enemy.y() < 0 + kYOffset || enemy.y() > 55 + kYOffset) {
			state.enemy_ydelta_[i] = -state.enemy_ydelta_[i];
			enemy.setY(enemy.y() + state.enemy_ydelta_[i]);
		}
		if (enemy.x() < 0 + kXOffset)
			enemy.setActive(false);
	}

	// Bullet impacts
	for (uint8_t i = 0; i < kNumPlayerBullets; ++i) {
		MaskedXYSprite& bullet = state.player_bullets_[i];
		if (!bullet.active()) continue;
		bool impact = false;
		for (uint8_t j = 0; j < kNumEnemyBullets; ++j) {
			MaskedXYSprite& enemy_bullet = state.enemy_bullets_[j];
			if (!enemy_bullet.active()) continue;
			if (bullet.intersects(enemy_bullet)) {
				enemy_bullet.setActive(false);
				impact = true;
				state.health_ += 1;
				state.score_ += 1;
			}
		}
		for (uint8_t j = 0; j < kNumEnemies; ++j) {
			MaskedXYSprite& enemy = state.enemy_[j];
			if (!enemy.active()) continue;
			if (bullet.intersects(enemy)) {
				enemy.setActive(false);
				state.enemy_exploding_[j] = kEnemyExplosionFrames;
				impact = true;
				state.health_ += 10;
				state.score_ += 10;
				ShmupSfx::enemyImpact();
			}
		}
		if (impact) bullet.setActive(false);
	}

	// Wave impacts
	for (uint8_t i = 0; i < kNumPlayerWaves; ++i) {
		MaskedXYSprite& wave = state.player_waves_[i];
		if (!wave.active()) continue;
		for (uint8_t j = 0; j < kNumEnemyBullets; ++j) {
			MaskedXYSprite& enemy_bullet = state.enemy_bullets_[j];
			if (!enemy_bullet.active()) continue;
			if (wave.intersects(enemy_bullet)) {
				enemy_bullet.setActive(false);
				state.health_ += 1;
				state.score_ += 1;
			}
		}
		for (uint8_t j = 0; j < kNumEnemies; ++j) {
			MaskedXYSprite& enemy = state.enemy_[j];
			if (!enemy.active()) continue;
			if (wave.intersects(enemy)) {
				enemy.setActive(false);
				state.enemy_exploding_[j] = kEnemyExplosionFrames;
				state.health_ += 10;
				state.score_ += 10;
				ShmupSfx::enemyImpact();
			}
		}
	}

	// Beam impacts
	if (state.player_beam_.active()) {
		MaskedXYSprite& beam = state.player_beam_;
		for (uint8_t i = 0; i < kNumPlayerBullets; ++i) {
			MaskedXYSprite& bullet = state.player_bullets_[i];
			if (!bullet.active()) continue;
			if (beam.intersects(bullet)) {
				bullet.setActive(false);
			}
		}
		for (uint8_t i = 0; i < kNumEnemyBullets; ++i) {
			MaskedXYSprite& bullet = state.enemy_bullets_[i];
			if (!bullet.active()) continue;
			if (beam.intersects(bullet)) {
				bullet.setActive(false);
				state.health_ += 1;
				state.score_ += 1;
			}
		}
		for (uint8_t i = 0; i < kNumEnemies; ++i) {
			MaskedXYSprite& wave = state.enemy_waves_[i];
			if (!wave.active()) continue;
			if (beam.intersects(wave)) {
				wave.setActive(false);
				state.health_ += 20;
				state.score_ += 20;
			}
		}
		for (uint8_t i = 0; i < kNumEnemies; ++i) {
			MaskedXYSprite& enemy = state.enemy_[i];
			if (!enemy.active()) continue;
			if (beam.intersects(enemy)) {
				enemy.setActive(false);
				state.enemy_exploding_[i] = kEnemyExplosionFrames;
				state.health_ += 10;
				state.score_ += 10;
				ShmupSfx::enemyImpact();
			}
		}
	}

	// Enemy bullet impacts
	for (uint8_t i = 0; i < kNumEnemyBullets; ++i) {
		MaskedXYSprite& bullet = state.enemy_bullets_[i];
		if (!bullet.active()) continue;
		if (bullet.intersects(player)) {
			if (!state.player_impacting_)
				state.health_ -= 500;
			bullet.setActive(false);
			state.player_impacting_ = kPlayerImpactFrames;
			ShmupSfx::playerImpact();
		}
	}

	// Enemy wave impacts
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		MaskedXYSprite& wave = state.enemy_waves_[i];
		if (!wave.active()) continue;
		for (uint8_t j = 0; j < kNumPlayerBullets; ++j) {
			MaskedXYSprite& bullet = state.player_bullets_[j];
			if (!bullet.active()) continue;
			if (wave.intersects(bullet)) {
				bullet.setActive(false);
			}
		}
		if (wave.intersects(player)) {
			if (!state.player_impacting_)
				state.health_ -= 500;
			state.player_impacting_ = kPlayerImpactFrames;
			ShmupSfx::playerImpact();
			wave.setActive(false);
		}
	}
	// Enemy impacts
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		MaskedXYSprite& enemy = state.enemy_[i];
		if (!enemy.active()) continue;
		if (enemy.intersects(player)) {
			if (!state.player_impacting_)
				state.health_ -= 500;
			enemy.setActive(false);
			state.player_impacting_ = kPlayerImpactFrames;
			ShmupSfx::playerImpact();
		}
	}

	// Player bullet firing
	if ((state.frame_ % 15) == 0) {
		for (uint8_t i = 0; i < kNumPlayerBullets; ++i) {
			MaskedXYSprite& bullet = state.player_bullets_[i];
			if (bullet.active()) continue;
			bullet.setX(player.x());
			bullet.setY(player.y());
			bullet.setActive(true);
			ShmupSfx::bulletFired();
			break;
		}
	}

	// Player wave firing
	if (state.wave_countdown_ > 0)
		--state.wave_countdown_;
	if (!(b & A_BUTTON)) state.wave_countdown_ = 0;
	if ((b & A_BUTTON) && !state.wave_countdown_) {
		for (uint8_t i = 0; i < kNumPlayerWaves; ++i) {
			MaskedXYSprite& wave = state.player_waves_[i];
			if (wave.active()) continue;
			wave.setX(player.x());
			wave.setY(player.y() - 4);
			wave.setActive(true);
			state.player_wave_ends_[i] = player.x() + kPlayerWaveDuration;
			state.wave_countdown_ = 24;
			ShmupSfx::waveFired();
			break;
		}
	}

	// Player beam firing
	if ((b & B_BUTTON) && state.health_ > 5) {
		state.player_beam_.setX(player.x() + 8);
		state.player_beam_.setY(player.y() - 4);
		state.player_beam_.setActive(true);
		state.health_ -= 5;
		ShmupSfx::beamFired();
	} else {
		state.player_beam_.setActive(false);
	}

	// Enemy spawning
	if ((rand() % 10) == 0) {
		for (uint8_t i = 0; i < kNumEnemies && i < 3 + state.score_ / 400; ++i) {
			MaskedXYSprite& enemy = state.enemy_[i];
			if (enemy.active()) continue;
			if (state.enemy_exploding_[i]) continue;
			enemy.setX(119 + kXOffset);
			enemy.setY(rand() % 56 + kYOffset);
			enemy.setActive(true);
			state.enemy_frame_[i] = rand() % 0x3F;
			state.enemy_ydelta_[i] = (rand() % 5) - 2;
			state.enemy_xdelta_[i] = (rand() % 4) / 2 + 1;
			break;
		}
	}

	// Enemy bullet/wave firing
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		MaskedXYSprite& enemy = state.enemy_[i];
		if (!enemy.active()) continue;
		if ((state.frame_ + state.enemy_frame_[i]) % 4 == 0) {
			if (rand() % 24 == 0) {
				int8_t spread = 4 + rand() % 5;
				for (uint8_t j = 0; j < kNumBulletsPerEnemy; ++j) {
					MaskedXYSprite& bullet = state.enemy_bullets_[i * kNumBulletsPerEnemy + j];
					if (bullet.active()) continue;
					bullet.setX(enemy.x());
					bullet.setY(enemy.y());
					bullet.setActive(true);
					state.enemy_bullet_ydelta_[i * kNumBulletsPerEnemy + j] = -spread;
					break;
				}
				for (uint8_t j = 0; j < kNumBulletsPerEnemy; ++j) {
					MaskedXYSprite& bullet = state.enemy_bullets_[i * kNumBulletsPerEnemy + j];
					if (bullet.active()) continue;
					bullet.setX(enemy.x());
					bullet.setY(enemy.y());
					bullet.setActive(true);
					state.enemy_bullet_ydelta_[i * kNumBulletsPerEnemy + j] = spread;
					break;
				}
			}
			if(rand() % 16 == 0 && !state.enemy_waves_[i].active()) {
				MaskedXYSprite& wave = state.enemy_waves_[i];
				wave.setActive(true);
				wave.setX(enemy.x());
				wave.setY(enemy.y() - 4);
				state.enemy_wave_ends[i] = enemy.x() - kEnemyWaveDuration;
				ShmupSfx::enemyWaveFired();
			}
		}
	}

	// Update numbers
	{
		char buf[kNumNumberSprites];
		memset(buf, 0, sizeof(buf));

		itoa(state.health_, buf, 10);

		char score[10];
		itoa(state.score_, score, 10);
		memcpy(buf + sizeof(buf) - strlen(score), score, strlen(score));


		uint16_t elapsed_seconds = (millis() - state.millis_start_) / 1000;
		sprintf(buf + 14, "%u:%02u", elapsed_seconds / 60, elapsed_seconds % 60);

		for (uint8_t i = 0; i < kNumNumberSprites; ++i) {
			state.number_sprites_[i].setActive(false);
		}

		for (uint8_t i = 0; i < kNumNumberSprites; ++i) {
			if (!buf[i]) continue;
			const uint8_t *raster = nullptr;
			switch(buf[i]) {
			case '-': raster = ShmupSprites::NUM_NEG; break;
			case '0': raster = ShmupSprites::NUM_0; break;
			case '1': raster = ShmupSprites::NUM_1; break;
			case '2': raster = ShmupSprites::NUM_2; break;
			case '3': raster = ShmupSprites::NUM_3; break;
			case '4': raster = ShmupSprites::NUM_4; break;
			case '5': raster = ShmupSprites::NUM_5; break;
			case '6': raster = ShmupSprites::NUM_6; break;
			case '7': raster = ShmupSprites::NUM_7; break;
			case '8': raster = ShmupSprites::NUM_8; break;
			case '9': raster = ShmupSprites::NUM_9; break;
			case ':': raster = ShmupSprites::NUM_COLON; break;
			}
			state.number_sprites_[i].setActive(true);
			state.number_sprites_[i].sprite() = Sprite(4, 6, raster, true);
		}
	}

	if (state.health_ < 0) {
		return false;
	}

	if (state.health_ < 500) setRGBled(state.frame_ & 16 ? 127 : 0, 0, 0);
	else if (state.health_ < 1000) setRGBled(127, 127, 0);
	else if (state.health_ < 1500) setRGBled(0, 127, 0);
	else setRGBled(0, 0, 127);

	if (state.player_impacting_ > 1 && (state.frame_ & 3))
		invert(play_inverted_);
	else
		invert(!play_inverted_);
	if (state.player_impacting_ > 0)
		--state.player_impacting_;

	uint32_t now = micros();
	while (now - state.frame_ts_ < 1000000 / (base_framerate_ + state.score_ / 125)) {
		now = micros();
	}
	state.frame_ts_ = now;

	for (int i = 0; i < kNumEnemies; ++i) {
		if (state.enemy_exploding_[i]) {
			state.enemy_[i].setActive(true);
			if (state.enemy_exploding_[i] & 1) {
				state.enemy_[i].sprite() = ShmupSprites::enemy;
			} else {
				state.enemy_[i].sprite() = ShmupSprites::enemyMask;
			}
		}
	}

	display(state);

	for (int i = 0; i < kNumEnemies; ++i) {
		if (state.enemy_exploding_[i]) {
			--state.enemy_exploding_[i];
			state.enemy_[i].setActive(false);
		}
	}
	return true;
}

void statefulLoop(uint16_t *score, uint32_t *millis_elapsed) {
	ShmupSfx::beginGame();
	State state;
	setup(state);
	while(loop(state));
	ShmupSfx::reset();
	*score = state.score_;
	*millis_elapsed = millis() - state.millis_start_;
}

void gameover(uint16_t score, uint16_t seconds_elapsed) {
	setRGBled(0, 0, 0);

	invert(true);
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	print("Game Over", 0, 0, buf);
	char sbuf[32];
	sprintf(sbuf, "Score %u / %u:%02u", score, seconds_elapsed/60, seconds_elapsed%60);
	print(sbuf, 0, 16, buf);
	sprintf(sbuf, "Best %u / %u:%02u", getHighScore(), getHighTime() / 60, getHighTime() % 60);
	print(sbuf, 0, 32, buf);
	if (score > getHighScore()) {
		print("NEW HIGH SCORE", 0, 48, buf);
	}
	if (seconds_elapsed > getHighTime()) {
		print("NEW HIGH TIME", 0, 56, buf);
	}

	if (score > getHighScore())
		setHighScore(score);
	if (seconds_elapsed > getHighTime())
		setHighTime(seconds_elapsed);

	Arduboy2Core::paintScreen(buf);
	if (write_display_) Serial.write(buf, 1024);

	ShmupSfx::gameOver();
	ShmupSfx::gameOver();

	uint32_t now = micros();
	while (micros() < now + 1000000)
		Arduboy2Core::idle();

	ShmupSfx::tick();

	buttonWait();
}

void showTitle() {
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	memcpy_P(buf, ShmupSprites::LOGO, 1285-541+1);
	char hs[22];
	sprintf(hs, "Best %u / %u:%02u", getHighScore(), getHighTime() / 60, getHighTime() % 60);
	print(hs, (128 - 6 * strlen(hs)) / 2, 56, buf);

	Arduboy2Core::paintScreen(buf);
	if (write_display_) Serial.write(buf, 1024);

	if (Arduboy2Core::buttonsState())
		while (Arduboy2Core::buttonsState()) Arduboy2Core::idle();
	else {
		for (;;) {
			uint8_t b;
			while (!(b = Arduboy2Core::buttonsState())) Arduboy2Core::idle();
			uint8_t b2;
			while ((b2 = Arduboy2Core::buttonsState())) { b |= b2; Arduboy2Core::idle(); }

			if (b != (UP_BUTTON | DOWN_BUTTON | RIGHT_BUTTON | LEFT_BUTTON)) return;

			Arduboy2Core::displayOff();

			do {
				b = 0;
				while (!(b = Arduboy2Core::buttonsState())) Arduboy2Core::idle();
				while ((b2 = Arduboy2Core::buttonsState())) { b |= b2; Arduboy2Core::idle(); }
			} while (b != (UP_BUTTON | DOWN_BUTTON | RIGHT_BUTTON | LEFT_BUTTON));

			Arduboy2Core::displayOn();
		}
	}
}

void storeConfiguration() {
	Arduboy2Audio::saveOnOff();
	for (uint8_t i = 0; i < sizeof(kEepromMagic); ++i)
		EEPROM.update(kEepromMagicOffset + i, kEepromMagic[i]);
	EEPROM.update(kEepromFramerateOffset, base_framerate_);
	EEPROM.update(kEepromInvertOffset, play_inverted_);
	EEPROM.update(kEepromPauseOffset, enable_pause_);
	EEPROM.update(kEepromHelpOffset, enable_help_);
}

void loadConfiguration() {
	Arduboy2Audio::begin();
	has_magic_ = true;
	for (uint8_t i = 0; i < sizeof(kEepromMagic); ++i) {
		if (EEPROM.read(kEepromMagicOffset + i) != kEepromMagic[i])
			has_magic_ = false;
	}
	if (has_magic_) {
		base_framerate_ = EEPROM.read(kEepromFramerateOffset);
		play_inverted_ = EEPROM.read(kEepromInvertOffset);
		enable_pause_ = EEPROM.read(kEepromPauseOffset);
		enable_help_ = EEPROM.read(kEepromHelpOffset);
	} else {
		base_framerate_ = 45;
		play_inverted_ = false;
		enable_pause_ = true;
		enable_help_ = true;
		setHighScore(0);
		setHighTime(0);
		storeConfiguration();
	}
}

void configure() {
	invert(true);
	loadConfiguration();

	uint8_t buf[1024];

	bool sound_enabled = EEPROM.read(EEPROM_AUDIO_ON_OFF);
	switch(base_framerate_) {
	case 15: case 30: case 45: case 60: case 75: break;
	default: base_framerate_ = 45;
	}
	bool reset_high_score = false;

	int8_t option = 0;
	while(true) {
		memset(buf, 0, 1024);
		print(" Sound:", 0, 0, buf);
		print(sound_enabled ? "ON" : "OFF", 7*6, 0, buf);
		print(" Framerate:", 0, 8, buf);
		char ibuf[4];
		itoa(base_framerate_, ibuf, 10);
		print(ibuf, 11*6, 8, buf);
		print(" Invert:", 0, 16, buf);
		print(play_inverted_ ? "ON" : "OFF", 8*6, 16, buf);
		print(" Pause:", 0, 24, buf);
		print(enable_pause_ ? "ON" : "OFF", 7*6, 24, buf);
		print(" Show help:", 0, 32, buf);
		print(enable_help_ ? "ON" : "OFF", 11*6, 32, buf);
		print(" Clear score:", 0, 40, buf);
		print(reset_high_score ? "YES" : "NO", 13*6, 40, buf);
		print(">", 0, option * 8, buf);

		Arduboy2Core::paintScreen(buf);
		if (write_display_) Serial.write(buf, 1024);

		uint8_t b = buttonWait();
		if (b == UP_BUTTON) {
			option -= 1;
			if (option < 0) option = 5;
		}
		if (b == DOWN_BUTTON) {
			option += 1;
			if (option > 5) option = 0;
		}
		if (option == 0 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			sound_enabled = !sound_enabled;
		}
		if (option == 1 && b == LEFT_BUTTON) {
			base_framerate_ -= 15;
			if (base_framerate_ < 15) base_framerate_ = 75;
		}
		if (option == 1 && b == RIGHT_BUTTON) {
			base_framerate_ += 15;
			if (base_framerate_ > 75) base_framerate_ = 15;
		}
		if (option == 2 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			play_inverted_ = !play_inverted_;
		}
		if (option == 3 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			enable_pause_ = !enable_pause_;
		}
		if (option == 4 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			enable_help_ = !enable_help_;
		}
		if (option == 5 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			reset_high_score = !reset_high_score;
		}
		if (b == A_BUTTON)
			break;
	}

	if (sound_enabled) {
		Arduboy2Audio::on();
	} else {
		Arduboy2Audio::off();
	}
	storeConfiguration();
	if (reset_high_score) {
		setHighScore(0);
		setHighTime(0);
	}
}

void showHelp() {
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	print("ArduSHMUP Controls", 10, 0, buf);
	print("PWR+A:Options", 0, 12, buf);
	print("PWR+\031:Screen Mirror", 0, 20, buf);
	print("PWR+\030:Flash", 0, 28, buf);
	print("  A+B:Pause", 0, 36, buf);
	print("    A:Wave", 0, 44, buf);
	print("    B:Beam", 0, 52, buf);

	for(uint8_t i = 0; i < 128; ++i)
		buf[i] ^= 0xff;

	SPI.transfer(buf, 1024);
	buttonWait();
}

uint8_t enableMirroring() {
	Serial.begin(921600);
	write_display_ = true;
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	print("Screen mirroring on.", 0, 0, buf);
	print("Press any key.", 0, 8, buf);
	Serial.write(buf, 1024);
	SPI.transfer(buf, 1024);
	return buttonWait();
}

void setup() {
	Arduboy2Core::boot();
	uint8_t b = Arduboy2Core::buttonsState();
	if (b & UP_BUTTON) {
		Arduboy2Core::allPixelsOn(true);
		setRGBled(255, 255, 255);
		while (true) Arduboy2Core::idle();
	}
	if (b == DOWN_BUTTON) {
		b = enableMirroring();
	}

	if (b == A_BUTTON) {
		configure();
	}

	loadConfiguration();

	ShmupSfx::begin();
}

void loop() {
	showTitle();
	randomSeed(micros());
	if (enable_help_) showHelp();
	uint16_t score;
	uint32_t millis_elapsed;
	statefulLoop(&score, &millis_elapsed);
	gameover(score, millis_elapsed / 1000);
}
