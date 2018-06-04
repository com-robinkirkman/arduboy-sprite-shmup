/*
 * MaskedXYSprite.cpp
 *
 *  Created on: May 20, 2018
 *      Author: robin
 */

#include "MaskedXYSprite.h"
#include "ShmupSprites.h"

extern bool inverted_;

MaskedXYSprite::MaskedXYSprite(uint8_t x, uint8_t y, const Sprite& sprite, const Sprite& mask, bool active)
{
	x_ = x;
	y_ = y;
	sprite_ = sprite;
	mask_ = mask;
	active_ = active;
}

void MaskedXYSprite::render(uint8_t page_num, uint8_t *page) const {
	if (!active_) return;
	if (!inverted_) {
		XYSprite s(x_, y_, Sprite::kOr, mask_);
		s.render(page_num, page);
		s.setSprite(sprite_);
		s.render(page_num, page, Sprite::kMask);
	} else {
		XYSprite s(x_, y_, Sprite::kMask, mask_);
		s.render(page_num, page);
		s.setSprite(sprite_);
		s.render(page_num, page, Sprite::kOr);
	}
}

bool MaskedXYSprite::intersects(const MaskedXYSprite& other) const {
	if (!active_ || !other.active_) return false;
	return XYSprite(x_, y_, Sprite::kOr, mask_).intersects(XYSprite(other.x_, other.y_, Sprite::kOr, other.mask_));
}
