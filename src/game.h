#ifndef H_GAME
#define H_GAME

#include "input.h"
#include "texture.h"
#include "mesh.h"
#include "context.h"
#include "shader.h"
#include "camera.h"

namespace Game {
    Camera  *camera;
    Texture *texture;
    Shader  *shader;
    Mesh    *mesh;

    void init() {
        Context::init();
        texture = new Texture("bricks.tga");
        shader  = new Shader("shader.glsl");

        Index indices[] = { 0, 1, 2, 0, 2, 3 };

        Vertex vertices[] = {
            { 1, 0, 0,  0, 0,  -0.5f, -0.5f },
            { 0, 1, 0,  1, 0,   0.5f, -0.5f },
            { 0, 0, 1,  1, 1,   0.5f,  0.5f },
            { 1, 0, 1,  0, 1,  -0.5f,  0.5f },
        };

        mesh = new Mesh(indices, sizeof(indices) / sizeof(indices[0]), vertices, sizeof(vertices) / sizeof(vertices[0]));

        camera = new Camera();
    }

    void deinit() {
        delete camera;
        delete mesh;
        delete texture;
        Context::deinit();
    }

    void update() {
        camera->update();
    }

    void render() {
        glViewport(0, 0, Context::width, Context::height);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader->bind();
        shader->setParam(uViewProj, camera->mViewProj);
        shader->setParam(uColor, vec4(1.0f));

        mesh->bind();
        texture->bind(0);

        mesh->draw();
    }
}

#endif