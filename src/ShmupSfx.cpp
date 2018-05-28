/*
 * ShmupSfx.cpp
 *
 *  Created on: May 28, 2018
 *      Author: robin
 */

#include <ShmupSfx.h>

#include <ArduboyTones.h>

namespace {
ArduboyTones tones(ShmupSfx::isEnabled);
}

bool ShmupSfx::enabled_ = true;
uint8_t ShmupSfx::state_ = NONE;

void ShmupSfx::reset() {
	state_ = NONE;
	tones.noTone();
}

void ShmupSfx::tick() {
	if (!tones.playing())
		state_ = NONE;
}

void ShmupSfx::bulletFired() {
	if (state_ > BULLET_FIRED) return;
	state_ = BULLET_FIRED;
	tones.tone(500, 5);
}
void ShmupSfx::waveFired() {
	if (state_ > WAVE_FIRED) return;
	state_ = WAVE_FIRED;
	tones.tone(800, 10, 200, 10, 800, 10);
}
void ShmupSfx::beamFired() {
	if (state_ > BEAM_FIRED) return;
	state_ = BEAM_FIRED;
	tones.tone(2000, 10);
}
void ShmupSfx::enemyImpact() {
	if (state_ > ENEMY_IMPACT) return;
	state_ = ENEMY_IMPACT;
	tones.tone(1000, 20, 1500, 20, 1000, 20);
}
void ShmupSfx::playerImpact() {
	if (state_ > PLAYER_IMPACT) return;
	state_ = PLAYER_IMPACT;
	tones.tone(1500, 30, 3000, 10, 1500, 30);
}

