#include <windows.h>
#include <string>
#include <iostream>
using namespace std;

#define VERSION "0.0.5"
#define IDI_MYICON 101

// ΢�����������
// https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes
// �������֡���ĸ
void write_word(unsigned char vk)
{
    // keybd_event(vk,0,0,0);
    // keybd_event(vk,0,KEYEVENTF_KEYUP,0);
    INPUT inputs = {0};

    inputs.type = INPUT_KEYBOARD;
    inputs.ki.wVk = vk;

    UINT uSent = SendInput(1, &inputs, sizeof(INPUT));
    if (uSent != 1)
    {
        cout << L"SendInput failed: " << HRESULT_FROM_WIN32(GetLastError()) << endl;
    }
}

// �������
void WriteCharacter(unsigned char vk)
{
    switch (vk)
    {
    case '-':
        keybd_event(VK_OEM_MINUS, 0, 0, 0);
        keybd_event(VK_OEM_MINUS, 0, KEYEVENTF_KEYUP, 0);
        break;
    case '.':
        keybd_event(VK_OEM_PERIOD, 0, 0, 0);
        keybd_event(VK_OEM_PERIOD, 0, KEYEVENTF_KEYUP, 0);
        break;
    case ' ':
        keybd_event(0x20, 0, 0, 0);
        keybd_event(0x20, 0, KEYEVENTF_KEYUP, 0);
        break;
    case '\'':
        keybd_event(VK_OEM_7, 0, 0, 0);
        keybd_event(VK_OEM_7, 0, KEYEVENTF_KEYUP, 0);
        break;
    }
}

void Enter()
{
    keybd_event(13, 0, 0, 0);
    keybd_event(13, 0, KEYEVENTF_KEYUP, 0);
}

void ShiftDown() // ����shift
{
    keybd_event(16, 0, 0, 0);
}

void ShiftUp() // ����shift
{
    keybd_event(16, 0, KEYEVENTF_KEYUP, 0);
}

void CtrlV() // ճ��
{
    keybd_event(162, 0, 0, 0);
    write_word('V');
    keybd_event(162, 0, KEYEVENTF_KEYUP, 0);
}

void AutoWrite(string word) // ����һ������
{
    size_t n = word.length();

    for (size_t i = 0; i < n; i++)
    {
        unsigned char zm = word.at(i);
        if ((zm >= 'a' && zm <= 'z'))
        {
            write_word(zm - 32);
        }
        else if (zm >= 'A' && zm <= 'Z')
        {
            ShiftDown();
            write_word(zm);
            ShiftUp();
        }
        else if ((zm >= '0' && zm <= '9'))
        {
            write_word(zm);
        }
        else if (zm == ' ' || zm == '.' || zm == '-' || zm == '\'')
        {
            WriteCharacter(zm);
        }
        Sleep(10);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditName;
    switch (msg)
    {
    case WM_CREATE:
    {
        // �����ı���
        hEditName = CreateWindow(
            TEXT("Edit"),
            TEXT(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER /*�߿�*/ | WS_TABSTOP /*����ʹ��TABѡ��*/ | ES_MULTILINE,
            10, 10, 765, 500,
            hwnd,
            (HMENU)2,
            (HINSTANCE)(LONG_PTR)GetWindowLong(hwnd, GWLP_HINSTANCE),
            NULL);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            bool isrightstr = true;

            int n = GetWindowTextLength(hEditName); // ��ȡ�ı����е��ı����� ��������ȡ�ı����е��ı�����;
            if (n > 0)
            {
                TCHAR *szBuffer = (TCHAR *)malloc(sizeof(char) * (n + 1)); // ������
                GetWindowText(hEditName, szBuffer, n + 1);                 // ��ȡ�ı�
                szBuffer[n + 1] = '\0';

                for (char c = szBuffer[0]; c != '\0'; c = szBuffer[++n])
                {
                    // ����ַ����Ƿ����Ҫ��
                    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ' || c == '.' || c == '\''))
                    {
                        isrightstr = false;
                        break;
                    }
                }

                if (isrightstr)
                {
                    MessageBox(NULL, "���ڹر���֮��2s�ڽ�����ָ���ƶ�����Ҫ�������ֵĿ���", "��ʾ", MB_OK | MB_ICONINFORMATION);
                    Sleep(2000);
                    AutoWrite(szBuffer);
                }
                else
                {
                    MessageBox(NULL, "������Ч���ַ���", "��ʾ", MB_OK | MB_ICONINFORMATION);
                }
                free(szBuffer);
            }
            else
            {
                MessageBox(NULL, "��TM�������밡", "��ʾ", MB_OK | MB_ICONWARNING);
            }
        }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    const char CLASS_NAME[] = "Hello, World!";

    WNDCLASS wc = {0};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    // HICON hIcon = (HICON)LoadImage(NULL, "MYICON.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));

    wc.hIcon = hIcon;

    // �����������Ա�ĳ�ʼ��
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                                                                   // Optional window styles.
        CLASS_NAME,                                                          // Window class
        ("AutoWrite - �Զ����ֻ� ���ߣ������ V" + string(VERSION)).c_str(), // Window text
        WS_OVERLAPPEDWINDOW,                                                 // Window style

        // Size and position
        100, 100, 800, 600,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    // Add button
    CreateWindow(
        "BUTTON",                                              // Predefined class; Unicode assumed
        "���ҿ�ʼ",                                            // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles
        350,                                                   // x position
        525,                                                   // y position
        100,                                                   // Button width
        30,                                                    // Button height
        hwnd,                                                  // Parent window
        (HMENU)1,                                              // Button identifier.
        (HINSTANCE)(LONG_PTR)GetWindowLong(hwnd, GWLP_HINSTANCE),
        NULL); // Pointer not needed.

    ShowWindow(hwnd, nCmdShow);

    MessageBox(hwnd, ("����������������� ���鲻Ҫ����̫���ַ� �汾" + string(VERSION)).c_str(), "��ʾ", MB_OK);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
