/*
 * MaskedXYSprite.h
 *
 *  Created on: May 20, 2018
 *      Author: robin
 */

#ifndef SRC_MASKEDXYSPRITE_H_
#define SRC_MASKEDXYSPRITE_H_

#include "XYSprite.h"

class MaskedXYSprite {
public:
	static constexpr uint8_t kXOffset = XYSprite::kXOffset;
	static constexpr uint8_t kYOffset = XYSprite::kYOffset;

	MaskedXYSprite(const Sprite& sprite, const Sprite& mask) : MaskedXYSprite(0, 0, sprite, mask, false) {}
	MaskedXYSprite() : MaskedXYSprite(0, 0, {}, {}, false) {}
	MaskedXYSprite(uint8_t x, uint8_t y, const Sprite& sprite, const Sprite& mask, bool active);

	void render(uint8_t page_num, uint8_t *page) const;

	bool intersects(const MaskedXYSprite& other) const;

	inline bool active() const { return active_; }
	inline void setActive(bool active) { active_ = active; }

	inline uint8_t x() const { return x_; }
	inline void setX(uint8_t x) { x_ = x; }

	inline uint8_t y() const { return y_; }
	inline void setY(uint8_t y) { y_ = y; }

	inline Sprite& sprite() { return sprite_; }

	inline Sprite& mask() { return mask_; }

private:
	bool active_;
	uint8_t x_, y_;
	Sprite sprite_, mask_;
};

#endif /* SRC_MASKEDXYSPRITE_H_ */
