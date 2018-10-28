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
#include "bullet.h"
#include "sound.h"
#include "ui.h"

Level       *level;
BasePlayer  *players[8];
int         playersCount;

namespace Game {
    Camera  *camera;
    BasePlayer *curPlayer;
    bool    splitscreen;

    void init() {
        Context::init();
        UI::init();

        camera = new Camera();
        level = new Level("level1.lvl");

        playersCount = 0;
        players[playersCount++] = new Player(level, Player::PLAYER_1);
        players[playersCount++] = new Player(level, Player::PLAYER_2);

        curPlayer = players[0];

        splitscreen = true;
        initBullets();
    }

    void deinit() {
        deinitBullets();
        for (int i = 0; i < playersCount; i++)
            delete players[i];
        delete level;
        delete camera;
        UI::deinit();
        Context::deinit();
    }

    void updateTick() {
        level->update();
        updateBullets();
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
        glEnable(GL_DEPTH_TEST);

        if (!camera->freeCam) {
            camera->pos = ((Player*)curPlayer)->getHeadPos();
            camera->rot = ((Player*)curPlayer)->rot;
            camera->aspect = aspect;
        }
        camera->update();

        level->render(camera);
        for (int i = 0; i < playersCount; i++)
            if (players[i] != curPlayer)
                players[i]->render(camera);

        renderBullets(camera);

        UI::begin(aspect);
        Texture *tex = UI::crosshair;
        UI::sprite(tex, Context::height * aspect * 0.5f, Context::height * 0.5f, tex->width * 0.5f, tex->height * 0.5f);
        char buf[256];
        sprintf(buf, "%d", ((Player*)curPlayer)->health);
        UI::textOut(buf, 64, Context::height - 64.0f, 2.0f, 2.0f);
        UI::end();
    }

    void render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)Context::width / (float)Context::height;

        if (splitscreen) {
            float splitAspect = aspect * 0.5f;

            curPlayer = players[0];
            glViewport(0, 0, Context::width / 2, Context::height);
            renderView(splitAspect);
            curPlayer = players[1];
            glViewport(Context::width / 2, 0, Context::width / 2, Context::height);
            renderView(splitAspect);
        } else {
            curPlayer = players[0];
            glViewport(0, 0, Context::width, Context::height);
            renderView(aspect);
        }

        glViewport(0, 0, Context::width, Context::height);
        UI::begin(aspect);
        const char *title = "Wellcome to Real Tournament!";
        UI::textOut(title, (Context::width  - strlen(title) * 16.0f) / 2, 32, 1.0f, 1.0f);
        UI::end();
    }
}

#endif