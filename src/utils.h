#ifndef H_UTILS
#define H_UTILS

#include <stdio.h>
#include <math.h>

#define LOG(...) printf(__VA_ARGS__)

typedef unsigned char  uint8;
typedef signed   char  int8;
typedef unsigned short uint16;
typedef signed   short int16;
typedef unsigned int   uint32;
typedef signed   int   int32;

extern float deltaTime;

#define PI      3.14159265358979323846f
#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)


struct vec3 {
    float x, y, z;

    vec3() {}
    vec3(float v) : x(v), y(v), z(v) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    vec3 operator - () const { return vec3(-x, -y, -z); }
    vec3 operator + (const vec3 &v) const { return vec3(x + v.x, y + v.y, z + v.z); }
    vec3 operator - (const vec3 &v) const { return vec3(x - v.x, y - v.y, z - v.z); }
    vec3 operator * (const float s) const { return vec3(x * s, y * s, z * s); }
};

struct vec4 {
    float x, y, z, w;

    vec4() {}
    vec4(float v) : x(v), y(v), z(v), w(v) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct mat4 {

    float e00, e10, e20, e30,
          e01, e11, e21, e31,
          e02, e12, e22, e32,
          e03, e13, e23, e33;

    void identity() {
        memset(this, 0, sizeof(*this));
        e00 = e11 = e22 = e33 = 1.0f;
    }

    mat4 operator * (const mat4 &m) const {
        mat4 r;
        r.e00 = e00 * m.e00 + e01 * m.e10 + e02 * m.e20 + e03 * m.e30;
        r.e10 = e10 * m.e00 + e11 * m.e10 + e12 * m.e20 + e13 * m.e30;
        r.e20 = e20 * m.e00 + e21 * m.e10 + e22 * m.e20 + e23 * m.e30;
        r.e30 = e30 * m.e00 + e31 * m.e10 + e32 * m.e20 + e33 * m.e30;
        r.e01 = e00 * m.e01 + e01 * m.e11 + e02 * m.e21 + e03 * m.e31;
        r.e11 = e10 * m.e01 + e11 * m.e11 + e12 * m.e21 + e13 * m.e31;
        r.e21 = e20 * m.e01 + e21 * m.e11 + e22 * m.e21 + e23 * m.e31;
        r.e31 = e30 * m.e01 + e31 * m.e11 + e32 * m.e21 + e33 * m.e31;
        r.e02 = e00 * m.e02 + e01 * m.e12 + e02 * m.e22 + e03 * m.e32;
        r.e12 = e10 * m.e02 + e11 * m.e12 + e12 * m.e22 + e13 * m.e32;
        r.e22 = e20 * m.e02 + e21 * m.e12 + e22 * m.e22 + e23 * m.e32;
        r.e32 = e30 * m.e02 + e31 * m.e12 + e32 * m.e22 + e33 * m.e32;
        r.e03 = e00 * m.e03 + e01 * m.e13 + e02 * m.e23 + e03 * m.e33;
        r.e13 = e10 * m.e03 + e11 * m.e13 + e12 * m.e23 + e13 * m.e33;
        r.e23 = e20 * m.e03 + e21 * m.e13 + e22 * m.e23 + e23 * m.e33;
        r.e33 = e30 * m.e03 + e31 * m.e13 + e32 * m.e23 + e33 * m.e33;
        return r;
    }

    void translate(const vec3 &offset) {
        mat4 m;
        m.identity();
        m.e03 = offset.x;
        m.e13 = offset.y;
        m.e23 = offset.z;
        *this = *this * m;
    }

    void rotateX(float angle) {
        mat4 m;
        m.identity();
        float s = sinf(angle), c = cosf(angle);
        m.e11 = c;  m.e21 = s;
        m.e12 = -s; m.e22 = c;
        *this = *this * m;
    }

    void rotateY(float angle) {
        mat4 m;
        m.identity();
        float s = sinf(angle), c = cosf(angle);
        m.e00 = c;  m.e20 = -s;
        m.e02 = s;  m.e22 = c;
        *this = *this * m;
    }

    void rotateZ(float angle) {
        mat4 m;
        m.identity();
        float s = sinf(angle), c = cosf(angle);
        m.e00 = c;  m.e01 = -s;
        m.e10 = s;  m.e11 = c;
        *this = *this * m;
    }

    void ortho(float l, float r, float b, float t, float znear, float zfar) {
        identity();
        e00 = 2.0f / (r - l);
        e11 = 2.0f / (t - b);
        e22 = 2.0f / (znear - zfar);
        e03 = (l + r) / (l - r);
        e13 = (t + b) / (b - t);
        e23 = (zfar + znear) / (znear - zfar);
    }

    void perspective(float fov, float aspect, float znear, float zfar) {
        identity();
        float k = 1.0f / tanf(fov * 0.5f * DEG2RAD);
        e00 = k / aspect;
        e11 = k;
        e33 = 0.0f;
        e32 = -1.0f;
        e22 = (znear + zfar) / (znear - zfar);
        e23 = 2.0f * zfar * znear / (znear - zfar);
    }
};

struct Stream {
    FILE *file;
    int  pos;
    int  size;

    Stream(const char *name) : file(NULL), pos(0), size(0) {
        file = fopen(name, "rb");
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);
    }

    ~Stream() {
        fclose(file);
    }

    void seek(int offset) {
        fseek(file, offset, SEEK_CUR);
        pos += offset;
    }

    int read(void *data, int count) {
        int res = fread(data, 1, count, file);
        pos += res;
        return res;
    }
};

#endif