/*
 * ShmupSfx.cpp
 *
 *  Created on: May 28, 2018
 *      Author: robin
 */

#include <ShmupSfx.h>

#include <Arduboy2Audio.h>
#include <Arduboy2Beep.h>
#include <Arduboy2Core.h>

uint8_t ShmupSfx::state1_ = NONE;
uint8_t ShmupSfx::state2_ = NONE;

const uint16_t off_[] = { 0, 0 };

const uint16_t bullet_[] = { 250, 3, 0, 0};
const uint16_t wave_[] = { 7500, 3, 2500, 1, 7500, 3, 2500, 1, 7500, 3, 2500, 1, 0, 0 };
const uint16_t beam_[] = { 2000, 1, 0, 0 };
const uint16_t enemy_impact_[] = { 1000, 2, 900, 2, 800, 2, 700, 2, 0, 0 };
const uint16_t player_impact_[] = { 1000, 3, 1200, 3, 1400, 3, 1600, 3, 0, 0 };

const uint16_t *sfx1_ = off_;
const uint16_t *sfx2_ = off_;

void ShmupSfx::sfx(uint8_t priority, const uint16_t *sptr) {
	uint16_t freq = *(sptr++);
	uint16_t duration = *(sptr++);
	if (priority > state1_) {
		state1_ = priority;
		sfx1_ = sptr;
		BeepPin1::tone(BeepPin1::freq(freq), duration);
	} else if(priority > state2_) {
		state2_ = priority;
		sfx2_ = sptr;
		BeepPin2::tone(BeepPin2::freq(freq), duration);
	}
}

void ShmupSfx::begin() {
	BeepPin1::begin();
	BeepPin2::begin();
}

void ShmupSfx::beginGame() {
	state1_ = NONE;
	state2_ = NONE;
	sfx1_ = off_;
	sfx2_ = off_;
	BeepPin1::noTone();
	BeepPin2::noTone();
}

void ShmupSfx::reset() {
	state1_ = NONE;
	state2_ = NONE;
	BeepPin1::noTone();
	BeepPin2::noTone();
}

void ShmupSfx::tick() {
	BeepPin1::timer();
	BeepPin2::timer();
	if (!BeepPin1::duration && state1_) {
		uint16_t freq = *(sfx1_++);
		uint16_t duration = *(sfx1_++);
		if (freq == 0) {
			state1_ = NONE;
		} else {
			BeepPin1::tone(BeepPin1::freq(freq), duration);
		}
	}
	if (!BeepPin2::duration && state2_) {
		uint16_t freq = *(sfx2_++);
		uint16_t duration = *(sfx2_++);
		if (freq == 0) {
			state2_ = NONE;
		} else {
			BeepPin2::tone(BeepPin2::freq(freq), duration);
		}
	}
}

void ShmupSfx::bulletFired() {
	sfx(BULLET_FIRED, bullet_);
}
void ShmupSfx::waveFired() {
	sfx(WAVE_FIRED, wave_);
}
void ShmupSfx::beamFired() {
	sfx(BEAM_FIRED, beam_);
}
void ShmupSfx::enemyImpact() {
	sfx(ENEMY_IMPACT, enemy_impact_);
}
void ShmupSfx::playerImpact() {
	sfx(PLAYER_IMPACT, player_impact_);
}

