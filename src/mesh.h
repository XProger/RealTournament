#ifndef H_MESH
#define H_MESH

#include "context.h"
#include "shader.h"

struct Mesh {
    GLuint ID[2];
    int iCount;
    int vCount;

    Mesh(Index *indices, int iCount, Vertex *vertices, int vCount) : iCount(iCount), vCount(vCount) {
        glGenBuffers(2, &ID[0]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, iCount * sizeof(Index), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, ID[1]);
        glBufferData(GL_ARRAY_BUFFER, vCount * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    }

    ~Mesh() {
        glDeleteBuffers(2, &ID[0]);
    }

    void update(Index *indices, int iCount, Vertex *vertices, int vCount) {
        if (indices != NULL) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID[0]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, iCount * sizeof(Index), indices);
        }
        this->iCount = iCount;

        if (vertices != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, ID[1]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vCount * sizeof(Vertex), vertices);
        }
        this->vCount = vCount;
    }


    void bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID[0]);
        glBindBuffer(GL_ARRAY_BUFFER, ID[1]);

        glEnableVertexAttribArray(aCoord);
        glEnableVertexAttribArray(aNormal);
        glEnableVertexAttribArray(aTexCoord);

        Vertex *v = NULL;

        glVertexAttribPointer(aCoord,    3, GL_FLOAT, false, sizeof(Vertex), &v->coord);
        glVertexAttribPointer(aNormal,   3, GL_FLOAT, false, sizeof(Vertex), &v->normal);
        glVertexAttribPointer(aTexCoord, 4, GL_FLOAT, false, sizeof(Vertex), &v->texCoord);
    }

    void draw() {
        glDrawElements(GL_TRIANGLES, iCount, GL_UNSIGNED_SHORT, NULL);
    }
};

#endif