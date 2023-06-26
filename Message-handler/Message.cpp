// Итоговая работа.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "Еще одна попытка.h"

using namespace std;

HWND messageEditTimer;
HWND messageEdit;
const int MAX_TEXT_LEN = 30;
RWorker base;

#define MAX_LOADSTRING 100

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Регистрируем класс окна
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(wcex);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"MyWindowClass";
    ATOM classAtom = RegisterClassEx(&wcex);

    // Создаем окно
    HWND hWnd = CreateWindowEx(
        0,
        MAKEINTATOM(classAtom),
        L"My Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        550, 160,
        0, 0, hInstance, 0
    );

    // Создаем элементы управления
    messageEdit = CreateWindowEx(
        0,
        L"EDIT",
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        10, 10, 400, 30,
        hWnd,
        (HMENU)Messages,
        hInstance,
        nullptr
    );

     messageEditTimer = CreateWindowEx(
        0,
        L"EDIT",
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        10, 45, 400, 30,
        hWnd,
        (HMENU)Timer,
        hInstance,
        nullptr
    );

    HWND sendButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Отправить",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        420, 10, 100, 30,
        hWnd,
        (HMENU)Push,
        hInstance,
        nullptr
    );

    HWND editButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Изменить",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        420, 45, 100, 30,
        hWnd,
        (HMENU)Edit,
        hInstance,
        nullptr
    );

    HWND stopButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Start",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        420, 80, 100, 30,
        hWnd,
        (HMENU)Stop,
        hInstance,
        nullptr
    );

    // Отображаем окно
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Запускаем обработку сообщений
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //base.startWork();

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case Stop:
            if (!base.isStop()) {
                base.stopWork();
                SetWindowTextA(GetDlgItem(hWnd, Stop), "Start");
            }
            else {
                base.startWork();
                SetWindowTextA(GetDlgItem(hWnd, Stop), "Stop");
            }
            break;

        case Push:
            {   
                int len = GetWindowTextLength(messageEdit);
                if (len > MAX_TEXT_LEN) len = MAX_TEXT_LEN;
                wchar_t buff[MAX_TEXT_LEN + 1] = { 0 };
                GetWindowText(messageEdit, buff, len + 1);

                Message message = {base.GetNumber(), wstring(buff), time(NULL)};
                base.AddMessage(message);
                SetWindowText(messageEdit, TEXT(""));
            }

            break;

        case Edit:
            {
            int len = GetWindowTextLength(messageEditTimer);
            if (len > MAX_TEXT_LEN) len = MAX_TEXT_LEN;
            wchar_t buff[MAX_TEXT_LEN + 1] = { 0 };
            GetWindowText(messageEditTimer, buff, len + 1);
            base.SetTimer(int(buff[0] - L'0') * 1000);
            }
            break;
        }

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }   

    return 0;
}
