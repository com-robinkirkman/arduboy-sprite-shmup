/*
 * main.cpp
 *
 *  Created on: May 15, 2018
 *      Author: robin
 */

#include <Arduino.h>
#include <SpriteCore.h>
#include <Sprite.h>
#include <XYSprite.h>

#include "ShmupSprites.h"
#include "MaskedXYSprite.h"

constexpr int kNumPlayerBullets = 6;
constexpr int kNumPlayerWaves = 3;
constexpr int kNumEnemies = 10;
constexpr int kNumBulletsPerEnemy = 3;

ArrayList<MaskedXYSprite, 1> player_;
ArrayList<MaskedXYSprite, kNumPlayerBullets> player_bullets_;
ArrayList<MaskedXYSprite, kNumPlayerWaves> player_waves_;
ArrayList<uint8_t, kNumPlayerWaves> player_wave_ends_;

ArrayList<MaskedXYSprite, kNumEnemies> enemy_;
ArrayList<uint8_t, kNumEnemies> enemy_frame_;
ArrayList<int8_t, kNumEnemies> enemy_ydelta_;
ArrayList<MaskedXYSprite, kNumEnemies * kNumBulletsPerEnemy> enemy_bullets_;

ArrayList<MaskedXYSprite, 10> score_sprites_;

ArrayList<List<MaskedXYSprite>*, 6> sprites_;

uint8_t frame_ = 0;
uint32_t frame_ts_ = 0;

uint8_t wave_countdown_ = 0;

int32_t score_ = 0;
int8_t player_impacting_ = 0;

void display() {
	uint8_t page[128];
	for (int n = 0; n < 8; ++n) {
		memset(page, 0, 128);
		for (size_t i = 0; i < sprites_.size(); ++i) {
			List<MaskedXYSprite> &list = *sprites_[i];
			for (size_t j = 0; j < list.size(); ++j) {
				list[j].render(n, page);
			}
		}
		SPI.transfer(page, 128);
	}
}

void reset() {
	player_[0] = MaskedXYSprite(0, 28, ShmupSprites::player, ShmupSprites::playerMask, true);

	for (size_t i = 0; i < player_bullets_.size(); ++i)
		player_bullets_[i] = MaskedXYSprite(ShmupSprites::bullet, ShmupSprites::bulletMask);
	for (size_t i = 0; i < player_waves_.size(); ++i)
		player_waves_[i] = MaskedXYSprite(ShmupSprites::wave, ShmupSprites::waveMask);
	for (size_t i = 0; i < enemy_.size(); ++i)
		enemy_[i] = MaskedXYSprite(ShmupSprites::enemy, ShmupSprites::enemyMask);
	for (size_t i = 0; i < enemy_bullets_.size(); ++i)
		enemy_bullets_[i] = MaskedXYSprite(ShmupSprites::bullet, ShmupSprites::bulletMask);

	score_ = 0;
	player_impacting_ = 0;
}

void setup() {
	SpriteCore::begin();
	if (SpriteCore::buttonsState() && UP_BUTTON) {
		while (true) SpriteCore::idle();
	}

	SpriteCore::invert(true);

	sprites_[0] = &score_sprites_;
	sprites_[1] = &player_bullets_;
	sprites_[2] = &enemy_bullets_;
	sprites_[3] = &player_waves_;
	sprites_[4] = &enemy_;
	sprites_[5] = &player_;

	reset();
}


void loop() {
	uint32_t now = micros();
	while (now - frame_ts_ < 1000000 / 90) {
		now = micros();
	}
	frame_ts_ = now;

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
	if ((b & RIGHT_BUTTON) && player.x() < 120)
		player.setX(player.x() + 1);

	// Bullet movement
	for (size_t i = 0; i < player_bullets_.size(); ++i) {
		MaskedXYSprite& bullet = player_bullets_[i];
		if (!bullet.active()) continue;
		bullet.setX(bullet.x() + 1);
		if (bullet.x() >= 128)
			bullet.setActive(false);
	}
	for (size_t i = 0; i < enemy_bullets_.size(); ++i) {
		MaskedXYSprite& bullet = enemy_bullets_[i];
		if (!bullet.active()) continue;
		bullet.setX(bullet.x() - 1);
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
		if ((enemy_frame_[i] + frame_) % 0x7) continue;
		enemy.setX(enemy.x() - 1);
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
				score_ += 1;
			}
		}
		for (size_t j = 0; j < enemy_.size(); ++j) {
			MaskedXYSprite& enemy = enemy_[j];
			if (!enemy.active()) continue;
			if (bullet.intersects(enemy)) {
				enemy.setActive(false);
				impact = true;
				score_ += 10;
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
				score_ += 1;
			}
		}
		for (size_t j = 0; j < enemy_.size(); ++j) {
			MaskedXYSprite& enemy = enemy_[j];
			if (!enemy.active()) continue;
			if (wave.intersects(enemy)) {
				enemy.setActive(false);
				score_ += 10;
			}
		}
	}

	// Enemy bullet impacts
	for (size_t i = 0; i < enemy_bullets_.size(); ++i) {
		MaskedXYSprite& bullet = enemy_bullets_[i];
		if (!bullet.active()) continue;
		if (bullet.intersects(player)) {
			score_ -= 500;
			bullet.setActive(false);
			player_impacting_ = 16;
		}
	}

	// Enemy impacts
	for (size_t i = 0; i < enemy_.size(); ++i) {
		MaskedXYSprite& enemy = enemy_[i];
		if (!enemy.active()) continue;
		if (enemy.intersects(player)) {
			score_ -= 500;
			enemy.setActive(false);
			player_impacting_ = 16;
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
			wave.setY(player.y());
			wave.setActive(true);
			player_wave_ends_[i] = player.x() + 64;
			wave_countdown_ = 8;
			break;
		}
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
			break;
		}
	}

	// Enemy bullet firing
	for (size_t i = 0; i < enemy_.size(); ++i) {
		MaskedXYSprite& enemy = enemy_[i];
		if (!enemy.active()) continue;
		if ((frame_ + enemy_frame_[i]) & 0x3F) continue;
		for (size_t j = 0; j < kNumBulletsPerEnemy; ++j) {
			MaskedXYSprite& bullet = enemy_bullets_[i * kNumBulletsPerEnemy + j];
			if (bullet.active()) continue;
			bullet.setX(enemy.x());
			bullet.setY(enemy.y());
			bullet.setActive(true);
			break;
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
			score_sprites_[i] = MaskedXYSprite(4 * i + 1, 55, Sprite(4, 6, raster, true), {}, true);
		}
	}

	if (score_ < -500) reset();

	if (player_impacting_ > 1)
		SpriteCore::invert(false);
	if (player_impacting_ == 0)
		SpriteCore::invert(true);
	if (player_impacting_ > 0)
		--player_impacting_;

	display();
}
