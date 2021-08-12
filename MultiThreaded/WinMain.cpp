#include "stdafx.h"
#include "Game.h" // Include the Game class.
#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

// Foward Declarations:
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void OnPaint(HWND hWnd);
void OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

FMOD::System *system2;
FMOD_RESULT result;
FMOD::Sound *music;
FMOD::Sound *pac;
FMOD::Channel* channel;


Game object;
bool full = false;

// Entry point function for the game:
unsigned int WINAPI GameMain(void* lpvArgList);

// Function that sets the display's resolution:
void SetDisplayMode(int iWidth, int iHeight, int iBpp, int iRefreshRate);

// Function that toggles a window between fullscreen and windowed mode:
void ToggleFullscreenMode(HWND hWnd, BOOL bFullScreen,
			int iWidth, int iHeight, int iBpp, int iRefreshRate);

// The WinMain function is the entry point for a Graphical Windows Application.
int CALLBACK _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	lpCmdLine = lpCmdLine;
	hPrevInstance = hPrevInstance;
	// Initialize the COM Library.
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Step 1: Register a Window Class
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(wcex);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hInstance = hInstance;
	wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpfnWndProc = WindowProc; // Identify which function will be doing message processing.
	wcex.lpszClassName = _T("CLASS NAME"); // Identify these window's attributes by name.
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wcex);

	// Step 2: Create the Main Window
	HWND hWnd = CreateWindow(wcex.lpszClassName, _T("Pong"),WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN ,CW_USEDEFAULT,	//BUTTON - CLIPCHILDREN
							CW_USEDEFAULT,818, 640,HWND_DESKTOP,NULL,hInstance,NULL);
	if (!hWnd)
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to create main window"),
			_T("Program Name"), MB_OK | MB_ICONERROR);
		return -1;
	}

	//int CALLBACK _tWinMain(
	
	
	result = FMOD::System_Create(&system2);
	if (result != FMOD_OK)
		exit (-1);
	result = system2->init(1, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		exit (-1);								//FMOD_DEFAULT	-just plays a sound
	result = system2->createStream("pacman_beginning.wav", FMOD_DEFAULT, 0, &music);
	result = system2->createStream("pacman_chomp.wav",FMOD_LOOP_NORMAL, 0, &pac);	//this plays music loop FMOD_LOOP_NORMAL
	result = system2->playSound(FMOD_CHANNEL_FREE, music, false, &channel);
	if (result != FMOD_OK)
		exit(-1);

	//if (object.PlayPac)
	//{
	//result = system->createStream("pacman_chomp.wav",FMOD_LOOP_NORMAL, 0, &music);	//this plays music loop FMOD_LOOP_NORMAL
	//result = system->playSound(FMOD_CHANNEL_FREE, music, false, &channel);
	//if (result != FMOD_OK)
	//	exit(-1);
	//}
	//SET VOLUME
	result = channel->setVolume(0.25f);	//255 high, 0 low

	//SetWindowText - Change the window title bar after the window is created

	//Game pong; // The Game class.

	//// Attach the game class pointer to the HWND handle
	//SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)&pong);

	// Retrieve the user data section which contains the game class pointer.
	//Game* pong = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	// Create the Game Thread:
	object.hGameThread = (HANDLE)_beginthreadex(NULL, 0,
		GameMain, (void*)hWnd, 0, NULL);
#ifdef _DEBUG
	Sleep(1);  // Sleep after thread creation for debugging to work properly.
#endif

	// Step 3: Show the Main Window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd); // Send the first WM_PAINT message.

	HACCEL haccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	haccel = haccel;

	// Step 4: Enter the Main Message Loop
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))// GetMessage returns false when the WM_QUIT message shows up.
	{
			if (! TranslateAccelerator(hWnd, haccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); // Invoke the proper window procedure to do message processing
		} // Invoke the proper window procedure to do message processing
	}

	// Destroy the Game Thread:
	CloseHandle(object.hGameThread);

	// Uninitialize the COM Library.
	CoUninitialize();

	return msg.wParam;
}

// Entry point function for the game:
unsigned int WINAPI GameMain(void* lpvArgList)
{
	// Initialize the COM Library.
	CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE);

	// Increase the accuracy/precision of the Windows timer:
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	timeBeginPeriod(tc.wPeriodMin);

	// Get the HWND value out of the ArgList parameter.
	HWND hWnd = (HWND)lpvArgList;

	//// Retrieve the user data section which contains the game class pointer.
	//Game* pong = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	object.SetHwnd(hWnd);
	object.Startup();



		/*object.RenderPause();*/
	while (object.bRunning)
	{
		if (object.title)
		{
		object.RenderTitle();			//new title image from paint (create a new function)
		}

		if (object.Play)				//BUTTON
		{
			if (!object.pause)
			{
				object.Input();				
				object.Simulate();
				object.Render1975();
				object.ifCheckWin(hWnd);
			}
			else
				object.RenderPause();
		}

		if (object.PlayPac)
		{
		object.Input();				
		object.SimulatePacMan();
		object.RenderPac();
		}

		if (object.PlayDemo)
		{
		object.Input();
		object.SimulateDemo();
		object.RenderDemo();
		}

		if (object.instruc)
		{
		object.RenderInstructions();
		}

		// 1 Frame == 1 iteration of Input, Simulate, & Render
	}
	

	object.Shutdown();

	// Restore the accuracy/precision of the Windows timer:
	timeEndPeriod(tc.wPeriodMin);

	// Uninitialize the COM Library.
	CoUninitialize();

	return 0;
}

#define ID_BUTTON 1001
#define ID_BUTTON2 1002
#define ID_BUTTON3 1003
// Step 5: Implement the Window Procedure.
LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//// Retrieves the user data section which contains the game pointer.
	//Game* pong = (Game*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (uMsg)
	{
	// Window Creation:
	case WM_CREATE:

		if (object.title)
		{
		CreateWindow(_T("button"),  _T("1975 Pong"),						//BUTTON
					WS_VISIBLE | WS_CHILD, 450, 50, 300, 100,
					hWnd, (HMENU) ID_BUTTON, NULL, NULL);

				CreateWindow(_T("button"),  _T("Pac-Man Pong"),				//BUTTON
					WS_VISIBLE | WS_CHILD, 450, 200, 300, 100,
					hWnd, (HMENU) ID_BUTTON2, NULL, NULL);

				CreateWindow(_T("button"),  _T("DEMO"),				//BUTTON
					WS_VISIBLE | WS_CHILD, 450, 350, 300, 100,
					hWnd, (HMENU) ID_BUTTON3, NULL, NULL);
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break; //return -1 to abort window creation.

	// Window Destruction:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	// Handle Close Button [X] Pressed:
	case WM_CLOSE:
		object.bRunning = FALSE; // Begin GameMain shutdown.

		// Wait until Game Thread terminates:
		WaitForSingleObject(object.hGameThread, INFINITE);

		DestroyWindow(hWnd); // Begin WinMain shutdown.
		break;

	// Window Size Change:
	case WM_SIZE:
		OnSize(hWnd, wParam, lParam);
		break;

	// Window Background Rendering:
	case WM_ERASEBKGND:
		return TRUE;

	// Window Rendering:
	case WM_PAINT:
		OnPaint(hWnd);
		break;

	// Handle keyboard input:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		GetKeyboardState(object.keys);
		DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;

	// Handle window activation:
	case WM_ACTIVATE:
		if (LOWORD(wParam) != WA_INACTIVE)
			GetKeyboardState(object.keys);
		else
			ZeroMemory(object.keys, 256);
		break;

	// Left Mouse Button Pressed:
	case WM_LBUTTONDOWN:
		OnLButtonDown(hWnd, wParam, lParam);
		break;

	// Menu Item Selected:
	case WM_COMMAND:
		OnCommand(hWnd, wParam, lParam);

			
		if (LOWORD(wParam) == ID_BUTTON)		
		{
			object.Play = true;
			object.PlayPac = false;
			object.title = false;
			object.AI = false;
			object.PlayDemo = false;
			object.instruc = false;

			SetForegroundWindow(hWnd);
			SetFocus(hWnd);
			HWND hButton = GetDlgItem(hWnd, ID_BUTTON);
			ShowWindow(hButton, SW_HIDE);
			HWND hButton2 = GetDlgItem(hWnd, ID_BUTTON2);
			ShowWindow(hButton2, SW_HIDE);
			HWND hButton3 = GetDlgItem(hWnd, ID_BUTTON3);
			ShowWindow(hButton3, SW_HIDE);
		}
		if (LOWORD(wParam) == ID_BUTTON2)		
		{
			object.PlayPac = true;
			object.Play = false;
			object.title = false;
			object.AI = false;
			object.PlayDemo = false;
			object.instruc = false;

			SetForegroundWindow(hWnd);
			SetFocus(hWnd);
			HWND hButton = GetDlgItem(hWnd, ID_BUTTON);
			ShowWindow(hButton, SW_HIDE);
			HWND hButton2 = GetDlgItem(hWnd, ID_BUTTON2);
			ShowWindow(hButton2, SW_HIDE);
			HWND hButton3 = GetDlgItem(hWnd, ID_BUTTON3);
			ShowWindow(hButton3, SW_HIDE);
		
		result = system2->playSound(FMOD_CHANNEL_REUSE, pac, false, &channel);
		if (result != FMOD_OK)
			exit(-1);
		result = channel->setVolume(0.25f);
		}
		if (LOWORD(wParam) == ID_BUTTON3)		
		{
			object.AI = true;
			object.PlayDemo = true;
			object.PlayPac = false;
			object.Play = false;
			object.title = false;
			object.instruc = false;
			
			SetForegroundWindow(hWnd);
			SetFocus(hWnd);
			HWND hButton = GetDlgItem(hWnd, ID_BUTTON);
			ShowWindow(hButton, SW_HIDE);
			HWND hButton2 = GetDlgItem(hWnd, ID_BUTTON2);
			ShowWindow(hButton2, SW_HIDE);
			HWND hButton3 = GetDlgItem(hWnd, ID_BUTTON3);
			ShowWindow(hButton3, SW_HIDE);
		}
		break;

	// Do the default message processing:
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void OnPaint(HWND hWnd)
{
	ValidateRect(hWnd, NULL); //VERY IMPORTANT: Clear the invalid flag.
}

void OnLButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	wParam =wParam;
	POINT ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);

	InvalidateRect(hWnd, NULL, TRUE); // Tell Windows we need to repaint
}

void OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	lParam = lParam;
	wParam =wParam;
	hWnd =hWnd;
	// NOTE: Comment the following lines in to disable Direct2D auto-scaling.

    // Resize the Render Target based upon the new client.
    //D2D1_SIZE_U size = D2D1::SizeU(LOWORD(lParam), HIWORD(lParam));
    //pRT->Resize(size);
}

COLORREF clrarrCustom[16] =
{
	// Array of COLORREFs used to store the custom colors.
	RGB(255,0,255),RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),
	RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),
	RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),
	RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),RGB(255,255,255),
};


void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	lParam = lParam;
	int menuid = LOWORD(wParam);
	switch (menuid)
	{
	case IDM_MUTE_MUTE:
		{
			result = channel->setVolume(0.0f);	//255 high, 0 low
		}
		break;
	case IDM_MUTE_UNMUTE:
		{
			result = channel->setVolume(0.25f);	//255 high, 0 low
		}
		break;

	//case IDM_VOLUME_INCREASE:							//doesnt work.
	//	{
	//		if (result == 0)
	//		result = channel->setVolume(0.25f);
	//		if (result == 0.25)
	//		result = channel->setVolume(1.0f);
	//		if (result == 1.0f)
	//		result = channel->setVolume(1.5f);
	//		if (result == 1.5f)
	//		result = channel->setVolume(2.0f);
	//	}
	//	break;

	case IDM_FILE_NEW:	
		{
			object.SetRightWins(0);		//doesnt work
			object.SetLeftWins(0);
			object.SetHwnd(hWnd);
			object.reStart();
			HWND hButton = GetDlgItem(hWnd, ID_BUTTON);
			ShowWindow(hButton, SW_SHOW);
			HWND hButton2 = GetDlgItem(hWnd, ID_BUTTON2);
			ShowWindow(hButton2, SW_SHOW);
			HWND hButton3 = GetDlgItem(hWnd, ID_BUTTON3);
			ShowWindow(hButton3, SW_SHOW);
			
			result = system2->playSound(FMOD_CHANNEL_REUSE, music, false, &channel);
			result = channel->setVolume(0.25f);
		}
		break;

	case IDM_OPTIONS_INSTRUCTIONS:
		{
			if (object.instruc && !object.Play && !object.title)
			{
			object.instruc = false;
			object.Play = true;
			object.title = false;
			}
			else if (!object.instruc && object.Play)
			{
				object.instruc = true;
				object.Play = false;
				object.title = false;
			}

			else if (object.instruc && !object.title)
			{
				object.instruc = false;
				object.title = true;
				object.Play = false;
			}
			else if (!object.instruc && object.title)
			{
				object.instruc = true;
				object.title = false;
				object.Play = false;
			}
		
		}
		break;

	case IDM_DISPLAY_FPS:						//doesnt work
		{
			if (object.FPS == false)
			{
				object.FPS = true;
				break;
			}
			else if (object.FPS == true)
			{
				object.FPS = false;
				break;
			}
		}
		break;

	case IDM_FILE_EXIT:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case IDM_DISPLAY_PAUSE:
		{
			if (object.pause == false)
			{
				object.pause = true;
				break;
			}
			else if (object.pause == true)
			{
				object.pause = false;
				break;
			}
		}
//--------------------------------- COLORS -----------------------------------------

	case IDM_COLORS_RANDOM40033:
		{
			srand((unsigned int)time(nullptr));
			COLORREF ranColor = RGB(rand() % 256,rand() % 256,rand() % 256);

					object.SetBG( D2D1::ColorF(GetRValue(ranColor)<<16|GetGValue(ranColor)<<8|GetBValue(ranColor)));
					ranColor = RGB(rand() % 256,rand() % 256,rand() % 256);

					object.SetScore( D2D1::ColorF(GetRValue(ranColor)<<16|GetGValue(ranColor)<<8|GetBValue(ranColor)));
					ranColor = RGB(rand() % 256,rand() % 256,rand() % 256);

					object.SetBall(D2D1::ColorF(GetRValue(ranColor)<<16|GetGValue(ranColor)<<8|GetBValue(ranColor)));
					ranColor = RGB(rand() % 256,rand() % 256,rand() % 256);

					object.SetLeftPaddle( D2D1::ColorF(GetRValue(ranColor)<<16|GetGValue(ranColor)<<8|GetBValue(ranColor)));
					ranColor = RGB(rand() % 256,rand() % 256,rand() % 256);

					object.SetRightPaddle( D2D1::ColorF(GetRValue(ranColor)<<16|GetGValue(ranColor)<<8|GetBValue(ranColor)));
					ranColor = RGB(rand() % 256,rand() % 256,rand() % 256);

				// Repaint the screen.
			InvalidateRect(hWnd, NULL, TRUE);
		}
	break;

	case IDM_COLORS_SCOREBOARD40028:
		{
			CHOOSECOLOR cc;
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.Flags = CC_RGBINIT;
			cc.lpCustColors = clrarrCustom;

			if(ChooseColor(&cc))
			{
				// NOTE: The selected color is stored in the cc.rgbResult variable.
				object.SetScore(D2D1::ColorF(GetRValue(cc.rgbResult)<<16|				//doesnt work... not changing color!
					GetGValue(cc.rgbResult)<<8|
					GetBValue(cc.rgbResult)));

				// Repaint the screen.
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;

	case IDM_COLORS_BACKGROUND40029:
		{
			CHOOSECOLOR cc;
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.Flags = CC_RGBINIT;
			cc.lpCustColors = clrarrCustom;

			if(ChooseColor(&cc))
			{
				// NOTE: The selected color is stored in the cc.rgbResult variable.
				object.SetBG(D2D1::ColorF(GetRValue(cc.rgbResult)<<16|				//doesnt work... not changing color!
					GetGValue(cc.rgbResult)<<8|
					GetBValue(cc.rgbResult)));

				// Repaint the screen.
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;

	case IDM_COLORS_BALL40030:
		{
			CHOOSECOLOR cc;
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.Flags = CC_RGBINIT;
			cc.lpCustColors = clrarrCustom;

			if(ChooseColor(&cc))
			{
				// NOTE: The selected color is stored in the cc.rgbResult variable.
				object.SetBall(D2D1::ColorF(GetRValue(cc.rgbResult)<<16|				//doesnt work... not changing color!
					GetGValue(cc.rgbResult)<<8|
					GetBValue(cc.rgbResult)));

				// Repaint the screen.
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;

	case IDM_COLORS_LEFTPADDLE40031:
		{
			CHOOSECOLOR cc;
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.Flags = CC_RGBINIT;
			cc.lpCustColors = clrarrCustom;

			if(ChooseColor(&cc))
			{
				// NOTE: The selected color is stored in the cc.rgbResult variable.
				object.SetRightPaddle(D2D1::ColorF(GetRValue(cc.rgbResult)<<16|				//doesnt work... not changing color!
					GetGValue(cc.rgbResult)<<8|
					GetBValue(cc.rgbResult)));

				// Repaint the screen.
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;

		case IDM_COLORS_RIGHTPADDLE40032:
		{
			CHOOSECOLOR cc;
			ZeroMemory(&cc, sizeof(CHOOSECOLOR));
			cc.lStructSize = sizeof(CHOOSECOLOR);
			cc.hwndOwner = hWnd;
			cc.Flags = CC_RGBINIT;
			cc.lpCustColors = clrarrCustom;

			if(ChooseColor(&cc))
			{
				// NOTE: The selected color is stored in the cc.rgbResult variable.
				object.SetLeftPaddle(D2D1::ColorF(GetRValue(cc.rgbResult)<<16|
					GetGValue(cc.rgbResult)<<8|
					GetBValue(cc.rgbResult)));

				// Repaint the screen.
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;
	
	case IDM_OPTIONS_FULLSCREEN:
			// Go Fullscreen Mode.
		{
		//HDC hdc = GetDC(hWnd);

		if (full == true)
			full = false;
		else if (full == false)
			full = true;

			if (full == true)
			{
			ToggleFullscreenMode(hWnd, TRUE,0, 0, 0, 0);
			}

			else if (full == false)
			{
			ToggleFullscreenMode(hWnd, FALSE,0, 0, 0, 0);
			}
		}
		break;
	}
}
// Function that sets the display's resolution:
void SetDisplayMode(int iWidth, int iHeight, int iBpp, int iRefreshRate)
{
	if(iWidth == 0 && iHeight == 0 && iBpp == 0 && iRefreshRate == 0)
	{
		// Restore display settings to those stored in the registry.
		ChangeDisplaySettings(NULL, 0);
		return;
	}

	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);

	int i = 0;
	while(EnumDisplaySettings(NULL, i++, &dm))
	{
		// Iterate through the display settings until a match is found.
		if(dm.dmPelsWidth == (unsigned int)iWidth && dm.dmPelsHeight == (unsigned int)iHeight &&
			dm.dmBitsPerPel == (unsigned int)iBpp && dm.dmDisplayFrequency == (unsigned int)iRefreshRate)
		{
			dm.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

			// Put the new settings into effect.
			if(ChangeDisplaySettings(&dm, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
			{
				return;
			}
		}
	}
}

// Function that toggles a window between fullscreen and windowed mode:
void ToggleFullscreenMode(HWND hWnd, BOOL bFullScreen, int iWidth, int iHeight, int iBpp, int iRefreshRate)
{
	static WINDOWPLACEMENT wp = {0};
	static HMENU hMenu = NULL;

	if(bFullScreen)
	{
		// Remember the window position.
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hWnd, &wp);
		// Remove the window's title bar.
		SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP  | WS_CLIPCHILDREN);
		// Put the changes to the window into effect.
		SetWindowPos(hWnd, 0, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		// Remember the menu, then remove it.
		hMenu = GetMenu(hWnd);
		SetMenu(hWnd, NULL);
		// Switch to the requested display mode.
		SetDisplayMode(iWidth, iHeight, iBpp, iRefreshRate);
		// Position the window to cover the entire screen.
		SetWindowPos(hWnd, HWND_TOPMOST,
			0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_SHOWWINDOW);
		// Remove the cursor.
		ShowCursor(FALSE); // SetCursor(NULL);
	}
	else
	{
		// Restore the window's title bar.
		SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
		// Put the changes to the window into effect.
		SetWindowPos(hWnd, 0, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		// Restore the window's menu.
		SetMenu(hWnd, hMenu);
		// Restore the display mode.
		SetDisplayMode(0, 0, 0, 0);
		// Restore the window's original position.
		SetWindowPlacement(hWnd, &wp);
		// Restore the cursor.
		ShowCursor(TRUE);
	}
}





// Win32 API
//	- The API (Application Programming Interface) is written in the C language.
//	- This means no classes, no friend function, no default arguments, no bool keyword
//	- There will be many new types to use
//	- BOOL, TRUE, FALSE =  bool, true, false
//	- BYTE, WORD, DWORD, __int64
//	- CHAR, WCHAR, TCHAR
//	- LPSTR, LPWSTR, LPTSTR
//	- HANDLE, HINSTANCE, HWND, HDC, HICON, HCURSOR

//char - 1 byte per character - "ASCII"
//wchar_t - 2 bytes per character - L"UNICODE"
//TCHAR* string = _T("TCHAR string")

//// Convert numbers into strings
//wostringstream oss;
//oss << "My Name is Rob" << 3000;
//oss.str() // Give me the string.
//oss.str().c_str() // Give me the char pointer.
//oss.str(_T("")); // Clear the ostringstream