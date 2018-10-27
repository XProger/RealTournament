#ifndef H_PLAYER
#define H_PLAYER

#include "utils.h"
#include "level.h"

#define PLAYER_RADIUS           0.25f
#define PLAYER_RADIUS_GROUND    0.15f
#define PLAYER_HEIGHT           1.75f
#define PLAYER_MAX_SPEED        8.0f
#define PLAYER_ACCEL_GROUND     10.0f
#define PLAYER_ACCEL_AIR        5.0f
#define PLAYER_FRICTION_GROUND  0.999f
#define PLAYER_FRICTION_AIR     0.7f
#define PLAYER_FRICTION_STOP    0.025f
#define PLAYER_JUMP_SPEED       8.0f

struct Player {
    enum {
        LEFT    = 1,
        RIGHT   = 2,
        UP      = 4,
        DOWN    = 8,
        JUMP    = 16,
    };

    int  health;

    vec3 pos;
    vec3 rot;
    vec3 velocity;

    bool onGround;

    Player() {
        respawn(vec3(0.0f, 3.0f, 0.0f), vec3(0.0f));
    }

    ~Player() {}

    void respawn(const vec3 &pos, const vec3 &rot) {
        health = 100;
        this->pos = pos;
        this->rot = rot;
        velocity = vec3(0.0f);
        onGround = false;
    }

    vec3 getHeadPos() {
        return pos + vec3(0.0f, PLAYER_HEIGHT, 0.0f);
    }

    void accelerate(const vec3 &dir, float speed) {
        float currentSpeed = velocity.dot(dir);
        float addSpeed = speed - currentSpeed;

        if (addSpeed < 0.0f)
            return;

        float accelSpeed = PLAYER_ACCEL_GROUND * deltaTime * speed * PLAYER_FRICTION_GROUND;
        
        if (accelSpeed > addSpeed)
            accelSpeed = addSpeed;

        velocity = velocity + dir * accelSpeed;
    }

    void update(Level *level) {
        int32 input = 0;
        if (Input::down['A']) input |= LEFT;
        if (Input::down['D']) input |= RIGHT;
        if (Input::down['W']) input |= UP;
        if (Input::down['S']) input |= DOWN;
        if (Input::down[VK_SPACE] && onGround) input |= JUMP;

        velocity.y = velocity.y - level->gravity * deltaTime;

        if (onGround)
            velocity.y = 0.0f;

        rot.x -= Input::mouseDelta.y * 0.001f;
        rot.y -= Input::mouseDelta.x * 0.001f;

        mat4 m;
        m.identity();
        m.rotateZ(-rot.z);
        m.rotateX(-rot.x);
        m.rotateY(-rot.y);

        float fmove = 0.0f, smove = 0.0f;

        if (input & LEFT)  smove = -1.0f;
        if (input & RIGHT) smove = +1.0f;
        if (input & UP)    fmove = -1.0f;
        if (input & DOWN)  fmove = +1.0f;

        vec3 dir   = vec3(m.e20, m.e21, m.e22);
        vec3 right = vec3(m.e00, m.e01, m.e02);

        velocity.x = (dir.x * fmove + right.x * smove) * PLAYER_MAX_SPEED;
        velocity.z = (dir.z * fmove + right.z * smove) * PLAYER_MAX_SPEED;

        /*
        float drop = 0.0f;
        float speed = velocity.length();

        if (onGround) {
            float control = (speed < PLAYER_MAX_SPEED) ? PLAYER_MAX_SPEED : speed;
            drop = control * PLAYER_FRICTION_GROUND * deltaTime;
        }

        float newspeed = speed - drop;

        if (newspeed != speed) {
            newspeed /= speed;
            velocity = velocity * newspeed;
        }

        float fmove = 0.0f, smove = 0.0f;

        if (input & LEFT)  smove = -1.0f;
        if (input & RIGHT) smove = +1.0f;
        if (input & UP)    fmove = -1.0f;
        if (input & DOWN)  fmove = +1.0f;

        vec3 dir   = vec3(m.e20, m.e21, m.e22);
        vec3 right = vec3(m.e00, m.e01, m.e02);

        vec3 wishVel = dir * fmove + right * smove;
        float wishSpeed = wishVel.length();

        if (wishSpeed != 0.0f) {

            vec3 wishDir = wishVel.normal();

            if (wishSpeed > PLAYER_MAX_SPEED) {
                wishVel = wishVel * (PLAYER_MAX_SPEED / wishSpeed);
                wishSpeed = PLAYER_MAX_SPEED;
            }

            accelerate(wishDir, wishSpeed);
        }
        */
        /*

        float friction = onGround ? PLAYER_FRICTION_GROUND : PLAYER_FRICTION_AIR;

        vec3 v = vec3(velocity.x, 0.0f, velocity.z);
        float damping = pow(friction, deltaTime);
        v.x *= damping;
        v.z *= damping;

        velocity.x = v.x;
        velocity.z = v.z;
        LOG("v: %f\n", velocity.length());

        */
//            float vlen = v.length();
//            if (vlen > PLAYER_MAX_SPEED) {
//                v = v.normal() * PLAYER_MAX_SPEED;
//            }


        
        if (input & JUMP) {
            velocity.y = PLAYER_JUMP_SPEED; 
        }

        pos = pos + velocity * deltaTime;
     
        onGround = false;
        for (int i = 0; i < level->objectsCount; i++)
            collide(level->objects[i]);
    }

    void collide(Level::Object *obj) {
        for (int i = 0; i < obj->iCount; i += 3) {
            vec3 &a = obj->vertices[obj->indices[i + 0]].coord;
            vec3 &b = obj->vertices[obj->indices[i + 1]].coord;
            vec3 &c = obj->vertices[obj->indices[i + 2]].coord;

            vec3 n;
            float t;

            if ((a.x == b.x && a.y == b.y && a.z == b.z) ||
                (c.x == b.x && c.y == b.y && c.z == b.z) ||
                (a.x == c.x && a.y == c.y && a.z == c.z)) {
                continue;
            }

            if (Sphere(pos + vec3(0.0f, PLAYER_RADIUS, 0.0f), PLAYER_RADIUS).intersect(Triangle(a, b, c), n, t) && t != 0.0f) {
                if (fabsf(n.y) > fabsf(n.x) && fabsf(n.y) > fabsf(n.z)) {
                    onGround = true;
                }

                if (fabsf(n.y) < 0.1 && t > 0.0f)
                    velocity = velocity - n * velocity.dot(n);

                pos = pos + n * t;
            }
        }
    }
};

#endif