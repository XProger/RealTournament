#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include "crtdbg.h"
    #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define new DEBUG_NEW
#endif

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


#define SND_BUFFER_SIZE 8192

HWAVEOUT     waveOut;
WAVEFORMATEX waveFmt;
WAVEHDR      waveHdr[2];
char        *sndData;

void sndFill(HWAVEOUT waveOut, LPWAVEHDR waveHdr) {
    waveOutUnprepareHeader(waveOut, waveHdr, sizeof(WAVEHDR));
    Sound::fill((Sound::Frame*)waveHdr->lpData, SND_BUFFER_SIZE / 4);
    waveOutPrepareHeader(waveOut, waveHdr, sizeof(WAVEHDR));
    waveOutWrite(waveOut, waveHdr, sizeof(WAVEHDR));
}

void sndInit() {
    Sound::init();

    waveFmt.cbSize          = sizeof(waveFmt);
    waveFmt.nChannels       = 2;
    waveFmt.wFormatTag      = WAVE_FORMAT_PCM;
    waveFmt.wBitsPerSample  = 16;
    waveFmt.nSamplesPerSec  = 44100;
    waveFmt.nBlockAlign     = 4;
    waveFmt.nAvgBytesPerSec = 44100 * 4;

    sndData = NULL;

    if (waveOutOpen(&waveOut, WAVE_MAPPER, &waveFmt, (INT_PTR)hWnd, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR)
        return;

    sndData = new char[SND_BUFFER_SIZE * 2];
    memset(&waveHdr, 0, sizeof(waveHdr));
    for (int i = 0; i < 2; i++) {
        waveHdr[i].dwBufferLength = SND_BUFFER_SIZE;
        waveHdr[i].lpData         = sndData + SND_BUFFER_SIZE * i;
        sndFill(waveOut, &waveHdr[i]);
    }
}

void sndFree() {
    delete[] sndData;
    waveOutUnprepareHeader(waveOut, &waveHdr[0], sizeof(WAVEHDR));
    waveOutUnprepareHeader(waveOut, &waveHdr[1], sizeof(WAVEHDR));
    waveOutReset(waveOut);
    waveOutClose(waveOut);
    Sound::deinit();
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

POINT center = { 0, 0 };

void inputUpdate() {
    Input::mouseDelta = vec3(0.0f);

    POINT p, delta;
    GetCursorPos(&p);

    delta.x = p.x - center.x;
    delta.y = p.y - center.y;

    ScreenToClient(hWnd, &p);
    Input::mousePos = vec3((float)p.x, (float)p.y, 0.0f);

    if (Input::mouseCapture) {
        RECT r;
        GetWindowRect(hWnd, &r);
        center.x = (r.left + r.right) / 2;
        center.y = (r.top + r.bottom) / 2;

        Input::mouseDelta = vec3((float)delta.x, (float)delta.y, 0.0f);

        SetCursorPos(center.x, center.y);

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
        case MM_WOM_DONE :
            sndFill((HWAVEOUT)wParam, (WAVEHDR*)lParam);
            break;
        default : 
            return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

int main(int argc, char **argv) {
#ifdef _DEBUG
    _CrtMemState _msBegin, _msEnd, _msDiff;
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    _CrtMemCheckpoint(&_msBegin);
#endif

    hWnd = CreateWindowA("static", "Real Tournament", WS_OVERLAPPEDWINDOW, 0, 0, 1280, 720, 0, 0, 0, 0);

    initGL();
    inputInit();
    sndInit();

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
            if (time == lastTime) continue;

            deltaTime = (time - lastTime) / 1000.0f;
            lastTime = time;

            inputUpdate();
            Game::update();
            Game::render();

            SwapBuffers(hDC);
        }
    }

    Game::deinit();

    sndFree();
    freeGL();

    DestroyWindow(hWnd);

 #ifdef _DEBUG
    _CrtMemCheckpoint(&_msEnd);

    if (_CrtMemDifference(&_msDiff, &_msBegin, &_msEnd) > 0) {
        _CrtDumpMemoryLeaks();
        system("pause");
    }
#endif
}