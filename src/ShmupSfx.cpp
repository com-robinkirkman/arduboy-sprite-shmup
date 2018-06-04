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

const uint16_t bullet_[] PROGMEM = { 250, 3, 0, 0};
const uint16_t wave_[] PROGMEM = { 1000, 2, 2500, 2, 1000, 2, 2500, 2, 1000, 2, 2500, 2, 0, 0 };
const uint16_t enemy_wave_[] PROGMEM = { 7500, 2, 5000, 2, 7500, 2, 5000, 2, 7500, 2, 5000, 2, 0, 0 };
const uint16_t beam_[] PROGMEM = { 2000, 1, 0, 0 };
const uint16_t enemy_impact_[] PROGMEM = { 1000, 2, 900, 2, 800, 2, 700, 2, 0, 0 };
const uint16_t player_impact_[] PROGMEM = { 1000, 3, 1200, 3, 1400, 3, 1600, 3, 1000, 3, 1200, 3, 1400, 3, 1600, 3, 0, 0 };

const uint16_t *sfx1_;
const uint16_t *sfx2_;

void ShmupSfx::sfx(uint8_t priority, const uint16_t *sptr) {
	uint16_t freq = pgm_read_word(sptr++);
	if (freq == 0)
		return;
	uint16_t duration = pgm_read_word(sptr++);
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
		uint8_t s = state1_;
		state1_ = NONE;
		sfx(s, sfx1_);
	}
	if (!BeepPin2::duration && state2_) {
		uint8_t s = state2_;
		state2_ = NONE;
		sfx(s, sfx2_);
	}
}

void ShmupSfx::bulletFired() {
	sfx(BULLET_FIRED, bullet_);
}
void ShmupSfx::waveFired() {
	sfx(WAVE_FIRED, wave_);
}
void ShmupSfx::enemyWaveFired() {
	sfx(ENEMY_WAVE_FIRED, enemy_wave_);
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

