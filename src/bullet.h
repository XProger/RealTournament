#ifndef H_BULLET
#define H_BULLET 

#include "utils.h"
#include "level.h"
#include "player.h"

extern Level  *level;
extern BasePlayer *players[8];
extern int    playersCount;

Level *bulletModels[1];
int bulletModelsCount;

struct Bullet;

Bullet *bullets[256];
int bulletsCount;

void initBullets() {
    memset(bullets, 0, sizeof(bullets));
    bulletsCount = 0;

    bulletModelsCount = 0;
    bulletModels[bulletModelsCount++] = new Level("bullet.lvl");
}

void addBullet(Bullet *bullet) {
    bullets[bulletsCount++] = bullet;
}

struct Bullet {
    vec3 pos;
    vec3 velocity;
    bool isDead;

    int modelIndex;

    Bullet(const vec3 &pos, const vec3 &velocity, int modelIndex) : pos(pos), velocity(velocity), isDead(false), modelIndex(modelIndex) {
        addBullet(this);
    }

    virtual ~Bullet() {}
    virtual void update() {}

    bool trace(const vec3 &a, const vec3 &b, vec3 &pos, int damage) {
        vec3 rayDir = velocity.normal();

        float t = 8192.0f;
        level->trace(pos, rayDir, t);

        BasePlayer *hitPlayer = NULL;

        for (int i = 0; i < playersCount; i++) {
            float t0 = 8192.0f;
            players[i]->trace(pos, rayDir, t0);
            if (t0 < t) {
                hitPlayer = players[i];
                t = t0;
            }
        }

        if (t != 8192.0f) {
            if (t > (b - a).length())
                return false;

            if (hitPlayer)
                hitPlayer->hit(damage);
            pos = a + rayDir * t;
            return true;
        }

        return false;
    }

    void render(Camera *camera) {
        if (modelIndex == -1) return;
        Level *model = bulletModels[modelIndex];
        mat4 &m = model->objects[0]->matrix;
        m.identity();
        m.translate(pos);
        m.rotateY(atan2f(-velocity.x, -velocity.z));
        model->render(camera);
    }
};

void deinitBullets() {
    for (int i = 0; i < bulletModelsCount; i++)
        delete bulletModels[i];

    for (int i = 0; i < bulletsCount; i++)
        delete bullets[i];
}

void renderBullets(Camera *camera) {
    for (int i = 0; i < bulletsCount; i++)
        bullets[i]->render(camera);
}

void updateBullets() {
    int i = 0;
    while (i < bulletsCount) {
        bullets[i]->update();
        if (bullets[i]->isDead) {
            delete bullets[i];
            bullets[i] = bullets[--bulletsCount];
        } else
            i++;
    }
    //LOG("bullets: %d\n", bulletsCount);
}

struct SimpleBullet : Bullet {

    SimpleBullet(const vec3 &pos, const vec3 &velocity) : Bullet(pos, velocity, 0) {}

    virtual void update() {
        if (isDead) return;

        vec3 nextPos = pos + velocity * deltaTime;

        if (trace(pos, nextPos, pos, 10)) {
            isDead = true;
        } else
            pos = nextPos;
    }

};



#endif