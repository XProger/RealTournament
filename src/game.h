#ifndef H_GAME
#define H_GAME

#include "input.h"
#include "texture.h"
#include "mesh.h"
#include "context.h"
#include "shader.h"
#include "camera.h"
#include "level.h"
#include "player.h"

namespace Game {
    Camera  *camera;
    Level   *level;
    Player  *player;

    void init() {
        Context::init();

        camera = new Camera();
        level = new Level("level1.lvl");
        player = new Player();
    }

    void deinit() {
        delete player;
        delete level;
        delete camera;
        Context::deinit();
    }

    void updateTick() {
        level->update();
        player->update(level);
        if (!camera->freeCam) {
            camera->pos = player->getHeadPos();
            camera->rot = player->rot;
        }
        camera->update();
    }

    void update() {
        float d = deltaTime;
        while (d > 0.0f) {
            deltaTime = min(d, 1.0f / 60.0f);
            updateTick();
            d -= deltaTime;
        }
    }

    void render() {
        glViewport(0, 0, Context::width, Context::height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        level->render(camera);
    }
}

#endif