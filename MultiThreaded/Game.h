#pragma once
#include "stdafx.h"
#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

// Create a programmer defined Windows Message.
#define WM_SFX WM_APP+1

enum { LeftTurn = 0, RightTurn = 1 };
class Game
{
	// Window Handle:
	HWND hWnd;

	// The templated function used for cleaning up COM objects
	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}
	HRESULT CreateGraphics(HWND hWnd);
	void DestroyGraphics(void);
	#define D2DColor(clr) D2D1::ColorF(D2D1::ColorF::clr)

	// File image loader function.
    HRESULT LoadBitmapFromFile(
        ID2D1RenderTarget *pRenderTarget,
        IWICImagingFactory *pIWICFactory,
        PCWSTR uri,
        UINT destinationWidth,
        UINT destinationHeight,
        ID2D1Bitmap **ppBitmap
        );

	HRESULT LoadBitmapFromResources(
		ID2D1RenderTarget *pRenderTarget,
		IWICImagingFactory *pIWICFactory,
		PCWSTR resourceName,
		PCWSTR resourceType,
		UINT destinationWidth,
		UINT destinationHeight,
		ID2D1Bitmap **ppBitmap
		);

	// Direct2D Interfaces:
	ID2D1Factory* pD2DFactory;
	ID2D1HwndRenderTarget* pRT;
	ID2D1SolidColorBrush *pBrush;

	D2D1_COLOR_F leftPaddle;
	D2D1_COLOR_F rightPaddle;
	D2D1_COLOR_F Ball;
	D2D1_COLOR_F bg;
	D2D1_COLOR_F score;

	

	ID2D1Bitmap* background; // The Background Image.
	ID2D1Bitmap* sprite;	 // The Sprite Image.
	ID2D1Bitmap* Rightsprite;	 // The Sprite Image.
	ID2D1Bitmap* GhostSprite;	 // The Sprite Image.
	ID2D1Bitmap* RedSprite;		// The Sprite Image.
	ID2D1Bitmap* PinkSprite;	 // The Sprite Image.
	ID2D1Bitmap* TealSprite;	 // The Sprite Image.
	ID2D1Bitmap* TanSprite;	 // The Sprite Image.
	ID2D1Bitmap* particle;	 // The Particle Image.
	ID2D1Bitmap* background2;	 // The Particle Image.
	ID2D1Bitmap* PacLife;
	ID2D1Bitmap* titlepac;
	ID2D1Bitmap* cloud;
	ID2D1RadialGradientBrush *gBrush;

	// DirectWrite Interfaces:
	IDWriteFactory* pDWFactory;
	IDWriteTextFormat* pTF;
	IDWriteTextFormat* pTFF;	
	IDWriteTextFormat* pTFFF;

	// Windows Imaging Component Interfaces:
	IWICImagingFactory* pWICFactory; // Starting point for WIC.

	// Game Objects:
	D2D1_RECT_F ballpos;
	D2D1_SIZE_F ballvec;

	D2D1_RECT_F padpos;
	D2D1_SIZE_F padvec;

	D2D1_RECT_F Lpadpos;
	D2D1_SIZE_F Lpadvec;

	D2D1_RECT_F Tpadpos;
	D2D1_SIZE_F Tpadvec;

	//Game Objects for PacMan
	D2D1_RECT_F ballpos1;
	D2D1_SIZE_F ballvec1;
	D2D1_RECT_F ballpos2;
	D2D1_SIZE_F ballvec2;
	D2D1_RECT_F ballpos3;
	D2D1_SIZE_F ballvec3;
	D2D1_RECT_F ballpos4;
	D2D1_SIZE_F ballvec4;
		D2D1_RECT_F ballpos5;
	D2D1_SIZE_F ballvec5;
		D2D1_RECT_F ballpos6;
	D2D1_SIZE_F ballvec6;
		D2D1_RECT_F ballpos7;
	D2D1_SIZE_F ballvec7;
		D2D1_RECT_F ballpos8;
	D2D1_SIZE_F ballvec8;
		D2D1_RECT_F ballpos9;
	D2D1_SIZE_F ballvec9;
		D2D1_RECT_F ballpos10;
	D2D1_SIZE_F ballvec10;

	// Variables used to calculate frames per second:
	DWORD dwFrames;
	DWORD dwCurrentTime;
	DWORD dwLastUpdateTime;
	DWORD dwElapsedTime;
	std::wstring szFPS;

	FMOD::Sound *music;
	FMOD::Channel* channel;
	FMOD_RESULT result;
	FMOD::System *run;

public:
	BOOL bRunning;		// Is the game running?
	HANDLE hGameThread; // Handle to the GameMain thread.
	BYTE keys[256];		// Current state of the keyboard.

	Game(void);
	virtual ~Game(void);
	void ifCheckWin (HWND hWnd);
	void RenderInstructions(void);
	void MusicLoop (void);
	void Startup(void);
	void reStart(void);
	void Shutdown(void);
	void Input(void);
	void Simulate(void);
	void SimulateDemo(void);
	void SimulatePacMan(void);
	void Render1975(void);
	void RenderPac(void);
	void RenderDemo(void);
	void RenderTitle(void);
	void SetHwnd(HWND _hWnd) {hWnd = _hWnd;}
	int GetLeftWins() { return lWins; }
	void SetLeftWins(int x) { lWins = x; }
	int GetRightWins() { return rWins; }
	void SetRightWins(int o) { rWins = o; }
	int GetTurn() { return turn; }
	void SetTurn(int x) { turn = x; }
	D2D1_COLOR_F GetLeftPaddle() { return leftPaddle; }
	void SetLeftPaddle(D2D1_COLOR_F x) { leftPaddle = x; }
	void SetRightPaddle(D2D1_COLOR_F x) { rightPaddle = x; }
	void SetBall(D2D1_COLOR_F x) { Ball = x; }
	void SetBG(D2D1_COLOR_F x) { bg = x; }
	void SetScore(D2D1_COLOR_F x) { score = x; }
	void RenderPause (void);

	bool OutOfBounds( float y) const;
	bool Play;
	bool PlayPac;
	bool PlayDemo;
	bool title;
	bool AI;
	bool Ghost;
	bool FPS;
	bool instruc;
	bool pause;


private:
	int lWins, rWins;
	bool win;
	float lineWidth;
	int turn;
	bool incoming;
	bool stop;
};

//// {E0C640A1-CD0C-429B-B1C9-AF3A6A49BBE0}
//static const GUID myguid = 
//{ 0xe0c640a1, 0xcd0c, 0x429b, { 0xb1, 0xc9, 0xaf, 0x3a, 0x6a, 0x49, 0xbb, 0xe0 } };

//PlaySound
//Beep
//MessageBeep
//
//FMOD, XAudio2