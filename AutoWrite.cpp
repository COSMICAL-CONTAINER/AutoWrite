#include<windows.h>
#include<string>
#include<iostream>


using namespace std; 	//makes main() shorter and easier to type.

#define VERSION "0.0.3"
#define IDI_MYICON 101

void write_word(int vk)//输入数字、字母 
{
	keybd_event(vk,0,0,0);
	keybd_event(vk,0,KEYEVENTF_KEYUP,0);
} 
 
void WriteCharacter(int vk)//输入符号 
{
    switch(vk)
    {
        case '-':
            keybd_event(VK_OEM_MINUS,0,0,0);	
            keybd_event(VK_OEM_MINUS,0,KEYEVENTF_KEYUP,0);
            break;
        case '.':
            keybd_event(VK_OEM_PERIOD,0,0,0);	
            keybd_event(VK_OEM_PERIOD,0,KEYEVENTF_KEYUP,0);
            break;
        case ' ':
            keybd_event(0x20,0,0,0);	
            keybd_event(0x20,0,KEYEVENTF_KEYUP,0);
            break;
    }
}
 
void Enter()
{
	keybd_event(13,0,0,0);	
	keybd_event(13,0,KEYEVENTF_KEYUP,0);
}
 
void ShiftDown()//按下shift 
{
	keybd_event(16,0,0,0);	
}
 
void ShiftUp()//弹起shift 
{
	keybd_event(16,0,KEYEVENTF_KEYUP,0);
}
 
void CtrlV()//粘贴 
{
	keybd_event(162,0,0,0);	
	write_word('V');
	keybd_event(162,0,KEYEVENTF_KEYUP,0);
}

void AutoWrite(string word)//输入一串句子 
{
	size_t n = word.length();
	for(size_t i = 0;i < n; i++)
	{
		int zm = word.at(i);
		if((zm >= 'a' && zm <= 'z'))
		{
			write_word(zm - 32);
		}
        else if( zm >= 'A' && zm <= 'Z' )
        {
            ShiftDown();
            write_word(zm);
            ShiftUp();
        }
		else if((zm >= '0' && zm <= '9'))
		{
			write_word(zm);
		}
        else if(zm == ' ' || zm == '.'|| zm == ' ')
        {
            WriteCharacter(zm);
        }
		Sleep(10);
	}
}
 
// int main()
// {
// 	cout<<"程序开始!"<<endl;
// 	Sleep(2000);
// 	for(int i=0;i< 10;i++)
// 	{
// 		// AutoWrite("BOZHUZUISHUAI1");//这里得输入大写字母才能打小写，1是拼音键盘上的词语选择。  
// 		juzi("ABcdaDsA");//这里得输入大写字母才能打小写，1是拼音键盘上的词语选择。  
// 		Enter();
// 	}
// 	return 0;
// }

int DisplayConfirmSaveAsMessageBox()
{
    int msgboxID = MessageBox(
        NULL,
        "temp.txt already exists.\nDo you want to replace it?",
        "Confirm Save As",
        MB_ICONEXCLAMATION | MB_YESNO
    );

    if (msgboxID == IDYES)
    {
        // TODO: add code
    }

    return msgboxID;    
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEditName;
    switch (msg)
    {
		case WM_CREATE:
		{
				// 创建文本框 

			hEditName = CreateWindow(
				TEXT("Edit"), 
				TEXT(""),
				WS_CHILD | WS_VISIBLE | WS_BORDER /*边框*/ | WS_TABSTOP /*允许使用TAB选中*/ | ES_MULTILINE,
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
                int n = GetWindowTextLength(hEditName); // 获取文本框中的文本长度 描述：获取文本框中的文本长度;
				if (n > 0) 
                {
                    TCHAR* szBuffer = (TCHAR*)malloc(sizeof(char) * (n + 1));              // 缓冲区
                    GetWindowText(hEditName, szBuffer, n);  // 获取文本
                    szBuffer[n] = '\0';
                    
                    for (auto c : string(szBuffer)) 
                    {
                        // 检查字符串2是否只包含数字和字母
                        if (!isalnum(c)) {isrightstr = false; break;}
                    }
                    if(isrightstr)
                    {
                        MessageBox(NULL, "请在关闭我之后马上去到想要输入文字的框中", "提示", MB_OK);
                        
                        // 显示文本
                        // MessageBox(hwnd, szBuffer, TEXT("Edit Control"), MB_OK);
                        Sleep(2000);
                        AutoWrite(szBuffer);//这里得输入大写字母才能打小写，1是拼音键盘上的词语选择。 
                    }
                    else
                    {
                        MessageBox(NULL, "不是有效的字符串", "提示", MB_OK);
                    }
                    free(szBuffer); 
				}
                else
                {
                    MessageBox(NULL, "你TM倒是输入啊", "提示", MB_OK);
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

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    // HICON hIcon = (HICON)LoadImage(NULL, "MYICON.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));

    wc.hIcon = hIcon;

    // 其他窗口类成员的初始化
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        ("AutoWrite - 自动打字机 作者：寰宇体 V"+ string(VERSION)).c_str(),   // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        100, 100, 800, 600,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    // Add button
    CreateWindow(
        "BUTTON",                       // Predefined class; Unicode assumed 
        "点我开始",                     // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        350,                             // x position 
        525,                             // y position 
        100,                            // Button width
        30,                             // Button height
        hwnd,                           // Parent window
        (HMENU)1,                       // Button identifier.
        (HINSTANCE)(LONG_PTR)GetWindowLong(hwnd, GWLP_HINSTANCE),
        NULL);                          // Pointer not needed.


    ShowWindow(hwnd, nCmdShow);

	MessageBox(hwnd, ("本代码由寰宇体制作 建议不要输入太多字符 版本" + string(VERSION)).c_str(), "提示", MB_OK);
	
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
