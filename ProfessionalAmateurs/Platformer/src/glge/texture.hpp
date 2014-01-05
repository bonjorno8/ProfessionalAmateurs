#ifndef GLGE_TEXTURE_HPP
#define GLGE_TEXTURE_HPP

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

class texture {
public:
    texture (const texture& texture_in) {
        this->operator() ((texture&)(texture_in));
    }

    template <typename T>
    texture (T data[], GLsizei width_height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
        this->operator() (data, width_height, alpha, TextureFilterLinear, TextureWrapRepeat, InternalFormat32F);
    }

    texture (GLsizei width_height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
        this->operator() (width_height, alpha, TextureFilterLinear, TextureWrapRepeat, InternalFormat32F);
    }

    texture (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
        this->operator() (path, TextureFilterLinear, TextureWrapRepeat, InternalFormat32F);
    }

    texture (void) {
        texture_id = 0;
        texture_size = 0;
        texture_alpha = false;
    }

    ~texture (void) {
        if (texture_id && texture_copies[texture_id]) texture_copies[texture_id]--;
        if (texture_id && !texture_copies[texture_id]) glDeleteTextures (1, &texture_id);
        texture_id = 0;
        texture_size = 0;
        texture_alpha = false;
    }

public:
    void operator() (glge::texture& texture_in) {
        if (texture_id && texture_in.texture_id != texture_id) texture_copies[texture_id]--;
        if (texture_id && !texture_copies[texture_id]) glDeleteTextures (1, &texture_id);
        texture_id = texture_in.texture_id;
        texture_size = texture_in.texture_size;
        texture_alpha = texture_in.texture_alpha;
        texture_copies[texture_id]++;
    }

    template <typename T>
    void operator() (T data[], GLsizei width_height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
        GLenum data_type = 0;
        if (typeid (T) == typeid (GLbyte)) data_type = GL_BYTE;
        if (typeid (T) == typeid (GLubyte)) data_type = GL_UNSIGNED_BYTE;
        if (typeid (T) == typeid (GLshort)) data_type = GL_SHORT;
        if (typeid (T) == typeid (GLushort)) data_type = GL_UNSIGNED_SHORT;
        if (typeid (T) == typeid (GLint)) data_type = GL_INT;
        if (typeid (T) == typeid (GLuint)) data_type = GL_UNSIGNED_INT;
        if (typeid (T) == typeid (GLfloat)) data_type = GL_FLOAT;
        if (data_type == 0) return;

        texture_size = width_height;
        texture_alpha = alpha;
        if (texture_id && texture_copies[texture_id]) texture_copies[texture_id]--;
        if (texture_id && !texture_copies[texture_id]) glDeleteTextures (1, &texture_id);
        glGenTextures (1, &texture_id);
        glBindTexture (GL_TEXTURE_2D, texture_id);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        glTexImage2D (GL_TEXTURE_2D, 0, texture_alpha ? (InternalFormat32F ? GL_RGBA32F : GL_RGBA) : (InternalFormat32F ? GL_RGB32F : GL_RGB), texture_size, texture_size, 0, texture_alpha ? GL_RGBA : GL_RGB, GL_FLOAT, data);
        glBindTexture (GL_TEXTURE_2D, 0);

        texture_copies[texture_id]++;
    }

    void operator() (GLsizei width_height, bool alpha, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
        texture_size = width_height;
        texture_alpha = alpha;
        if (texture_id) texture_copies[texture_id]--;
        if (texture_id && !texture_copies[texture_id]) glDeleteTextures (1, &texture_id);
        glGenTextures (1, &texture_id);
        glBindTexture (GL_TEXTURE_2D, texture_id);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        glTexImage2D (GL_TEXTURE_2D, 0, texture_alpha ? (InternalFormat32F ? GL_RGBA32F : GL_RGBA) : (InternalFormat32F ? GL_RGB32F : GL_RGB), texture_size, texture_size, 0, texture_alpha ? GL_RGBA : GL_RGB, GL_FLOAT, NULL);
        glBindTexture (GL_TEXTURE_2D, 0);

        texture_copies[texture_id]++;
    }

public:
    void operator() (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
        unsigned int path_length = strlen (path);

        if (
            path[path_length - 4] == '.' &&
            path[path_length - 3] == 'b' &&
            path[path_length - 2] == 'm' &&
            path[path_length - 1] == 'p'
        ) LoadBMP (path, TextureFilterLinear, TextureWrapRepeat, InternalFormat32F);

        if (
            path[path_length - 4] == '.' &&
            path[path_length - 3] == 't' &&
            path[path_length - 2] == 'g' &&
            path[path_length - 1] == 'a'
        ) LoadTGA (path, TextureFilterLinear, TextureWrapRepeat, InternalFormat32F);
    }

    void LoadBMP (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
        FILE* file = fopen (path, "rb");

        char bmp_header[52];
        fread (bmp_header, 52, 1, file);

        int width = *((int*)(&bmp_header[18]));
        int height = *((int*)(&bmp_header[22]));
        if (width == height) texture_size = width;
        else {
            fclose (file);
            return;
        }

        char pixels[width * height * 3];
        fread (pixels, width * height * 3, 1, file);

        fclose (file);

        if (texture_id) texture_copies[texture_id]--;
        if (texture_id && !texture_copies[texture_id]) glDeleteTextures (1, &texture_id);
        glGenTextures (1, &texture_id);
        glBindTexture (GL_TEXTURE_2D, texture_id);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        texture_alpha = false;
        glTexImage2D (GL_TEXTURE_2D, 0, InternalFormat32F ? GL_RGB32F : GL_RGB, texture_size, texture_size, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);

        glBindTexture (GL_TEXTURE_2D, 0);
        texture_copies[texture_id]++;
    }

    void LoadTGA (const char* path, bool TextureFilterLinear = true, bool TextureWrapRepeat = true, bool InternalFormat32F = false) {
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
        if (ImageWidth == ImageHeight) texture_size = ImageWidth;
        else {
            fclose (file);
            return;
        }

        unsigned char BitCount;
        fread (&BitCount, sizeof (unsigned char), 1, file);

        fread (&ucharBad, sizeof (unsigned char), 1, file);

        unsigned char ColorMode = BitCount / 8;
        if (ColorMode == 4) texture_alpha = true;
        else texture_alpha = false;
        long ImageSize = ImageWidth * ImageHeight * ColorMode;
        unsigned char ImageData[ImageSize];
        fread (ImageData, sizeof (unsigned char), ImageSize, file);

        fclose (file);

        if (texture_id) texture_copies[texture_id]--;
        if (texture_id && !texture_copies[texture_id]) glDeleteTextures (1, &texture_id);
        glGenTextures (1, &texture_id);
        glBindTexture (GL_TEXTURE_2D, texture_id);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterLinear ? GL_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterLinear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapRepeat ? GL_REPEAT : GL_CLAMP);

        glTexImage2D (GL_TEXTURE_2D, 0, texture_alpha ? (InternalFormat32F ? GL_RGBA32F : GL_RGBA) : (InternalFormat32F ? GL_RGB32F : GL_RGB), texture_size, texture_size, 0, texture_alpha ? GL_BGRA : GL_BGR, GL_UNSIGNED_BYTE, ImageData);

        glBindTexture (GL_TEXTURE_2D, 0);
        texture_copies[texture_id]++;
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

        glBindTexture (GL_TEXTURE_2D, texture_id);
        glGetTexImage (GL_TEXTURE_2D, 0, texture_alpha ? GL_RGBA : GL_RGB, data_type, data);
        glBindTexture (GL_TEXTURE_2D, 0);
    }

    GLuint getID (void) { return texture_id; }
    GLsizei getSize (void) { return texture_size; }
    bool hasAlpha (void) { return texture_alpha; }

    void bind (void) { glBindTexture (GL_TEXTURE_2D, texture_id); }

private:
    GLuint texture_id;
    GLsizei texture_size;
    bool texture_alpha;
    static std::map <GLuint, unsigned int> texture_copies;
};

std::map <GLuint, unsigned int> texture::texture_copies;

}

#endif//GLGE_TEXTURE_HPP
