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

    void bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID[0]);
        glBindBuffer(GL_ARRAY_BUFFER, ID[1]);

        glEnableVertexAttribArray(aCoord);
        glEnableVertexAttribArray(aTexCoord);
        glEnableVertexAttribArray(aColor);

        Vertex *v = NULL;

        glVertexAttribPointer(aCoord, 2, GL_FLOAT, false, sizeof(Vertex), &v->x);
        glVertexAttribPointer(aTexCoord, 2, GL_FLOAT, false, sizeof(Vertex), &v->s);
        glVertexAttribPointer(aColor, 3, GL_FLOAT, false, sizeof(Vertex), &v->r);
    }

    void draw() {
        glDrawElements(GL_TRIANGLES, iCount, GL_UNSIGNED_SHORT, NULL);
    }
};

#endif