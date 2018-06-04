/*
 * XYSprite.h
 *
 *  Created on: May 13, 2018
 *      Author: robin
 */

#ifndef SRC_XYSPRITE_H_
#define SRC_XYSPRITE_H_

#include "Sprite.h"

class XYSprite {
public:
	XYSprite() : XYSprite(0, 0, Sprite::kNone, {}) {}
	XYSprite(int x, int y, Sprite::RenderMode mode, const Sprite sprite);

	int x() const { return x_; }
	int y() const { return y_; }

	void setX(int x) { x_ = x; }
	void setY(int y) { y_ = y; }

	inline const Sprite sprite() const { return sprite_; }

	inline void setSprite(const Sprite sprite) { sprite_ = sprite; }

	inline Sprite::RenderMode mode() { return (Sprite::RenderMode) mode_; }
	inline void setMode(Sprite::RenderMode mode) { mode_ = mode; }

	void render(uint8_t page_num, uint8_t *page) const;
	void render(uint8_t page_num, uint8_t *page, Sprite::RenderMode mode) const;

	bool intersects(const XYSprite& other) const;

	inline int top() const { return y(); }
	inline int bottom() const { return y() + sprite().height() - 1; }
	inline int left() const { return x(); }
	inline int right() const { return x() + sprite().width() - 1; }

private:
	uint8_t mode_;
	Sprite sprite_;
	int x_;
	int y_;
};

#endif /* SRC_XYSPRITE_H_ */
