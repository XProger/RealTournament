#ifndef H_TEXTURE
#define H_TEXTURE

#include "context.h"

struct Texture {

    GLuint ID;

    Texture(const char *name) {
        Stream stream(name);

        uint8  idlength;
        uint8  colourmaptype;
        uint8  datatypecode;
        uint16 colourmaporigin;
        uint16 colourmaplength;
        uint8  colourmapdepth;
        uint16 x_origin;
        uint16 y_origin;
        uint16 width;
        uint16 height;
        uint8  bitsperpixel;
        uint8  imagedescriptor;

        stream.read(&idlength, sizeof(idlength));
        stream.read(&colourmaptype, sizeof(colourmaptype));
        stream.read(&datatypecode, sizeof(datatypecode));
        stream.read(&colourmaporigin, sizeof(colourmaporigin));
        stream.read(&colourmaplength, sizeof(colourmaplength));
        stream.read(&colourmapdepth, sizeof(colourmapdepth));
        stream.read(&x_origin, sizeof(x_origin));
        stream.read(&y_origin, sizeof(y_origin));
        stream.read(&width, sizeof(width));
        stream.read(&height, sizeof(height));
        stream.read(&bitsperpixel, sizeof(bitsperpixel));
        stream.read(&imagedescriptor, sizeof(imagedescriptor));

        stream.seek(idlength);

        uint8 *data = new uint8[width * height * 4];
        stream.read(data, width * height * 4);

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        delete[] data;

        LOG("load image: %s (%dx%d)\n", name, width, height);
    }

    ~Texture() {
        glDeleteTextures(1, &ID);
    }

    void bind(int sampler) {
        glActiveTexture(GL_TEXTURE0 + sampler);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
};

#endif