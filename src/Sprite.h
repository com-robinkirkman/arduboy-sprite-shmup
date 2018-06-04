/*
 * Sprite.h
 *
 *  Created on: May 13, 2018
 *      Author: robin
 */

#ifndef SRC_SPRITE_H_
#define SRC_SPRITE_H_

#include <stdint.h>

class Sprite {
public:
	enum RenderMode {
		kNone = 0,
		kAnd = 1,
		kOr = 2,
		kMask = 3,
	};

	static constexpr int rasterLength(uint8_t width, uint8_t height) {
		return width * ((height >> 3) + (height & 7 ? 1 : 0));
	}

	static void transverseRaster(uint8_t *raster, uint8_t width, uint8_t height, uint8_t col_bytes, uint8_t data[]);

	static void setFontRaster(unsigned char c, uint8_t *raster);

	Sprite() : Sprite(0, 0, nullptr, false) {}
	/*
	 * Construct a sprite.  `raster` contains horizontal stripes of vertical bytes.
	 */
	Sprite(uint8_t width, uint8_t height, const uint8_t *raster, bool progmem = false);

	Sprite(unsigned char c);

	inline uint8_t width() const { return width_; }
	inline uint8_t height() const { return height_; }
	inline const uint8_t *raster() const { return raster_; }

	void render(RenderMode mode, int x, int y, int w, uint8_t *dst) const;

private:
	void renderRamSpace(RenderMode mode, int x, int y, int w, uint8_t *dst) const;
	void renderPgmSpace(RenderMode mode, int x, int y, int w, uint8_t *dst) const;

	bool progmem_;
	uint8_t width_;
	uint8_t height_;
	const uint8_t *raster_;
};

#endif /* SRC_SPRITE_H_ */
