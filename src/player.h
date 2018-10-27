#ifndef H_PLAYER
#define H_PLAYER

#include "utils.h"
#include "level.h"
#include "weapon.h"

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

struct Player : BasePlayer {
    enum PlayerType {
        PLAYER_1,
        PLAYER_2,
        PLAYER_NET,
    } type;

    enum {
        LEFT    = 1,
        RIGHT   = 2,
        UP      = 4,
        DOWN    = 8,
        JUMP    = 16,
        FIRE_A  = 32,
        FIRE_B  = 64,
    };

    Level  *level;
    Weapon *weapon;

    int  health;

    vec3 pos;
    vec3 rot;
    vec3 velocity;

    Level *model;

    bool onGround;

    Player(Level *level, PlayerType type) : BasePlayer(), level(level), type(type) {
        respawn();
        model = new Level("enemy.lvl");
        weapon = new Pistol();
    }

    virtual ~Player() {
        delete weapon;
        delete model;
    }

    void respawn() {
        Level::Object *obj = level->getRespawn();
        if (!obj) return;

        mat4 &m = obj->matrix;
        pos = vec3(m.e03, m.e13, m.e23);

        rot = vec3(0.0f, atan2f(m.e02, -m.e22), 0.0f);

        health = 100;
        velocity = vec3(0.0f);
        onGround = false;
    }

    vec3 getHeadPos() {
        return pos + vec3(0.0f, PLAYER_HEIGHT, 0.0f);
    }
    /*
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
    */
    virtual void update() {
        int32 input = 0;

        switch (type) {
            
            case PLAYER_1 : 
                rot.x -= Input::mouseDelta.y * 0.001f;
                rot.y -= Input::mouseDelta.x * 0.001f;

                if (Input::down['R']) {
                    respawn();
                    Input::down['R'] = false;
                }
                if (Input::down['A']) input |= LEFT;
                if (Input::down['D']) input |= RIGHT;
                if (Input::down['W']) input |= UP;
                if (Input::down['S']) input |= DOWN;
                if (Input::down[VK_SPACE]) input |= JUMP;
                if (Input::down[VK_LBUTTON]) input |= FIRE_A;
                if (Input::down[VK_RBUTTON]) input |= FIRE_B;
                break;


            case PLAYER_2 : 
                if (Input::down[VK_LEFT]) input |= LEFT;
                if (Input::down[VK_RIGHT]) input |= RIGHT;
                if (Input::down[VK_UP]) input |= UP;
                if (Input::down[VK_DOWN]) input |= DOWN;
                if (Input::down[VK_RETURN]) input |= JUMP;
                break;

            default : ;
        }

        velocity.y = velocity.y - level->gravity * deltaTime;

        if (onGround)
            velocity.y = 0.0f;

        rot.x = clamp(rot.x, -PI * 0.5f, +PI * 0.5f);

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


        
        if ((input & JUMP) && onGround) {
            velocity.y = PLAYER_JUMP_SPEED; 
        }

        pos = pos + velocity * deltaTime;
     
        onGround = false;
        for (int i = 0; i < level->objectsCount; i++) {
            Level::Object *obj = level->objects[i];
            if (obj->type != Level::Object::MESH)
                continue;
            collide(obj);
        }

        weapon->fire(pos + vec3(0.0f, PLAYER_HEIGHT * 0.5f, 0.0f), rot, (input & (FIRE_A | FIRE_B)) != 0, (input & FIRE_B) != 0);

        {
            mat4 &m = model->objects[0]->matrix;
            m.identity();
            m.translate(pos);
            m.rotateY(rot.y);
        }
    }

    virtual void render(Camera *camera) {
        model->render(camera);
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

    virtual void hit(int damage) {
        health -= damage;
        LOG("hit: %d\n", health);
        if (health <= 0)
            respawn();
    }

    virtual void trace(const vec3 &rayPos, const vec3 &rayDir, float &t) {
        model->trace(rayPos, rayDir, t);
    }
};

#endif