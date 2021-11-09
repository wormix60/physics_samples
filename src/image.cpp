#include "image.h"

#include <iostream>

Image::Image() {}


Image::Image(const Image &image) {
    size_w_   = image.size_w_  ;
    size_h_   = image.size_h_  ;
    im_       = image.im_      ;

    is_valid_ = image.is_valid_;
    bpp_      = image.bpp_     ;
    type_     = image.type_    ;
}


Image::Image(const char *path){
	(*this).load(path);
}


Image &Image::operator=(const Image & image) {
    size_w_   = image.size_w_  ;
    size_h_   = image.size_h_  ;
    im_       = image.im_      ;

    is_valid_ = image.is_valid_;
    bpp_      = image.bpp_     ;
    type_     = image.type_    ;

    return *this;
}

void Image::load(const char *path){
	fipImage image;

    image.load(path);

    if (!image.isValid()) {
        return;
    }

    type_ = image.getImageType();
    bpp_ = image.getBitsPerPixel();

    size_w_ = image.getWidth();
   	size_h_ = image.getHeight();

    image.convertToType(FIT_RGBAF);

    im_ = std::vector<Pixel>(size_w_ * size_h_);

    FIRGBAF *colors = (FIRGBAF *)image.accessPixels();

    Pixel pix;

    for (size_t i = 0; i < im_.size(); i++) {
        pix.r = colors[i].red;
        pix.g = colors[i].green;
        pix.b = colors[i].blue;
        pix.a = colors[i].alpha;

        im_[i] = pix;
    }

    is_valid_ = true;
}


void Image::save(const char *path){
    if (!is_valid_) {
        return;
    }

	fipImage image(type_, size_w_, size_h_, bpp_);

    
    if (type_ == FIT_BITMAP && bpp_ == 24) {
        RGBTRIPLE *colors = (RGBTRIPLE *) image.accessPixels();
        RGBTRIPLE pix;

        for (size_t i = 0; i < im_.size(); i++) {
            pix.rgbtRed       = (BYTE) (255.0f * im_[i].r);
            pix.rgbtGreen     = (BYTE) (255.0f * im_[i].g);
            pix.rgbtBlue      = (BYTE) (255.0f * im_[i].b);

            colors[i] = pix;
        }
    } else if (type_ == FIT_BITMAP && bpp_ == 32) {
        RGBQUAD *colors = (RGBQUAD *) image.accessPixels();
        RGBQUAD pix;

        for (size_t i = 0; i < im_.size(); i++) {
            pix.rgbRed        = (BYTE) (255.0f * im_[i].r);
            pix.rgbGreen      = (BYTE) (255.0f * im_[i].g);
            pix.rgbBlue       = (BYTE) (255.0f * im_[i].b);
            pix.rgbReserved   = (BYTE) (255.0f * im_[i].a);

            colors[i] = pix;
        }
    } else if (type_ == FIT_UINT16) {
        WORD *colors = (WORD *) image.accessPixels();
        WORD pix;

        for (size_t i = 0; i < im_.size(); i++) {
            pix               = (WORD)(((65535.0f * im_[i].r) + 
                                        (65535.0f * im_[i].g) + 
                                        (65535.0f * im_[i].b)) / 3.0f);

            colors[i] = pix;
        }
    } else if (type_ == FIT_UINT32) {
        DWORD *colors = (DWORD *) image.accessPixels();
        DWORD pix;

        for (size_t i = 0; i < im_.size(); i++) {
            pix               = (DWORD)(((4294967295.0f * im_[i].r) + 
                                         (4294967295.0f * im_[i].g) + 
                                         (4294967295.0f * im_[i].b)) / 3.0f);

            colors[i] = pix;
        }
    } else if (type_ == FIT_FLOAT) {
        float *colors = (float *) image.accessPixels();
        float pix;

        for (size_t i = 0; i < im_.size(); i++) {
            pix               = (((im_[i].r) + 
                                  (im_[i].g) + 
                                  (im_[i].b)) / 3.0f);

            colors[i] = pix;
        }
    } else {
        FIRGBAF *colors = (FIRGBAF *) image.accessPixels();
        FIRGBAF pix;
        
        for (size_t i = 0; i < im_.size(); i++) {
            pix.red           = im_[i].r;
            pix.green         = im_[i].g;
            pix.blue          = im_[i].b;
            pix.alpha         = im_[i].a;

            colors[i] = pix;
        }
    }


    image.save(path);

}


unsigned Image::getHeight() const {
    if (!is_valid_) {
        return 0;
    }
	return size_h_;
}	


unsigned Image::getWidth() const {
    if (!is_valid_) {
        return 0;
    }
	return size_w_;
}


bool Image::isValid() const {
    return is_valid_;
}


Pixel Image::getPixel(unsigned x, unsigned y) const {
    if (!is_valid_) {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
	return im_[y * size_w_ + x];
}

void Image::setPixel(unsigned x, unsigned y, Pixel pix){
    if (!is_valid_) {
        return;
    }
	im_[y * size_w_ + x] = pix;
}


void *Image::getPixelArray() const {
    return (void *)im_.data();
}