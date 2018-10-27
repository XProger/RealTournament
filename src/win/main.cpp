#include <stdio.h>
#include <windows.h>
#include <gl/GL.h>

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

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg) {
        case WM_DESTROY :
            PostQuitMessage(0);
            break;
        default : 
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

int main(int argc, char **argv) {
    hWnd = CreateWindowA("static", "Real Tournament", WS_OVERLAPPEDWINDOW, 0, 0, 1280, 720, 0, 0, 0, 0);

    initGL();

    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&WndProc);
    ShowWindow(hWnd, SW_SHOWDEFAULT);

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
            if (deltaTime > 0.1f) {
                deltaTime = 0.1f;
            }
            lastTime = time;

            glClear(GL_COLOR_BUFFER_BIT);

            glRotatef(90 * deltaTime, 0, 0, 1);

            glBegin(GL_QUADS);
                glColor3f(1, 0, 0); glVertex2f(-0.5f, -0.5f);
                glColor3f(0, 1, 0); glVertex2f( 0.5f, -0.5f);
                glColor3f(0, 0, 1); glVertex2f( 0.5f,  0.5f);
                glColor3f(1, 0, 1); glVertex2f(-0.5f,  0.5f);
            glEnd();

            SwapBuffers(hDC);
        }
    }

    freeGL();

    DestroyWindow(hWnd);
}
