#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <atlImage.h>
#include <math.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hinst;
LPCTSTR lpszClass = TEXT("First");

BOOL CollisionCheck(int x, int y, int mx, int my);


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) //h의 의미? = 핸들
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hinst = hinstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //배경색
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW); //프로그램 내에서 보이는 커서
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //아이콘 
	WndClass.hInstance = hinstance; //현재 실행되고 있는 객체의 핸들
	WndClass.lpfnWndProc = WndProc; //프로시저함수의 이름 
	WndClass.lpszClassName = lpszClass; //윈도우 클래스 내용의 이름 
	WndClass.lpszMenuName = NULL; // 메뉴바 이름
	WndClass.style = CS_HREDRAW | CS_VREDRAW; //윈도우 출력 스타일
											  // 여기까지 WNDCLASS구조체의 변수들에 값을 대입

	RegisterClass(&WndClass); // 윈도우 클래스를 운영체제에 등록

	hWnd = CreateWindow(lpszClass, "Windows Program 1-1", WS_OVERLAPPEDWINDOW, 0, 0, 1280,1080, NULL, (HMENU)NULL, hinstance, NULL);
	//CreateWindow(클래스 이름,타이틀 이름,윈도우 스타일, 윈도우 위치좌표x , y,윈도우 가로크기, 윈도우 세로크기, 부모 윈도우 핸들, 메뉴 핸들, 응용프로그램 인스턴스, 생성 윈도우 정보(NULL))
	ShowWindow(hWnd, nCmdShow); //윈도우의 화면 출력
	UpdateWindow(hWnd); //OS에 WM_PAINT메시지 전송
	while (GetMessage(&Message, NULL, 0, 0))
	{
		//윈도우 프로시저에서 PostQuitMessage()를 호출할때 종료됨.
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}//메시지루프를 돌림.
	return (int)Message.wParam;
}

struct map {
	int x;
	int y;
	int index = -1;
	int check = 0;
};

struct Drop {
	int x;
	int y;
	int xpos;
	int ypos;
	int index;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(윈도우 핸들, 처리해야 할 메시지의 값,입력장치의 값1,2)
{
	PAINTSTRUCT ps;
	HDC hdc, memdc, memdc2, chardc; //화면 출력을 위한 DC들 chardc는 캐릭터dc입니다.
									//전부다 합쳐서 출력을 준비하는 DC들
	HBITMAP block[6];
	//	CImage g_cimgTest;
	//	HDC adddc = g_cimgTest; 
	HBITMAP backGroundSky, backGroundGround, groundBit, oldgroundBit, skyBit, oldSkyBit, charBit, oldCharBit, addBit;
	//HDC gdc = g_cimgTest.GetDC();
	static HBITMAP hbit, hbit2, oldBit1, oldBit2;
	static RECT rtChar;                              //주인공의 캐릭터 영역
	static RECT rtplane;							//충돌체크할 비행기 바나나 영역
	static RECT rtCup;			//충돌체크할 컵 바나나 영역
	HPEN hPen, oldPen;

	static int x = 200 + 300;
	static Drop play;
	static int xpos = 5;
	static int ypos = 0;
	static int y = 0;
	static int count = 0;
	static int temp;
	static map Map[10][20];
	static int Mapcount[20] = { 0 };
	switch (iMessage) //메시지의 번호
	{
	case WM_CREATE:
		play.x = x;
		play.y = y;
		play.xpos = 5;
		play.ypos = 0;
		play.index = rand() % 6;

		SetTimer(hWnd, 1, 100, NULL);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_LEFT) {
			if (play.xpos >= 0) {
				play.x -= 50;
				play.xpos -= 1;
			}
		}
		
		else if(wParam == VK_RIGHT){
			if (play.xpos <= 7) {
				play.x += 50;
				play.xpos += 1;
			}
		}
		else if (wParam == VK_RETURN) {
			if (play.index == 0)
				play.index = 3;
			else if (play.index == 3)
				play.index = 4;
			else if (play.index == 4)
				play.index = 5;
			else if (play.index == 5)
				play.index = 0;
			else if (play.index == 1)
				play.index = 2;
			else if (play.index == 2)
				play.index = 1;
		}

		break;
	case WM_CHAR:

		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			play.y += 50;
			play.ypos += 1;
			if (Map[play.xpos + 1][play.ypos].check == 1) {
				Map[play.xpos + 1][play.ypos - 1].check = 1;
				Map[play.xpos + 1][play.ypos - 1].index = play.index;
				if (play.index == 1)
					Mapcount[play.ypos - 1] += 1;
				if (Mapcount[play.ypos - 1] == 10) {
					temp = play.ypos - 1;
					for (int j = temp; j > 0; --j) {
						for (int i = 0; i < 10; ++i) {
							Map[i][j].check = Map[i][j - 1].check;
							Map[i][j].index = Map[i][j - 1].index;
							Map[i][j].x = Map[i][j - 1].x;
							Map[i][j].y = Map[i][j - 1].y;
						}
						Mapcount[j] = Mapcount[j - 1];
					}
				}
				play.x = x;
				play.y = y;
				play.xpos = 5;
				play.ypos = 0;
				play.index = rand() % 6;
			}

				if (play.ypos == 19) {
					Map[play.xpos + 1][play.ypos].check = 1;
					Map[play.xpos + 1][play.ypos].index = play.index;
					if (play.index == 1)
						Mapcount[play.ypos] += 1;
					if (Mapcount[play.ypos] == 10) {
						temp = play.ypos;
						for (int j = temp; j > 0; --j) {
							for (int i = 0; i < 10; ++i) {
								Map[i][j].check = Map[i][j - 1].check;
								Map[i][j].index = Map[i][j - 1].index;
								Map[i][j].x = Map[i][j - 1].x;
								Map[i][j].y = Map[i][j - 1].y;
							}
							Mapcount[j] = Mapcount[j - 1];
						}
					}
					play.x = x;
					play.y = y;
					play.xpos = 5;
					play.ypos = 0;
					play.index = rand() % 6;
				}
				
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		memdc2 = CreateCompatibleDC(memdc);
		//	gdc = CreateCompatibleDC(memdc);
//		hbit2 = LoadBitmap(g_hinst, MAKEINTRESOURCE(BackGroundSky));
		hbit = CreateCompatibleBitmap(hdc, 1280, 1080);

		block[0] = LoadBitmap(g_hinst, MAKEINTRESOURCE(101));
		block[1] = LoadBitmap(g_hinst, MAKEINTRESOURCE(102));
		block[2] = LoadBitmap(g_hinst, MAKEINTRESOURCE(103));
		block[3] = LoadBitmap(g_hinst, MAKEINTRESOURCE(104));
		block[4] = LoadBitmap(g_hinst, MAKEINTRESOURCE(105));
		block[5] = LoadBitmap(g_hinst, MAKEINTRESOURCE(106));

		oldBit1 = (HBITMAP)SelectObject(memdc, hbit);
		oldBit2 = (HBITMAP)SelectObject(memdc2, NULL);

		
		
		for (int i = 0; i < 10; ++i) {
			for (int j = 0; j < 10; ++j) {
				Map[j][i].x = 200 + (i * 50);
				Map[j][i].y = 200 + (j * 50);
			}
		}

		Rectangle(memdc, 200, 0, 700, 1000);
		(HBITMAP)SelectObject(memdc2, block[play.index]);
		TransparentBlt(memdc, play.x, play.y, 50, 50, memdc2, 0, 0, 50, 50, RGB(255, 255, 255)); //크기확대는 첫번쨰에서 결정함.
		SelectObject(memdc2, NULL);

		for (int i = 0; i < 10; ++i) {
			for (int j = 0; j < 20; ++j) {
				if (Map[i][j].check == 1) {
					(HBITMAP)SelectObject(memdc2, block[Map[i][j].index]);
					TransparentBlt(memdc, 200 + (50 * i), (50 * j), 50, 50, memdc2, 0, 0, 50, 50, RGB(255, 255, 255));
					SelectObject(memdc2, NULL);
				}
			}
		}
		hPen = CreatePen(PS_DOT,1,RGB(200, 200, 100));
		oldPen = (HPEN)SelectObject(memdc, hPen);
		for (int i = 0; i < 10; i++) {
			MoveToEx(memdc, 200 + 50 * (i + 1), 0, NULL);
			LineTo(memdc, 200 + 50 * (i + 1), 1000);
		}
		for (int i = 0; i < 20; i++) {
			MoveToEx(memdc, 200, 50 * (i + 1), NULL);
			LineTo(memdc, 700, 50 * (i + 1));
		}



		BitBlt(hdc, 0, 0, 1280, 1080, memdc, 0, 0, SRCCOPY);



		SelectObject(memdc, oldBit1);

		for (int i = 0; i < 6; ++i) {
			DeleteObject(block[i]);
		}

		SelectObject(memdc, oldPen);
		DeleteObject(hPen);

		DeleteObject(hbit);
		DeleteDC(memdc);
		DeleteDC(memdc2);
		DeleteObject(hbit2);



		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	//case에서 정의되지 않은 메시지는 커널이 처리하도록 메시지 전달
}



BOOL CollisionCheck(int x, int y, int mx, int my)
{
	if (mx > x&&mx<x + 150 && my>y&&my < y + 150)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
