#ifndef IMAGE_H
#define IMAGE_H

/*
 * color = 0x00RRGGBB
 *
 * yh
 * ^
 * |
 * |
 * |--->
 * o--->wx
 *
 */

#include <vector>

#include <FreeImagePlus.h>


struct Pixel {
    float r, g, b, a;
};


class Image {
public:
	Image();
    Image(const Image &image);
	Image(const char *path);

    Image &operator=(const Image &image);

	void load(const char *path);
	void save(const char *path);

	unsigned getHeight() const;
	unsigned getWidth() const;
    bool isValid() const;

	Pixel getPixel(unsigned x, unsigned y) const;
	void setPixel(unsigned x, unsigned y, Pixel pix);

    void *getPixelArray() const;

private:

	unsigned size_w_{};
	unsigned size_h_{};

    bool is_valid_{};

    unsigned bpp_{};

    FREE_IMAGE_TYPE type_{};

	std::vector<Pixel> im_{};
};

#endif // IMAGE_H