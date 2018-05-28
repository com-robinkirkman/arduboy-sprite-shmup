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
#include <SpriteGfx.h>

#include "ShmupSprites.h"
#include "MaskedXYSprite.h"
#include "ShmupSfx.h"

constexpr int kNumPlayerBullets = 6;
constexpr int kNumPlayerWaves = 3;
constexpr int kNumEnemies = 10;
constexpr int kNumBulletsPerEnemy = 4;

struct State {
ArrayList<MaskedXYSprite, 1> player_;
ArrayList<MaskedXYSprite, kNumPlayerBullets> player_bullets_;
ArrayList<MaskedXYSprite, kNumPlayerWaves> player_waves_;
ArrayList<uint8_t, kNumPlayerWaves> player_wave_ends_;
ArrayList<MaskedXYSprite, 1> player_beam_;

ArrayList<MaskedXYSprite, kNumEnemies> enemy_;
ArrayList<uint8_t, kNumEnemies> enemy_frame_;
ArrayList<int8_t, kNumEnemies> enemy_ydelta_;
ArrayList<uint8_t, kNumEnemies> enemy_xdelta_;
ArrayList<MaskedXYSprite, kNumEnemies * kNumBulletsPerEnemy> enemy_bullets_;

ArrayList<MaskedXYSprite, 10> health_sprites_;
ArrayList<MaskedXYSprite, 10> score_sprites_;

ArrayList<List<MaskedXYSprite>*, 8> sprites_;

uint8_t frame_ = 0;
uint32_t frame_ts_ = 0;

uint8_t wave_countdown_ = 0;

int32_t health_ = 0;
uint32_t score_ = 0;
int8_t player_impacting_ = 0;
};

uint8_t base_framerate_;
bool write_display_ = false;
bool inverted_ = false;

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

void display(const List<MaskedXYSprite>& sprites) {
	uint8_t page[128];
	for (int n = 0; n < 8; ++n) {
		memset(page, inverted_ ? 0 : 255, 128);
		for (size_t i = 0; i < sprites.size(); ++i) {
			sprites[i].render(n, page);
		}
		if (write_display_) {
			Serial.write(page, 128);
		}
		SPI.transfer(page, 128);
	}
}

void display(List<List<MaskedXYSprite>*> sprites) {
	uint8_t page[128];
	bool write_display = write_display_ && (millis() - last_frame_) / (1000 / 20);
	if (write_display)
		last_frame_ = millis();
	for (int n = 0; n < 8; ++n) {
		memset(page, inverted_ ? 0 : 255, 128);
		for (size_t i = 0; i < sprites.size(); ++i) {
			List<MaskedXYSprite> &list = *sprites[i];
			for (size_t j = 0; j < list.size(); ++j) {
				list[j].render(n, page);
			}
		}
		if (write_display) {
			Serial.write(page, 128);
		}
		SPI.transfer(page, 128);
	}
}

void reset(State& state) {
	state.player_[0] = MaskedXYSprite(0, 28, ShmupSprites::player, ShmupSprites::playerMask, true);

	for (size_t i = 0; i < state.player_bullets_.size(); ++i)
		state.player_bullets_[i] = MaskedXYSprite(ShmupSprites::bullet, ShmupSprites::bulletMask);
	for (size_t i = 0; i < state.player_waves_.size(); ++i)
		state.player_waves_[i] = MaskedXYSprite(ShmupSprites::wave, ShmupSprites::waveMask);
	for (size_t i = 0; i < state.enemy_.size(); ++i)
		state.enemy_[i] = MaskedXYSprite(ShmupSprites::enemy, ShmupSprites::enemyMask);
	for (size_t i = 0; i < state.enemy_bullets_.size(); ++i)
		state.enemy_bullets_[i] = MaskedXYSprite(ShmupSprites::bullet, ShmupSprites::bulletMask);

	state.player_beam_[0] = {ShmupSprites::beam, ShmupSprites::beamMask};

	state.health_ = 500;
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
	int i = 0;
	state.sprites_[i++] = &state.health_sprites_;
	state.sprites_[i++] = &state.score_sprites_;
	state.sprites_[i++] = &state.player_bullets_;
	state.sprites_[i++] = &state.enemy_bullets_;
	state.sprites_[i++] = &state.player_waves_;
	state.sprites_[i++] = &state.enemy_;
	state.sprites_[i++] = &state.player_beam_;
	state.sprites_[i++] = &state.player_;

	reset(state);
	invert(false);
}

bool loop(State& state) {
	ArrayList<MaskedXYSprite, 1> &player_ = state.player_;
	ArrayList<MaskedXYSprite, kNumPlayerBullets> &player_bullets_ = state.player_bullets_;
	ArrayList<MaskedXYSprite, kNumPlayerWaves> &player_waves_ = state.player_waves_;
	ArrayList<uint8_t, kNumPlayerWaves> &player_wave_ends_ = state.player_wave_ends_;
	ArrayList<MaskedXYSprite, 1> &player_beam_ = state.player_beam_;

	ArrayList<MaskedXYSprite, kNumEnemies> &enemy_ = state.enemy_;
	ArrayList<uint8_t, kNumEnemies> &enemy_frame_ = state.enemy_frame_;
	ArrayList<int8_t, kNumEnemies> &enemy_ydelta_ = state.enemy_ydelta_;
	ArrayList<MaskedXYSprite, kNumEnemies * kNumBulletsPerEnemy> &enemy_bullets_ = state.enemy_bullets_;

	ArrayList<MaskedXYSprite, 10> &health_sprites_ = state.health_sprites_;
	ArrayList<MaskedXYSprite, 10> &score_sprites_ = state.score_sprites_;

	ArrayList<List<MaskedXYSprite>*, 8> &sprites_ = state.sprites_;

	uint8_t &frame_ = state.frame_;
	uint32_t &frame_ts_ = state.frame_ts_;

	uint8_t &wave_countdown_ = state.wave_countdown_;

	int32_t &health_ = state.health_;
	uint32_t &score_ = state.score_;
	int8_t &player_impacting_ = state.player_impacting_;

	ShmupSfx::tick();

	++frame_;

	MaskedXYSprite& player = player_[0];
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
	for (size_t i = 0; i < player_bullets_.size(); ++i) {
		MaskedXYSprite& bullet = player_bullets_[i];
		if (!bullet.active()) continue;
		bullet.setX(bullet.x() + 2);
		if (bullet.x() >= 128)
			bullet.setActive(false);
	}
	for (size_t i = 0; i < enemy_bullets_.size(); ++i) {
		MaskedXYSprite& bullet = enemy_bullets_[i];
		if (!bullet.active()) continue;
		size_t enemy = i / kNumBulletsPerEnemy;
		bullet.setX(bullet.x() - (1 + state.enemy_xdelta_[enemy]));
		if (bullet.x() < 0)
			bullet.setActive(false);
	}

	// Wave movement
	for (size_t i = 0; i < player_waves_.size(); ++i) {
		MaskedXYSprite& wave = player_waves_[i];
		if (!wave.active()) continue;
		wave.setX(wave.x() + 1);
		if (wave.x() >= 128 || wave.x() >= player_wave_ends_[i])
			wave.setActive(false);
	}

	// Enemy movement
	for (size_t i = 0; i < enemy_.size(); ++i) {
		MaskedXYSprite& enemy = enemy_[i];
		if (!enemy.active()) continue;
		if ((enemy_frame_[i] + frame_) % 0x3) continue;
		enemy.setX(enemy.x() - state.enemy_xdelta_[i]);
		enemy.setY(enemy.y() + enemy_ydelta_[i]);
		if (enemy.y() < 0 || enemy.y() > 55) {
			enemy_ydelta_[i] = -enemy_ydelta_[i];
			enemy.setY(enemy.y() + enemy_ydelta_[i]);
		}
		if (enemy.x() < 0)
			enemy.setActive(false);
	}

	// Bullet impacts
	for (size_t i = 0; i < player_bullets_.size(); ++i) {
		MaskedXYSprite& bullet = player_bullets_[i];
		if (!bullet.active()) continue;
		bool impact = false;
		for (size_t j = 0; j < enemy_bullets_.size(); ++j) {
			MaskedXYSprite& enemy_bullet = enemy_bullets_[j];
			if (!enemy_bullet.active()) continue;
			if (bullet.intersects(enemy_bullet)) {
				enemy_bullet.setActive(false);
				impact = true;
				health_ += 1;
				score_ += 1;
			}
		}
		for (size_t j = 0; j < enemy_.size(); ++j) {
			MaskedXYSprite& enemy = enemy_[j];
			if (!enemy.active()) continue;
			if (bullet.intersects(enemy)) {
				enemy.setActive(false);
				impact = true;
				health_ += 10;
				score_ += 10;
				ShmupSfx::enemyImpact();
			}
		}
		if (impact) bullet.setActive(false);
	}

	// Wave impacts
	for (size_t i = 0; i < player_waves_.size(); ++i) {
		MaskedXYSprite& wave = player_waves_[i];
		if (!wave.active()) continue;
		for (size_t j = 0; j < enemy_bullets_.size(); ++j) {
			MaskedXYSprite& enemy_bullet = enemy_bullets_[j];
			if (!enemy_bullet.active()) continue;
			if (wave.intersects(enemy_bullet)) {
				enemy_bullet.setActive(false);
				health_ += 1;
				score_ += 1;
			}
		}
		for (size_t j = 0; j < enemy_.size(); ++j) {
			MaskedXYSprite& enemy = enemy_[j];
			if (!enemy.active()) continue;
			if (wave.intersects(enemy)) {
				enemy.setActive(false);
				health_ += 10;
				score_ += 10;
				ShmupSfx::enemyImpact();
			}
		}
	}

	// Beam impacts
	if (player_beam_[0].active()) {
		MaskedXYSprite& beam = player_beam_[0];
		for (size_t i = 0; i < player_bullets_.size(); ++i) {
			MaskedXYSprite& bullet = player_bullets_[i];
			if (!bullet.active()) continue;
			if (beam.intersects(bullet)) {
				bullet.setActive(false);
			}
		}
		for (size_t i = 0; i < enemy_bullets_.size(); ++i) {
			MaskedXYSprite& bullet = enemy_bullets_[i];
			if (!bullet.active()) continue;
			if (beam.intersects(bullet)) {
				bullet.setActive(false);
				health_ += 1;
				score_ += 1;
			}
		}
		for (size_t i = 0; i < enemy_.size(); ++i) {
			MaskedXYSprite& enemy = enemy_[i];
			if (!enemy.active()) continue;
			if (beam.intersects(enemy)) {
				enemy.setActive(false);
				health_ += 10;
				score_ += 10;
				ShmupSfx::enemyImpact();
			}
		}
	}

	// Enemy bullet impacts
	for (size_t i = 0; i < enemy_bullets_.size(); ++i) {
		MaskedXYSprite& bullet = enemy_bullets_[i];
		if (!bullet.active()) continue;
		if (bullet.intersects(player)) {
			health_ -= 500;
			bullet.setActive(false);
			player_impacting_ = 16;
			ShmupSfx::playerImpact();
		}
	}

	// Enemy impacts
	for (size_t i = 0; i < enemy_.size(); ++i) {
		MaskedXYSprite& enemy = enemy_[i];
		if (!enemy.active()) continue;
		if (enemy.intersects(player)) {
			health_ -= 500;
			enemy.setActive(false);
			player_impacting_ = 16;
			ShmupSfx::playerImpact();
		}
	}

	// Player bullet firing
	if ((frame_ % 10) == 0) {
		for (size_t i = 0; i < player_bullets_.size(); ++i) {
			MaskedXYSprite& bullet = player_bullets_[i];
			if (bullet.active()) continue;
			bullet.setX(player.x());
			bullet.setY(player.y());
			bullet.setActive(true);
			ShmupSfx::bulletFired();
			break;
		}
	}

	// Player wave firing
	if (wave_countdown_ > 0)
		--wave_countdown_;
	if ((b & A_BUTTON) && !wave_countdown_) {
		for (size_t i = 0; i < player_waves_.size(); ++i) {
			MaskedXYSprite& wave = player_waves_[i];
			if (wave.active()) continue;
			wave.setX(player.x());
			wave.setY(player.y() - 4);
			wave.setActive(true);
			player_wave_ends_[i] = player.x() + 64;
			wave_countdown_ = 8;
			ShmupSfx::waveFired();
			break;
		}
	}

	// Player beam firing
	if ((b & B_BUTTON) && health_ > 5) {
		player_beam_[0].setX(player.x() + 8);
		player_beam_[0].setY(player.y());
		player_beam_[0].setActive(true);
		health_ -= 5;
		ShmupSfx::beamFired();
	} else {
		player_beam_[0].setActive(false);
	}

	// Enemy spawning
	if ((frame_ % 24) == 0) {
		for (size_t i = 0; i < enemy_.size(); ++i) {
			MaskedXYSprite& enemy = enemy_[i];
			if (enemy.active()) continue;
			enemy.setX(119);
			enemy.setY(rand() % 56);
			enemy.setActive(true);
			enemy_frame_[i] = rand() % 0x3F;
			enemy_ydelta_[i] = (rand() % 5) - 2;
			state.enemy_xdelta_[i] = (rand() % 4) / 2 + 1;
			break;
		}
	}

	// Enemy bullet firing
	for (size_t i = 0; i < enemy_.size(); ++i) {
		MaskedXYSprite& enemy = enemy_[i];
		if (!enemy.active()) continue;
		if ((frame_ + enemy_frame_[i]) % 48) continue;
		for (size_t j = 0; j < kNumBulletsPerEnemy; ++j) {
			MaskedXYSprite& bullet = enemy_bullets_[i * kNumBulletsPerEnemy + j];
			if (bullet.active()) continue;
			bullet.setX(enemy.x());
			bullet.setY(enemy.y());
			bullet.setActive(true);
			ShmupSfx::bulletFired();
			break;
		}
	}

	// Update health
	{
		char buf[health_sprites_.size()];
		itoa(health_, buf, 10);
		for (size_t i = 0; i < health_sprites_.size(); ++i) {
			health_sprites_[i] = {};
		}
		for (size_t i = 0; i < health_sprites_.size(); ++i) {
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
			health_sprites_[i] = MaskedXYSprite(4 * i + 1, 55, Sprite(4, 6, raster, true), {}, true);
		}
	}

	// Update score
	{
		char buf[score_sprites_.size()];
		itoa(score_, buf, 10);
		for (size_t i = 0; i < score_sprites_.size(); ++i) {
			score_sprites_[i] = {};
		}
		for (size_t i = 0; i < score_sprites_.size(); ++i) {
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
			score_sprites_[i] = MaskedXYSprite(128 - 4 * strlen(buf) + 4 * i, 55, Sprite(4, 6, raster, true), {}, true);
		}
	}

	if (health_ < 0) {
		return false;
	}

	if (health_ < 500) setRGBled(frame_ & 16 ? 127 : 0, 0, 0);
	else if (health_ < 1000) setRGBled(127, 127, 0);
	else if (health_ < 1500) setRGBled(0, 127, 0);
	else setRGBled(0, 0, 127);

	if (player_impacting_ > 1)
		invert(true);
	if (player_impacting_ == 0)
		invert(false);
	if (player_impacting_ > 0)
		--player_impacting_;

	uint32_t now = micros();
	while (now - frame_ts_ < 1000000 / (base_framerate_ + score_ / 50)) {
		now = micros();
	}
	frame_ts_ = now;

	display(sprites_);

	return true;
}

uint32_t statefulLoop() {
	State state;
	setup(state);
	while(loop(state));
	return state.score_;
}

void showHighScore() {
	invert(true);
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	SpriteGfx buf_gfx(128, 64, buf);
	buf_gfx.setTextColor(SpriteGfx::kWhite);
	buf_gfx.print("High Score: ");
	buf_gfx.print(getHighScore());

	ArrayList<MaskedXYSprite, 1> score;
	score[0] = {Sprite(128, 64, buf, false), {}};
	score[0].setActive(true);
	display(score);

	buttonWait();
}

void gameover(uint32_t score) {
	setRGBled(0, 0, 0);

	invert(true);
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	SpriteGfx buf_gfx(128, 64, buf);
	buf_gfx.setTextColor(SpriteGfx::kWhite);
	buf_gfx.print("Game Over");
	buf_gfx.setCursor(0, 16);
	buf_gfx.print("Score: ");
	buf_gfx.print(score);
	buf_gfx.setCursor(0, 32);
	buf_gfx.print("High Score: ");
	buf_gfx.print(getHighScore());
	if (score > getHighScore()) {
		buf_gfx.setCursor(0, 48);
		buf_gfx.print("NEW HIGH SCORE");
	}

	ArrayList<MaskedXYSprite, 1> sprite;
	sprite[0] = {Sprite(128, 64, buf, false), {}};
	sprite[0].setActive(true);
	display(sprite);

	uint32_t now = micros();
	while (micros() < now + 1000000)
		SpriteCore::idle();

	buttonWait();

	if (score > getHighScore())
		setHighScore(score);
}

void showTitle() {
	ArrayList<MaskedXYSprite, 1> shmup;
	uint8_t buf[1024];
	memset(buf, 0, sizeof(buf));
	SpriteGfx buf_gfx(128, 64, buf);
	buf_gfx.setTextColor(SpriteGfx::kWhite);
	buf_gfx.setTextSize(3);
	buf_gfx.setCursor(28, 8);
	buf_gfx.print("Ardu");
	buf_gfx.setCursor(19, 32);
	buf_gfx.print("SHMUP");
	shmup[0] = MaskedXYSprite(Sprite(128, 64, buf, false), {});
	shmup[0].setActive(true);
	display(shmup);

	if (SpriteCore::buttonsState())
		while (SpriteCore::buttonsState()) SpriteCore::idle();
	else
		buttonWait();
}

void loadConfiguration() {
	ShmupSfx::enable(EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 4));
	base_framerate_ = EEPROM.read(SpriteCore::EEPROM_STORAGE_SPACE_START + 5);
}

void storeConfiguration() {
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 4, ShmupSfx::isEnabled());
	EEPROM.write(SpriteCore::EEPROM_STORAGE_SPACE_START + 5, base_framerate_);
}

void configure() {
	invert(true);
	loadConfiguration();

	uint8_t buf[1024];
	SpriteGfx gfx(128, 64, buf);
	gfx.setTextColor(SpriteGfx::kWhite);
	ArrayList<MaskedXYSprite, 1> sprite;
	sprite[0] = {Sprite(128, 64, buf, false), {}};
	sprite[0].setActive(true);

	bool sound_enabled = ShmupSfx::isEnabled();
	uint8_t base_framerate = base_framerate_;
	switch(base_framerate) {
	case 15: case 30: case 45: case 60: case 75: break;
	default: base_framerate = 45;
	}

	int option = 0;
	while(true) {
		memset(buf, 0, 1024);
		gfx.setCursor(0, 0);
		gfx.print(" Sound:");
		gfx.print(sound_enabled ? "ON" : "OFF");
		gfx.setCursor(0, 8);
		gfx.print(" Framerate:");
		gfx.print(base_framerate);
		gfx.setCursor(0, option * 8);
		gfx.print('>');
		display(sprite);

		uint8_t b = buttonWait();
		if (b == UP_BUTTON) {
			option -= 1;
			if (option < 0) option = 1;
		}
		if (b == DOWN_BUTTON) {
			option += 1;
			if (option > 1) option = 0;
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
		if (b == A_BUTTON)
			break;
	}

	ShmupSfx::enable(sound_enabled);
	base_framerate_ = base_framerate;
	storeConfiguration();
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
		Serial.begin(115200);
		write_display_ = true;
		b = buttonWait();
	}


	showTitle();
	if (b == (A_BUTTON | B_BUTTON)) {
		setHighScore(0);
	} else if (b == A_BUTTON) {
		configure();
	}

	loadConfiguration();
	showHighScore();
}

void loop() {
	uint32_t score = statefulLoop();
	gameover(score);
}
