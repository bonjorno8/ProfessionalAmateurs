#ifndef GLGE_BUFFER_HPP
#define GLGE_BUFFER_HPP

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

class buffer {
public:
    buffer (const buffer& buffer_in) {
        this->operator() ((buffer&)(buffer_in));
    }

    template <typename T>
    buffer (T data[], GLsizei data_size) {
        this->operator() (data, data_size);
    }

    buffer (void) {
        buffer_id = 0;
        buffer_size = 0;
        buffer_count = 0;
    }

    ~buffer (void) {
        if (buffer_id && buffer_copies[buffer_id]) buffer_copies[buffer_id]--;
        if (buffer_id && !buffer_copies[buffer_id]) glDeleteBuffers (1, &buffer_id);
        buffer_id = 0;
        buffer_size = 0;
        buffer_count = 0;
    }

public:
    void operator() (buffer& buffer_in) {
        if (buffer_id && buffer_in.buffer_id != buffer_id) buffer_copies[buffer_id]--;
        if (buffer_id && !buffer_copies[buffer_id]) glDeleteBuffers (1, &buffer_id);
        buffer_id = buffer_in.buffer_id;
        buffer_size = buffer_in.buffer_size;
        buffer_count = buffer_in.buffer_count;
        buffer_copies[buffer_id]++;
    }

    template <typename T>
    void operator() (T data[], GLsizei data_size) {
        buffer_size = data_size;
        buffer_count = buffer_size / sizeof (T);
        if (buffer_id && buffer_copies[buffer_id]) buffer_copies[buffer_id]--;
        if (buffer_id && !buffer_copies[buffer_id]) glDeleteBuffers (1, &buffer_id);
        glGenBuffers (1, &buffer_id);
        glBindBuffer (GL_ARRAY_BUFFER, buffer_id);
        glBufferData (GL_ARRAY_BUFFER, buffer_size, data, GL_STATIC_DRAW);
        glBindBuffer (GL_ARRAY_BUFFER, 0);
        buffer_copies[buffer_id]++;
    }

public:
    template <typename T>
    void operator() (T data[]) {
        glBindBuffer (GL_ARRAY_BUFFER, buffer_id);
        glGetBufferSubData (GL_ARRAY_BUFFER, 0, buffer_size, data);
        glBindBuffer (GL_ARRAY_BUFFER, 0);
    }

    GLuint getID (void) {
        return buffer_id;
    }

    GLsizei getSize (void) {
        return buffer_size;
    }

    GLsizei getCount (void) {
        return buffer_size;
    }

private:
    GLuint buffer_id;
    GLsizei buffer_size;
    GLsizei buffer_count;
    static std::map <GLuint, unsigned int> buffer_copies;
};

std::map <GLuint, unsigned int> buffer::buffer_copies;

}

#endif//GLGE_BUFFER_HPP
