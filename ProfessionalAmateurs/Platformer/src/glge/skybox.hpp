#ifndef GLGE_SKYBOX_HPP
#define GLGE_SKYBOX_HPP

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
#include "texture.hpp"
#include "program.hpp"
#include "camera.hpp"

namespace glge {

class skybox {
public:
    skybox (const skybox& SkyboxIn) {
        this->operator() ((skybox&)(SkyboxIn));
    }

    skybox (texture& TextureBack, texture& TextureFront, texture& TextureBottom, texture& TextureTop, texture& TextureLeft, texture& TextureRight) {
        init ();
        this->operator() (TextureBack, TextureFront, TextureBottom, TextureTop, TextureLeft, TextureRight);
    }

    skybox (void) {
        init ();
    }

    ~skybox (void) {  }

public:
    void operator() (skybox& SkyboxIn) {
        SkyboxTextureBack (SkyboxIn.SkyboxTextureBack);
        SkyboxTextureFront (SkyboxIn.SkyboxTextureFront);
        SkyboxTextureBottom (SkyboxIn.SkyboxTextureBottom);
        SkyboxTextureTop (SkyboxIn.SkyboxTextureTop);
        SkyboxTextureLeft (SkyboxIn.SkyboxTextureLeft);
        SkyboxTextureRight (SkyboxIn.SkyboxTextureRight);
    }

    void operator() (texture& TextureBack, texture& TextureFront, texture& TextureBottom, texture& TextureTop, texture& TextureLeft, texture& TextureRight) {
        SkyboxTextureBack (TextureBack);
        SkyboxTextureFront (TextureFront);
        SkyboxTextureBottom (TextureBottom);
        SkyboxTextureTop (TextureTop);
        SkyboxTextureLeft (TextureLeft);
        SkyboxTextureRight (TextureRight);
    }

public:
    void draw (camera& Camera) {
        glDisable (GL_DEPTH_TEST);
        glUseProgram (SkyboxProgram.getID ());
        glm::mat4 SkyboxMatVP = Camera.getProjection () * Camera.getRotate ();
        SkyboxProgram.setUniformMat (SkyboxMatrixVP, &SkyboxMatVP[0][0], 4);

        glEnableVertexAttribArray (SkyboxVertexXYZ);
        glEnableVertexAttribArray (SkyboxVertexUV);
        glBindBuffer (GL_ARRAY_BUFFER, SkyboxBuffer.getID ());

        glVertexAttribPointer (SkyboxVertexXYZ, 3, GL_FLOAT, GL_FALSE, 0 * sizeof (GLfloat), (GLvoid*)(0));
        glVertexAttribPointer (SkyboxVertexUV, 2, GL_FLOAT, GL_FALSE, 0 * sizeof (GLfloat), (GLvoid*)(288));

        glBindTexture (GL_TEXTURE_2D, SkyboxTextureBack.getID ());
        glDrawArrays (GL_QUADS, 0,   4);
        glBindTexture (GL_TEXTURE_2D, SkyboxTextureFront.getID ());
        glDrawArrays (GL_QUADS, 4,   4);
        glBindTexture (GL_TEXTURE_2D, SkyboxTextureBottom.getID ());
        glDrawArrays (GL_QUADS, 8,   4);
        glBindTexture (GL_TEXTURE_2D, SkyboxTextureTop.getID ());
        glDrawArrays (GL_QUADS, 12,  4);
        glBindTexture (GL_TEXTURE_2D, SkyboxTextureLeft.getID ());
        glDrawArrays (GL_QUADS, 16,  4);
        glBindTexture (GL_TEXTURE_2D, SkyboxTextureRight.getID ());
        glDrawArrays (GL_QUADS, 20,  4);

        glBindBuffer (GL_ARRAY_BUFFER, 0);
        glDisableVertexAttribArray (SkyboxVertexUV);
        glDisableVertexAttribArray (SkyboxVertexXYZ);

        glBindTexture (GL_TEXTURE_2D, 0);
        glUseProgram (0);
        glEnable (GL_DEPTH_TEST);
    }

public:
    texture SkyboxTextureBack;
    texture SkyboxTextureFront;
    texture SkyboxTextureBottom;
    texture SkyboxTextureTop;
    texture SkyboxTextureLeft;
    texture SkyboxTextureRight;
private:
    static buffer SkyboxBuffer;
    static program SkyboxProgram;
    static GLint SkyboxMatrixVP;
    static GLint SkyboxVertexXYZ;
    static GLint SkyboxVertexUV;
    static void init (void) {
        static bool SkyboxInitDone = false;
        if (SkyboxInitDone) return;

        GLfloat SkyboxBufferData[] = {
            -1,-1,-1,  1,-1,-1,  1, 1,-1, -1, 1,-1,
             1,-1, 1, -1,-1, 1, -1, 1, 1,  1, 1, 1,
            -1,-1, 1,  1,-1, 1,  1,-1,-1, -1,-1,-1,
            -1, 1,-1,  1, 1,-1,  1, 1, 1, -1, 1, 1,
            -1,-1, 1, -1,-1,-1, -1, 1,-1, -1, 1, 1,
             1,-1,-1,  1,-1, 1,  1, 1, 1,  1, 1,-1,

             0.001,0.001, 0.999,0.001, 0.999,0.999, 0.001,0.999,
             0.001,0.001, 0.999,0.001, 0.999,0.999, 0.001,0.999,
             0.001,0.001, 0.999,0.001, 0.999,0.999, 0.001,0.999,
             0.001,0.001, 0.999,0.001, 0.999,0.999, 0.001,0.999,
             0.001,0.001, 0.999,0.001, 0.999,0.999, 0.001,0.999,
             0.001,0.001, 0.999,0.001, 0.999,0.999, 0.001,0.999,
        };
        SkyboxBuffer (SkyboxBufferData, sizeof (SkyboxBufferData));

        SkyboxProgram (
            "#version 330 core" "\n"
            "uniform mat4 MatrixVP;" "\n"
            "in vec3 VertexXYZ;" "\n"
            "in vec2 VertexUV;" "\n"
            "out vec2 FragmentUV;" "\n"
            "void main (void) {" "\n"
            "    gl_Position = MatrixVP * vec4 (VertexXYZ, 1.0);" "\n"
            "    FragmentUV = VertexUV;" "\n"
            "}" "\n"
            ,
            "#version 330 core" "\n"
            "uniform sampler2D TextureSampler;" "\n"
            "in vec2 FragmentUV;" "\n"
            "out vec4 FragmentColor;" "\n"
            "void main (void) {" "\n"
            "    FragmentColor = vec4 (texture (TextureSampler, FragmentUV).rgb, 1.0);" "\n"
            "}" "\n"
        );
        if (SkyboxProgram.getErrors () != "") {
            std::ofstream SkyboxProgram_error_file ("SkyboxProgram_error_file.txt");
            SkyboxProgram_error_file << SkyboxProgram.getErrors ();
            SkyboxProgram_error_file.close ();
        }

        SkyboxMatrixVP = SkyboxProgram.getUniformLocation ("MatrixVP");
        SkyboxVertexXYZ = SkyboxProgram.getAttribLocation ("VertexXYZ");
        SkyboxVertexUV = SkyboxProgram.getAttribLocation ("VertexUV");

        SkyboxInitDone = true;
    }
};

buffer skybox::SkyboxBuffer;
program skybox::SkyboxProgram;
GLint skybox::SkyboxMatrixVP;
GLint skybox::SkyboxVertexXYZ;
GLint skybox::SkyboxVertexUV;

}

#endif//GLGE_SKYBOX_HPP
