#ifndef H_UI
#define H_UI

#include "utils.h"
#include "context.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"

#define UI_MAX_QUADS 1024

namespace UI {
    mat4 mViewProj;
    Texture *font;
    Shader  *shader;
    Mesh    *mesh;

    Index  *indices;
    Vertex *vertices;
    int    quadsCount;

    void init() {
        quadsCount = 0;

        font   = new Texture("font.tga");
        shader = new Shader("ui.glsl");

        indices  = new Index[UI_MAX_QUADS * 6];
        vertices = new Vertex[UI_MAX_QUADS * 4];

        for (int i = 0; i < UI_MAX_QUADS; i++) {
            indices[i * 6 + 0] = i * 4 + 0;
            indices[i * 6 + 1] = i * 4 + 1;
            indices[i * 6 + 2] = i * 4 + 2;
            indices[i * 6 + 3] = i * 4 + 0;
            indices[i * 6 + 4] = i * 4 + 2;
            indices[i * 6 + 5] = i * 4 + 3;
        }

        mesh = new Mesh(indices, UI_MAX_QUADS * 6, vertices, UI_MAX_QUADS * 4);
    }

    void deinit() {
        delete mesh;
        delete[] indices;
        delete[] vertices;
        delete shader;
        delete font;
    }

    void flush() {
        if (quadsCount == 0) return;

        shader->bind();
        shader->setParam(uViewProj, mViewProj);

        font->bind(sDiffuse);

        mesh->update(NULL, quadsCount * 6, vertices, quadsCount * 4);
        mesh->bind();
        mesh->draw();

        quadsCount = 0;
    }

    void begin(float aspect) {
        glDisable(GL_DEPTH_TEST);
        mViewProj.ortho(0, (float)Context::height * aspect, (float)Context::height, 0, 0, 1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void end() {
        flush();
    }

    void textOut(const char *str, float x, float y, float sx, float sy) {
        float ox = x;
        float oy = y;

        float s = 1.0f / 16.0f;
        float dx = 16.0f * sx;
        float dy = 16.0f * sy;

        for (int i = 0; i < (int)strlen(str); i++) {
            uint8 c = (uint8)str[i];
            if (c == '\t' || c == ' ') {
                ox += 16.0f;
                continue;
            }

            float u = (c % 16) * s;
            float v = (c / 16) * s;

            int q = quadsCount * 4;
            vertices[q + 0].coord = vec3(ox,      oy,      0);
            vertices[q + 1].coord = vec3(ox + dx, oy,      0);
            vertices[q + 2].coord = vec3(ox + dx, oy + dy, 0);
            vertices[q + 3].coord = vec3(ox,      oy + dy, 0);

            vertices[q + 0].texCoord = vec4(u,     v,     0, 0);
            vertices[q + 1].texCoord = vec4(u + s, v,     0, 0);
            vertices[q + 2].texCoord = vec4(u + s, v + s, 0, 0);
            vertices[q + 3].texCoord = vec4(u,     v + s, 0, 0);

            ox += 16.0f * sx;
            quadsCount++;
        }
    }

}

#endif