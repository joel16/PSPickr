#include <cstring>
#include <png.h>
#include <pspdebug.h>

#include "textures.hpp"

extern unsigned char lives_png_start[];
extern unsigned int lives_png_size;

namespace Textures {
    static g2dTexture *lives_tex;

    g2dTexture *LoadImageBufferPNG(unsigned char *data, int size) {
        g2dTexture *tex = nullptr;
        png_image image;
        std::memset(&image, 0, (sizeof image));
        image.version = PNG_IMAGE_VERSION;
        
        if (png_image_begin_read_from_memory(&image, data, size) != 0) {
            png_bytep buffer;
            image.format = PNG_FORMAT_RGBA;
            buffer = new png_byte[PNG_IMAGE_SIZE(image)];
            
            if (buffer != nullptr && png_image_finish_read(&image, nullptr, buffer, 0, nullptr) != 0) {
                tex = g2dTexLoad(buffer, image.width, image.height, G2D_SWIZZLE);
                delete[] buffer;
                png_image_free(&image);
            }
            else {
                if (buffer == nullptr) {
                    pspDebugScreenPrintf("png_byte buffer: returned nullptr\n");
                    png_image_free(&image);
                }
                else {
                    pspDebugScreenPrintf("png_image_finish_read failed: %s\n", image.message);
                    delete[] buffer;
                }
            }
        }
        else
            pspDebugScreenPrintf("png_image_begin_read_from_memory failed: %s\n", image.message);
        
        return tex;
    }

    void Init(void) {
        lives_tex = Textures::LoadImageBufferPNG(lives_png_start, lives_png_size);
    }

    void Exit(void) {
        g2dTexFree(&lives_tex);
    }

    g2dTexture *GetLives(void) {
        return lives_tex;
    }
}
