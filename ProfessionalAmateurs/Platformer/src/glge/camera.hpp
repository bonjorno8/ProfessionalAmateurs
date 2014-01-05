#ifndef GLGE_CAMERA_HPP
#define GLGE_CAMERA_HPP

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

class camera {
public:
    camera (const camera& Camera) {
        this->operator= ((camera&)(Camera));
    }

    camera (void) {
        camera_pos = glm::mat4 (1.0f);
        camera_rot = glm::mat4 (1.0f);
        camera_view = glm::mat4 (1.0f);
        camera_fov = 45.0f;
        camera_aspect = 1.0f;
        camera_near = 0.01f;
        camera_far = 100.0f;
        camera_proj = glm::perspective (camera_fov, camera_aspect, camera_near, camera_far);
    }

    ~camera (void) {
        camera_pos = glm::mat4 (0.0f);
        camera_rot = glm::mat4 (0.0f);
        camera_view = glm::mat4 (0.0f);
        camera_fov = 0.0f;
        camera_aspect = 0.0f;
        camera_near = 0.0f;
        camera_far = 0.0f;
        camera_proj = glm::mat4 (0.0f);
    }

public:
    void operator= (camera& Camera) {
        camera_pos = Camera.camera_pos;
        camera_rot = Camera.camera_rot;
        camera_view = Camera.camera_view;
        camera_fov = Camera.camera_fov;
        camera_aspect = Camera.camera_aspect;
        camera_near = Camera.camera_near;
        camera_far = Camera.camera_far;
        camera_proj = Camera.camera_proj;
    }

    glm::vec3 getTranslation (void) { return glm::vec3 (-camera_pos * glm::vec4 ( 0.0f, 0.0f, 0.0f, 1.0f)); }
    glm::mat4 getTranslate (void) { return camera_pos; }
    void setTranslate (glm::mat4 pos) {
        camera_pos = pos;
        camera_view = camera_rot * camera_pos;
    }
    void setTranslate (float x, float y, float z) {
        camera_pos = glm::translate (-x, -y, -z);
        camera_view = camera_rot * camera_pos;
    }
    void translate (float x, float y, float z) {
        camera_pos = glm::translate (camera_pos, -x, -y, -z);
        camera_view = camera_rot * camera_pos;
    }

    glm::vec3 getRotation (void) { return glm::transpose (glm::mat3 (camera_rot)) * glm::vec3 ( 0.0f, 0.0f,-1.0f); }
    glm::mat4 getRotate (void) { return camera_rot; }
    void setRotate (glm::mat4 rot) {
        camera_rot = rot;
        camera_view = camera_rot * camera_pos;
    }
    void setRotate (float x, float y, float z) {
        camera_rot = glm::mat4 (1.0f);
        camera_rot = glm::rotate (camera_rot, -x, 1.0f, 0.0f, 0.0f);
        camera_rot = glm::rotate (camera_rot, -y, 0.0f, 1.0f, 0.0f);
        camera_rot = glm::rotate (camera_rot, -z, 0.0f, 0.0f, 1.0f);
        camera_view = camera_rot * camera_pos;
    }
    void rotate (float x, float y, float z) {
        camera_rot = glm::rotate (camera_rot, -x, 1.0f, 0.0f, 0.0f);
        camera_rot = glm::rotate (camera_rot, -y, 0.0f, 1.0f, 0.0f);
        camera_rot = glm::rotate (camera_rot, -z, 0.0f, 0.0f, 1.0f);
        camera_view = camera_rot * camera_pos;
    }

public:
    void move_x (float d) {
        camera_pos = glm::translate (camera_pos, glm::vec3 (camera_rot * glm::vec4 (-1.0f, 0.0f, 0.0f, 1.0f) * glm::vec4 ( d,-d,-d, 1.0f)));
        camera_view = camera_rot * camera_pos;
    }
    void move_y (float d) {
        camera_pos = glm::translate (camera_pos, glm::vec3 (camera_rot * glm::vec4 ( 0.0f,-1.0f, 0.0f, 1.0f) * glm::vec4 (-d, d,-d, 1.0f)));
        camera_view = camera_rot * camera_pos;
    }
    void move_z (float d) {
        camera_pos = glm::translate (camera_pos, glm::vec3 (camera_rot * glm::vec4 ( 0.0f, 0.0f,-1.0f, 1.0f) * glm::vec4 (-d,-d, d, 1.0f)));
        camera_view = camera_rot * camera_pos;
    }

    void turn_x (float d) {
        camera_rot = glm::rotate (camera_rot, d, glm::vec3 (glm::transpose (camera_rot) * glm::vec4 (-1.0f, 0.0f, 0.0f, 0.0f)));
        camera_view = camera_rot * camera_pos;
    }
    void turn_y (float d) {
        camera_rot = glm::rotate (camera_rot, d, glm::vec3 (glm::transpose (camera_rot) * glm::vec4 ( 0.0f,-1.0f, 0.0f, 0.0f)));
        camera_view = camera_rot * camera_pos;
    }
    void turn_z (float d) {
        camera_rot = glm::rotate (camera_rot, d, glm::vec3 (glm::transpose (camera_rot) * glm::vec4 ( 0.0f, 0.0f,-1.0f, 0.0f)));
        camera_view = camera_rot * camera_pos;
    }

/*public:
    void sceneAdd (object& Object) {

    }

    void sceneRem (object& Object) {

    }

    void sceneRender (void) {

    }*/

public:
    float getFov (void) { return camera_fov; }
    void setFov (float fov) {
        camera_fov = fov;
        camera_proj = glm::perspective (camera_fov, camera_aspect, camera_near, camera_far);
    }

    float getAspect (void) { return camera_aspect; }
    void setAspect (float aspect) {
        camera_aspect = aspect;
        camera_proj = glm::perspective (camera_fov, camera_aspect, camera_near, camera_far);
    }

    float getNear (void) { return camera_near; }
    void setNear (float near) {
        camera_near = near;
        camera_proj = glm::perspective (camera_fov, camera_aspect, camera_near, camera_far);
    }

    float getFar (void) { return camera_far; }
    void setFar (float far) {
        camera_far = far;
        camera_proj = glm::perspective (camera_fov, camera_aspect, camera_near, camera_far);
    }

public:
    glm::mat4 getView (void) { return camera_view; }
    glm::mat4 getProjection (void) { return camera_proj; }

private:
    glm::mat4 camera_pos, camera_rot;
    glm::mat4 camera_view;
    float camera_fov, camera_aspect, camera_near, camera_far;
    glm::mat4 camera_proj;
};

}

#endif//GLGE_CAMERA_HPP
