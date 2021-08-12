#include "stdafx.h"
#include "Game.h"

Game::Game(void)
{
	bRunning = TRUE;
	hWnd = NULL;
	ZeroMemory(keys, sizeof(keys));

	pD2DFactory = NULL;
	pRT = NULL;
	pTFF = NULL;
	pBrush = NULL;
	gBrush = NULL;

	leftPaddle = D2DColor(White);
	rightPaddle = D2DColor(White);
	Ball = D2DColor(White);
	bg = D2DColor(Black);
	score = D2DColor(White);

	background = NULL;
	sprite = NULL;
	particle = NULL;


	pDWFactory = NULL;
	pTF = NULL;
	pTFF = NULL;

	pWICFactory = NULL;

	lWins = 0;
	rWins = 0;
	win = false;
	lineWidth = 10;
	Play = false;
	turn = 1;
	PlayPac = false;
	title = true;
	incoming = false;
	stop = false;
	AI = false;
	PlayDemo = false;
	Ghost = false;
	FPS = false;
	instruc = false;
	pause = false;

	// Zero out the frames per second variables:
	dwFrames = 0;
	dwCurrentTime = 0;
	dwLastUpdateTime = 0;
	dwElapsedTime = 0;
	szFPS = _T("");
}

void Game::MusicLoop (void)
{
	result = FMOD::System_Create(&run);
	if (result != FMOD_OK)
		exit (-1);
	result = run->init(1, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		exit (-1);								//FMOD_DEFAULT	-just plays a sound

	result = run->createStream("pacman_chomp.wav",FMOD_LOOP_NORMAL, 0, &music);	//this plays music loop FMOD_LOOP_NORMAL
	result = run->playSound(FMOD_CHANNEL_FREE, music, false, &channel);
	if (result != FMOD_OK)
		exit(-1);
	
	//SET VOLUME
	result = channel->setVolume(0.25f);	//255 high, 0 low
}

Game::~Game(void)
{
}

void Game::reStart(void)
{
		win = true;

		if(PlayPac)
		{
			PlayPac = false;
			Play = false;
			title = true;
			PlayDemo = false;
		}
		if(Play)
		{
			Play = false;
			title = true;
			PlayDemo = false;
			PlayPac = false;
		}
		if(PlayDemo)
		{
			PlayDemo = false;
			Play = false;
			PlayPac = false;
			title = true;
		}

		AI = false;
		lWins = 0;
		rWins = 0;	
		RenderTitle();

		HMENU hmenu = GetMenu(hWnd);
		CheckMenuItem(hmenu,IDM_ACHIEVEMENTS_LEFTPADDLEWINS3TIMES, MF_UNCHECKED);
		CheckMenuItem(hmenu,IDM_ACHIEVEMENTS_RIGHTPADDLEWINS3TIMES, MF_UNCHECKED);
		CheckMenuItem(hmenu,IDM_ACHIEVEMENTS_LEFTPADDLEWINS, MF_UNCHECKED);
};

void Game::Startup(void)
{
	//TODO: Create Back Buffer

	// Initialize Direct2D and DirectWrite.
	HRESULT hr = CreateGraphics(hWnd);
	if (FAILED(hr))
		return;
	//TODO: Load Bitmaps/Brushes/Pens/Fonts

	//TODO: Initialize Game Components

	ballpos = D2D1::RectF(20, 20, 60, 60);						//this is the ball starting point
	ballvec = D2D1::SizeF(3, 3);

	//------------------------------------- RIGHT -------------------------------------------
	padpos = D2D1::RectF(pRT->GetSize().width-30, 300, pRT->GetSize().width-10, 400);	//this is the right paddel
	padvec = D2D1::SizeF(0, 4);
	//------------------------------------- LEFT -------------------------------------------
	Lpadpos = D2D1::RectF(30, 300, 10, 400);					//this is the left paddel
	Lpadvec = D2D1::SizeF(0, 4);
	//------------------------------------- TOP -------------------------------------------
	Tpadpos = D2D1::RectF(300, 30, 400, 10);					//this is the left paddel
	Tpadvec = D2D1::SizeF(4, 0);

	//PacMan balls-------------------------------------------------------------------------------------
	ballpos1 = D2D1::RectF(400,400,420, 420);						//Pellet 1
	ballvec1 = D2D1::SizeF(5, 5);
	ballpos2 = D2D1::RectF(100,100,120, 120);						//Pellet 2
	ballvec2 = D2D1::SizeF(3, 5);
	ballpos3 = D2D1::RectF(20, 20, 65, 65);							//RedGhost
	ballvec3 = D2D1::SizeF(4, 4);
	ballpos4 = D2D1::RectF(200, 200, 230, 230);						//Power Pellet 1
	ballvec4 = D2D1::SizeF(6, 3);
	ballpos5 = D2D1::RectF(280, 280, 300, 300);						//Pellet 3
	ballvec5 = D2D1::SizeF(4, 6);
	ballpos6 = D2D1::RectF(500, 500, 520, 520);						//Pellet 4
	ballvec6 = D2D1::SizeF(1, 2);
	ballpos7 = D2D1::RectF(0, 0, 45, 45);							//TealGhost
	ballvec7 = D2D1::SizeF(2, 2);
	ballpos8 = D2D1::RectF(300, 300, 345,345);						//PinkGhost
	ballvec8 = D2D1::SizeF(3, 4);
	ballpos9 = D2D1::RectF(150, 150, 195, 195);						//TanGhost
	ballvec9 = D2D1::SizeF(5, 2);
	ballpos10 = D2D1::RectF(620, 620, 650, 650);					//Power Pellet 2
	ballvec10 = D2D1::SizeF(6, 6);
}

bool Game::OutOfBounds(float y) const 
{
	if ( y >= pRT->GetSize().height - 3 ||  y <= 0)	
		return true;
	else
		return false;
}

void Game::Shutdown(void)
{
	//TODO: Unload Bitmaps/Brushes/Pens/Fonts

	//TODO: Destroy Back Buffer

	// Shutdown Direct2D and DirectWrite.
	DestroyGraphics();
}

void Game::Input(void)
{
	if (keys['A'] & 0x80)
	{
		if (Tpadpos.left <= 1)
		{
			Tpadpos.left = 1;	//TOP paddle
		}
		else
		{
			//TOP paddle
			Tpadpos.left -= Tpadvec.width;
			Tpadpos.right -= Tpadvec.width;
		}
	}

		if (keys['D'] & 0x80)
	{
		if (Tpadpos.right >= pRT->GetSize().width)
		{
			Tpadpos.right = pRT->GetSize().width;	//TOP paddle
		}
		else
		{
			//TOP paddle
			Tpadpos.left += Tpadvec.width;
			Tpadpos.right += Tpadvec.width;
		}
	}

	if (keys['W'] & 0x80)
	{
		if (Lpadpos.top <= 1)
		{
			Lpadpos.top = 1;	//Left paddle
		}
		else
		{
			//Left paddle
			Lpadpos.top -= Lpadvec.height;
			Lpadpos.bottom -= Lpadvec.height;
		}
	}

	if (keys['S'] & 0x80)
	{
		if (OutOfBounds(Lpadpos.bottom))
		{
			//this is the bottom of the right Paddel
			Lpadpos.bottom = pRT->GetSize().height;
		}
		else
		{
			Lpadpos.top += Lpadvec.height;
			Lpadpos.bottom += Lpadvec.height;
		}
	}

	// Check if the Up Arrow Key is pressed:
	if (keys[VK_UP] & 0x80)
	{

		if (padpos.top <= 1)
		{
			//right paddle
			padpos.top = 1;
		}
		else
		{
			// rigth paddle up
			padpos.top -= padvec.height;
			padpos.bottom -= padvec.height;
		}
	}

	if (keys[VK_DOWN] & 0x80)
	{
		if (OutOfBounds(padpos.bottom))		
		{
			//this is the bottom of the right Paddel
			padpos.bottom = pRT->GetSize().height;
		}
		else
		{
			// right padel down
			padpos.top += padvec.height;
			padpos.bottom += padvec.height;
		}
	}
}

void Game::ifCheckWin (HWND hWnd)
{
	HMENU hmenu = GetMenu(hWnd);	
	
	if (lWins == 3)
	{
		CheckMenuItem(hmenu,IDM_ACHIEVEMENTS_LEFTPADDLEWINS3TIMES, MF_CHECKED);
		InvalidateRect(hWnd, NULL, TRUE);
	}
		if (rWins == 3)
	{
		CheckMenuItem(hmenu,IDM_ACHIEVEMENTS_RIGHTPADDLEWINS3TIMES, MF_CHECKED);
		InvalidateRect(hWnd, NULL, TRUE);
	}
	if (lWins == 5)
	{
		CheckMenuItem(hmenu,IDM_ACHIEVEMENTS_LEFTPADDLEWINS, MF_CHECKED);
		InvalidateRect(hWnd, NULL, TRUE);
		MessageBeep(0x00000030L);
		Sleep (5000);

	}

	if (rWins == 5)
	{
		MessageBeep(0x00000030L);
		Sleep (5000);
	}
	
};

void Game::SimulateDemo(void)
{
	//TODO: Simulate AI
	//TRACKING-------------------------------------------------------------------------------------------------------------------
	if (AI)
	{
		double ballCenter = (ballpos.bottom + ballpos.top) * 0.5;
		double padCenter = (padpos.bottom + padpos.top) * 0.5;

		if (ballCenter < padCenter && ballpos.right > (pRT->GetSize().width * 0.5) && padpos.top >= 1)
		{
			padpos.top -= padvec.height;
			padpos.bottom -= padvec.height;
		}
		if (ballCenter > padCenter && ballpos.right > (pRT->GetSize().width * 0.5) && padpos.bottom <= pRT->GetSize().height)
		{

			padpos.top += padvec.height;
			padpos.bottom += padvec.height;
		}
		//---------------------------------------------------------------------------------------------------------------------------

		//Efficient AI---------------------------------------------------------------------------------------------------------------
		if(ballvec.width > 0)	//moving right
		{
			incoming = false;
			stop = false;
		}
		else					//moving left
		{
			incoming = true;
		}
		if (incoming)	//LEFT
		{
			double holdvec = ballvec.height;
			double futureHeight = ballCenter;
			float futureWidth = ballpos.left;
			double LpadCenter = (Lpadpos.bottom + Lpadpos.top) * 0.5;
			double half = (ballpos.bottom - ballpos.top) * 0.5;
			while(futureWidth > 30)
			{
				futureWidth += ballvec.width;
				if (futureHeight + half > pRT->GetSize().height || futureHeight - half < 0)
				{
					holdvec = -holdvec;
				}
				futureHeight += holdvec;
			}
			if (futureHeight < 1)
			{
				futureHeight = 1;
			}
			if (futureHeight > pRT->GetSize().height)
			{
				futureHeight = pRT->GetSize().height;
			}
			if (stop == false)
			{
				if (futureHeight < LpadCenter)
				{
					if (Lpadpos.top >= 1 && !(LpadCenter >= futureHeight -1 && LpadCenter <= futureHeight -3))
					{
						Lpadpos.top -= padvec.height;
						Lpadpos.bottom -= padvec.height;
					}
				}//if (futureHeight
				else
				{
					if (Lpadpos.bottom <= pRT->GetSize().height && !(LpadCenter >= futureHeight + 1  && LpadCenter <= futureHeight +3))
					{
						Lpadpos.top += padvec.height;
						Lpadpos.bottom += padvec.height;
					}
				}
			}//if (!stop)
			if (LpadCenter >= futureHeight -1 && LpadCenter <= futureHeight -3)
			{
				stop = true;
			}
		}
	}
	//Simulate Physics
	ballpos.left += ballvec.width;
	ballpos.top += ballvec.height;
	ballpos.right += ballvec.width;
	ballpos.bottom += ballvec.height;

	//Simulate Collision Detection & Response
	if (ballpos.bottom > pRT->GetSize().height)
	{
		ballvec.height = -ballvec.height;
	}

	if (ballpos.top <= 0)
	{
		ballvec.height = -ballvec.height;
	}

	if (ballpos.right < 0)
	{
		if (win)
		{
			++rWins;
			++turn;
			win = false;
			Sleep(500);
			Startup();
		}
	}

	if (ballpos.left > pRT->GetSize().width)
	{
		if (win)
		{
			++lWins;
			win = false;
			++turn;
			Sleep(500);
			Startup();
		}
	}

	//---------------------------------------------PADDLES-----------------------------------------------------

	//RIGHT PADDLE
	RECT rIntersect;
	RECT rBall = {(LONG)(ballpos.left),(LONG)(ballpos.top),(LONG)(ballpos.right),(LONG)(ballpos.bottom)};
	RECT rPaddle = {(LONG)(padpos.left),(LONG)(padpos.top),(LONG)(padpos.right),(LONG)(padpos.bottom)};

	if(IntersectRect(&rIntersect, &rBall, &rPaddle))	//bounces the ball
	{
		ballvec.width = -ballvec.width;
		MessageBeep(0x00000040L);
	}
	//LEFT PADDLE
	RECT lPaddle = {(LONG)(Lpadpos.right),(LONG)(Lpadpos.top),(LONG)(Lpadpos.left),(LONG)(Lpadpos.bottom)}; 
	if(IntersectRect(&rIntersect, &rBall, &lPaddle))
	{
		ballvec.width = -ballvec.width;
		MessageBeep(0x00000040L);
	}
}

void Game::RenderInstructions(void)
{
	pRT->BeginDraw();
	//pRT->Clear(bg);
		pRT->Clear(bg);

	pBrush->SetColor(D2DColor(White));
		// Draw Shapes
	pBrush->SetColor(D2DColor(Violet));
	pRT->DrawRectangle(D2D1::RectF(30, 20, 350, 150),
		pBrush, 3);

	wostringstream oss;
	oss << "Pong - Score by bouncing the ball to the other side. The most points wins. \nPacMan Pong - Dodge the ghosts, eat power pellets to make the ghosts blue, then score by eatting the blue ghosts.";
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFFF, D2D1::RectF(50,50,300,300), pBrush);
	pBrush->SetColor(D2DColor(White));
	oss.clear();
	oss.str(_T(""));

	HRESULT hr = pRT->EndDraw(); // Present the Back Buffer
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DestroyGraphics();
		CreateGraphics(hWnd);
	}
};

void Game::RenderPause(void)
{
	pRT->BeginDraw();

	pRT->Clear(bg);
	wostringstream oss;
	pBrush->SetColor(score);
	oss << "PAUSED!";
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width-100,300,(pRT->GetSize().width/2), 300), pBrush);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(bg);
	oss.clear();
	oss.str(_T(""));

	HRESULT hr = pRT->EndDraw(); // Present the Back Buffer
	if (hr == D2DERR_RECREATE_TARGET)
	{
		//TODO: Destroy Direct2D.
		DestroyGraphics();
		//TODO: Recreate Direct2D.
		CreateGraphics(hWnd);
	}

};

void Game::SimulatePacMan(void)
{
	//Simulate Physics
	ballpos1.left += ballvec1.width;
	ballpos1.top += ballvec1.height;
	ballpos1.right += ballvec1.width;
	ballpos1.bottom += ballvec1.height;

	ballpos2.left += ballvec2.width;
	ballpos2.top += ballvec2.height;
	ballpos2.right += ballvec2.width;
	ballpos2.bottom += ballvec2.height;

	ballpos3.left += ballvec3.width;
	ballpos3.top += ballvec3.height;
	ballpos3.right += ballvec3.width;
	ballpos3.bottom += ballvec3.height;

	ballpos4.left += ballvec4.width;
	ballpos4.top += ballvec4.height;
	ballpos4.right += ballvec4.width;
	ballpos4.bottom += ballvec4.height;

	ballpos5.left += ballvec5.width;
	ballpos5.top += ballvec5.height;
	ballpos5.right += ballvec5.width;
	ballpos5.bottom += ballvec5.height;

	ballpos6.left += ballvec6.width;
	ballpos6.top += ballvec6.height;
	ballpos6.right += ballvec6.width;
	ballpos6.bottom += ballvec6.height;

	ballpos7.left += ballvec7.width;
	ballpos7.top += ballvec7.height;
	ballpos7.right += ballvec7.width;
	ballpos7.bottom += ballvec7.height;

	ballpos8.left +=	ballvec8.width;
	ballpos8.top +=		ballvec8.height;
	ballpos8.right +=	ballvec8.width;
	ballpos8.bottom +=	ballvec8.height;

	ballpos9.left +=	ballvec9.width;
	ballpos9.top +=		ballvec9.height;
	ballpos9.right +=	ballvec9.width;
	ballpos9.bottom +=	ballvec9.height;

	ballpos10.left +=	ballvec10.width;
	ballpos10.top +=	ballvec10.height;
	ballpos10.right +=	ballvec10.width;
	ballpos10.bottom +=	ballvec10.height;

	//Simulate Collision Detection & Response
	if (ballpos1.bottom > pRT->GetSize().height)
	{
		ballvec1.height = -ballvec1.height;
	}

	if (ballpos1.top <= 0)
	{
		ballvec1.height = -ballvec1.height;
	}

	if (ballpos1.left <= 0)
	{
		ballvec1.width = -ballvec1.width;
	}

	if (ballpos1.right >= pRT->GetSize().width)
	{
		ballvec1.width = -ballvec1.width;
	}
//---------------------BALL2---------------------------------------
	if (ballpos2.bottom > pRT->GetSize().height)
	{
		ballvec2.height = -ballvec2.height;
	}

	if (ballpos2.top <= 0)
	{
		ballvec2.height = -ballvec2.height;
	}

	if (ballpos2.left <= 0)
	{
		ballvec2.width = -ballvec2.width;
	}

	if (ballpos2.right >= pRT->GetSize().width)
	{
		ballvec2.width = -ballvec2.width;
	}
	//---------------------BALL3---------------------------------------
	if (ballpos3.bottom > pRT->GetSize().height)
	{
		ballvec3.height = -ballvec3.height;
	}

	if (ballpos3.top <= 0)
	{
		ballvec3.height = -ballvec3.height;
	}

	if (ballpos3.left <= 0)
	{
		ballvec3.width = -ballvec3.width;
	}

	if (ballpos3.right >= pRT->GetSize().width)
	{
		ballvec3.width = -ballvec3.width;
	}
		//---------------------BALL4---------------------------------------
	if (ballpos4.bottom > pRT->GetSize().height)
	{
		ballvec4.height = -ballvec4.height;
	}

	if (ballpos4.top <= 0)
	{
		ballvec4.height = -ballvec4.height;
	}

	if (ballpos4.left <= 0)
	{
		ballvec4.width = -ballvec4.width;
	}

	if (ballpos4.right >= pRT->GetSize().width)
	{
		ballvec4.width = -ballvec4.width;
	}
//---------------------BALL5---------------------------------------
	if (ballpos5.bottom > pRT->GetSize().height)
	{
		ballvec5.height = -ballvec5.height;
	}

	if (ballpos5.top <= 0)
	{
		ballvec5.height = -ballvec5.height;
	}

	if (ballpos5.left <= 0)
	{
		ballvec5.width = -ballvec5.width;
	}

	if (ballpos5.right >= pRT->GetSize().width)
	{
		ballvec5.width = -ballvec5.width;
	}
	//---------------------BALL6---------------------------------------
	if (ballpos6.bottom > pRT->GetSize().height)
	{
		ballvec6.height = -ballvec6.height;
	}

	if (ballpos6.top <= 0)
	{
		ballvec6.height = -ballvec6.height;
	}

	if (ballpos6.left <= 0)
	{
		ballvec6.width = -ballvec6.width;
	}

	if (ballpos6.right >= pRT->GetSize().width)
	{
		ballvec6.width = -ballvec6.width;
	}
//---------------------BALL7---------------------------------------
	if (ballpos7.bottom > pRT->GetSize().height)
	{
		ballvec7.height = -ballvec7.height;
	}

	if (ballpos7.top <= 0)
	{
		ballvec7.height = -ballvec7.height;
	}

	if (ballpos7.left <= 0)
	{
		ballvec7.width = -ballvec7.width;
	}

	if (ballpos7.right >= pRT->GetSize().width)
	{
		ballvec7.width = -ballvec7.width;
	}
	//---------------------BALL8---------------------------------------
	if (ballpos8.bottom > pRT->GetSize().height)
	{
		ballvec8.height = -ballvec8.height;
	}

	if (ballpos8.top <= 0)
	{
		ballvec8.height = -ballvec8.height;
	}

	if (ballpos8.left <= 0)
	{
		ballvec8.width = -ballvec8.width;
	}

	if (ballpos8.right >= pRT->GetSize().width)
	{
		ballvec8.width = -ballvec8.width;
	}
		//---------------------BALL9---------------------------------------
	if (ballpos9.bottom > pRT->GetSize().height)
	{
		ballvec9.height = -ballvec9.height;
	}

	if (ballpos9.top <= 0)
	{
		ballvec9.height = -ballvec9.height;
	}

	if (ballpos9.left <= 0)
	{
		ballvec9.width = -ballvec9.width;
	}

	if (ballpos9.right >= pRT->GetSize().width)
	{
		ballvec9.width = -ballvec9.width;
	}
//---------------------BALL10---------------------------------------
	if (ballpos10.bottom > pRT->GetSize().height)
	{
		ballvec10.height = -ballvec9.height;
	}

	if (ballpos10.top <= 0)
	{
		ballvec10.height = -ballvec10.height;
	}

	if (ballpos10.left <= 0)
	{
		ballvec10.width = -ballvec10.width;
	}

	if (ballpos10.right >= pRT->GetSize().width)
	{
		ballvec10.width = -ballvec10.width;
	}
	//---------------------------------------------PADDLES-----------------------------------------------------

	//RIGHT PADDLE
	RECT rIntersect;
	RECT rBall = {(LONG)(ballpos1.left),(LONG)(ballpos1.top),(LONG)(ballpos1.right),(LONG)(ballpos1.bottom)};
	RECT rBall2 = {(LONG)(ballpos2.left),(LONG)(ballpos2.top),(LONG)(ballpos2.right),(LONG)(ballpos2.bottom)};
	RECT rBall3 = {(LONG)(ballpos3.left),(LONG)(ballpos3.top),(LONG)(ballpos3.right),(LONG)(ballpos3.bottom)};
	RECT rBall4 = {(LONG)(ballpos4.left),(LONG)(ballpos4.top),(LONG)(ballpos4.right),(LONG)(ballpos4.bottom)};
	RECT rBall5 = {(LONG)(ballpos5.left),(LONG)(ballpos5.top),(LONG)(ballpos5.right),(LONG)(ballpos5.bottom)};
	RECT rBall6 = {(LONG)(ballpos6.left),(LONG)(ballpos6.top),(LONG)(ballpos6.right),(LONG)(ballpos6.bottom)};
	RECT rBall7 = {(LONG)(ballpos7.left),(LONG)(ballpos7.top),(LONG)(ballpos7.right),(LONG)(ballpos7.bottom)};
	RECT rBall8 = {(LONG)(ballpos8.left),(LONG)(ballpos8.top),(LONG)(ballpos8.right),(LONG)(ballpos8.bottom)};
	RECT rBall9 = {(LONG)(ballpos9.left),(LONG)(ballpos9.top),(LONG)(ballpos9.right),(LONG)(ballpos9.bottom)};
	RECT rBall10 = {(LONG)(ballpos10.left),(LONG)(ballpos10.top),(LONG)(ballpos10.right),(LONG)(ballpos10.bottom)};
	RECT rPaddle = {(LONG)(padpos.left),(LONG)(padpos.top),(LONG)(padpos.right),(LONG)(padpos.bottom)};

	if(IntersectRect(&rIntersect, &rBall, &rPaddle))	//bounces the ball
	{
		ballvec1.width = -ballvec1.width;
	}
	if(IntersectRect(&rIntersect, &rBall2, &rPaddle))	//bounces the ball
	{
		ballvec2.width = -ballvec2.width;
	}
	if(IntersectRect(&rIntersect, &rBall3, &rPaddle))	//bounces the ball
	{
		ballvec3.width = -ballvec3.width;
		++lWins;
	}
	if(IntersectRect(&rIntersect, &rBall4, &rPaddle))	//bounces the ball
	{
		ballvec4.width = -ballvec4.width;
	}
	if(IntersectRect(&rIntersect, &rBall5, &rPaddle))	//bounces the ball
	{
		ballvec5.width = -ballvec5.width;
	}
	if(IntersectRect(&rIntersect, &rBall6, &rPaddle))	//bounces the ball
	{
		ballvec6.width = -ballvec6.width;
	}
	if(IntersectRect(&rIntersect, &rBall7, &rPaddle))	//bounces the ball
	{
		ballvec7.width = -ballvec7.width;
		++lWins;
	}
	if(IntersectRect(&rIntersect, &rBall8, &rPaddle))	//bounces the ball
	{
		ballvec8.width = -ballvec8.width;
		++lWins;
	}
	if(IntersectRect(&rIntersect, &rBall9, &rPaddle))	//bounces the ball
	{
		ballvec9.width = -ballvec9.width;
		++lWins;
	}
		if(IntersectRect(&rIntersect, &rBall10, &rPaddle))	//bounces the ball
	{
		ballvec10.width = -ballvec10.width;
	}

	//LEFT PADDLE
	RECT lPaddle = {(LONG)(Lpadpos.right),(LONG)(Lpadpos.top),(LONG)(Lpadpos.left),(LONG)(Lpadpos.bottom)}; 
	if(IntersectRect(&rIntersect, &rBall, &lPaddle))
	{
		ballvec1.width = -ballvec1.width;	//1) power pellet - remove power pellet, change all ghosts into blue ghosts, change music
											//2) blue ghosts - remove ghost, add score, create new red ghost
											//3) normal ghosts - pacman dies, add music. start over									
	}
	if(IntersectRect(&rIntersect, &rBall2, &lPaddle))	//bounces the ball
	{
		ballvec2.width = -ballvec2.width;
	}
	if(IntersectRect(&rIntersect, &rBall3, &lPaddle))	//bounces the ball
	{
		ballvec3.width = -ballvec3.width;
		++rWins;
	}
	if(IntersectRect(&rIntersect, &rBall4, &lPaddle))	//bounces the ball
	{
		ballvec4.width = -ballvec4.width;
	}
	if(IntersectRect(&rIntersect, &rBall5, &lPaddle))	//bounces the ball
	{
		ballvec5.width = -ballvec5.width;
	}
	if(IntersectRect(&rIntersect, &rBall6, &lPaddle))	//bounces the ball
	{
		ballvec6.width = -ballvec6.width;
	}
	if(IntersectRect(&rIntersect, &rBall7, &lPaddle))	//bounces the ball
	{
		ballvec7.width = -ballvec7.width;
		++rWins;
	}
	if(IntersectRect(&rIntersect, &rBall8, &lPaddle))	//bounces the ball
	{
		ballvec8.width = -ballvec8.width;
		++rWins;
	}
	if(IntersectRect(&rIntersect, &rBall9, &lPaddle))	//bounces the ball
	{
		ballvec9.width = -ballvec9.width;
		++rWins;
	}
	if(IntersectRect(&rIntersect, &rBall10, &lPaddle))	//bounces the ball
	{
		ballvec10.width = -ballvec10.width;
	}
}

void Game::Simulate(void)
{
	win = false;
	//Simulate Physics
	ballpos.left += ballvec.width;
	ballpos.top += ballvec.height;
	ballpos.right += ballvec.width;
	ballpos.bottom += ballvec.height;

	//Simulate Collision Detection & Response
	if (ballpos.bottom > pRT->GetSize().height)
	{
		ballvec.height = -ballvec.height;
	}

	if (ballpos.top <= 0)
	{
		ballvec.height = -ballvec.height;
	}

	if (ballpos.right < 0)
	{
		win = true;
		if (win)
		{
			++rWins;
			++turn;
			win = false;
			Sleep(500);
		
			ballpos = D2D1::RectF(20, 20, 60, 60);						//this is the ball starting point
			ballvec = D2D1::SizeF(3, 3);
		}
	}

	if (ballpos.left > pRT->GetSize().width)
	{
		win = true;
		if (win)
		{
			++lWins;
			win = false;
			++turn;
			Sleep(500);
			
			ballpos = D2D1::RectF(20, 20, 60, 60);						//this is the ball starting point
			ballvec = D2D1::SizeF(3, 3);
		}
	}
	//---------------------------------------------PADDLES-----------------------------------------------------

	//RIGHT PADDLE
	RECT rIntersect;
	RECT rBall = {(LONG)(ballpos.left),(LONG)(ballpos.top),(LONG)(ballpos.right),(LONG)(ballpos.bottom)};
	RECT rPaddle = {(LONG)(padpos.left),(LONG)(padpos.top),(LONG)(padpos.right),(LONG)(padpos.bottom)};

	if(IntersectRect(&rIntersect, &rBall, &rPaddle))	//bounces the ball
	{
		ballvec.width = -ballvec.width;
	}
	//LEFT PADDLE
	RECT lPaddle = {(LONG)(Lpadpos.right),(LONG)(Lpadpos.top),(LONG)(Lpadpos.left),(LONG)(Lpadpos.bottom)}; 
	if(IntersectRect(&rIntersect, &rBall, &lPaddle))
	{
		ballvec.width = -ballvec.width;
	}
	//Top PADDLE
	RECT tPaddle = {(LONG)(Tpadpos.left),(LONG)(Tpadpos.top),(LONG)(Tpadpos.right),(LONG)(Tpadpos.bottom)};		//doesnt work
	if(IntersectRect(&rIntersect, &rBall, &tPaddle))
	{
		ballvec.height = -ballvec.height;
	}
}

void Game::RenderDemo(void)
{
	pRT->BeginDraw();
	//------------------------Draw the background (stretched).---------------------
	pRT->DrawBitmap(background2,
		D2D1::RectF(0,0, pRT->GetSize().width, pRT->GetSize().height));
	//-----------------------------------------------------------------------------
	D2D1_ELLIPSE ball;	// Render the Ball.
	ball.point.x = (ballpos.right+ballpos.left)/2;
	ball.point.y = (ballpos.bottom+ballpos.top)/2;
	ball.radiusX = (ballpos.right-ballpos.left)/2;
	ball.radiusY = (ballpos.bottom-ballpos.top)/2;
	//pRT->FillRectangle(ball, pBrush);// Render the Left Paddle.
	pBrush->SetOpacity(0.5f);
	pBrush->SetColor(D2DColor(White));
	pRT->FillEllipse(ball, pBrush);
	pRT->FillRectangle(padpos, pBrush);	// Render the Right Paddle.
	pRT->FillRectangle(Lpadpos, pBrush);// Render the Left Paddle.
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(D2DColor(Black));
	dwFrames++;	// Calculate the number of frames per one second:
	dwCurrentTime = GetTickCount(); // Even better to use timeGetTime()
	dwElapsedTime = dwCurrentTime - dwLastUpdateTime;
	if(dwElapsedTime >= 1000)
	{
		wostringstream oss;
		oss << _T("FPS = ") << dwFrames;
		szFPS = oss.str();
		dwFrames = 0;
		dwLastUpdateTime = dwCurrentTime;
	}

		if(FPS)
	{
		// Display the FPS.
		pBrush->SetColor(D2DColor(White));
		pRT->DrawTextW(szFPS.c_str(), szFPS.length(), pTF,
			D2D1::RectF(0,pRT->GetSize().height-30,100,pRT->GetSize().height), pBrush);
	}
	pBrush->SetColor(D2DColor(White));
	wostringstream oss;
	oss << "DEMO MODE";
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width,500,(pRT->GetSize().width/2)-150, 500), pBrush);
	pBrush->SetColor(D2DColor(White));
	oss.clear();
	oss.str(_T(""));

	//pRT->DrawBitmap(cloud,D2D1::RectF(15,15,150,150));
//-------------------------------------------------------------- CLOUDS ---------------------------
//	static int iSpriteFrame = 0;
//	static float fSpriteX = 0;
//	float fSpriteWidth = cloud->GetSize().width / 1;
//	float fSpriteHeight = cloud->GetSize().height / 1;
//	pRT->DrawBitmap(cloud,D2D1::RectF(15, 15, 150, 150),1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
//		D2D1::RectF(fSpriteWidth*iSpriteFrame,0,fSpriteWidth*iSpriteFrame+fSpriteWidth, fSpriteHeight));
//static DWORD dwSpriteTime = GetTickCount();
//	if (GetTickCount() - dwSpriteTime > 100 )
//	{
//		iSpriteFrame++;
//		if (iSpriteFrame == 1)
//			iSpriteFrame = 0;
//		dwSpriteTime = GetTickCount();
//	}
//-------------------------------------------------------------- CLOUDS ---------------------------


	//pBrush->SetColor(D2DColor(White));	// Display the FPS.
	//pRT->DrawTextW(szFPS.c_str(), szFPS.length(), pTF,
	//	D2D1::RectF(0,pRT->GetSize().height-30,100,pRT->GetSize().height), pBrush);
	pBrush->SetOpacity(0.25f);		//Draw the score board - LEFT
	pBrush->SetColor(D2DColor(White));
	oss << GetLeftWins();
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width,10,(pRT->GetSize().width/2)-150, 500), pBrush);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(D2DColor(Black));
	oss.clear();
	oss.str(_T(""));
	pBrush->SetOpacity(0.25f);	//Draw the score board - RIGHT
	pBrush->SetColor(D2DColor(White));
	oss << GetRightWins();
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width,10,(pRT->GetSize().width/2)+100, 500), pBrush);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(D2DColor(Black));
	oss.clear();
	oss.str(_T(""));
	HRESULT hr = pRT->EndDraw(); // Present the Back Buffer
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DestroyGraphics();
		CreateGraphics(hWnd);
	}
};

void Game::RenderPac(void)
{
	//MusicLoop();		//doesnt work!!
	pRT->BeginDraw();
//------------------------Draw the background (stretched).---------------------
	pRT->DrawBitmap(background,D2D1::RectF(0,0, pRT->GetSize().width, pRT->GetSize().height));
//-Sprite--------------------------------------------------------------------------- LEFT PADDLE ----------------------------------------------------------------------
	float fSpriteWidth = sprite->GetSize().width / 4.0f;
	float fSpriteHeight = sprite->GetSize().height / 1.0f;
	static int iSpriteFrame = 0;
	static float fSpriteX = 0;
//----------------------------- SHADOW -----------------------
		// Apply a skew transform.
	pRT->SetTransform(D2D1::Matrix3x2F::Skew(-40.0f, 10.0f,
		D2D1::Point2F((Lpadpos.left+Lpadpos.right+fSpriteWidth)/2,(Lpadpos.top+Lpadpos.bottom+fSpriteHeight)/2)));

	pBrush->SetColor(D2DColor(White));
	pBrush->SetOpacity(0.5f);
	// Fill Opacity Mask only works in Aliased Mode.
	pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	// Draw the sprite sheet using the brush.
	pRT->FillOpacityMask(sprite, pBrush,
		D2D1_OPACITY_MASK_CONTENT_GRAPHICS,
		D2D1::RectF(Lpadpos.left+35, Lpadpos.top+35,
					Lpadpos.right, Lpadpos.bottom),
		D2D1::RectF(fSpriteWidth*iSpriteFrame, 0,
					fSpriteWidth*iSpriteFrame+fSpriteWidth, fSpriteHeight));

	// Restore the Antialias mode.
	pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	pBrush->SetOpacity(1.0f);

	// Reset the transform.
	pRT->SetTransform(D2D1::IdentityMatrix());
//-Sprite--------------------------------------------------------------------------- LEFT PADDLE ----------------------------------------------------------------------
	pRT->DrawBitmap(sprite,
		D2D1::RectF(Lpadpos.left+35, Lpadpos.top+35, Lpadpos.right, Lpadpos.bottom),		//mess with to change sprite of pacman, paddle pos might be off as well.?>?
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,D2D1::RectF(fSpriteWidth*iSpriteFrame,0,fSpriteWidth*iSpriteFrame+fSpriteWidth, fSpriteHeight));

//------------------------------------------------ RIGHT PADDLE -------------------------------------------------------------------------------------------------------

	float fRSpriteWidth = Rightsprite->GetSize().width / 4.0f;
	float fRSpriteHeight = Rightsprite->GetSize().height / 1.0f;
	static int iRSpriteFrame = 0;
	static float fRSpriteX = 0;

	pRT->DrawBitmap(Rightsprite,
		D2D1::RectF(padpos.left-60, padpos.top-60, padpos.left, padpos.top),
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,D2D1::RectF(fRSpriteWidth*iRSpriteFrame, 0,fRSpriteWidth*iRSpriteFrame+fRSpriteWidth, fRSpriteHeight));

//----------------------------------------------------------------RED GHOST--------------------------------------------------------------------------------------------------------
	float fRedSpriteWidth = RedSprite->GetSize().width / 4.0f;
	float fRedSpriteHeight = RedSprite->GetSize().height / 1.0f;
	static int iRedSpriteFrame = 0;
	static float fRedSpriteX = 0;

	 pRT->DrawBitmap(RedSprite,
		D2D1::RectF(ballpos3.left, ballpos3.top, ballpos3.right, ballpos3.bottom ),1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
						 D2D1::RectF(fRedSpriteWidth*iRedSpriteFrame, 0,fRedSpriteWidth*iRedSpriteFrame+fRedSpriteWidth, fRedSpriteHeight));

	 //----------------------------------------------------------------TEAL GHOST--------------------------------------------------------------------------------------------------------
	float fTealSpriteWidth = TealSprite->GetSize().width / 4.0f;
	float fTealSpriteHeight = TealSprite->GetSize().height / 1.0f;
	static int iTealSpriteFrame = 0;
	static float fTealSpriteX = 0;

	 pRT->DrawBitmap(TealSprite,
		D2D1::RectF(ballpos7.left, ballpos7.top, ballpos7.right, ballpos7.bottom ),1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
						 D2D1::RectF(fTealSpriteWidth*iTealSpriteFrame, 0,fTealSpriteWidth*iTealSpriteFrame+fTealSpriteWidth, fTealSpriteHeight));
 //------------------------------------------------------------------ PINK GHOST ---------------------------------------------------------------------------------------------------

	 float fPinkSpriteWidth = PinkSprite->GetSize().width / 4.0f;
	float fPinkSpriteHeight = PinkSprite->GetSize().height / 1.0f;
	static int iPinkSpriteFrame = 0;
	static float fPinkSpriteX = 0;

	 pRT->DrawBitmap(PinkSprite,
		D2D1::RectF(ballpos8.left, ballpos8.top, ballpos8.right, ballpos8.bottom ),1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
						 D2D1::RectF(fPinkSpriteWidth*iPinkSpriteFrame, 0,fPinkSpriteWidth*iPinkSpriteFrame+fPinkSpriteWidth, fPinkSpriteHeight));
//------------------------------------------------------------------ TAN GHOST ---------------------------------------------------------------------------------------------------

	float fTanSpriteWidth = TanSprite->GetSize().width / 4.0f;
	float fTanSpriteHeight = TanSprite->GetSize().height / 1.0f;
	static int iTanSpriteFrame = 0;
	static float fTanSpriteX = 0;

	 pRT->DrawBitmap(TanSprite,
		D2D1::RectF(ballpos9.left, ballpos9.top, ballpos9.right, ballpos9.bottom ),1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
						 D2D1::RectF(fTanSpriteWidth*iTanSpriteFrame, 0,fTanSpriteWidth*iTanSpriteFrame+fTanSpriteWidth, fTanSpriteHeight));
//------------------------------------------------------------ PACLIFE -------------------------------------------------------------------------------------

	//float fPacLifeSpriteWidth = PacLife->GetSize().width / 4.0f;
	//float fPacLifeSpriteHeight = PacLife->GetSize().height / 1.0f;
	//static int iPacLifeSpriteFrame = 0;
	//static float fPacLifeSpriteX = 0;

	//pRT->DrawBitmap(PacLife,
	//	D2D1::RectF(50, 50, 370, 370),		//mess with to change sprite of pacman, paddle pos might be off as well.?>?
	//	1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,D2D1::RectF(fPacLifeSpriteWidth*iPacLifeSpriteFrame,0,fPacLifeSpriteWidth*iPacLifeSpriteFrame+fPacLifeSpriteWidth, fPacLifeSpriteHeight));
//------------------------------------------------------------------------ LIFE BARS --------------------------------------------------------------------------------

	//pBrush->SetColor(D2DColor(White));
	//pBrush->SetColor(D2DColor(Violet));
	//pRT->DrawRectangle(D2D1::RectF(150, 20, 380, 40),pBrush, 3);
	


	static DWORD dwSpriteTime = GetTickCount();

	if (GetTickCount() - dwSpriteTime > 100 )
	{
		// Update frame counter.
		iSpriteFrame++;
		if (iSpriteFrame == 4)
			iSpriteFrame = 0;

		iRedSpriteFrame++;
		if (iRedSpriteFrame == 4)
			iRedSpriteFrame = 0;

		iTealSpriteFrame++;
		if (iTealSpriteFrame == 4)
			iTealSpriteFrame = 0;

		iPinkSpriteFrame++;
		if (iPinkSpriteFrame == 4)
			iPinkSpriteFrame = 0;

		iTanSpriteFrame++;
		if (iTanSpriteFrame == 4)
			iTanSpriteFrame = 0;

		iRSpriteFrame++;
		if (iRSpriteFrame == 4)
			iRSpriteFrame = 0;

		//iPacLifeSpriteFrame++;
		//if (iPacLifeSpriteFrame == 4)
		//	iPacLifeSpriteFrame = 0;

		dwSpriteTime = GetTickCount();
	}
	
	D2D1_ELLIPSE ball;	// Render the Ball.
	ball.point.x = (ballpos1.right+ballpos1.left)/2;
	ball.point.y = (ballpos1.bottom+ballpos1.top)/2;
	ball.radiusX = (ballpos1.right-ballpos1.left)/2;
	ball.radiusY = (ballpos1.bottom-ballpos1.top)/2;

	D2D1_ELLIPSE ball2;
	ball2.point.x = (ballpos2.right+ballpos2.left)/2;
	ball2.point.y = (ballpos2.bottom+ballpos2.top)/2;
	ball2.radiusX = (ballpos2.right-ballpos2.left)/2;
	ball2.radiusY = (ballpos2.bottom-ballpos2.top)/2;

	D2D1_ELLIPSE ball4;
	ball4.point.x = (ballpos4.right+ballpos4.left)/2;
	ball4.point.y = (ballpos4.bottom+ballpos4.top)/2;
	ball4.radiusX = (ballpos4.right-ballpos4.left)/2;
	ball4.radiusY = (ballpos4.bottom-ballpos4.top)/2;

	D2D1_ELLIPSE ball5;
	ball5.point.x = (ballpos5.right+ballpos5.left)/2;
	ball5.point.y = (ballpos5.bottom+ballpos5.top)/2;
	ball5.radiusX = (ballpos5.right-ballpos5.left)/2;
	ball5.radiusY = (ballpos5.bottom-ballpos5.top)/2;

	D2D1_ELLIPSE ball6;
	ball6.point.x = (ballpos6.right+ballpos6.left)/2;
	ball6.point.y = (ballpos6.bottom+ballpos6.top)/2;
	ball6.radiusX = (ballpos6.right-ballpos6.left)/2;
	ball6.radiusY = (ballpos6.bottom-ballpos6.top)/2;

	D2D1_ELLIPSE ball10;
	ball10.point.x = (ballpos10.right+ballpos10.left)/2;
	ball10.point.y = (ballpos10.bottom+ballpos10.top)/2;
	ball10.radiusX = (ballpos10.right-ballpos10.left)/2;
	ball10.radiusY = (ballpos10.bottom-ballpos10.top)/2;

//------------ Gradient Colors ----------------------------------------------------------
	HRESULT hr;
	ID2D1GradientStopCollection *pGradientStops = NULL;
	D2D1_GRADIENT_STOP gradientStops[3];
	gradientStops[0].color = D2DColor(Yellow);
	gradientStops[1].color = D2DColor(White);
	gradientStops[2].color = D2DColor(Teal);
	gradientStops[0].position = 1.0f;
	gradientStops[1].position = 0.5f;
	gradientStops[2].position = 0.25f;
	hr = pRT->CreateGradientStopCollection(gradientStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pGradientStops);
	if (SUCCEEDED(hr))
	hr = pRT->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F((ballpos4.left+ballpos4.right)*0.5f,(ballpos4.bottom+ballpos4.top)*0.5f), D2D1::Point2F(0.0f,0.0f), ((ballpos4.left+ballpos4.right)*0.5f) - ballpos4.left, ((ballpos4.left+ballpos4.right)*0.5f) - ballpos4.left), pGradientStops, &gBrush); 
		pRT->FillEllipse(ball4, gBrush);	//<--- Power pellet
	hr = pRT->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F((ballpos10.left+ballpos10.right)*0.5f,(ballpos10.bottom+ballpos10.top)*0.5f), D2D1::Point2F(0.0f,0.0f), ((ballpos10.left+ballpos10.right)*0.5f) - ballpos10.left, ((ballpos10.left+ballpos10.right)*0.5f) - ballpos10.left), pGradientStops, &gBrush); 
	
//------------ Gradient Colors ----------------------------------------------------------

	pBrush->SetOpacity(0.5f);
	pBrush->SetColor(D2DColor(White));
	pRT->FillEllipse(ball, pBrush);
	pRT->FillEllipse(ball2, pBrush);
	pRT->FillEllipse(ball5, pBrush);
	pRT->FillEllipse(ball6, pBrush);
	pBrush->SetColor(D2DColor(Yellow));

	pRT->FillEllipse(ball10, gBrush);	//<--- Power pellet

	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(D2DColor(Black));

//------------------------------------------------------------------ FPS -------------------------------------------
		// Calculate the number of frames per one second:
		dwFrames++;
		dwCurrentTime = GetTickCount(); // Even better to use timeGetTime()
		dwElapsedTime = dwCurrentTime - dwLastUpdateTime;
		if(dwElapsedTime >= 1000)
		{
			wostringstream oss;
			oss << _T("FPS = ") << dwFrames;
			szFPS = oss.str();
			dwFrames = 0;
			dwLastUpdateTime = dwCurrentTime;
		}
	if(FPS)
	{
		// Display the FPS.
		pBrush->SetColor(D2DColor(White));
		pRT->DrawTextW(szFPS.c_str(), szFPS.length(), pTF,
			D2D1::RectF(0,pRT->GetSize().height-30,100,pRT->GetSize().height), pBrush);
	}

	//Draw the score board - LEFT
	pBrush->SetOpacity(0.5f);
	pBrush->SetColor(D2DColor(Yellow));
	wostringstream tss;
	tss << GetLeftWins();
	pRT->DrawTextW(tss.str().c_str(), tss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width,10,(pRT->GetSize().width/2)-150, 500), pBrush);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(D2DColor(Black));
	tss.clear();
	tss.str(_T(""));

	//Draw the score board - RIGHT
	pBrush->SetOpacity(0.5f);
	pBrush->SetColor(D2DColor(Yellow));
	tss << GetRightWins();
	pRT->DrawTextW(tss.str().c_str(), tss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width,10,(pRT->GetSize().width/2)+100, 500), pBrush);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(D2DColor(Black));
	tss.clear();
	tss.str(_T(""));

	hr = pRT->EndDraw(); // Present the Back Buffer
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DestroyGraphics();
		CreateGraphics(hWnd);
	}


};

void Game::Render1975(void)
{
	pRT->BeginDraw();

	pRT->Clear(bg);

	//mid of board Line
	pBrush->SetColor(score);
	pBrush->SetOpacity(0.25f);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 0), D2D1::Point2F(pRT->GetSize().width/2,47.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 67.5), D2D1::Point2F(pRT->GetSize().width/2,115), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 135), D2D1::Point2F(pRT->GetSize().width/2,182.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 202.5), D2D1::Point2F(pRT->GetSize().width/2,250), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 270), D2D1::Point2F(pRT->GetSize().width/2,317.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 337.5), D2D1::Point2F(pRT->GetSize().width/2,384.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 404.5), D2D1::Point2F(pRT->GetSize().width/2,452), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 472), D2D1::Point2F(pRT->GetSize().width/2,519.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 539.5), D2D1::Point2F(pRT->GetSize().width/2,587), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 607), D2D1::Point2F(pRT->GetSize().width/2,654.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 674.5), D2D1::Point2F(pRT->GetSize().width/2,722), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 742), D2D1::Point2F(pRT->GetSize().width/2,789.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 809.5), D2D1::Point2F(pRT->GetSize().width/2,857.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 877), D2D1::Point2F(pRT->GetSize().width/2,924.5), pBrush, lineWidth);
	pRT->DrawLine(D2D1::Point2F(pRT->GetSize().width/2, 944.5), D2D1::Point2F(pRT->GetSize().width/2,992), pBrush, lineWidth);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(bg);

	// Render the Ball.
	D2D1_ELLIPSE ball;										//want to draw a square not a ball
	ball.point.x = (ballpos.right+ballpos.left)/2;
	ball.point.y = (ballpos.bottom+ballpos.top)/2;
	ball.radiusX = (ballpos.right-ballpos.left)/2;
	ball.radiusY = (ballpos.bottom-ballpos.top)/2;

	//pRT->FillRectangle(ball, pBrush);// Render the Left Paddle.
	pBrush->SetOpacity(0.5f);
	pBrush->SetColor(Ball);
	pRT->FillEllipse(ball, pBrush);
	pBrush->SetColor(leftPaddle);

	pRT->FillRectangle(padpos, pBrush);	// Render the Right Paddle.
	pBrush->SetColor(rightPaddle);

	pRT->FillRectangle(Lpadpos, pBrush);// Render the Left Paddle.
	//pRT->FillRectangle(Tpadpos, pBrush);//Top Paddle
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(bg);

	// Calculate the number of frames per one second:
	dwFrames++;
	dwCurrentTime = GetTickCount(); // Even better to use timeGetTime()
	dwElapsedTime = dwCurrentTime - dwLastUpdateTime;
	if(dwElapsedTime >= 1000)
	{
		wostringstream oss;
		oss << _T("FPS = ") << dwFrames;
		szFPS = oss.str();
		dwFrames = 0;
		dwLastUpdateTime = dwCurrentTime;
	}
	if(FPS)
	{
		// Display the FPS.
		pBrush->SetColor(D2DColor(White));
		pRT->DrawTextW(szFPS.c_str(), szFPS.length(), pTF,
			D2D1::RectF(0,pRT->GetSize().height-30,100,pRT->GetSize().height), pBrush);
	}

	//Draw the score board - LEFT
	pBrush->SetOpacity(0.25f);
	pBrush->SetColor(score);
	wostringstream oss;
	oss << GetLeftWins();
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width,10,(pRT->GetSize().width/2)-150, 500), pBrush);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(bg);
	oss.clear();
	oss.str(_T(""));

	//Draw the score board - RIGHT
	pBrush->SetOpacity(0.25f);
	pBrush->SetColor(score);
	oss << GetRightWins();
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFF, D2D1::RectF(pRT->GetSize().width,10,(pRT->GetSize().width/2)+100, 500), pBrush);
	pBrush->SetOpacity(1.0f);
	pBrush->SetColor(bg);
	oss.clear();
	oss.str(_T(""));



	HRESULT hr = pRT->EndDraw(); // Present the Back Buffer
	if (hr == D2DERR_RECREATE_TARGET)
	{
		//TODO: Destroy Direct2D.
		DestroyGraphics();
		//TODO: Recreate Direct2D.
		CreateGraphics(hWnd);
	}
	//}
}

void Game::RenderTitle(void)
{
	pRT->BeginDraw();
	//pRT->DrawBitmap(sprite, padpos);

	pRT->Clear(D2DColor(Black));


	//// Draw the background (stretched).
	//pRT->DrawBitmap(background,
	//	D2D1::RectF(0,0, pRT->GetSize().width, pRT->GetSize().height));
	float fGSpriteWidth = GhostSprite->GetSize().width / 5;
	float fGSpriteHeight = GhostSprite->GetSize().height / 1;

	float fRSpriteWidth = RedSprite->GetSize().width / 4;
	float fRSpriteHeight = RedSprite->GetSize().height / 1;

	float fSpriteWidth = sprite->GetSize().width / 4;
	float fSpriteHeight = sprite->GetSize().height / 1;

	static int iSpriteFrame = 0;
	static int iGhostSpriteFrame = 0;
	static float fSpriteX = 0;
	static float fGhostSpriteX = 0;
	static float fSpriteAngle = 0.0f;
	//float fSpriteScale = 1.0f;
	static DWORD dwSpriteTime = GetTickCount();


	if (fSpriteX >= pRT->GetSize().width)	//this will never be true.. the size of the image will never be > than the background
	{
		Ghost = true;
	}
	if (fGhostSpriteX >= pRT->GetSize().width)
	{
		Ghost = false;
	}

	if (Ghost)
	{
			// Apply a scale (mirrored) transform.
	pRT->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f,
		D2D1::Point2F((fSpriteX+fSpriteX+fSpriteWidth)/2,(500+500+fSpriteHeight)/2)));

	pRT->DrawBitmap(sprite,						//PacMan
		D2D1::RectF(fSpriteX, 460, fSpriteX+fSpriteWidth, 460+fSpriteHeight),
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(fSpriteWidth*iSpriteFrame, 0,
					fSpriteWidth*iSpriteFrame+fSpriteWidth, fSpriteHeight));

		pRT->DrawBitmap(GhostSprite,						//Ghost
		D2D1::RectF(fSpriteX+150, 458, fSpriteX+fGSpriteWidth+150, 458+fGSpriteHeight),
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(fGSpriteWidth*iSpriteFrame, 0,
					fGSpriteWidth*iSpriteFrame+fGSpriteWidth, fGSpriteHeight));

			// Update position.
	fSpriteX-=1;
	if (fSpriteX > pRT->GetSize().width)
		fGhostSpriteX = +fGSpriteWidth;
				// Update position.
	fSpriteX-=1;
	if (fSpriteX > pRT->GetSize().width)
		fSpriteX = +fSpriteWidth;
		
	// Reset the transform.
	pRT->SetTransform(D2D1::IdentityMatrix());
	}
	if (!Ghost)
	{
		// Draw the sprite sheet (moving, cropped, animated,
		//						  mirrored by SetTransform).
	pRT->DrawBitmap(sprite,									//PacMan
		D2D1::RectF(fSpriteX+150, 460,
					fSpriteX+fSpriteWidth+150, 460+fSpriteHeight),
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(fSpriteWidth*iSpriteFrame, 0,
					fSpriteWidth*iSpriteFrame+fSpriteWidth, fSpriteHeight));

		pRT->DrawBitmap(RedSprite,						// Red Ghost
		D2D1::RectF(fSpriteX, 458, fSpriteX+fRSpriteWidth, 458+fRSpriteHeight),
		1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(fRSpriteWidth*iSpriteFrame, 0,
					fRSpriteWidth*iSpriteFrame+fRSpriteWidth, fRSpriteHeight));
			// Update position.
	fSpriteX+=1;
	if (fSpriteX > pRT->GetSize().width)
		fSpriteX = -fSpriteWidth;
			// Update position.
	fSpriteX+=1;
	if (fSpriteX > pRT->GetSize().width)
		fSpriteX = -fRSpriteWidth;
	}


	if (GetTickCount() - dwSpriteTime > 100 )
	{
		// Update frame counter.
		iSpriteFrame++;
		if (iSpriteFrame == 4)
			iSpriteFrame = 0;
		dwSpriteTime = GetTickCount();

		iGhostSpriteFrame++;
		if (iGhostSpriteFrame == 4)
			iGhostSpriteFrame = 0;
		dwSpriteTime = GetTickCount();
	}
	//if (GetTickCount() - dwSpriteTime > 700 )
	//{
	//	// Update frame counter.
	//	iGhostSpriteFrame++;
	//	if (iGhostSpriteFrame == 4)
	//		iGhostSpriteFrame = 0;
	//	dwSpriteTime = GetTickCount();
	//}

	float fSpriteScale = 2.8f;
	pRT->DrawBitmap(titlepac,D2D1::RectF(15, 15,150*fSpriteScale, 150*fSpriteScale));
	/*pRT->DrawBitmap(titlepac,D2D1::RectF(15, 15,150*fSpriteScale, 150*fSpriteScale),1.0f,
	D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,D2D1::RectF(15,0,15, 150));*/

	wostringstream oss;
	oss << "Created by: Corey Morehead\tcoreyjamesmorehead@gmail.com\t\tWindows Programming I\tProject2";
	pRT->DrawTextW(oss.str().c_str(), oss.str().length(), pTFFF, D2D1::RectF(0,0,800,15), pBrush);
	pBrush->SetColor(D2DColor(White));
	oss.clear();
	oss.str(_T(""));

		HRESULT hr = pRT->EndDraw(); // Present the Back Buffer
	if (hr == D2DERR_RECREATE_TARGET)
	{
		//TODO: Destroy Direct2D.
		DestroyGraphics();
		//TODO: Recreate Direct2D.
		CreateGraphics(hWnd);
	}
};

HRESULT Game::CreateGraphics(HWND hWnd)
{
	// Initialize the Direct2D Factory.
	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Factory."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Get the dimensions of the client.
	RECT rc;
	GetClientRect(hWnd, &rc);

	// Initialize a Direct2D Size Structure.
	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);


	// Create the Direct2D Render Target.
	hr = pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, size), &pRT);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Render Target."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
	pRT->SetDpi(96.0f, 96.0f);

	// Create the Direct2D Solid Color Brush.
	hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0x0), &pBrush);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Solid Color Brush."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Initialize the DirectWrite Factory.
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		(IUnknown**)&pDWFactory);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Factory."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create the DirectWrite Text Format.
	hr = pDWFactory->CreateTextFormat(_T("Veranda"), NULL,
		DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL,
		24, _T(""), &pTF);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Text Format."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
		// Create the DirectWrite Text Format.
	hr = pDWFactory->CreateTextFormat(_T("Agency FB"), NULL,
		DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL,
		12, _T(""), &pTFFF);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Text Format."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create the DirectWrite Text Format.
	hr = pDWFactory->CreateTextFormat(_T("Agency FB"), NULL,
		DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL,
		68, _T(""), &pTFF);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Text Format."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
	pTF->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

//------------ Gradient Colors ----------------------------------------------------------------------------------------------------------------------------------
//	ID2D1GradientStopCollection *pGradientStops = NULL;
//	D2D1_GRADIENT_STOP gradientStops[3];
//	gradientStops[0].color = D2DColor(Yellow);
//	gradientStops[1].color = D2DColor(White);
//	gradientStops[2].color = D2DColor(Black);
//	gradientStops[0].position = 1.0f;
//	gradientStops[1].position = 0.5f;
//	gradientStops[2].position = 0.25f;
//	hr = pRT->CreateGradientStopCollection(gradientStops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pGradientStops);
//	if (SUCCEEDED(hr))
//	hr = pRT->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F((ballpos4.left+ballpos4.right)*0.5,(ballpos4.bottom+ballpos4.top)*0.5), D2D1::Point2F(0.0f,0.0f), ((ballpos4.left+ballpos4.right)*0.5) - ballpos4.left, ((ballpos4.left+ballpos4.right)*0.5) - ballpos4.left), pGradientStops, &gBrush); 
////------------ Gradient Colors ----------------------------------------------------------------------------------------------------------------------------------
//

	//____________________________
	//
	// IMAGE LOADING
	//____________________________
	//

	//// Create WIC factory.
	hr = CoCreateInstance(CLSID_WICImagingFactory,NULL,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pWICFactory));
	//// Create a bitmap by loading it from a file.
	hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\pacmac.png",0, 0,&background);
		//// Create a bitmap by loading it from a file.
	hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\puckmanDONE2.png",0, 0,&Rightsprite);
	 // Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\puckmanDONE.png",0, 0,&sprite);
		 // Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\ghostsprite.png",0, 0,&GhostSprite);
			 // Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\RedGhost.png",0, 0,&RedSprite);
				 // Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\TealGhost.png",0, 0,&TealSprite);
					 // Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\PinkGhost.png",0, 0,&PinkSprite);
						 // Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\TanGhost.png",0, 0,&TanSprite);
	// Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\tronbg2.png",0, 0,&background2);
		// Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\puckmanDONE.png",0, 0,&PacLife);
			// Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\titlepac.png",0, 0,&titlepac);
			// Create a bitmap by loading it from a file.
    hr = LoadBitmapFromFile(pRT,pWICFactory,L".\\cloud.png",0, 0,&cloud);
	

	if(FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to load the image"),
			_T("Program Name"), MB_OK | MB_ICONERROR);
		return hr;
	}
	return S_OK; // SUCCESS!!
}

void Game::DestroyGraphics(void)
{
	SafeRelease(&gBrush);
	SafeRelease(&cloud);
	SafeRelease(&titlepac);
	SafeRelease(&PacLife);
	SafeRelease(&TanSprite);
	SafeRelease(&PinkSprite);
	SafeRelease(&TealSprite);
	SafeRelease(&Rightsprite);
	SafeRelease(&background2);
	SafeRelease(&particle);
	SafeRelease(&GhostSprite);
	SafeRelease(&RedSprite);
	SafeRelease(&sprite);
	SafeRelease(&background);
	SafeRelease(&pWICFactory);
	SafeRelease(&pTFF);
	SafeRelease(&pTFFF);
	SafeRelease(&pTF);
	SafeRelease(&pDWFactory);
	SafeRelease(&pBrush);
	SafeRelease(&pRT);

	// Release the Direct2D Factory.
	SafeRelease(&pD2DFactory);
}

// Creates a Direct2D bitmap from the specified file name.
HRESULT Game::LoadBitmapFromFile(ID2D1RenderTarget *pRenderTarget,IWICImagingFactory *pIWICFactory,
	PCWSTR uri,UINT destinationWidth,UINT destinationHeight,ID2D1Bitmap **ppBitmap)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						destinationWidth,
						destinationHeight,
						WICBitmapInterpolationModeCubic
						);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
						);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);
	return hr;
}

