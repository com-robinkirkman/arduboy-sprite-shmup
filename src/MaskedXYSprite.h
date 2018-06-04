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
	MaskedXYSprite(const Sprite& sprite, const Sprite& mask) : MaskedXYSprite(0, 0, sprite, mask, false) {}
	MaskedXYSprite() : MaskedXYSprite(0, 0, {}, {}, false) {}
	MaskedXYSprite(int x, int y, const Sprite& sprite, const Sprite& mask, bool active);

	void render(int page_num, uint8_t *page) const;

	bool intersects(const MaskedXYSprite& other) const;

	inline bool active() const { return active_; }
	inline void setActive(bool active) { active_ = active; }

	inline int x() const { return x_; }
	inline void setX(int x) { x_ = x; }

	inline int y() const { return y_; }
	inline void setY(int y) { y_ = y; }

	inline Sprite& sprite() { return sprite_; }

	inline Sprite& mask() { return mask_; }

private:
	bool active_;
	int x_, y_;
	Sprite sprite_, mask_;
};

#endif /* SRC_MASKEDXYSPRITE_H_ */
