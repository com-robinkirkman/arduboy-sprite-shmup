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
	static constexpr uint8_t kXOffset = 64;
	static constexpr uint8_t kYOffset = 64;

	XYSprite() : XYSprite(0, 0, Sprite::kNone, {}) {}
	XYSprite(uint8_t x, uint8_t y, Sprite::RenderMode mode, const Sprite sprite);

	uint8_t x() const { return x_; }
	uint8_t y() const { return y_; }

	void setX(uint8_t x) { x_ = x; }
	void setY(uint8_t y) { y_ = y; }

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
	uint8_t x_;
	uint8_t y_;
};

#endif /* SRC_XYSPRITE_H_ */
