#ifndef GLGE_TEXRECT_HPP
#define GLGE_TEXRECT_HPP

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif//GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <cstring>

namespace glge {

class texrect {
public:
    texrect (const texrect& texrect_in) {
        this->operator() ((texrect&)(texrect_in));
    }

    template <typename T>
    texrect (T data[], GLsizei width, GLsizei height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        this->operator() (data, width, height, alpha, TextureFilterLinear, TextureWrapRepeat);
    }

    texrect (GLsizei width, GLsizei height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        this->operator() (width, height, alpha, TextureFilterLinear, TextureWrapRepeat);
    }

    texrect (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        this->operator() (path, TextureFilterLinear, TextureWrapRepeat);
    }

    texrect (void) {
        texrect_id = 0;
        texrect_width = 0;
        texrect_height = 0;
        texrect_alpha = false;
    }

    ~texrect (void) {
        if (texrect_id && texrect_copies[texrect_id]) texrect_copies[texrect_id]--;
        if (texrect_id && !texrect_copies[texrect_id]) glDeleteTextures (1, &texrect_id);
        texrect_id = 0;
        texrect_width = 0;
        texrect_height = 0;
        texrect_alpha = false;
    }

public:
    void operator() (glge::texrect& texrect_in) {
        if (texrect_id && texrect_in.texrect_id != texrect_id) texrect_copies[texrect_id]--;
        if (texrect_id && !texrect_copies[texrect_id]) glDeleteTextures (1, &texrect_id);
        texrect_id = texrect_in.texrect_id;
        texrect_width = texrect_in.texrect_width;
        texrect_height = texrect_in.texrect_height;
        texrect_alpha = texrect_in.texrect_alpha;
        texrect_copies[texrect_id]++;
    }

    template <typename T>
    void operator() (T data[], GLsizei width, GLsizei height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        GLenum data_type = 0;
        if (typeid (T) == typeid (GLbyte)) data_type = GL_BYTE;
        if (typeid (T) == typeid (GLubyte)) data_type = GL_UNSIGNED_BYTE;
        if (typeid (T) == typeid (GLshort)) data_type = GL_SHORT;
        if (typeid (T) == typeid (GLushort)) data_type = GL_UNSIGNED_SHORT;
        if (typeid (T) == typeid (GLint)) data_type = GL_INT;
        if (typeid (T) == typeid (GLuint)) data_type = GL_UNSIGNED_INT;
        if (typeid (T) == typeid (GLfloat)) data_type = GL_FLOAT;
        if (data_type == 0) return;

        texrect_width = width;
        texrect_height = height;
        texrect_alpha = alpha;
        if (texrect_id && texrect_copies[texrect_id]) texrect_copies[texrect_id]--;
        if (texrect_id && !texrect_copies[texrect_id]) glDeleteTextures (1, &texrect_id);
        glGenTextures (1, &texrect_id);
        glBindTexture (GL_TEXTURE_RECTANGLE, texrect_id);

        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        glTexImage2D (GL_TEXTURE_RECTANGLE, 0, texrect_alpha ? GL_RGBA : GL_RGB, texrect_width, texrect_height, 0, texrect_alpha ? GL_RGBA : GL_RGB, data_type, data);
        glBindTexture (GL_TEXTURE_RECTANGLE, 0);

        texrect_copies[texrect_id]++;
    }

    void operator() (GLsizei width, GLsizei height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        texrect_width = width;
        texrect_height = height;
        texrect_alpha = alpha;
        if (texrect_id) texrect_copies[texrect_id]--;
        if (texrect_id && !texrect_copies[texrect_id]) glDeleteTextures (1, &texrect_id);
        glGenTextures (1, &texrect_id);
        glBindTexture (GL_TEXTURE_2D, texrect_id);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        glTexImage2D (GL_TEXTURE_2D, 0, texrect_alpha ? GL_RGBA : GL_RGB, texrect_width, texrect_height, 0, texrect_alpha ? GL_RGBA : GL_RGB, GL_FLOAT, NULL);
        glBindTexture (GL_TEXTURE_2D, 0);

        texrect_copies[texrect_id]++;
    }

public:
    void operator() (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        unsigned int path_length = strlen (path);

        if (
            path[path_length - 4] == '.' &&
            path[path_length - 3] == 'b' &&
            path[path_length - 2] == 'm' &&
            path[path_length - 1] == 'p'
        ) LoadBMP (path, TextureFilterLinear, TextureWrapRepeat);

        if (
            path[path_length - 4] == '.' &&
            path[path_length - 3] == 't' &&
            path[path_length - 2] == 'g' &&
            path[path_length - 1] == 'a'
        ) LoadTGA (path, TextureFilterLinear, TextureWrapRepeat);
    }

    void LoadBMP (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        FILE* file = fopen (path, "rb");

        char bmp_header[52];
        fread (bmp_header, 52, 1, file);

        int width = *((int*)(&bmp_header[18]));
        int height = *((int*)(&bmp_header[22]));
        texrect_width = width;
        texrect_height = height;

        char pixels[width * height * 3];
        fread (pixels, width * height * 3, 1, file);

        fclose (file);

        if (texrect_id) texrect_copies[texrect_id]--;
        if (texrect_id && !texrect_copies[texrect_id]) glDeleteTextures (1, &texrect_id);
        glGenTextures (1, &texrect_id);
        glBindTexture (GL_TEXTURE_RECTANGLE, texrect_id);

        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        texrect_alpha = false;
        glTexImage2D (GL_TEXTURE_RECTANGLE, 0, GL_RGB, texrect_width, texrect_height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);

        glBindTexture (GL_TEXTURE_RECTANGLE, 0);
        texrect_copies[texrect_id]++;
    }

    void LoadTGA (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true) {
        FILE* file = fopen (path, "rb");

        unsigned char ucharBad;
        short shortBad;

        fread (&ucharBad, sizeof (unsigned char), 1, file);
        fread (&ucharBad, sizeof (unsigned char), 1, file);

        fread (&ucharBad, sizeof (unsigned char), 1, file);

        fread (&shortBad, sizeof (short), 1, file);
        fread (&shortBad, sizeof (short), 1, file);
        fread (&ucharBad, sizeof (unsigned char), 1, file);
        fread (&shortBad, sizeof (short), 1, file);
        fread (&shortBad, sizeof (short), 1, file);

        short ImageWidth, ImageHeight;
        fread (&ImageWidth, sizeof (short), 1, file);
        fread (&ImageHeight, sizeof (short), 1, file);
        texrect_width = ImageWidth;
        texrect_height = ImageHeight;

        unsigned char BitCount;
        fread (&BitCount, sizeof (unsigned char), 1, file);

        fread (&ucharBad, sizeof (unsigned char), 1, file);

        unsigned char ColorMode = BitCount / 8;
        if (ColorMode == 4) texrect_alpha = true;
        else texrect_alpha = false;
        long ImageSize = ImageWidth * ImageHeight * ColorMode;
        unsigned char ImageData[ImageSize];
        fread (ImageData, sizeof (unsigned char), ImageSize, file);

        fclose (file);

        if (texrect_id) texrect_copies[texrect_id]--;
        if (texrect_id && !texrect_copies[texrect_id]) glDeleteTextures (1, &texrect_id);
        glGenTextures (1, &texrect_id);
        glBindTexture (GL_TEXTURE_RECTANGLE, texrect_id);

        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        glTexImage2D (GL_TEXTURE_RECTANGLE, 0, texrect_alpha ? GL_RGBA : GL_RGB, texrect_width, texrect_height, 0, texrect_alpha ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, ImageData);

        glBindTexture (GL_TEXTURE_RECTANGLE, 0);
        texrect_copies[texrect_id]++;
    }

public:
    template <typename T>
    void operator() (T data[]) {
        GLenum data_type = 0;
        if (typeid (T) == typeid (GLbyte)) data_type = GL_BYTE;
        if (typeid (T) == typeid (GLubyte)) data_type = GL_UNSIGNED_BYTE;
        if (typeid (T) == typeid (GLshort)) data_type = GL_SHORT;
        if (typeid (T) == typeid (GLushort)) data_type = GL_UNSIGNED_SHORT;
        if (typeid (T) == typeid (GLint)) data_type = GL_INT;
        if (typeid (T) == typeid (GLuint)) data_type = GL_UNSIGNED_INT;
        if (typeid (T) == typeid (GLfloat)) data_type = GL_FLOAT;
        if (data_type == 0) return;

        glBindTexture (GL_TEXTURE_RECTANGLE, texrect_id);
        glGetTexImage (GL_TEXTURE_RECTANGLE, 0, texrect_alpha ? GL_RGBA : GL_RGB, data_type, data);
        glBindTexture (GL_TEXTURE_RECTANGLE, 0);
    }

    GLuint getID (void) { return texrect_id; }
    GLsizei getWidth (void) { return texrect_width; }
    GLsizei getHeight (void) { return texrect_height; }
    void getSize (GLsizei* width, GLsizei* height) { *width = texrect_width; *height = texrect_height; }
    bool hasAlpha (void) { return texrect_alpha; }

    void bind (void) { glBindTexture (GL_TEXTURE_RECTANGLE, texrect_id); }

private:
    GLuint texrect_id;
    GLsizei texrect_width, texrect_height;
    bool texrect_alpha;
    static std::map <GLuint, unsigned int> texrect_copies;
};

std::map <GLuint, unsigned int> texrect::texrect_copies;

}

#endif//GLGE_TEXRECT_HPP
