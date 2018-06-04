/*
 * ShmupSfx.h
 *
 *  Created on: May 28, 2018
 *      Author: robin
 */

#ifndef SRC_SHMUPSFX_H_
#define SRC_SHMUPSFX_H_

#include <stdint.h>

class ShmupSfx {
public:
	static void begin();

	static void beginGame();

	static void bulletFired();
	static void waveFired();
	static void enemyWaveFired();
	static void beamFired();
	static void enemyImpact();
	static void playerImpact();
	static void tick();
	static void reset();

private:
	static constexpr uint8_t NONE = 0;
	static constexpr uint8_t BULLET_FIRED = 1;
	static constexpr uint8_t ENEMY_IMPACT = 2;
	static constexpr uint8_t WAVE_FIRED = 3;
	static constexpr uint8_t ENEMY_WAVE_FIRED = 3;
	static constexpr uint8_t BEAM_FIRED = 4;
	static constexpr uint8_t PLAYER_IMPACT = 5;

	static void sfx(uint8_t priority, const uint16_t *sptr);

	static uint8_t state1_;
	static uint8_t state2_;
};

#endif /* SRC_SHMUPSFX_H_ */
