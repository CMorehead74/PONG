#include "Util.h"
//************************************************************
//FOUND THIS ON DANIWEB TO SET POSITION AND HIGHLIGHT COLOR	 *
//http://www.daniweb.com/search.php?q=change+cursor+position *
//************************************************************
void Util::Position(int x, int y)
{
	HANDLE out;
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	out = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(out, &csbiInfo);
	csbiInfo.dwCursorPosition.X = x;
	csbiInfo.dwCursorPosition.Y = y;
	SetConsoleCursorPosition(out, csbiInfo.dwCursorPosition);
}
void Util::highLight(int x)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
}
