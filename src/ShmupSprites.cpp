/*
 * ShmupSprites.cpp
 *
 *  Created on: Dec 4, 2016
 *      Author: robin
 */

#include <ShmupSprites.h>
#include <avr/pgmspace.h>

const Sprite ShmupSprites::none;
const Sprite ShmupSprites::player(8, 8, PLAYER, true);
const Sprite ShmupSprites::playerMask(8, 8, PLAYER_MASK, true);
const Sprite ShmupSprites::enemy(8, 8, ENEMY, true);
const Sprite ShmupSprites::enemyMask(8, 8, ENEMY_MASK, true);
const Sprite ShmupSprites::bullet(8, 8, BULLET, true);
const Sprite ShmupSprites::bulletMask(8, 8, BULLET_MASK, true);
const Sprite ShmupSprites::enemyBullet(4, 8, EBULLET, true);
const Sprite ShmupSprites::enemyBulletMask(4, 8, EBULLET, true);
const Sprite ShmupSprites::wave(5, 16, WAVE, true);
const Sprite ShmupSprites::waveMask(5, 16, WAVE, true);
const Sprite ShmupSprites::enemyWave(3, 8, EWAVE, true);
const Sprite ShmupSprites::enemyWaveMask(3, 8, EWAVE, true);
const Sprite ShmupSprites::beam(128, 16, BEAM, true);
const Sprite ShmupSprites::beamMask(128, 16, BEAM, true);
const Sprite ShmupSprites::pause(19, 7, PAUSE, true);

const uint8_t ShmupSprites::PAUSE[] PROGMEM = {
	0b1011111,
	0b1000101,
	0b1000111,
	0b1000000,
	0b1011110,
	0b1001001,
	0b1011110,
	0b1000000,
	0b1011111,
	0b1010000,
	0b1011111,
	0b1000000,
	0b1010111,
	0b1010101,
	0b1011101,
	0b1000000,
	0b1011111,
	0b1010101,
	0b1010101
};

const uint8_t ShmupSprites::PLAYER[] PROGMEM = {
		0b00011000,
		0b11100111,
		0b10000001,
		0b01000010,
		0b00100100,
		0b00100100,
		0b00100100,
		0b00011000,
};

const uint8_t ShmupSprites::PLAYER_MASK[] PROGMEM = {
		0b00011000,
		0b11111111,
		0b11111111,
		0b01111110,
		0b00111100,
		0b00111100,
		0b00111100,
		0b00011000,
};

const uint8_t ShmupSprites::ENEMY[] PROGMEM = {
		0b00011000,
		0b00100100,
		0b10100101,
		0b10100101,
		0b11000011,
		0b10000001,
		0b11100111,
		0b00011000,
};

const uint8_t ShmupSprites::ENEMY_MASK[] PROGMEM = {
		0b00011000,
		0b00111100,
		0b10111101,
		0b10111101,
		0b11111111,
		0b11111111,
		0b11111111,
		0b00011000,
};

const uint8_t ShmupSprites::BULLET[] PROGMEM = {
		0b00000000,
		0b00000000,
		0b00011000,
		0b00100100,
		0b00100100,
		0b00011000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::BULLET_MASK[] PROGMEM = {
		0b00000000,
		0b00000000,
		0b00011000,
		0b00111100,
		0b00111100,
		0b00011000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::EBULLET[] PROGMEM = {
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
};

const uint8_t ShmupSprites::NUM_NEG[] PROGMEM = {
		0b00100000,
		0b00100000,
		0b00100000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_0[] PROGMEM = {
		0b11111000,
		0b10001000,
		0b11111000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_1[] PROGMEM = {
		0b10010000,
		0b11111000,
		0b10000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_2[] PROGMEM = {
		0b11001000,
		0b10101000,
		0b10010000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_3[] PROGMEM = {
		0b10101000,
		0b10101000,
		0b11111000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_4[] PROGMEM = {
		0b00111000,
		0b00100000,
		0b11111000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_5[] PROGMEM = {
		0b10111000,
		0b10101000,
		0b01001000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_6[] PROGMEM = {
		0b11110000,
		0b10101000,
		0b11101000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_7[] PROGMEM = {
		0b10001000,
		0b01101000,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_8[] PROGMEM = {
		0b11111000,
		0b10101000,
		0b11111000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::NUM_9[] PROGMEM = {
		0b10111000,
		0b10101000,
		0b01111000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
};

const uint8_t ShmupSprites::WAVE[] PROGMEM = {
		0b00000001,
		0b11111110,
		0b00000000,
		0b00000001,
		0b11111110,
		0b10000000,
		0b01111111,
		0b00000000,
		0b10000000,
		0b01111111,
};

const uint8_t ShmupSprites::EWAVE[] PROGMEM = {
		0b11111111,
		0b11111111,
		0b10000001,
};

const uint8_t ShmupSprites::BEAM[] PROGMEM = {
		0b11100000,
		0b11111000,
		0b01011000,
		0b00100100,
		0b00100100,
		0b01011010,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,
		0b00100101,
		0b01011011,
		0b01011011,
		0b00100101,

		0b00000111,
		0b00011111,
		0b00011010,
		0b00100100,
		0b00100100,
		0b01011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
		0b10100100,
		0b11011010,
		0b11011010,
		0b10100100,
};

const uint8_t ShmupSprites::LOGO[] PROGMEM = {
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11000000,
		0b11110000,
		0b01111000,
		0b00011100,
		0b00001100,
		0b00001100,
		0b00001100,
		0b00001100,
		0b00001100,
		0b01111100,
		0b11111000,
		0b11000000,
		0b00000000,
		0b00000000,
		0b10000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b10000000,
		0b10000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11111100,
		0b11111110,
		0b00000110,
		0b00000110,
		0b00000110,
		0b11111110,
		0b11111100,
		0b10000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b10000000,
		0b00000000,
		0b10000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b11000000,
		0b10000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11000000,
		0b11110000,
		0b01111110,
		0b00011111,
		0b00000011,
		0b00000000,
		0b10000000,
		0b10010000,
		0b10011110,
		0b10011111,
		0b10011000,
		0b10000000,
		0b00000000,
		0b00000001,
		0b00001111,
		0b01111111,
		0b11111000,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11100001,
		0b11111001,
		0b00111100,
		0b00001100,
		0b11111100,
		0b11111111,
		0b00000111,
		0b00000001,
		0b00000001,
		0b00000000,
		0b11111000,
		0b11111100,
		0b11111100,
		0b11111001,
		0b00100001,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111111,
		0b11111111,
		0b11100000,
		0b11111111,
		0b00111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00001110,
		0b00011111,
		0b00011011,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011110,
		0b00001111,
		0b00000011,
		0b00000001,
		0b00000001,
		0b00000001,
		0b00001111,
		0b00011111,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011001,
		0b00011111,
		0b00011111,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011111,
		0b00001111,
		0b00000000,
		0b00000000,
		0b00000001,
		0b00000111,
		0b00001111,
		0b00001100,
		0b00011100,
		0b00011000,
		0b00011000,
		0b00011001,
		0b00011001,
		0b00011100,
		0b00011100,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011111,
		0b00001111,
		0b00000111,
		0b00001111,
		0b00011100,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011001,
		0b00011001,
		0b00011100,
		0b00011100,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011111,
		0b00001111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11000000,
		0b11100000,
		0b01110000,
		0b00111000,
		0b00011000,
		0b00011000,
		0b10011000,
		0b10011000,
		0b10011000,
		0b10011000,
		0b00011000,
		0b00011000,
		0b00111000,
		0b11110000,
		0b11100000,
		0b11110000,
		0b11111000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b11111000,
		0b11110000,
		0b00000000,
		0b00000000,
		0b11110000,
		0b11111000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b11111000,
		0b11110000,
		0b11110000,
		0b11111000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00111000,
		0b11110000,
		0b11100000,
		0b10000000,
		0b11100000,
		0b11110000,
		0b00111000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b11111000,
		0b11110000,
		0b11110000,
		0b11111000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b11111000,
		0b11110000,
		0b00000000,
		0b11110000,
		0b11111000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b11111000,
		0b11110000,
		0b11110000,
		0b11111000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b10011000,
		0b10011000,
		0b10011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00111000,
		0b01110000,
		0b11100000,
		0b11000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00011111,
		0b10111111,
		0b11110000,
		0b11100000,
		0b11000000,
		0b11000011,
		0b11000111,
		0b11000111,
		0b10000111,
		0b10000111,
		0b00001111,
		0b00001111,
		0b00011111,
		0b01111011,
		0b11110001,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11100111,
		0b11100111,
		0b01100110,
		0b01100110,
		0b11100111,
		0b11100111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111000,
		0b11100000,
		0b10000001,
		0b00000111,
		0b00001111,
		0b00000111,
		0b10000000,
		0b11100000,
		0b11111000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111111,
		0b11111111,
		0b00000000,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b10011111,
		0b10011111,
		0b10011111,
		0b10001111,
		0b10000000,
		0b10000000,
		0b11000000,
		0b11100000,
		0b01111111,
		0b00111111,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00001111,
		0b00011111,
		0b00111001,
		0b00110001,
		0b00110001,
		0b00110011,
		0b00110011,
		0b00110011,
		0b00110011,
		0b00110001,
		0b00110000,
		0b00111000,
		0b00011100,
		0b00001111,
		0b00000111,
		0b00011111,
		0b00111111,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00111111,
		0b00011111,
		0b00000000,
		0b00000000,
		0b00011111,
		0b00111111,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00111111,
		0b00011111,
		0b00011111,
		0b00111111,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00111111,
		0b00011111,
		0b00000111,
		0b00000110,
		0b00000110,
		0b00000110,
		0b00000111,
		0b00011111,
		0b00111111,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00111111,
		0b00011111,
		0b00000011,
		0b00001111,
		0b00011110,
		0b00011000,
		0b00111000,
		0b00110000,
		0b00110001,
		0b00110011,
		0b00110011,
		0b00110011,
		0b00110001,
		0b00110000,
		0b00111000,
		0b00011000,
		0b00011110,
		0b00001111,
		0b00000011,
		0b00011111,
		0b00111111,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00110000,
		0b00111111,
		0b00011111,
		0b00000001,
		0b00000001,
		0b00000001,
		0b00000001,
		0b00000001,
};
