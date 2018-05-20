/*
 * MaskedXYSprite.h
 *
 *  Created on: May 20, 2018
 *      Author: robin
 */

#ifndef SRC_MASKEDXYSPRITE_H_
#define SRC_MASKEDXYSPRITE_H_

#include <XYSprite.h>

class MaskedXYSprite {
public:
	MaskedXYSprite(const Sprite& sprite, const Sprite& mask) : MaskedXYSprite(0, 0, sprite, mask, false) {}
	MaskedXYSprite() : MaskedXYSprite(0, 0, {}, {}, false) {}
	MaskedXYSprite(int x, int y, const Sprite& sprite, const Sprite& mask, bool active);

	void render(int page_num, uint8_t *page) const;

	bool intersects(const MaskedXYSprite& other) const;

	bool active() const { return active_; }
	void setActive(bool active) { active_ = active; }

	int x() const { return sprite_.x(); }
	void setX(int x) { sprite_.setX(x); mask_.setX(x); }

	int y() const { return sprite_.y(); }
	void setY(int y) { sprite_.setY(y); mask_.setY(y); }

private:
	bool active_;
	XYSprite sprite_, mask_;
};

#endif /* SRC_MASKEDXYSPRITE_H_ */
