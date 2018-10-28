#ifndef H_WEAPON
#define H_WEAPON

#include "utils.h"
#include "bullet.h"
#include "sound.h"

struct Weapon {
    enum Type {
        PISTOL,
    } type;

    enum {
        STATE_NONE,
        STATE_FIRE_A,
        STATE_FIRE_B,
    } state;

    float reloadTime;

    Weapon(Type type) : type(type), state(STATE_NONE), reloadTime(0.0f) {}
    virtual ~Weapon() {}
    virtual void fire(const vec3 &pos, const vec3 &rot, bool hold, bool alternate) {}
};


#define PISTOL_RELOAD_A 0.4f
#define PISTOL_RELOAD_B 0.2f
#define PISTOL_BULLET_SPEED  50.0f

struct Pistol : Weapon {

    Pistol() : Weapon(PISTOL) {}
    
    void shot(const vec3 &pos, const vec3 &rot) {
        reloadTime = (state == STATE_FIRE_A) ? PISTOL_RELOAD_A : PISTOL_RELOAD_B;
//        LOG("fire %s\n", (state == STATE_FIRE_A) ? "A" : "B");

        mat4 mView;
        mView.identity();
        mView.rotateY(rot.y);
        mView.rotateX(rot.x);
        mView.rotateZ(rot.z);
        vec3 dir = -vec3(mView.e02, mView.e12, mView.e22);

        vec3 velocity = dir * PISTOL_BULLET_SPEED;
        new SimpleBullet(pos, velocity);

        Sound::play("sound/E_Shot.wav");
    }

    virtual void fire(const vec3 &pos, const vec3 &rot, bool hold, bool alternate) {
        reloadTime = max(0.0f, reloadTime - deltaTime);

        if (reloadTime > 0.0f)
            return;

        state = hold ? (alternate ? STATE_FIRE_B : STATE_FIRE_A) : STATE_NONE;

        if (state == STATE_NONE) return;

        shot(pos, rot);
    }
};


#endif