#ifndef H_INPUT
#define H_INPUT

#include "utils.h"

namespace Input {
    bool down[256];
    bool mouseCapture;
    vec3 mousePos;
    vec3 mouseDelta;

    void reset() {
        memset(down, 0, sizeof(down));
        mouseCapture = false;
        mousePos     = vec3(0.0f);
        mouseDelta   = vec3(0.0f);
    }
}

#endif