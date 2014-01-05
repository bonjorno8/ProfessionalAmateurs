#ifndef GLGE_OBJECT_HPP
#define GLGE_OBJECT_HPP

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif//GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstdio>
#include <fstream>

#include "buffer.hpp"
#include "texture.hpp"
#include "program.hpp"
#include "camera.hpp"

namespace glge {

class object {
public:
    object (const object& ObjectIn) {
        this->operator() ((object&)(ObjectIn));
    }

    object (buffer& VertexBuffer, buffer& UVBuffer, texture& Texture) {
        init ();
        this->operator() (VertexBuffer, UVBuffer, Texture);
        loadIdentity ();
    }

    object (buffer& VertexBuffer, buffer& UVBuffer) {
        init ();
        this->operator() (VertexBuffer, UVBuffer);
        loadIdentity ();
    }

    object (texture& Texture) {
        init ();
        this->operator() (Texture);
        loadIdentity ();
    }

    object (void) {
        init ();
        loadIdentity ();
    }

    ~object (void) {
        loadIdentity ();
    }

public:
    void operator() (object& ObjectIn) {
        ObjectVertexBuffer (ObjectIn.ObjectVertexBuffer);
        ObjectUVBuffer (ObjectIn.ObjectUVBuffer);
        ObjectTexture (ObjectIn.ObjectTexture);

        ObjectTranslate = ObjectIn.ObjectTranslate;
        ObjectRotate = ObjectIn.ObjectRotate;
        ObjectScale = ObjectIn.ObjectScale;
    }

    void operator() (buffer& VertexBuffer, buffer& UVBuffer, texture& Texture) {
        ObjectVertexBuffer (VertexBuffer);
        ObjectUVBuffer (UVBuffer);
        ObjectTexture (Texture);
    }

    void operator() (buffer& VertexBuffer, buffer& UVBuffer) {
        ObjectVertexBuffer (VertexBuffer);
        ObjectUVBuffer (UVBuffer);
    }

    void operator() (texture& Texture) {
        ObjectTexture (Texture);
    }

public:
    void translate (float x, float y, float z) { ObjectTranslate = glm::translate (ObjectTranslate, x, y, z); }
    void rotate (float x, float y, float z) {
        ObjectRotate = glm::rotate (ObjectRotate, x, 1.0f, 0.0f, 0.0f);
        ObjectRotate = glm::rotate (ObjectRotate, y, 0.0f, 1.0f, 0.0f);
        ObjectRotate = glm::rotate (ObjectRotate, z, 0.0f, 0.0f, 1.0f);
    }
    void scale (float x, float y, float z) { ObjectScale = glm::scale (ObjectScale, x, y, z); }
    void loadIdentity (void) {
        ObjectTranslate = glm::mat4 (1.0f);
        ObjectRotate = glm::mat4 (1.0f);
        ObjectScale = glm::mat4 (1.0f);
    }

    void setTranslate (float x, float y, float z) { ObjectTranslate = glm::translate (x, y, z); }
    void setRotate (float x, float y, float z) {
        ObjectRotate = glm::mat4 (1.0f);
        ObjectRotate = glm::rotate (ObjectRotate, x, 1.0f, 0.0f, 0.0f);
        ObjectRotate = glm::rotate (ObjectRotate, y, 0.0f, 1.0f, 0.0f);
        ObjectRotate = glm::rotate (ObjectRotate, z, 0.0f, 0.0f, 1.0f);
    }
    void setScale (float x, float y, float z) { ObjectScale = glm::scale (x, y, z); }

    glm::vec3 getTranslate (void) { return glm::vec3 (ObjectTranslate * glm::vec4 (0.0f, 0.0f, 0.0f, 1.0f)); }
    glm::vec3 getRotate (void) { return glm::vec3 (ObjectRotate * glm::vec4 (0.0f, 0.0f, 0.0f, 1.0f)); }
    glm::vec3 getScale (void) { return glm::vec3 (ObjectScale * glm::vec4 (0.0f, 0.0f, 0.0f, 1.0f)); }

public:
    buffer& getVertexBuffer (void) { return ObjectVertexBuffer; }
    buffer& getUVBuffer (void) { return ObjectUVBuffer; }
    texture& getTexture (void) { return ObjectTexture; }

    void setVertexBuffer (buffer& VertexBuffer) { ObjectVertexBuffer (VertexBuffer); }
    void setUVBuffer (buffer& UVBuffer) { ObjectUVBuffer (UVBuffer); }
    void setTexture (texture& Texture) { ObjectTexture (Texture); }

public:
    void draw (camera& Camera) {
        glm::mat4 ObjectMVP = Camera.getProjection () * Camera.getView () * (ObjectTranslate * ObjectRotate * ObjectScale);

        glUseProgram (ObjectProgram.getID ());
        ObjectProgram.setUniformMat (ObjectMatrixMVP, &ObjectMVP[0][0], 4);

        glActiveTexture (GL_TEXTURE0);
        ObjectTexture.bind ();
        glUniform1i (ObjectTextureSampler, 0);

        glEnableVertexAttribArray (ObjectVertexXYZ);
        glEnableVertexAttribArray (ObjectVertexUV);

        glBindBuffer (GL_ARRAY_BUFFER, ObjectVertexBuffer.getID ());
        glVertexAttribPointer (ObjectVertexXYZ, 3, GL_FLOAT, GL_FALSE, 0 * sizeof (GLfloat), (GLvoid*)(0));

        glBindBuffer (GL_ARRAY_BUFFER, ObjectUVBuffer.getID ());
        glVertexAttribPointer (ObjectVertexUV, 2, GL_FLOAT, GL_FALSE, 0 * sizeof (GLfloat), (GLvoid*)(0));

        glDrawArrays (GL_TRIANGLES, 0, ObjectVertexBuffer.getCount () / 3);
        glBindBuffer (GL_ARRAY_BUFFER, 0);

        glDisableVertexAttribArray (ObjectVertexUV);
        glDisableVertexAttribArray (ObjectVertexXYZ);

        glBindTexture (GL_TEXTURE_2D, 0);
        glUseProgram (0);
    }

private:
    buffer ObjectVertexBuffer;
    buffer ObjectUVBuffer;
    texture ObjectTexture;
    glm::mat4 ObjectTranslate, ObjectRotate, ObjectScale;
private:
    static program ObjectProgram;
    static GLint ObjectMatrixMVP;
    static GLint ObjectVertexXYZ, ObjectVertexUV;
    static GLint ObjectTextureSampler;

    static void init (void) {
        static bool ObjectInitDone = false;
        if (ObjectInitDone) return;

        ObjectProgram (
            "#version 330 core" "\n"
            "uniform mat4 MatrixMVP;" "\n"
            "in vec3 VertexXYZ;" "\n"
            "in vec2 VertexUV;" "\n"
            "out vec2 FragmentUV;" "\n"
            "void main (void) {" "\n"
            "    gl_Position = MatrixMVP * vec4 (VertexXYZ, 1.0);" "\n"
            "    FragmentUV = VertexUV;" "\n"
            "}" "\n"
            ,
            "#version 330 core" "\n"
            "uniform sampler2D TextureSampler;" "\n"
            "in vec2 FragmentUV;" "\n"
            "out vec4 FragmentColor;" "\n"
            "void main (void) {" "\n"
            "    FragmentColor = texture (TextureSampler, FragmentUV);" "\n"
            "}" "\n"
        );
        if (ObjectProgram.getErrors () != "") {
            std::ofstream ObjectProgram_errors ("ObjectProgram_errors.txt");
            ObjectProgram_errors << ObjectProgram.getErrors ();
            ObjectProgram_errors.close ();
        }

        ObjectMatrixMVP = glGetUniformLocation (ObjectProgram.getID (), "MatrixMVP");
        ObjectVertexXYZ = glGetAttribLocation (ObjectProgram.getID (), "VertexXYZ");
        ObjectVertexUV = glGetAttribLocation (ObjectProgram.getID (), "VertexUV");
        ObjectTextureSampler = ObjectProgram.getUniformLocation ("TextureSampler");

        ObjectInitDone = true;
    }
};

program object::ObjectProgram;
GLint object::ObjectMatrixMVP;
GLint object::ObjectVertexXYZ;
GLint object::ObjectVertexUV;
GLint object::ObjectTextureSampler;

}

#endif//GLGE_OBJECT_HPP
