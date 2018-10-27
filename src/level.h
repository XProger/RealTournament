#ifndef H_LEVEL
#define H_LEVEL

#include "utils.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"
#include "camera.h"

struct Level {

    struct Object {
        mat4    matrix;
        Texture *diffuse;
        Texture *lightmap;
        Mesh    *mesh;

        int32   iCount;
        int32   vCount;
        Index   *indices;
        Vertex  *vertices;

        Object(Stream &stream) {
            diffuse = lightmap = NULL;

            stream.read(&matrix, sizeof(matrix));
            char *tex1 = stream.readStr();
            char *tex2 = stream.readStr();

            if (tex1) diffuse  = new Texture(tex1);
            if (tex2) lightmap = new Texture(tex2);
        
            delete[] tex1;
            delete[] tex2;

            stream.read(&iCount, sizeof(iCount));
            indices = new Index[iCount];
            stream.read(indices, iCount * sizeof(Index));

            stream.read(&vCount, sizeof(vCount));
            vertices = new Vertex[vCount];
            stream.read(vertices, vCount * sizeof(Vertex));

            mesh = new Mesh(indices, iCount, vertices, vCount);
        }

        ~Object() {
            delete[] indices;
            delete[] vertices;
            delete diffuse;
            delete lightmap;
            delete mesh;
        }
    } *objects[32];
    int objectsCount;

    Shader *shader;

    float gravity;

    Level(const char *name) : objectsCount(0), gravity(2.0f * 9.81f) {
        Stream stream(name);

        while (stream.pos < stream.size)
            Object *obj = (objects[objectsCount++] = new Object(stream));

        shader = new Shader("shader.glsl");
    }

    ~Level() {
        delete shader;
        for (int i = 0; i < objectsCount; i++)
            delete objects[i];
    }

    void update() {
        //
    }

    void render(Camera *camera) {
        glEnable(GL_DEPTH_TEST);

        shader->bind();
        shader->setParam(uViewProj, camera->mViewProj);
        shader->setParam(uColor, vec4(1.0f));

        for (int i = 0; i < objectsCount; i++) {
            Object *obj = objects[i];

            shader->setParam(uModel, obj->matrix);

            obj->diffuse->bind(sDiffuse);
            obj->lightmap->bind(sLightmap);

            obj->mesh->bind();
            obj->mesh->draw();
        }
    }

};

#endif