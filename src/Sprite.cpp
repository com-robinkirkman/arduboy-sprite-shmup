/*
 * Sprite.cpp
 *
 *  Created on: May 13, 2018
 *      Author: robin
 */

#include "Sprite.h"

#include <glcdfont.c>

void Sprite::transverseRaster(uint8_t *raster, uint8_t width, uint8_t height, uint8_t col_bytes, uint8_t *data) {
	for (uint8_t y = 0; y < height; ++y) {
		int row = y >> 3;
		int row_mask = 1 << (y & 7);
		uint8_t col_mask = 1 << ((width - 1) & 7);
		for (uint8_t x = 0; x < width; ++x) {
			uint8_t col_offset = x >> 3;
			if (data[y * col_bytes + col_offset] & col_mask)
				raster[width * row + x] |= row_mask;
			col_mask = col_mask >> 1;
			if (col_mask == 0)
				col_mask = 128;
		}
	}

}

void Sprite::setFontRaster(unsigned char c, uint8_t *raster) {
	*((uint32_t*)(void*) raster) = pgm_read_dword(font + 5 * c);
	raster[4] = pgm_read_byte(font + 5 * c + 4);
}

Sprite::Sprite(uint8_t width, uint8_t height, const uint8_t *raster, bool progmem)
: progmem_(progmem), width_(width), height_(height), raster_(raster) {}

Sprite::Sprite(unsigned char c) : Sprite(5, 7, font + 5 * c, true) {}

void Sprite::render(RenderMode mode, int x, int y, int w, uint8_t *dst) const {
	if (!raster_)
		return;
	if (progmem_) {
		renderPgmSpace(mode, x, y, w, dst);
	} else {
		renderRamSpace(mode, x, y, w, dst);
	}
}

void Sprite::renderRamSpace(RenderMode mode, int x, int y, int w, uint8_t *dst) const {
	if (y < -7 || y >= height_) return;
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (x + w >= width_) {
		w = width_ - x;
	}
	if (w <= 0) return;
	int row = y >> 3;
	int offset = y & 7;

	const uint8_t *r1 = raster_ + width_ * row + x;
	if (offset == 0) {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= *r1++;
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= *r1++;
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ *r1++;
		}
		return;
	}
	const uint8_t *r2 = r1 + width_;
	if (row >= 0 && 8 * row + 8 < height_) {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= ((*r2++ << (8 - offset)) | (*r1++ >> offset));
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= ((*r2++ << (8 - offset)) | (*r1++ >> offset));
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ ((*r2++ << (8 - offset)) | (*r1++ >> offset));
		}
	} else if(row < 0) {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= ((*r2++ << (8 - offset)));
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= ((*r2++ << (8 - offset)));
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ ((*r2++ << (8 - offset)));
		}
	} else {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= ((*r1++ >> offset));
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= ((*r1++ >> offset));
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ ((*r1++ >> offset));
		}
	}
}

void Sprite::renderPgmSpace(RenderMode mode, int x, int y, int w, uint8_t *dst) const {
	if (y < -7 || y >= height_) return;
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (x + w >= width_) {
		w = width_ - x;
	}
	if (w <= 0) return;
	int row = y >> 3;
	int offset = y & 7;

	const uint8_t *r1 = raster_ + width_ * row + x;
	if (offset == 0) {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= pgm_read_byte(r1++);
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= pgm_read_byte(r1++);
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ pgm_read_byte(r1++);
		}
		return;
	}
	const uint8_t *r2 = r1 + width_;
	if (row >= 0 && 8 * row + 8 < height_) {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= ((pgm_read_byte(r2++) << (8 - offset)) | (pgm_read_byte(r1++) >> offset));
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= ((pgm_read_byte(r2++) << (8 - offset)) | (pgm_read_byte(r1++) >> offset));
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ ((pgm_read_byte(r2++) << (8 - offset)) | (pgm_read_byte(r1++) >> offset));
		}
	} else if(row < 0) {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= ((pgm_read_byte(r2++) << (8 - offset)));
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= ((pgm_read_byte(r2++) << (8 - offset)));
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ ((pgm_read_byte(r2++) << (8 - offset)));
		}
	} else {
		if (mode == kAnd) {
			for (int i = 0; i < w; ++i) *dst++ &= ((pgm_read_byte(r1++) >> offset));
		} else if (mode == kOr) {
			for (int i = 0; i < w; ++i) *dst++ |= ((pgm_read_byte(r1++) >> offset));
		} else if (mode == kMask) {
			for (int i = 0; i < w; ++i) *dst++ &= ~ ((pgm_read_byte(r1++) >> offset));
		}
	}
}
