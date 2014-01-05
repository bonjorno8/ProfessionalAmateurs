#ifndef GLGE_PROGRAM_HPP
#define GLGE_PROGRAM_HPP

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

class program {
public:
    program (const program& program_in) {
        this->operator() ((program&)(program_in));
    }

    program (const char* VertexShaderSource, const char* FragmentShaderSource) {
        this->operator() (VertexShaderSource, FragmentShaderSource);
    }

    program (void) {
        program_id = 0;
        program_errors = "";
    }

    ~program (void) {
        if (program_id && program_copies[program_id]) program_copies[program_id]--;
        if (program_id && !program_copies[program_id]) glDeleteProgram (program_id);
        program_id = 0;
        program_errors = "";
    }

public:
    void operator() (program& program_in) {
        if (program_id && program_in.program_id != program_id) program_copies[program_id]--;
        if (program_id && !program_copies[program_id]) glDeleteProgram (program_id);
        program_id = program_in.program_id;
        program_errors = program_in.program_errors;
        program_copies[program_id]++;
    }

    void operator() (const char* VertexShaderSource, const char* FragmentShaderSource) {
        program_errors = "";
        GLint ShaderResult = GL_FALSE;
        int InfoLogLength = 0;

        GLuint VertexShaderID = glCreateShader (GL_VERTEX_SHADER);
        glShaderSource (VertexShaderID, 1, &VertexShaderSource, NULL);
        glCompileShader (VertexShaderID);
        glGetShaderiv (VertexShaderID, GL_COMPILE_STATUS, &ShaderResult);
        glGetShaderiv (VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::string VertexShaderError (InfoLogLength, 0);
        glGetShaderInfoLog (VertexShaderID, InfoLogLength, NULL, &VertexShaderError[0]);
        if (InfoLogLength > 1) {
            program_errors += "<VertexShaderError/>\n";
            program_errors += VertexShaderError.substr (0, InfoLogLength-1);
            program_errors += "</VertexShaderError>\n";
        }

        GLuint FragmentShaderID = glCreateShader (GL_FRAGMENT_SHADER);
        glShaderSource (FragmentShaderID, 1, &FragmentShaderSource, NULL);
        glCompileShader (FragmentShaderID);
        glGetShaderiv (FragmentShaderID, GL_COMPILE_STATUS, &ShaderResult);
        glGetShaderiv (FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::string FragmentShaderError (InfoLogLength, 0);
        glGetShaderInfoLog (FragmentShaderID, InfoLogLength, NULL, &FragmentShaderError[0]);
        if (InfoLogLength > 1) {
            program_errors += "<FragmentShaderError/>\n";
            program_errors += FragmentShaderError.substr (0, InfoLogLength-1);
            program_errors += "</FragmentShaderError>\n";
        }

        if (program_id && program_copies[program_id]) program_copies[program_id]--;
        if (program_id && !program_copies[program_id]) glDeleteProgram (program_id);
        program_id = glCreateProgram ();
        glAttachShader (program_id, VertexShaderID);
        glAttachShader (program_id, FragmentShaderID);
        glLinkProgram (program_id);

        glDeleteShader (VertexShaderID);
        glDeleteShader (FragmentShaderID);

        program_copies[program_id]++;
    }

public:
    GLuint getID (void) {
        return program_id;
    }

    std::string getErrors (void) {
        return program_errors;
    }

public:
    GLint getUniformLocation (const char* name) {
        return glGetUniformLocation (program_id, name);
    }

    GLint getAttribLocation (const char* name) {
        return glGetAttribLocation (program_id, name);
    }

    template <typename T>
    void setUniform (GLint location, T v0) {
        if (typeid (T) == typeid (GLdouble)) glUniform1d (location, v0);
        if (typeid (T) == typeid (GLfloat)) glUniform1f (location, v0);
        if (typeid (T) == typeid (GLint)) glUniform1i (location, v0);
    }

    template <typename T>
    void setUniform (GLint location, T v0, T v1) {
        if (typeid (T) == typeid (GLdouble)) glUniform2d (location, v0, v1);
        if (typeid (T) == typeid (GLfloat)) glUniform2f (location, v0, v1);
        if (typeid (T) == typeid (GLint)) glUniform2i (location, v0, v1);
    }

    template <typename T>
    void setUniform (GLint location, T v0, T v1, T v2) {
        if (typeid (T) == typeid (GLdouble)) glUniform3d (location, v0, v1, v2);
        if (typeid (T) == typeid (GLfloat)) glUniform3f (location, v0, v1, v2);
        if (typeid (T) == typeid (GLint)) glUniform3i (location, v0, v1, v2);
    }

    template <typename T>
    void setUniform (GLint location, T v0, T v1, T v2, T v3) {
        if (typeid (T) == typeid (GLdouble)) glUniform4d (location, v0, v1, v2, v3);
        if (typeid (T) == typeid (GLfloat)) glUniform4f (location, v0, v1, v2, v3);
        if (typeid (T) == typeid (GLint)) glUniform4i (location, v0, v1, v2, v3);
    }

    template <typename T>
    void setUniformVec (GLint location, T* values, unsigned char value_count) {
        bool d = (typeid (T) == typeid (GLdouble));
        bool f = (typeid (T) == typeid (GLfloat));
        bool i = (typeid (T) == typeid (GLint));
        if (value_count == 1) {
            if (d) glUniform1dv (location, values);
            if (f) glUniform1fv (location, values);
            if (i) glUniform1iv (location, values);
        }
        if (value_count == 2) {
            if (d) glUniform2dv (location, values);
            if (f) glUniform2fv (location, values);
            if (i) glUniform2iv (location, values);
        }
        if (value_count == 3) {
            if (d) glUniform3dv (location, values);
            if (f) glUniform3fv (location, values);
            if (i) glUniform3iv (location, values);
        }
        if (value_count == 4) {
            if (d) glUniform4dv (location, values);
            if (f) glUniform4fv (location, values);
            if (i) glUniform4iv (location, values);
        }
    }

    void setUniformMat (GLint location, double* values, unsigned char mat_dim) {
        if (mat_dim == 2) glUniformMatrix2dv (location, 1, GL_FALSE, values);
        if (mat_dim == 3) glUniformMatrix3dv (location, 1, GL_FALSE, values);
        if (mat_dim == 4) glUniformMatrix4dv (location, 1, GL_FALSE, values);
    }

    void setUniformMat (GLint location, float* values, unsigned char mat_dim) {
        if (mat_dim == 2) glUniformMatrix2fv (location, 1, GL_FALSE, values);
        if (mat_dim == 3) glUniformMatrix3fv (location, 1, GL_FALSE, values);
        if (mat_dim == 4) glUniformMatrix4fv (location, 1, GL_FALSE, values);
    }

private:
    GLuint program_id;
    std::string program_errors;
    static std::map <GLuint, unsigned int> program_copies;

private:
    const std::string IF_I_DONT_HAVE_THIS_THE_PROGRAM_WILL_NOT_WORK;
};

std::map <GLuint, unsigned int> program::program_copies;

}

#endif//GLGE_PROGRAM_HPP
