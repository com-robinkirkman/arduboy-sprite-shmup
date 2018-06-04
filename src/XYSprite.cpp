/*
 * XYSprite.cpp
 *
 *  Created on: May 13, 2018
 *      Author: robin
 */

#include <XYSprite.h>
#include <string.h>

XYSprite::XYSprite(uint8_t x, uint8_t y, Sprite::RenderMode mode, const Sprite sprite) :
mode_(mode), sprite_(sprite), x_(x), y_(y) {}

void XYSprite::render(uint8_t page_num, uint8_t *page) const {
	render(page_num, page, (Sprite::RenderMode) mode_);
}

void XYSprite::render(uint8_t page_num, uint8_t *page, Sprite::RenderMode mode) const {
	if (!sprite_.raster())
		return;
	uint8_t page_y = page_num << 3;
	if (y_ + sprite_.height() < page_y + kYOffset || y_ >= page_y + 8 + kYOffset)
		return;
	if (x_ + sprite_.width() < kXOffset || x_ >= 128 + kXOffset)
		return;
	uint8_t width = sprite_.width();
	if (x_ + width - kXOffset > 128)
		width = 128 + kXOffset - x_;
	sprite_.render(mode, 0, page_y + kYOffset - y_, width, page + x_ - (int)kXOffset);
}

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

bool XYSprite::intersects(const XYSprite& other) const {
	uint8_t l = left(), other_l = other.left(); if (other_l > l) l = other_l;
	uint8_t r = right(), other_r = other.right(); if (other_r < r) r = other_r;
	if (r - l < 0)
		return false;
	uint8_t t = top(), other_t = other.top(); if (other_t > t) t = other_t;
	uint8_t b = bottom(), other_b = other.bottom(); if (other_b < b) b = other_b;
	if (b - t < 0)
		return false;

	for (uint8_t y = t; y <= b; y += 8) {
		for (uint8_t x = l; x <= r; ++x) {
			uint8_t r1 = 0, r2 = 0;
			sprite().render(Sprite::kOr, x - this->x(), y - this->y(), 1, &r1);
			other.sprite().render(Sprite::kOr, x - other.x(), y - other.y(), 1, &r2);
			if (r1 & r2)
				return true;
		}
	}

	return false;
}

