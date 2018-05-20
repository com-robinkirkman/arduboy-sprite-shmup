/*
 * MaskedXYSprite.cpp
 *
 *  Created on: May 20, 2018
 *      Author: robin
 */

#include "MaskedXYSprite.h"
#include "ShmupSprites.h"

MaskedXYSprite::MaskedXYSprite(int x, int y, const Sprite& sprite, const Sprite& mask, bool active)
{
	sprite_ = XYSprite(x, y, Sprite::kOr, sprite);
	mask_ = XYSprite(x, y, Sprite::kMask, mask);
	active_ = active;
}

void MaskedXYSprite::render(int page_num, uint8_t *page) const {
	if (!active_) return;
	mask_.render(page_num, page);
	sprite_.render(page_num, page);

}

bool MaskedXYSprite::intersects(const MaskedXYSprite& other) const {
	if (!active_ || !other.active_) return false;
	return mask_.intersects(other.mask_);
//	return ShmupSprites::Collides(x(), y(), mask_.sprite().raster(),
//			other.x(), other.y(), other.mask_.sprite().raster());
}
