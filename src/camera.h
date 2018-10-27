#ifndef H_CAMERA
#define H_CAMERA

#include "utils.h"
#include "input.h"

struct Camera {
    vec3 pos;
    vec3 rot;

    mat4 mView, mProj, mViewProj;

    Camera() {
        pos = vec3(0.0f);
        rot = vec3(0.0f);
    }

    void update() {
        rot.x -= Input::mouseDelta.y * 0.001f;
        rot.y -= Input::mouseDelta.x * 0.001f;

        mView.identity();
        mView.rotateZ(-rot.z);
        mView.rotateX(-rot.x);
        mView.rotateY(-rot.y);
        
        vec3 dir   = vec3(mView.e20, mView.e21, mView.e22);
        vec3 right = vec3(mView.e00, mView.e01, mView.e02);

        float k = deltaTime * 1.0f;

        if (Input::down['A']) pos = pos - right * k;
        if (Input::down['D']) pos = pos + right * k;
        if (Input::down['W']) pos = pos - dir * k;
        if (Input::down['S']) pos = pos + dir * k;
        
        mView.translate(-pos);

        mProj.perspective(90.0f, (float)Context::width / (float)Context::height, 0.01f, 100.0f);

        mViewProj = mProj * mView;
    }
};

#endif