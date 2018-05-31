/*
 * ShmupSprites.h
 *
 *  Created on: Dec 4, 2016
 *      Author: robin
 */

#ifndef SRC_SPRITES_H_
#define SRC_SPRITES_H_

#include <stdint.h>

#include <Sprite.h>

class ShmupSprites {
public:
  static const uint8_t PLAYER[];
  static const uint8_t PLAYER_MASK[];

  static const uint8_t ENEMY[];
  static const uint8_t ENEMY_MASK[];

  static const uint8_t BULLET[];
  static const uint8_t BULLET_MASK[];

  static const uint8_t EBULLET[];
  static const uint8_t EBULLET_MASK[];

  static const uint8_t WAVE[];
  static const uint8_t WAVE_MASK[];
  static const uint8_t EWAVE[];
  static const uint8_t EWAVE_MASK[];

  static const uint8_t BEAM[];
  static const uint8_t BEAM_MASK[];

  static const Sprite player;
  static const Sprite playerMask;
  static const Sprite enemy;
  static const Sprite enemyMask;
  static const Sprite bullet;
  static const Sprite bulletMask;
  static const Sprite enemyBullet;
  static const Sprite enemyBulletMask;
  static const Sprite wave;
  static const Sprite waveMask;
  static const Sprite enemyWave;
  static const Sprite enemyWaveMask;
  static const Sprite beam;
  static const Sprite beamMask;

  static const uint8_t NUM_NEG[];
  static const uint8_t NUM_0[];
  static const uint8_t NUM_1[];
  static const uint8_t NUM_2[];
  static const uint8_t NUM_3[];
  static const uint8_t NUM_4[];
  static const uint8_t NUM_5[];
  static const uint8_t NUM_6[];
  static const uint8_t NUM_7[];
  static const uint8_t NUM_8[];
  static const uint8_t NUM_9[];

  static bool Collides(int x1, int y1, const uint8_t *m1, int x2, int y2,
      const uint8_t *m2);
};

#endif /* SRC_SPRITES_H_ */
