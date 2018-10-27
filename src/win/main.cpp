#include <windows.h>
#include "utils.h"
#include "game.h"

bool  isQuit = false;
int   lastTime;
float deltaTime;

HWND  hWnd;
HDC   hDC;
HGLRC hRC;

int osGetTime() {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return int(count.QuadPart * 1000L / freq.QuadPart);
}

void initGL() {
    hDC = GetDC(hWnd);

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));

    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.cColorBits   = 32;
    pfd.cRedBits     = 8;
    pfd.cGreenBits   = 8;
    pfd.cBlueBits    = 8;
    pfd.cAlphaBits   = 8;
    pfd.cDepthBits   = 24;
    pfd.cStencilBits = 8;

    int fmt = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, fmt, &pfd);
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);
}

void freeGL() {
    wglMakeCurrent(0, 0);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

HCURSOR curNone, curArrow;

void inputInit() {
    curArrow = (HCURSOR)GetClassLong(hWnd, GCL_HCURSOR);

    uint8 maskA[32 * 32];
    uint8 maskX[32 * 32];
    memset(maskA, 0xFF, sizeof(maskA));
    memset(maskX, 0x00, sizeof(maskX));
    curNone = CreateCursor(0, 0, 0, 32, 32, maskA, maskX);
}

void inputUpdate() {
    Input::mouseDelta = vec3(0.0f);

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);
    Input::mousePos = vec3((float)p.x, (float)p.y, 0.0f);

    if (Input::mouseCapture) {
        RECT r;
        GetClientRect(hWnd, &r);
        POINT c;
        c.x = (r.left + r.right) / 2;
        c.y = (r.top + r.bottom) / 2;

        Input::mouseDelta = vec3((float)(p.x - c.x), (float)(p.y - c.y), 0.0f);

        ClientToScreen(hWnd, &c);
        SetCursorPos(c.x, c.y);

        SetClassLong(hWnd, GCL_HCURSOR, (LONG)curNone);
    } else
        SetClassLong(hWnd, GCL_HCURSOR, (LONG)curArrow);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg) {
        case WM_ACTIVATE :
            Input::reset();
            break;
        case WM_DESTROY :
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN :
        case WM_KEYUP   :
            Input::down[wParam] = Msg != WM_KEYUP;
            if (wParam == VK_ESCAPE) {
                Input::mouseCapture = false;
            }
            break;
        case WM_LBUTTONDOWN   :
        case WM_LBUTTONDBLCLK :
        case WM_LBUTTONUP     :
            Input::down[VK_LBUTTON] = Msg != WM_LBUTTONUP;
            Input::mouseCapture = true;
            break;
        case WM_RBUTTONDOWN   :
        case WM_RBUTTONDBLCLK :
        case WM_RBUTTONUP     :
            Input::down[VK_RBUTTON] = Msg != WM_RBUTTONUP;
            break;
        case WM_MBUTTONDOWN   :
        case WM_MBUTTONDBLCLK :
        case WM_MBUTTONUP     :
            Input::down[VK_MBUTTON] = Msg != WM_MBUTTONUP;
            break;
        case WM_SIZE :
            Context::width  = LOWORD(lParam);
            Context::height = HIWORD(lParam);
            break;
        default : 
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

int main(int argc, char **argv) {
    hWnd = CreateWindowA("static", "Real Tournament", WS_OVERLAPPEDWINDOW, 0, 0, 1280, 720, 0, 0, 0, 0);

    initGL();
    inputInit();

    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc);
    ShowWindow(hWnd, SW_SHOWDEFAULT);

    Game::init();

    lastTime = osGetTime();

    MSG msg;

    while (!isQuit) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                isQuit = true;
        } else {
            int time = osGetTime();
            deltaTime = (time - lastTime) / 1000.0f;
            lastTime = time;

            inputUpdate();
            Game::update();
            Game::render();

            SwapBuffers(hDC);
        }
    }

    Game::deinit();

    freeGL();

    DestroyWindow(hWnd);
}