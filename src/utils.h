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

#define EPS     0.00000f
#define PI      3.14159265358979323846f
#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

float clamp(float x, float a, float b) {
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

struct vec3 {
    float x, y, z;

    vec3() {}
    vec3(float v) : x(v), y(v), z(v) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    vec3 operator - () const { return vec3(-x, -y, -z); }
    vec3 operator + (const vec3 &v) const { return vec3(x + v.x, y + v.y, z + v.z); }
    vec3 operator - (const vec3 &v) const { return vec3(x - v.x, y - v.y, z - v.z); }
    vec3 operator * (const float s) const { return vec3(x * s, y * s, z * s); }

    vec3 cross(const vec3 &v) const {
        return vec3(y * v.z - z * v.y,
                    z * v.x - x * v.z,
                    x * v.y - y * v.x);
    }

    float dot(const vec3 &v) const { return x * v.x + y * v.y + z * v.z; }
    float length2() const { return this->dot(*this); };
    float length() const { return sqrtf(length2()); }
    vec3  normal() const { return *this * (1.0f / length()); }
};

struct vec4 {
    float x, y, z, w;

    vec4() {}
    vec4(const vec3 &xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
    vec4(float v) : x(v), y(v), z(v), w(v) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    operator vec3() const {
        return vec3(x, y, z);
    }
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

    vec4 operator * (const vec4 &v) const {
        return vec4(
            e00 * v.x + e01 * v.y + e02 * v.z + e03 * v.w,
            e10 * v.x + e11 * v.y + e12 * v.z + e13 * v.w,
            e20 * v.x + e21 * v.y + e22 * v.z + e23 * v.w,
            e30 * v.x + e31 * v.y + e32 * v.z + e33 * v.w);
    }

    mat4 inverseOrtho() const {
        mat4 r;
        r.e00 =  e00; r.e10 = e01; r.e20 = e02; r.e30 = 0;
        r.e01 =  e10; r.e11 = e11; r.e21 = e12; r.e31 = 0;
        r.e02 =  e20; r.e12 = e21; r.e22 = e22; r.e32 = 0;
        r.e03 = -(e03 * e00 + e13 * e10 + e23 * e20); // -dot(pos, right)
        r.e13 = -(e03 * e01 + e13 * e11 + e23 * e21); // -dot(pos, up)
        r.e23 = -(e03 * e02 + e13 * e12 + e23 * e22); // -dot(pos, dir)
        r.e33 = 1;
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


struct Triangle {
    vec3 a, b, c;

    Triangle(const vec3 &a, const vec3 &b, const vec3 &c) : a(a), b(b), c(c) {}

    vec3 closestPoint(const vec3 &p) const {
        vec3 ab = b - a;
        vec3 ac = c - a;
        vec3 ap = p - a;
        float d1 = ab.dot(ap);
        float d2 = ac.dot(ap);
        if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)

        // Check if P in vertex region outside B
        vec3 bp = p - b;
        float d3 = ab.dot(bp);
        float d4 = ac.dot(bp);
        if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)

        float vc = d1*d4 - d3*d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            float v = d1 / (d1 - d3);
            return a + ab * v; // barycentric coordinates (1-v,v,0)
        }

        vec3 cp = p - c;
        float d5 = ab.dot(cp);
        float d6 = ac.dot(cp);
        if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)

        float vb = d5*d2 - d1*d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            float w = d2 / (d2 - d6);
            return a + ac * w; // barycentric coordinates (1-w,0,w)
        }

        float va = d3*d6 - d5*d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + (c - b) * w; // barycentric coordinates (0,1-w,w)
        }

        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w;
    }

    bool intersect(const vec3 &rayPos, const vec3 &rayDir, bool twoSide, float &u, float &v, float &t) const {
        vec3 e1 = b - a;
        vec3 e2 = c - a;
        vec3 p = rayDir.cross(e2);
        float d = p.dot(e1);
        vec3 q;

        if (d > EPS) {
            vec3 r = rayPos - a;
            u = p.dot(r);
            if (u >= 0 && u <= d) {
                q = r.cross(e1);
                v = rayDir.dot(q);
                if (v < 0 || u + v > d) 
                    return false;
            } else
                return false;
        } else
            if (d < -EPS && twoSide) {
                vec3 r = rayPos - a;
                u = p.dot(r);
                if (u <= 0 && u >= d) {
                    q = r.cross(e1);
                    v = rayDir.dot(q);
                    if (v > 0 || u + v < d)
                        return false;
                } else
                    return false;
            } else
                return false;

        d = 1.0f / d;
        t = q.dot(e2);

        if (t > EPS) {
            u *= d;
            v *= d;
            t *= d;
            return true;
        }
        return false;
    }
};

struct Sphere {
    vec3  center;
    float radius;

    Sphere(const vec3 &center, float radius) : center(center), radius(radius) {}

    bool intersect(const Triangle &tri, vec3 &n, float &t) {
        vec3 p = tri.closestPoint(center);
        vec3 v = center - p;

        float d = v.length2();
        if (d > radius * radius)
            return false;
        n = v.normal();
        t = radius - sqrtf(d);
        return true;
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

    char* readStr() {
        uint8 len;
        read(&len, 1);
        if (len == 0)
            return NULL;
        char *str = new char[len + 1];
        read(str, len);
        str[len] = 0;
        return str;
    }
};

#endif