#ifndef GLGE_TEXTURE_RECTANGLE_TEST_HPP
#define GLGE_TEXTURE_RECTANGLE_TEST_HPP

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

#include "buffer.hpp"
#include "program.hpp"
#include "camera.hpp"

namespace glge {

class texrect_test {
public:
    texrect_test (void) { init (); }
    ~texrect_test (void) {  }

public:
    void draw (camera& Camera) {
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram (TexrectProgram.getID ());
        glBindTexture (GL_TEXTURE_RECTANGLE_ARB, TexrectTextureID);

        TexrectProgram.setUniformMat (TexrectMatrixVP, &(Camera.getProjection () * Camera.getView ())[0][0], 4);

        glEnableVertexAttribArray (TexrectVertexXY);
        glEnableVertexAttribArray (TexrectVertexUV);
        glBindBuffer (GL_ARRAY_BUFFER, TexrectBuffer.getID ());

        glVertexAttribPointer (TexrectVertexXY, 2, GL_FLOAT, GL_FALSE, 0 * sizeof (GLfloat), (GLvoid*)( 0));
        glVertexAttribPointer (TexrectVertexUV, 2, GL_FLOAT, GL_FALSE, 0 * sizeof (GLfloat), (GLvoid*)(32));
        glDrawArrays (GL_QUADS, 0, 4);

        glBindBuffer (GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray (TexrectVertexUV);
        glDisableVertexAttribArray (TexrectVertexXY);

        glBindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);
        glUseProgram (0);

        glDisable (GL_BLEND);
    }

public:
    static GLuint TexrectTextureID;
private:
    static buffer TexrectBuffer;
    static program TexrectProgram;
    static GLint TexrectMatrixVP;
    static GLint TexrectVertexXY;
    static GLint TexrectVertexUV;
    static void init (void) {
        static bool TexrectInitDone = false;
        if (TexrectInitDone) return;

        GLfloat TexrectBufferData[] = {
            -1.5f,-1.0f,
             1.5f,-1.0f,
             1.5f, 1.0f,
            -1.5f, 1.0f,

             0, 0,
             6, 0,
             6, 4,
             0, 4,
        };
        TexrectBuffer (TexrectBufferData, sizeof (TexrectBufferData));

        TexrectProgram (
            "#version 330 core" "\n"
            "uniform mat4 MatrixVP;" "\n"
            "in vec2 VertexXY;" "\n"
            "in vec2 VertexUV;" "\n"
            "out vec2 FragmentUV;" "\n"
            "void main (void) {" "\n"
            "    gl_Position = MatrixVP * vec4 (VertexXY, 8.0, 1.0);" "\n"
            "    FragmentUV = VertexUV;" "\n"
            "}" "\n"
            ,
            "#version 330 core" "\n"
            "#extension GL_ARB_texture_rectangle : enable" "\n"
            "uniform sampler2DRect TextureSampler;" "\n"
            "in vec2 FragmentUV;" "\n"
            "out vec4 Color;" "\n"
            "void main (void) {" "\n"
            "    Color = texture2DRect (TextureSampler, FragmentUV);" "\n"
            "}" "\n"
        );
        if (TexrectProgram.getErrors () != "") {
            std::ofstream TexrectProgram_error_file ("TexrectProgram_error_file.txt");
            TexrectProgram_error_file << TexrectProgram.getErrors ();
            TexrectProgram_error_file.close ();
        }

        TexrectMatrixVP = TexrectProgram.getUniformLocation("MatrixVP");
        TexrectVertexXY = TexrectProgram.getAttribLocation ("VertexXY");
        TexrectVertexUV = TexrectProgram.getAttribLocation ("VertexUV");


        glGenTextures (1, &TexrectTextureID);
        glBindTexture (GL_TEXTURE_RECTANGLE_ARB, TexrectTextureID);

        glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAX_LEVEL, 0);

        glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri (GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);

        GLfloat TexrectTextureData[] = {
            1,1,1,0, 1,0,1,1, 0,1,0,1, 0,1,0,1, 1,0,1,1, 1,1,1,0,
            1,0,0,1, 0,1,0,1, 0,0,1,0, 0,0,1,0, 0,1,0,1, 1,0,0,1,
            1,0,0,1, 0,1,0,1, 0,0,1,0, 0,0,1,0, 0,1,0,1, 1,0,0,1,
            1,1,1,0, 1,0,1,1, 0,1,0,1, 0,1,0,1, 1,0,1,1, 1,1,1,0,
        };

        glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, 6, 4, 0, GL_RGBA, GL_FLOAT, TexrectTextureData);
        glBindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);


        TexrectInitDone = true;
    }
};

GLuint texrect_test::TexrectTextureID;
buffer texrect_test::TexrectBuffer;
program texrect_test::TexrectProgram;
GLint texrect_test::TexrectMatrixVP;
GLint texrect_test::TexrectVertexXY;
GLint texrect_test::TexrectVertexUV;

}

#endif//GLGE_TEXTURE_RECTANGLE_TEST_HPP
