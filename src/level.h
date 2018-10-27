#ifndef H_LEVEL
#define H_LEVEL

#include "camera.h"

struct BasePlayer {
    virtual ~BasePlayer() {}
    virtual void hit(int damage) {}
    virtual void update() {}
    virtual void render(Camera *camera) {}
    virtual void trace(const vec3 &rayPos, const vec3 &rayDir, float &t) {}
};

#include "utils.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"

#define LEVEL_MAX_RESPAWNS 8

struct Level {

    struct Object {
        enum Type : uint32 {
            MESH,
            LAMP,
            RESPAWN,
        } type;

        mat4    matrix;
        Texture *diffuse;
        Texture *lightmap;
        Mesh    *mesh;

        int32   iCount;
        int32   vCount;
        Index   *indices;
        Vertex  *vertices;

        Object(Stream &stream) : diffuse(NULL), lightmap(NULL), indices(NULL), vertices(NULL), mesh(NULL) {
            stream.read(&type, sizeof(type));
            stream.read(&matrix, sizeof(matrix));

            if (type != MESH) return;

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

    Object* getRespawn() {
        Object *respawns[LEVEL_MAX_RESPAWNS];
        int count = 0;

        for (int i = 0; i < objectsCount; i++) {
            if (objects[i]->type == Object::RESPAWN) {
                respawns[count++] = objects[i];
                if (count >= LEVEL_MAX_RESPAWNS)
                    break;
            }
        }

        if (count == 0)
            return NULL;

        return respawns[rand() % count];
    }

    void render(Camera *camera) {
        shader->bind();
        shader->setParam(uViewProj, camera->mViewProj);
        shader->setParam(uColor, vec4(1.0f));

        for (int i = 0; i < objectsCount; i++) {
            Object *obj = objects[i];

            if (obj->type != Object::MESH)
                continue;

            shader->setParam(uModel, obj->matrix);

            obj->diffuse->bind(sDiffuse);
            obj->lightmap->bind(sLightmap);

            obj->mesh->bind();
            obj->mesh->draw();
        }
/*
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf((GLfloat*)&camera->mViewProj);
        glUseProgram(0);
        glPointSize(4);
        glBegin(GL_POINTS);
        for (int i = 0; i < 64; i++)
            glVertex3fv((GLfloat*)&points[i]);
        glEnd();
*/
    }

    void trace(const vec3 &rayPos, const vec3 &rayDir, float &t) {
        for (int j = 0; j < objectsCount; j++) {
            Object *obj = objects[j];
            if (obj->type != Level::Object::MESH)
                continue;

            mat4 mInv = obj->matrix.inverseOrtho();
            vec3 rayPosLocal = mInv * vec4(rayPos, 1.0);
            vec3 rayDirLocal = mInv * vec4(rayDir, 0.0);

            for (int i = 0; i < obj->iCount; i += 3) {
                vec3 &a = obj->vertices[obj->indices[i + 0]].coord;
                vec3 &b = obj->vertices[obj->indices[i + 1]].coord;
                vec3 &c = obj->vertices[obj->indices[i + 2]].coord;

                if ((a.x == b.x && a.y == b.y && a.z == b.z) ||
                    (c.x == b.x && c.y == b.y && c.z == b.z) ||
                    (a.x == c.x && a.y == c.y && a.z == c.z)) {
                    continue;
                }

                float u, v;
                float t0;
                if (Triangle(a, b, c).intersect(rayPosLocal, rayDirLocal, false, u, v, t0) && t0 < t)
                    t = t0;
            }
        }
    }

};

#endif