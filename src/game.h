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
    Player  *players[8];
    Player  *curPlayer;
    int     playersCount;
    bool    splitscreen;

    void init() {
        Context::init();

        camera = new Camera();
        level = new Level("level1.lvl");

        players[playersCount++] = new Player(level, Player::PLAYER_1);
        players[playersCount++] = new Player(level, Player::PLAYER_2);

        curPlayer = players[0];

        splitscreen = true;
    }

    void deinit() {
        for (int i = 0; i < playersCount; i++)
            delete players[i];
        delete level;
        delete camera;
        Context::deinit();
    }

    void updateTick() {
        level->update();
        for (int i = 0; i < playersCount; i++)
        players[i]->update();
    }

    void update() {
        float d = deltaTime;
        while (d > 0.0f) {
            deltaTime = min(d, 1.0f / 60.0f);
            updateTick();
            d -= deltaTime;
        }
    }

    void renderView(float aspect) {
        if (!camera->freeCam) {
            camera->pos = curPlayer->getHeadPos();
            camera->rot = curPlayer->rot;
            camera->aspect = aspect;
        }
        camera->update();

        level->render(camera);
        for (int i = 0; i < playersCount; i++)
            if (players[i] != curPlayer)
                players[i]->render(camera);
    }

    void render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        float aspect = (float)Context::width / (float)Context::height;

        if (splitscreen) {
            aspect *= 0.5f;

            curPlayer = players[0];
            glViewport(0, 0, Context::width / 2, Context::height);
            renderView(aspect);
            curPlayer = players[1];
            glViewport(Context::width / 2, 0, Context::width / 2, Context::height);
            renderView(aspect);
        } else {
            curPlayer = players[0];
            glViewport(0, 0, Context::width, Context::height);
            renderView(aspect);
        }
    }
}

#endif