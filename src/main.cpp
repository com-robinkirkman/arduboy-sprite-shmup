/*
 * main.cpp
 *
 *  Created on: May 15, 2018
 *      Author: robin
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <USBAPI.h>
#include <SpriteCore.h>
#include <Sprite.h>
#include <XYSprite.h>

#include "ShmupSprites.h"
#include "MaskedXYSprite.h"
#include "ShmupSfx.h"

constexpr uint8_t kNumPlayerBullets = 6;
constexpr uint8_t kNumPlayerWaves = 3;
constexpr uint8_t kNumEnemies = 16;
constexpr uint8_t kNumBulletsPerEnemy = 2;
constexpr uint8_t kNumEnemyBullets = kNumEnemies * kNumBulletsPerEnemy;
constexpr int kNumHealthSprites = 10;
constexpr int kNumScoreSprites = 10;

struct State {
MaskedXYSprite player_;
MaskedXYSprite player_bullets_[kNumPlayerBullets];
MaskedXYSprite player_waves_[kNumPlayerWaves];
uint8_t player_wave_ends_[kNumPlayerWaves];
MaskedXYSprite player_beam_;

MaskedXYSprite enemy_[kNumEnemies];
uint8_t enemy_frame_[kNumEnemies];
int8_t enemy_ydelta_[kNumEnemies];
uint8_t enemy_xdelta_[kNumEnemies];
MaskedXYSprite enemy_bullets_[kNumEnemyBullets];

MaskedXYSprite health_sprites_[kNumHealthSprites];
MaskedXYSprite score_sprites_[kNumScoreSprites];

uint8_t frame_ = 0;
uint32_t frame_ts_ = 0;

uint8_t wave_countdown_ = 0;

int16_t health_ = 0;
uint16_t score_ = 0;
int8_t player_impacting_ = 0;
};

void print(const char *s, int x, int y, uint8_t *buf) {
	for(char c = *s; c; c = *++s) {
		XYSprite sprite(x, y, Sprite::kOr, Sprite(c));
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

uint32_t last_frame_ = 0;

void invert(bool b) {
	inverted_ = b;
}

uint8_t buttonWait() {
	uint8_t b;
	while (SpriteCore::buttonsState()) SpriteCore::idle();
	while (!(b = SpriteCore::buttonsState())) SpriteCore::idle();
	while (SpriteCore::buttonsState()) SpriteCore::idle();
	return b;
}

inline void render(const MaskedXYSprite* s, uint8_t len, uint8_t page_num, uint8_t *page) {
	while(len--) {
		(*s++).render(page_num, page);
	}
}

void display(const State& state) {
	uint8_t page[128];
	bool write_display = write_display_ && (millis() - last_frame_) / (1000 / 30);
	if (write_display)
		last_frame_ = millis();
	for (uint8_t n = 0; n < 8; ++n) {
		memset(page, inverted_ ? 0 : 255, 128);

		render(state.health_sprites_, kNumHealthSprites, n, page);
		render(state.score_sprites_, kNumScoreSprites, n, page);
		render(state.player_bullets_, kNumPlayerBullets, n, page);
		render(state.enemy_bullets_, kNumEnemyBullets, n, page);
		render(state.player_waves_, kNumPlayerWaves, n, page);
		render(state.enemy_, kNumEnemies, n, page);
		render(&state.player_beam_, 1, n, page);
		render(&state.player_, 1, n, page);

		if (write_display) {
			Serial.write(page, 128);
		}
		SPI.transfer(page, 128);
	}
}

void reset(State& state) {
	state.player_ = MaskedXYSprite(0, 28, ShmupSprites::player, ShmupSprites::playerMask, true);

	for (uint8_t i = 0; i < kNumPlayerBullets; ++i)
		state.player_bullets_[i] = MaskedXYSprite(ShmupSprites::bullet, ShmupSprites::bulletMask);
	for (uint8_t i = 0; i < kNumPlayerWaves; ++i)
		state.player_waves_[i] = MaskedXYSprite(ShmupSprites::wave, ShmupSprites::waveMask);
	for (uint8_t i = 0; i < kNumEnemies; ++i)
		state.enemy_[i] = MaskedXYSprite(ShmupSprites::enemy, ShmupSprites::enemyMask);
	for (uint8_t i = 0; i < kNumEnemyBullets; ++i)
		state.enemy_bullets_[i] = MaskedXYSprite(ShmupSprites::enemyBullet, ShmupSprites::enemyBulletMask);

	state.player_beam_ = {ShmupSprites::beam, ShmupSprites::beamMask};

	for (uint8_t i = 0; i < kNumHealthSprites; ++i) {
		state.health_sprites_[i].setX(4 * i + 1);
		state.health_sprites_[i].setY(55);
	}

	for (uint8_t i = 0; i < kNumScoreSprites; ++i) {
		state.score_sprites_[i].setX(128 - 4 * kNumScoreSprites + 4 * i);
		state.score_sprites_[i].setY(55);
	}

	state.health_ = 1500;
	state.score_ = 0;
	state.player_impacting_ = 0;
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

uint32_t getHighScore() {
	uint32_t score = 0;
	score |= EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 0); score <<= 8;
	score |= EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 1); score <<= 8;
	score |= EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 2); score <<= 8;
	score |= EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 3);
	return score;
}

void setHighScore(uint32_t score) {
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 0, score >> 24);
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 1, score >> 16);
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 2, score >> 8);
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 3, score);
}


void setup(State& state) {
	reset(state);
	invert(false);
}

bool loop(State& state) {
	ShmupSfx::tick();

	++state.frame_;

	MaskedXYSprite& player = state.player_;
	uint8_t b = SpriteCore::buttonsState();

	// Player movement
	if ((b & UP_BUTTON) && player.y() > 0)
		player.setY(player.y() - 1);
	if ((b & DOWN_BUTTON) && player.y() < 56)
		player.setY(player.y() + 1);
	if ((b & LEFT_BUTTON) && player.x() > 0)
		player.setX(player.x() - 1);
	if ((b & RIGHT_BUTTON) && player.x() < 88)
		player.setX(player.x() + 1);

	// Bullet movement
	for (uint8_t i = 0; i < kNumPlayerBullets; ++i) {
		MaskedXYSprite& bullet = state.player_bullets_[i];
		if (!bullet.active()) continue;
		bullet.setX(bullet.x() + 2);
		if (bullet.x() >= 128)
			bullet.setActive(false);
	}
	for (uint8_t i = 0; i < kNumEnemyBullets; ++i) {
		MaskedXYSprite& bullet = state.enemy_bullets_[i];
		if (!bullet.active()) continue;
		uint8_t enemy = i / kNumBulletsPerEnemy;
		bullet.setX(bullet.x() - (1 + state.enemy_xdelta_[enemy]));
		if (bullet.x() < 0)
			bullet.setActive(false);
	}

	// Wave movement
	for (uint8_t i = 0; i < kNumPlayerWaves; ++i) {
		MaskedXYSprite& wave = state.player_waves_[i];
		if (!wave.active()) continue;
		wave.setX(wave.x() + 1);
		if (wave.x() >= 128 || wave.x() >= state.player_wave_ends_[i])
			wave.setActive(false);
	}

	// Enemy movement
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		MaskedXYSprite& enemy = state.enemy_[i];
		if (!enemy.active()) continue;
		if ((state.enemy_frame_[i] + state.frame_) % 0x3) continue;
		enemy.setX(enemy.x() - state.enemy_xdelta_[i]);
		enemy.setY(enemy.y() + state.enemy_ydelta_[i]);
		if (enemy.y() < 0 || enemy.y() > 55) {
			state.enemy_ydelta_[i] = -state.enemy_ydelta_[i];
			enemy.setY(enemy.y() + state.enemy_ydelta_[i]);
		}
		if (enemy.x() < 0)
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
			MaskedXYSprite& enemy = state.enemy_[i];
			if (!enemy.active()) continue;
			if (beam.intersects(enemy)) {
				enemy.setActive(false);
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
			state.player_impacting_ = base_framerate_;
			ShmupSfx::playerImpact();
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
			state.player_impacting_ = base_framerate_;
			ShmupSfx::playerImpact();
		}
	}

	// Player bullet firing
	if ((state.frame_ % 10) == 0) {
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
	if ((b & A_BUTTON) && !state.wave_countdown_) {
		for (uint8_t i = 0; i < kNumPlayerWaves; ++i) {
			MaskedXYSprite& wave = state.player_waves_[i];
			if (wave.active()) continue;
			wave.setX(player.x());
			wave.setY(player.y() - 4);
			wave.setActive(true);
			state.player_wave_ends_[i] = player.x() + 64;
			state.wave_countdown_ = 8;
			ShmupSfx::waveFired();
			break;
		}
	}

	// Player beam firing
	if ((b & B_BUTTON) && state.health_ > 5) {
		state.player_beam_.setX(player.x() + 8);
		state.player_beam_.setY(player.y());
		state.player_beam_.setActive(true);
		state.health_ -= 5;
		ShmupSfx::beamFired();
	} else {
		state.player_beam_.setActive(false);
	}

	// Enemy spawning
	if ((rand() % 8) == 0) {
		for (uint8_t i = 0; i < kNumEnemies && i < 3 + state.score_ / 500; ++i) {
			MaskedXYSprite& enemy = state.enemy_[i];
			if (enemy.active()) continue;
			enemy.setX(119);
			enemy.setY(rand() % 56);
			enemy.setActive(true);
			state.enemy_frame_[i] = rand() % 0x3F;
			state.enemy_ydelta_[i] = (rand() % 5) - 2;
			state.enemy_xdelta_[i] = (rand() % 4) / 2 + 1;
			break;
		}
	}

	// Enemy bullet firing
	for (uint8_t i = 0; i < kNumEnemies; ++i) {
		MaskedXYSprite& enemy = state.enemy_[i];
		if (!enemy.active()) continue;
		if ((state.frame_ + state.enemy_frame_[i]) % 16) continue;
		if (rand() % 4) continue;
		for (uint8_t j = 0; j < kNumBulletsPerEnemy; ++j) {
			MaskedXYSprite& bullet = state.enemy_bullets_[i * kNumBulletsPerEnemy + j];
			if (bullet.active()) continue;
			bullet.setX(enemy.x());
			bullet.setY(enemy.y() - 5);
			bullet.setActive(true);
			ShmupSfx::bulletFired();
			break;
		}
		for (uint8_t j = 0; j < kNumBulletsPerEnemy; ++j) {
			MaskedXYSprite& bullet = state.enemy_bullets_[i * kNumBulletsPerEnemy + j];
			if (bullet.active()) continue;
			bullet.setX(enemy.x());
			bullet.setY(enemy.y() + 5);
			bullet.setActive(true);
			ShmupSfx::bulletFired();
			break;
		}
	}

	// Update health
	{
		char buf[kNumHealthSprites];
		itoa(state.health_, buf, 10);
		for (uint8_t i = 0; i < kNumHealthSprites; ++i) {
			state.health_sprites_[i].setActive(false);
		}
		for (uint8_t i = 0; i < kNumHealthSprites; ++i) {
			if (!buf[i]) break;
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
			}
			state.health_sprites_[i].setActive(true);
			state.health_sprites_[i].sprite().setSprite(Sprite(4, 6, raster, true));
		}
	}

	// Update score
	{
		char buf[kNumScoreSprites];
		itoa(state.score_, buf, 10);
		for (uint8_t i = 0; i < kNumScoreSprites; ++i) {
			state.score_sprites_[i].setActive(false);
		}
		int skip = kNumScoreSprites - strlen(buf);
		for (uint8_t i = skip; i < kNumScoreSprites; ++i) {
			if (!buf[i - skip]) break;
			const uint8_t *raster = nullptr;
			switch(buf[i - skip]) {
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
			}
			state.score_sprites_[i].setActive(true);
			state.score_sprites_[i].sprite().setSprite(Sprite(4, 6, raster, true));
		}
	}

	if (state.health_ < 0) {
		return false;
	}

	if (state.health_ < 500) setRGBled(state.frame_ & 16 ? 127 : 0, 0, 0);
	else if (state.health_ < 1000) setRGBled(127, 127, 0);
	else if (state.health_ < 1500) setRGBled(0, 127, 0);
	else setRGBled(0, 0, 127);

	if (state.player_impacting_ > 1)
		invert(play_inverted_);
	if (state.player_impacting_ == 0)
		invert(!play_inverted_);
	if (state.player_impacting_ > 0)
		--state.player_impacting_;

	uint32_t now = micros();
	while (now - state.frame_ts_ < 1000000 / (base_framerate_ + state.score_ / 75)) {
		now = micros();
	}
	state.frame_ts_ = now;

	display(state);

	return true;
}

uint32_t statefulLoop() {
	State state;
	setup(state);
	while(loop(state));
	return state.score_;
}

void gameover(uint32_t score) {
	setRGBled(0, 0, 0);

	invert(true);
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	print("Game Over", 0, 0, buf);
	print("Score: ", 0, 16, buf);
	char sbuf[12];
	itoa(score, sbuf, 10);
	print(sbuf, 6*6, 16, buf);
	print("High Score: ", 0, 32, buf);
	itoa(getHighScore(), sbuf, 10);
	print(sbuf, 12*6, 32, buf);
	if (score > getHighScore()) {
		print("NEW HIGH SCORE", 0, 48, buf);
	}

	SpriteCore::paintScreen(buf);

	uint32_t now = micros();
	while (micros() < now + 1000000)
		SpriteCore::idle();

	buttonWait();

	if (score > getHighScore())
		setHighScore(score);
}

void showTitle() {
	uint8_t buf[1024];
	memset(buf, 0, sizeof(buf));
	char hs[22];
	memcpy(hs, "High Score: ", 11);
	itoa(getHighScore(), hs + 11, 10);
	print(hs, (128 - 6 * strlen(hs)) / 2, 56, buf);
	print("ArduSHMUP", (128-6*9)/2, 0, buf);

	SpriteCore::paintScreen(buf);

	if (SpriteCore::buttonsState())
		while (SpriteCore::buttonsState()) SpriteCore::idle();
	else
		buttonWait();
}

void loadConfiguration() {
	ShmupSfx::enable(EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 4));
	base_framerate_ = EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 5);
	play_inverted_ = EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 6);
}

void storeConfiguration() {
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 4, ShmupSfx::isEnabled());
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 5, base_framerate_);
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 6, play_inverted_);
}

void configure() {
	invert(true);
	loadConfiguration();

	uint8_t buf[1024];

	bool sound_enabled = ShmupSfx::isEnabled();
	uint8_t base_framerate = base_framerate_;
	switch(base_framerate) {
	case 15: case 30: case 45: case 60: case 75: break;
	default: base_framerate = 45;
	}
	bool reset_high_score = false;

	int8_t option = 0;
	while(true) {
		memset(buf, 0, 1024);
		print(" Sound:", 0, 0, buf);
		print(sound_enabled ? "ON" : "OFF", 7*6, 0, buf);
		print(" Framerate:", 0, 8, buf);
		char ibuf[4];
		itoa(base_framerate, ibuf, 10);
		print(ibuf, 11*6, 8, buf);
		print(" Invert:", 0, 16, buf);
		print(play_inverted_ ? "ON" : "OFF", 8*6, 16, buf);
		print(" Clear score:", 0, 24, buf);
		print(reset_high_score ? "YES" : "NO", 13*6, 24, buf);
		print(">", 0, option * 8, buf);

		SpriteCore::paintScreen(buf);

		uint8_t b = buttonWait();
		if (b == UP_BUTTON) {
			option -= 1;
			if (option < 0) option = 3;
		}
		if (b == DOWN_BUTTON) {
			option += 1;
			if (option > 3) option = 0;
		}
		if (option == 0 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			sound_enabled = !sound_enabled;
		}
		if (option == 1 && b == LEFT_BUTTON) {
			base_framerate -= 15;
			if (base_framerate < 15) base_framerate = 75;
		}
		if (option == 1 && b == RIGHT_BUTTON) {
			base_framerate += 15;
			if (base_framerate > 75) base_framerate = 15;
		}
		if (option == 2 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			play_inverted_ = !play_inverted_;
		}
		if (option == 3 && (b == LEFT_BUTTON || b == RIGHT_BUTTON)) {
			reset_high_score = !reset_high_score;
		}
		if (b == A_BUTTON)
			break;
	}

	ShmupSfx::enable(sound_enabled);
	base_framerate_ = base_framerate;
	storeConfiguration();
	if (reset_high_score)
		setHighScore(0);
}

void setup() {
	SpriteCore::begin();
	uint8_t b = SpriteCore::buttonsState();
	if (b & UP_BUTTON) {
		SpriteCore::allPixelsOn(true);
		setRGBled(255, 255, 255);
		while (true) SpriteCore::idle();
	}
	if (b == DOWN_BUTTON) {
		Serial.begin(4000000);
		write_display_ = true;
		b = buttonWait();
	}


	if (b == A_BUTTON) {
		configure();
	}

	loadConfiguration();

	randomSeed(micros());
}

void loop() {
	showTitle();
	uint32_t score = statefulLoop();
	gameover(score);
}
