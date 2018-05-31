/*
 * ShmupSprites.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: robin
 */

#include <ShmupSprites.h>
#include <avr/pgmspace.h>

const Sprite ShmupSprites::player(8, 8, PLAYER, true);
const Sprite ShmupSprites::playerMask(8, 8, PLAYER_MASK, true);
const Sprite ShmupSprites::enemy(8, 8, ENEMY, true);
const Sprite ShmupSprites::enemyMask(8, 8, ENEMY_MASK, true);
const Sprite ShmupSprites::bullet(8, 8, BULLET, true);
const Sprite ShmupSprites::bulletMask(8, 8, BULLET_MASK, true);
const Sprite ShmupSprites::enemyBullet(8, 8, EBULLET, true);
const Sprite ShmupSprites::enemyBulletMask(8, 8, EBULLET_MASK, true);
const Sprite ShmupSprites::wave(8, 16, WAVE, true);
const Sprite ShmupSprites::waveMask(8, 16, WAVE_MASK, true);
const Sprite ShmupSprites::enemyWave(8, 8, EWAVE, true);
const Sprite ShmupSprites::enemyWaveMask(8, 8, EWAVE_MASK, true);
const Sprite ShmupSprites::beam(128, 8, BEAM, true);
const Sprite ShmupSprites::beamMask(128, 8, BEAM_MASK, true);

const uint8_t ShmupSprites::PLAYER[] PROGMEM = {
    0b00011000,
    0b11100111,
    0b10000001,
    0b01000010,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00011000, };

const uint8_t ShmupSprites::PLAYER_MASK[] PROGMEM = {
    0b00011000,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00011000, };

const uint8_t ShmupSprites::ENEMY[] PROGMEM = {
    0b00011000,
    0b00100100,
    0b10100101,
    0b10100101,
    0b11000011,
    0b10000001,
    0b11100111,
    0b00011000, };

const uint8_t ShmupSprites::ENEMY_MASK[] PROGMEM = {
    0b00011000,
    0b00111100,
    0b10111101,
    0b10111101,
    0b11111111,
    0b11111111,
    0b11111111,
    0b00011000, };

const uint8_t ShmupSprites::BULLET[] PROGMEM = {
    0b00000000,
    0b00000000,
    0b00011000,
    0b00100100,
    0b00100100,
    0b00011000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::BULLET_MASK[] PROGMEM = {
    0b00000000,
    0b00000000,
    0b00011000,
    0b00111100,
    0b00111100,
    0b00011000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::EBULLET[] PROGMEM = {
	    0b00010000,
	    0b00001000,
	    0b00010000,
	    0b00001000,
	    0b00010000,
	    0b00001000,
	    0b00010000,
	    0b00001000,
};

const uint8_t ShmupSprites::EBULLET_MASK[] PROGMEM = {
	    0b00010000,
	    0b00001000,
	    0b00010000,
	    0b00001000,
	    0b00010000,
	    0b00001000,
	    0b00010000,
	    0b00001000,
};

const uint8_t ShmupSprites::NUM_NEG[] PROGMEM = {
    0b00100000,
    0b00100000,
    0b00100000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_0[] PROGMEM = {
    0b11111000,
    0b10001000,
    0b11111000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_1[] PROGMEM = {
    0b10010000,
    0b11111000,
    0b10000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_2[] PROGMEM = {
    0b11001000,
    0b10101000,
    0b10010000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_3[] PROGMEM = {
    0b10101000,
    0b10101000,
    0b11111000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_4[] PROGMEM = {
    0b00111000,
    0b00100000,
    0b11111000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_5[] PROGMEM = {
    0b10111000,
    0b10101000,
    0b01001000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_6[] PROGMEM = {
    0b11110000,
    0b10101000,
    0b11101000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_7[] PROGMEM = {
    0b10001000,
    0b01101000,
    0b00011000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_8[] PROGMEM = {
    0b11111000,
    0b10101000,
    0b11111000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::NUM_9[] PROGMEM = {
    0b10111000,
    0b10101000,
    0b01111000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000, };

const uint8_t ShmupSprites::WAVE[] PROGMEM = {
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000001,
	    0b11111110,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b10000000,
	    0b01111111,
};

const uint8_t ShmupSprites::WAVE_MASK[] PROGMEM = {
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000001,
	    0b11111110,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b00000000,
	    0b10000000,
	    0b01111111,
};

const uint8_t ShmupSprites::EWAVE[] PROGMEM = {
    0b01111110,
    0b10000001,
    0b00000000,
    0b01111110,
    0b10000001,
    0b00000000,
    0b00000000,
    0b00000000,
};

const uint8_t ShmupSprites::EWAVE_MASK[] PROGMEM = {
    0b01111110,
    0b10000001,
    0b00000000,
    0b01111110,
    0b10000001,
    0b00000000,
    0b00000000,
    0b00000000,
};

const uint8_t ShmupSprites::BEAM[] PROGMEM = {
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011010,
		0b00100100,
};

const uint8_t ShmupSprites::BEAM_MASK[] PROGMEM = {
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
		0b01111110,
};

bool ShmupSprites::Collides(int x1, int y1, const uint8_t *m1, int x2, int y2,
    const uint8_t *m2) {
  if (x1 - x2 >= 8 || x2 - x1 >= 8 || y1 - y2 >= 8 || y2 - y1 >= 8)
    return false;
  int xoff = x2 - x1;
  int yoff = y2 - y1;
  int abs_xoff = (xoff >= 0) ? xoff : -xoff;

  for (int i = 0; i < 8 - abs_xoff; i++) {
    uint8_t b1, b2;
    b1 = pgm_read_byte(m1 + i + (xoff > 0 ? xoff : 0));
    b2 = pgm_read_byte(m2 + i + (xoff < 0 ? -xoff : 0));
    if (yoff > 0)
      b2 = b2 << yoff;
    if (yoff < 0)
      b1 = b1 << -yoff;
    if (b1 & b2)
      return true;
  }

  return false;
}

