#ifndef H_SHADER
#define H_SHADER

#include "context.h"

#define SHADER_SAMPLER(E)\
    E( sDiffuse )\
    E( sLightmap )


#define SHADER_ATTRIB(E)\
    E( aCoord )\
    E( aNormal )\
    E( aTexCoord )

#define SHADER_UNIFORM(E)\
    E( uViewProj )\
    E( uModel )\
    E( uColor )

#define DECL_ENUM(V)    V,
#define DECL_STR(V)     #V,

enum SamplerType {
    SHADER_SAMPLER(DECL_ENUM)
    sMAX
};

enum AttribType {
    SHADER_ATTRIB(DECL_ENUM)
    aMAX
};

enum UniformType {
    SHADER_UNIFORM(DECL_ENUM)
    uMAX
};

const char *SamplerName[sMAX] = { SHADER_SAMPLER(DECL_STR) };
const char *AttribName[aMAX]  = { SHADER_ATTRIB(DECL_STR)  };
const char *UniformName[uMAX] = { SHADER_UNIFORM(DECL_STR) };

struct Shader {
    GLuint ID;
    GLint  UID[uMAX];

    Shader(const char *name) {
        Stream stream(name);

        int length = stream.size;
        char *source = new char[length + 1];
        stream.read(source, length);
        source[length] = 0;

        ID = glCreateProgram();
        
        const int types[2] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
        const char *text[2][2] {
            { "#version 120\n#define VERTEX\n#line 0\n", source },
            { "#version 120\n#define FRAGMENT\n#line 0\n", source },
        };

        GLchar info[1024];

        for (int i = 0; i < 2; i++) {
            GLuint obj = glCreateShader(types[i]);
            glShaderSource(obj, 2, text[i], NULL);
            glCompileShader(obj);

            glGetShaderInfoLog(obj, sizeof(info), NULL, info);
        
            if (strlen(info) > 8)
                LOG("! shader: %s\n", info);

            glAttachShader(ID, obj);
            glDeleteShader(obj);
        }

        delete[] source;

        for (int i = 0; i < aMAX; i++)
            glBindAttribLocation(ID, i, AttribName[i]);

        glLinkProgram(ID);

        glGetProgramInfoLog(ID, sizeof(info), NULL, info);
        if (strlen(info) > 8)
            LOG("! program: %s\n", info);

        for (int i = 0; i < uMAX; i++)
            UID[i] = glGetUniformLocation(ID, UniformName[i]);

        glUseProgram(ID);
        for (int i = 0; i < sMAX; i++) {
            int sampler = glGetUniformLocation(ID, SamplerName[i]);
            if (sampler != -1) {
                glUniform1iv(sampler, 1, &i);
            }
        }
    }

    ~Shader() {
        glDeleteProgram(ID);
    }

    void bind() {
        glUseProgram(ID);
    }

    void setParam(UniformType type, const vec4 &value, int count = 1) {
        if (UID[type] == -1) return;
        glUniform4fv(UID[type], count,(GLfloat*)&value);
    }

    void setParam(UniformType type, const mat4 &value, int count = 1) {
        if (UID[type] == -1) return;
        glUniformMatrix4fv(UID[type], count, false, (GLfloat*)&value);
    }
};


#endif