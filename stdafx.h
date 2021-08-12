// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <Windows.h>
// Include the Windows Multimedia Library:
#include <mmsystem.h>               // Multimedia header file.
#pragma comment(lib, "winmm.lib")   // Multimedia import library.
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <process.h>
#include <tchar.h> // Include support for UNICODE.
#include <sstream> // Include support for ostringstream.
using std::wostringstream;
#include <d2d1.h> // Include the Direct2D API.
#pragma comment(lib, "d2d1.lib") // Connect to the Direct2D Import Library.
#include <dwrite.h> // Include the DirectWrite API.
#pragma comment(lib, "dwrite.lib") // Connect to the DirectWrite Import Library.
#include <commdlg.h> // Include the Common Dialogs.
#include "resource.h" // Include the resource Ids.
#include <ctime>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")
//-------------------------FMOD -----------------------
#include <stdio.h>
#include <conio.h>


