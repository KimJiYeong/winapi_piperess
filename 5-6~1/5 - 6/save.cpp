//<**일기장**>>해야할 일 : 오른쪽 왼쪽 충돌체크 // 한바퀴 돌면 사라지는거 체크하기 
//집에가면 출력되는 애니메이션은 2차원 배열로 
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

#define MAX_QUEUE_SIZE 100
#define MAX_SIZE_X 600
//큐를 위한 선언 블록 이미지를 넣고 다음 인덱스를 보여주려면 큐 구조가 좋을것같아! 선입 선출~~
//typedef int Element;
typedef int Element;
//교과서 큐 코드
Element data[MAX_QUEUE_SIZE];
int front; int rear;
void inti_queue() { front = rear = 0; }
int is_empty() { return front == rear; }
int is_full() { return front == (rear + 1) % MAX_QUEUE_SIZE; }
int size() { return (rear - front + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE; }

void enqueue(Element val) {
	if (is_full()) {
		;
	}
	rear = (rear + 1) % MAX_QUEUE_SIZE;
	data[rear] = val;
}
Element dequeue() {
	front = (front + 1) % MAX_QUEUE_SIZE;
	return data[front];
}

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

	hWnd = CreateWindow(lpszClass, "Windows Program 1-1", WS_OVERLAPPEDWINDOW, 100, 0, 900, 1100, NULL, (HMENU)NULL, hinstance, NULL);
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
struct BLOCK {
	int select;
	int move_y;
	int move_x;
	BOOL lf, ri, up, dw = FALSE;//방향을 알려주는 BOOL
	BOOL Draw_true = TRUE; //충돌하면 그리는지 안그리는지 체크
	int down_count = 0; //블럭이 좌우로 움직이는거 체크
	int move_count = 0; //블럭이 위치할 곳 표시
};
struct BACK {
	int save_sel = -1;
	int save_y;
	int save_x;
	BOOL Draw_true = FALSE; //충돌하면 그리는지 안그리는지 체크
	int count_x;
	int count_y;
};
//UI 설정
struct UI {
	RECT re;
	POINT pt;
	BOOL install;
	int UI_r, UI_g, UI_b;
	HBRUSH UI_button, oldUI_button; //이러면 안될것같긴 한데;;;
};
static int que_save; //다음 블럭을 알려준다.
					 //------------------충돌 함수 구현

					 //충돌체크를 위한 큐를 따로 선언한다.
					 //이렇게 선언해도 되려나아
typedef float Cull;

Cull Cull_find[MAX_QUEUE_SIZE];
int Cull_front; int Cull_rear;
int Cull_size() { return (Cull_rear - Cull_front + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE; }

Cull Cull_enqueue(Cull val_x) {
	if (is_full()) {
		;
	}
	Cull_rear = (Cull_rear + 1) % MAX_QUEUE_SIZE;

	Cull_find[Cull_rear] = val_x;
	return 0;
}
Cull Cull_dequeue() {
	Cull_front = (Cull_front + 1) % MAX_QUEUE_SIZE;
	return Cull_find[Cull_front];
}

//뒷 출력
static BACK back2[40][20];
static BACK fake[20]; //체크하는 페이크 함수 한줄밖에 없으니 한줄만 선언해준다.
static int Cull_save_x[50];//체크함수
static int Cull_save_y[50];//체크함수

static POINT _save;
static int Count = 0;
//사이클 출력(회전 했는가 안했는가??)
static BOOL Cycle = TRUE;
//뭔가 이상해 
static char cycle_num[100];
static char all_check[10][100];
static BOOL Sele = FALSE;
int direct;//방향짱


		   //애니메이션 함수
static HBITMAP hAni_fly[4], hAni_Bomb[3];
static int Ani_timer, Ani_fly_count, Ani_bomb_count, Ani_save, Ani_return_count;
static BOOL Ani_bool;//애니메이션
static BACK Ani[7];


BOOL _find(int posY, int posX, HWND hWnd) { //좌표를 받아온다.

											//탐색함수를 만들자.
											//시계 방향으로 회전한다. 
											//Ani_save = 0;
	Cull_save_x[Count] = posX;
	Cull_save_y[Count] = posY;
	if (_save.x == Cull_save_x[Count] && _save.y == Cull_save_y[Count] && Count != 0) { // :(
																						//받아가서 저장한다.
		for (int i = 0; i < Count; ++i) { //나는 쓰레기야 난 배열하고 좌표값도 구분 못하는 쓰레기야 ㅠㅠ

			Ani[Ani_save].count_x = Cull_save_x[i]; //몇번째 배열인지 [19][20] //x = 950
			Ani[Ani_save].count_y = Cull_save_y[i];
			Ani_save += 1;//크기가 커가면서 save를 저장
		}
		Ani_return_count = Count;
		return TRUE;
	}
	else {
		Ani_save = 0;

	}
	if (Cycle == TRUE) {//사이클이 트루일때만
		if (back2[posY][posX].save_sel == 2)
		{
			if (direct == 0) {
				posX -= 1;
			}
			else {
				posY -= 1;
			}

			if (back2[posY][posX].save_sel == -1)	//예외
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 3 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 4) && direct == 0)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 1:
					direct = 0;
					break;
				case 3:
					direct = 1;
					break;
				case 4:
					direct = 3;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 4) && direct == 1)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 0:
					direct = 1;
					break;
				case 4:
					direct = 2;
					break;
				case 5:
					direct = 0;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else {
				Cycle = FALSE;
			}

		}//sel == 2
		else if (back2[posY][posX].save_sel == 3) {
			if (direct == 2) {
				posX += 1;
			}
			else {
				posY -= 1;
			}

			if (back2[posY][posX].save_sel == -1)	//예외
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 2) && direct == 2)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 1:
					direct = 2;
					break;
				case 2:
					direct = 1;
					break;
				case 5:
					direct = 3;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 4) && direct == 1)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 0:
					direct = 1;
					break;
				case 4:
					direct = 2;
					break;
				case 5:
					direct = 0;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else {
				Cycle = FALSE;
			}

		}//sel == 3
		else if (back2[posY][posX].save_sel == 4) {
			if (direct == 2) {
				posX += 1;
			}
			else {
				posY += 1;
			}
			if (back2[posY][posX].save_sel == -1)	//예외
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 2) && direct == 2)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 1:
					direct = 2;
					break;
				case 2:
					direct = 1;
					break;
				case 5:
					direct = 3;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else if ((back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 2 || back2[posY][posX].save_sel == 3) && direct == 3)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 0:
					direct = 3;
					break;
				case 2:
					direct = 0;
					break;
				case 3:
					direct = 2;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else {
				Cycle = FALSE;
			}

		}//sel == 4
		else if (back2[posY][posX].save_sel == 5) {
			if (direct == 0) {
				posX -= 1;
			}
			else {
				posY += 1;
			}
			if (back2[posY][posX].save_sel == -1)	//예외
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 3 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 4) && direct == 0)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 1:
					direct = 0;
					break;
				case 3:
					direct = 1;
					break;
				case 4:
					direct = 3;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else if ((back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 2 || back2[posY][posX].save_sel == 3) && direct == 3)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 0:
					direct = 3;
					break;
				case 2:
					direct = 0;
					break;
				case 3:
					direct = 2;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else {
				Cycle = FALSE;
			}

		}//sel == 5
		else if (back2[posY][posX].save_sel == 0) {
			if (direct == 1) {
				posY -= 1;
			}
			else {
				posY += 1;
			}

			if (back2[posY][posX].save_sel == -1)	//예외
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 4) && direct == 1)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 0:
					direct = 1;
					break;
				case 4:
					direct = 2;
					break;
				case 5:
					direct = 0;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else if ((back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 2 || back2[posY][posX].save_sel == 3) && direct == 3)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 0:
					direct = 3;
					break;
				case 2:
					direct = 0;
					break;
				case 3:
					direct = 2;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else {
				Cycle = FALSE;
			}

		}//sel == 0
		else if (back2[posY][posX].save_sel == 1) {
			if (direct == 0) {
				posX -= 1;
			}
			else {
				posX += 1;
			}
			if (back2[posY][posX].save_sel == -1)	//예외
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 3 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 4) && direct == 0)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 1:
					direct = 0;
					break;
				case 3:
					direct = 1;
					break;
				case 4:
					direct = 3;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 2) && direct == 2)//ㄴ 모양
			{
				Count++;
				switch (back2[posY][posX].save_sel)
				{
				case 1:
					direct = 2;
					break;
				case 2:
					direct = 1;
					break;
				case 5:
					direct = 3;
					break;
				}
				return _find(posY, posX, hWnd);
			}
			else {
				Cycle = FALSE;
			}
		}//sel == 1
		else if (back2[posY][posX].save_sel == -1) { // save_sel이 -1 이면
			Cycle = FALSE;
			Sele = FALSE;
		}



	}//cyclie 끝
	else {
		//Sele = TRUE;
		return FALSE;
	}
	return FALSE;
}

//background image
struct UI_BACK_IMAGE //백그라운드 출력하기
{
	int _back_R; //빨강
	int _back_G; //초록
	int _back_B; //파랑

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(윈도우 핸들, 처리해야 할 메시지의 값,입력장치의 값1,2)
{
	PAINTSTRUCT ps;
	HDC hdc, boardDC, aniDC;
	//더블버퍼 처리
	HDC memdc, memdc2; //더블버퍼 처리
	HBITMAP hDouble, hOld_db;
	//보드판 사이즈를 결정해주는 함수
	static int Board_sizeX, Board_sizeY, All_sizeX, All_sizeY;
	static int now_size;//칸 수
	static int  x_cungdol;
	//비트맵
	static HBITMAP hBlock_image[6], next_block, fake_block; //블록 함수 
	static int put_num;
	static BLOCK Block;
	static BACK fake[20]; //체크하는 페이크 함수 한줄밖에 없으니 한줄만 선언해준다.
	static BOOL fake_bk;//페이크함수 판단
	static HBRUSH hFake, oldFake;//페이크함수 그리기
	static int b_count = 0; //블럭 카운트
							//시간 지정
	static int All_time_count;//전체시간
	static int Set_time = 200;//설정한 스피드
	static char show_count[100];//카운트 출력
	static int stop = 0; //일시정지
	static int move_cc;//
					   //UI함수
	static BOOL ui_bool_game_over, ui_bool_game_pause = FALSE;
	static POINT mouse, end_mouse;
	static BOOL Small_board, Middle_board, Large_board, Re_start = FALSE;
	static UI ui[6];
	static char text[][20] = { "Small 5 X 10" , "Middle 10 X 20" ,"Large 20 X 40" ,"Reset","Stop" ,"EXIT" }; //UI 출력
	static UI_BACK_IMAGE ui_back[6];
	static int BG_UI_count, BG_UI_fake_count;//배경이미지 카운터
	static HBRUSH hui_back_Brush, hui_back_old_Brush;


	//----------------------------

	//떨어졌는지 안떨어졌는지 체크
	static BOOL Fall;

	switch (iMessage) //메시지의 번호
	{
	case WM_CREATE:
		//블록에 이미지를 넣어주자!
		for (int i = 0; i < 6; i++) {
			hBlock_image[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(101 + i));
		}
		for (int i = 0; i < 4; i++) {
			hAni_fly[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(201 + i));//날라다니는 비행기
		}
		for (int i = 0; i < 3; i++) {
			hAni_Bomb[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(205 + i));//폭탄 떨구기
		}
		fake_block = LoadBitmap(g_hinst, MAKEINTRESOURCE(107));

		//블록 1 , 2 -> 일자블록
		//큐에다가 집어넣어주자!
		for (int i = 0; i < 10; i++) {
			que_save = rand() % 5;
			enqueue(que_save); //큐에다가 초기 값을 10번 집어넣는다.
		}
		//보드 판 크기 정의
		All_sizeX = MAX_SIZE_X;
		All_sizeY = 1000;
		Board_sizeX = 10;
		Board_sizeY = 20;
		now_size = 2;
		Fall = FALSE;
		//이미지를 출력했으면 그 다음에 넣어야겠지?? 일단 큐에서 이미지를 받아오자!
		Block.select = 2; // 블럭에 select 이미지를 저장한다.
		que_save = rand() % 5; //이미지를 넣었으면 바로 빼도록 하자!
		enqueue(que_save);
		//--다음에 보여주는 이미지
		put_num = dequeue();
		que_save = rand() % 5; //이미지를 넣었으면 바로 빼도록 하자!
		enqueue(que_save);
		//페이크 블록 정의
		for (int i = 0; i < 20; i++) {
			fake[i].save_x = (All_sizeX / Board_sizeX) *i;
			fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			//애니메이션 블록 정의	

		}

		//페이크 블록 나타내는걸 위한 정의
		Block.move_count = 0;
		//UI 초기값
		x_cungdol = 2;

		for (int i = 0; i < 6; i++) {
			ui[i].re.left = MAX_SIZE_X + 100;
			ui[i].re.right = MAX_SIZE_X + 200;
			//----------------------------------
			ui[i].re.top = 400 + 70 * i;
			ui[i].re.bottom = 450 + 70 * i;
			ui[i].install = FALSE;
			ui[i].UI_r = 128;
			ui[i].UI_g = 176;
			ui[i].UI_b = 120;
		}
		Middle_board = TRUE;
		ui_bool_game_over = FALSE;
		ui_bool_game_pause = FALSE;

		//ui 백그라운드 이미지
		BG_UI_count = 1;

		ui_back[1]._back_R = 255;
		ui_back[1]._back_G = 255;
		ui_back[1]._back_B = 255;

		ui_back[2]._back_R = 255;
		ui_back[2]._back_G = 227;
		ui_back[2]._back_B = 119;

		ui_back[3]._back_R = 255;
		ui_back[3]._back_G = 155;
		ui_back[3]._back_B = 0;

		ui_back[4]._back_R = 255;
		ui_back[4]._back_G = 103;
		ui_back[4]._back_B = 119;

		ui_back[5]._back_R = 180;
		ui_back[5]._back_G = 27;
		ui_back[5]._back_B = 10;

		//애니메이션 초기화
		Ani_fly_count = 0;
		//그럼 게임 시작!
		SetTimer(hWnd, 1, Set_time, NULL);
		SetTimer(hWnd, 2, 100, NULL);//시간에 따라 내려오는거 구현
		break;
	case WM_KEYUP:
		//left
		if (!ui_bool_game_over) { //멈추는 2개 동작이 FALSE이면
			if (!ui_bool_game_pause) { //멈추는 2개 동작이 FALSE이면

				if (wParam == VK_LEFT) {
					if (Block.move_x > 0 && Block.move_y < fake[Block.move_count - 1].save_y) {
						Block.lf = TRUE;
						Block.ri = FALSE;

						Block.move_x -= (All_sizeX / Board_sizeX);
						Block.move_count--;
					}
				}
				//right
				else if (wParam == VK_RIGHT) {
					if (Block.move_x <= MAX_SIZE_X - 50 * x_cungdol && Block.move_y < fake[Block.move_count + 1].save_y) {
						Block.ri = TRUE;
						Block.lf = FALSE;
						Block.move_x += (All_sizeX / Board_sizeX);
						Block.move_count++;
					}
				}
				else if (wParam == VK_DOWN) {
					if (Block.move_y < fake[Block.move_count].save_y - (All_sizeY / Board_sizeY)) {
						Block.ri = FALSE;
						Block.lf = FALSE;
						Block.move_y += (All_sizeY / Board_sizeY);
						Block.down_count++;
					}
				}
				else if (wParam == VK_UP) { //순간이동 기능 완료
					Block.ri = FALSE;
					Block.lf = FALSE;
					//순간이동 얍!
					for (int i = Block.move_y; i < (fake[Block.move_count].save_y); i += (All_sizeY / Board_sizeY)) {
						Block.move_y = i;
						Block.down_count++;
					}
					Block.down_count -= 1;
				}
				else if (wParam == VK_SPACE) {
					if (Block.select == 0)
					{
						Block.select = 1;
					}
					else if (Block.select == 1)
					{
						Block.select = 0;
					}
					else if (Block.select == 2)
					{
						Block.select = 3;
					}
					else if (Block.select == 3)
					{
						Block.select = 4;
					}
					else if (Block.select == 4)
					{
						Block.select = 5;
					}
					else if (Block.select == 5)
					{
						Block.select = 2;
					}
				}
				Block.dw = TRUE;
			}
		}

		InvalidateRect(hWnd, NULL, FALSE);

		break;
	case WM_CHAR:
		//보드 사이즈를 결정해 준다.
		//우선 중 사이즈만 해주자!
		if (wParam == 'w' || wParam == 'W') {
			//배경 이미지 초기화
			BG_UI_count = 1;
			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;
			x_cungdol = 2;
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {


				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//블럭 초기화

					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;
				}
			}
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);


			//키를 누르면 처음부터 다시 시작하기 //초기화 구현
		}
		else if (wParam == 'E' || wParam == 'e') {
			//배경 이미지 초기화
			BG_UI_count = 1;
			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 4; //값을 쉽게 주기 위해서 2의 배수로 했습니다
			x_cungdol = 1;
			//-----------------초기값으로 돌려주자------------
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {


				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//블럭 초기화

					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;

				}
			}
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		else if (wParam == 'q' || wParam == 'Q') {
			//배경 이미지 초기화
			BG_UI_count = 1;

			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
			x_cungdol = 3;
			//초기값으로 돌려주자
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {
				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//블럭 초기화

					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;
				}
			}
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		else if (wParam == 'O' || wParam == 'o') {
			PostQuitMessage(0);
		}//꺼지기
		 //일시정지
		else if (wParam == 's' || wParam == 'S') {
			stop++;
			if (stop % 2 == 1) {
				ui_bool_game_pause = TRUE;
				KillTimer(hWnd, 1);
			}
			else {
				ui_bool_game_pause = FALSE;
				SetTimer(hWnd, 1, Set_time, NULL);
			}
		}
		else if (wParam == 'r' || wParam == 'R') { //리셋

			if (ui_bool_game_over) {
				ui_bool_game_over = FALSE;
			}
			//배경 이미지 초기화
			BG_UI_count = 1;

			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;
			x_cungdol = 2;
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {

				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//블럭 초기화

					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;

				}
			}
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			//down
			//충돌 체크를 해주자
			Block.move_y += (All_sizeY / Board_sizeY);
			Block.down_count++; //내려갈때마다 처리가 된다. // ->> 정상적으로 출력이 가능하다.

			for (int i = 0; i < now_size * 5; i++) { //x 좌표
													 //배경 이미지를 설정해준다.
				if (fake[i].save_y == 0) {
					ui_bool_game_over = TRUE;
					KillTimer(hWnd, 1);
					KillTimer(hWnd, 2);

				}


				if (fake[i].save_x == Block.move_x && fake[i].save_y == Block.move_y) {
					//만약 내려오는 블록과 가짜 블록이 충돌하면
					//페이크가 위로 올라가면 false ㄱㄱ

					//이미지를 출력하기 위해 처리를 해준다. (조심 !! down)
					back2[Block.down_count][i].save_sel = Block.select; //이미지 저장
																		//--위치를 출력
					back2[Block.down_count][i].save_x = fake[i].save_x; //너는 정상이고
					back2[Block.down_count][i].save_y = fake[i].save_y; //이게 문제인것 같은데...
					back2[Block.down_count][i].Draw_true = TRUE;

					fake[i].save_y -= (All_sizeY / Board_sizeY);
					//b_count 터지는걸 막아보자
					//큐 초기화
					for (int i = 0; i < 50; i++) {
						Cull_save_x[i] = NULL;
						Cull_save_y[i] = NULL;
					}
					Cycle = TRUE;
					Count = 0;
					Sele = FALSE;

					//초기 위치값을 저장해주자.
					_save.x = i;
					_save.y = Block.down_count;

					//디렉션을 정해보자
					switch (back2[Block.down_count][i].save_sel)
					{
					case 1:
						direct = 0;
						break;
					case 2:
						direct = 0;
						break;
					case 3:
						direct = 2;
						break;
					case 4:
						direct = 2;
						break;
					case 5:
						direct = 0;
						break;
					default:
						direct = 1;
						break;
					}
					//--------삭제하는 알고리즘
					Sele = _find(Block.down_count, i, hWnd);//
															//while 을 다시한번 쓰면 찬희랑 동급이다
															//----------------------------------
					if (Sele) {

						SetTimer(hWnd, 3, 100, NULL);

					}

					//새 블럭 출력--------------------------------------------------------------
					//값을 집어넣는다.
					Block.select = put_num; // 블럭은 이전 que에서 받아온 값을 읽는다.
					put_num = dequeue();
					que_save = rand() % 5; //이미지를 넣었으면 바로 빼도록 하자!
					enqueue(que_save);


					//초기값으로 돌려주자
					Block.move_x = 0;
					Block.move_y = 0;
					Block.move_count = 0;
					Block.down_count = 0;

				}
			}
			//게임 오버일때를 체크해주자!
			break;
		case 2:
			if (All_time_count % 500 == 0) { //시간이 지나면 점점 속도가 빨라진다. //50초 마다 한번씩
				Set_time -= 10;
				KillTimer(hWnd, 1);
				SetTimer(hWnd, 1, Set_time, NULL);
			}
			All_time_count++;

			if (Ani_bool) {
				Ani_timer++;
			}
			else {
				Ani_timer = 0;
				Ani_save = 0;
				Ani_fly_count = 0; //애니카운트 0으로 초기화
			}

			InvalidateRect(hWnd, NULL, FALSE); //타이머 페인트 부르는 함수
			break;
		case 3:
			Ani_bool = TRUE;//애니메이션 출력 완료!

			if (Ani_bool) {
				if (Ani_timer > Ani_return_count * 20) {
					Ani_bool = FALSE;
					KillTimer(hWnd, 3);
				}
				if (Ani_timer % 4 == 0) {
					if (Ani_save < Ani_return_count) {
						back2[Ani[Ani_save].count_y][Ani[Ani_save].count_x].Draw_true = FALSE;
						back2[Ani[Ani_save].count_y][Ani[Ani_save].count_x].save_sel = -1;
						Ani_save++;
					}
				}
				Ani_fly_count += 1;
				Ani_fly_count = Ani_fly_count % 4;


				InvalidateRect(hWnd, NULL, FALSE); //타이머 페인트 부르는 함수
			}
			break;
		}//switch 괄호
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		hDouble = CreateCompatibleBitmap(hdc, 900, 1100);
		hOld_db = (HBITMAP)SelectObject(memdc, hDouble);
		PatBlt(memdc, 0, 0, 900, 1100, WHITENESS);

		//이것은 배경입니다!
		hui_back_Brush = CreateSolidBrush(RGB(ui_back[BG_UI_count]._back_R, ui_back[BG_UI_count]._back_G, ui_back[BG_UI_count]._back_B));
		hui_back_old_Brush = (HBRUSH)SelectObject(memdc, hui_back_Brush);
		Rectangle(memdc, 0, 0, All_sizeX, All_sizeY);
		SelectObject(memdc, hui_back_old_Brush);
		DeleteObject(hui_back_Brush);

		//화면 오른쪽에 출력되는친구들이 있어야겠지?? //시스템 설정도 해두자구~!
		for (int i = 0; i <= Board_sizeX; ++i) {
			MoveToEx(memdc, (All_sizeX / Board_sizeX) * i, 0, NULL);
			LineTo(memdc, (All_sizeX / Board_sizeX) * i, All_sizeY);
		}//세로 칸
		for (int i = 0; i <= Board_sizeY; ++i) {
			MoveToEx(memdc, 0, (All_sizeY / Board_sizeY) * i, NULL);
			LineTo(memdc, All_sizeX, (All_sizeY / Board_sizeY) * i);
		}//가로 칸 이거 총 사이즈는 500 입니다!

		 //내려오는 블럭
		boardDC = CreateCompatibleDC(memdc);
		//애니메이션 출력

		SelectObject(boardDC, hBlock_image[Block.select]);
		TransparentBlt(memdc, Block.move_x, Block.move_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, RGB(255, 255, 255));

		//--오른쪽 이미지 출력
		SelectObject(boardDC, hBlock_image[put_num]);
		BitBlt(memdc, MAX_SIZE_X + 100, 100, 50, 50, boardDC, 0, 0, SRCCOPY);

		//--save image 출력
		for (int i = 0; i < now_size * 10; i++) {
			for (int j = 0; j < now_size * 5; j++) {
				if (back2[i][j].Draw_true) {
					//만약 해당 y 좌표에 해당하는게 널값이 아니면 
					//높이 출력 4등분을 시킨후에
					//-----------높이 마다 색을 다르게 출력 (경고)
					if (back2[now_size * 7][j].save_sel != -1) {
						BG_UI_count = 2;
					}
					if (back2[now_size * 5][j].save_sel != -1) {
						BG_UI_count = 3;
					}
					if (back2[now_size * 3][j].save_sel != -1) {
						BG_UI_count = 4;
					}
					if (back2[now_size * 1][j].save_sel != -1) {
						BG_UI_count = 5;
					}
					//휴우 일단 완성

					SelectObject(boardDC, hBlock_image[back2[i][j].save_sel]);
					TransparentBlt(memdc, back2[i][j].save_x, back2[i][j].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, RGB(255, 255, 255));


				}
				//애니메이션 출력
				if (Ani_bool) {
					aniDC = CreateCompatibleDC(memdc);
					SelectObject(aniDC, hAni_fly[Ani_fly_count]);


					TransparentBlt(memdc,
						back2[Ani[Ani_save].count_y][Ani[Ani_save].count_x].save_x,
						back2[Ani[Ani_save].count_y][Ani[Ani_save].count_x].save_y,
						(All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), aniDC, 0, 0, 150, 150, RGB(0, 0, 255));
					DeleteDC(aniDC);
				}

			}
		}

		hFake = CreateSolidBrush(RGB(81, 200, 200));
		oldFake = (HBRUSH)SelectObject(memdc, hFake);
		//SelectObject(boardDC, fake_block);
		//충돌체크 연습을 위한 이미지 출력
		SelectObject(boardDC, hBlock_image[Block.select]);
		StretchBlt(memdc, fake[Block.move_count].save_x, fake[Block.move_count].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, MERGECOPY);

		DeleteDC(boardDC);
		SelectObject(memdc, oldFake);
		DeleteObject(hFake);
		//UI 출력

		for (int i = 0; i < 6; i++) {
			ui[i].UI_button = CreateSolidBrush(RGB(ui[i].UI_r, ui[i].UI_g, ui[i].UI_b));
			ui[i].oldUI_button = (HBRUSH)SelectObject(memdc, ui[i].UI_button);

			Rectangle(memdc, ui[i].re.left, ui[i].re.top, ui[i].re.right, ui[i].re.bottom);
			SetBkColor(memdc, RGB(ui[i].UI_r, ui[i].UI_g, ui[i].UI_b));
			DrawText(memdc, text[i], strlen(text[i]), &ui[i].re, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			SelectObject(memdc, ui[i].oldUI_button);
			DeleteObject(ui[i].UI_button);

		}
		wsprintf(show_count, "Time : %d.%d Speed: %d ", All_time_count / 10, All_time_count % 10, 100 - Set_time);
		TextOut(memdc, MAX_SIZE_X + 100, 250, show_count, strlen(show_count));
		TextOut(memdc, MAX_SIZE_X + 100, 300, cycle_num, strlen(cycle_num));

		BitBlt(hdc, 0, 0, 900, 1100, memdc, 0, 0, SRCCOPY);
		SelectObject(memdc, hOld_db);
		EndPaint(hWnd, &ps);
		break;

		//UI를 위한 작업
	case WM_MOUSEMOVE:
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);

		for (int i = 0; i < 6; i++) {
			if (PtInRect(&ui[i].re, mouse)) {
				ui[i].UI_r = 205;
				ui[i].UI_g = 155;
				ui[i].UI_b = 155;

			}
			else {
				if (ui[4].install == TRUE) {
					ui[4].UI_r = 255;
					ui[4].UI_g = 78;
					ui[4].UI_b = 78;

				}
				ui[i].UI_r = 128;
				ui[i].UI_g = 176;
				ui[i].UI_b = 120;
			}
			ui[0].install = FALSE;
			ui[1].install = FALSE;
			ui[2].install = FALSE;
			ui[3].install = FALSE;
			ui[5].install = FALSE;


			//	}
		}

		break;
	case WM_LBUTTONDOWN:
		end_mouse.x = LOWORD(lParam);
		end_mouse.y = HIWORD(lParam);
		for (int i = 0; i < 6; i++) {
			if (PtInRect(&ui[i].re, end_mouse)) {
				//stop은 예외 처리 해준다.
				if (PtInRect(&ui[4].re, end_mouse)) {
					stop++;
					if (stop % 2 == 1) {
						ui[i].UI_r = 255;
						ui[i].UI_g = 78;
						ui[i].UI_b = 78;
						ui[4].install = TRUE;
						wsprintf(text[4], "Play");

					}
					else {
						ui[4].install = FALSE;
						ui[i].UI_r = 128;
						ui[i].UI_g = 176;
						ui[i].UI_b = 120;
						wsprintf(text[4], "Stop");

					}
				}
				else {
					ui[i].install = TRUE;
				}
			}
		}
		//install 관리
		if (ui[0].install) {

			//배경 이미지 초기화
			BG_UI_count = 1;

			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
			x_cungdol = 3;
			//초기값으로 돌려주자
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {


				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//블럭 초기화
					//애니메이션 블록 정의	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;

				}
			}
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);
		}
		//-1. middle
		else if (ui[1].install) {

			//배경 이미지 초기화
			BG_UI_count = 1;

			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;
			x_cungdol = 2;
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {

				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//블럭 초기화
					//애니메이션 블록 정의	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;

				}
			}
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		//-2 large
		else if (ui[2].install) {
			//배경 이미지 초기화
			BG_UI_count = 1;

			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 4; //값을 쉽게 주기 위해서 2의 배수로 했습니다
			x_cungdol = 1;
			//-----------------초기값으로 돌려주자------------
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {

				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//블럭 초기화
					//애니메이션 블록 정의	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;

				}
			}
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		//리셋짱?
		else if (ui[3].install) {

			//배경 이미지 초기화
			BG_UI_count = 1;

			//시간 초기화
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;
			x_cungdol = 2;
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {
				//페이크 블록 정의
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//애니메이션 블록 정의	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					//블럭 초기화
					back2[j][i].save_sel = -1; //이미지 저장
					back2[j][i].Draw_true = FALSE;

				}
			}
			ui_bool_game_over = FALSE;
			//설정 모두 저장 후에 다시 시작
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		//- 4 일시정지
		else if (ui[4].install) {
			ui_bool_game_pause = TRUE;

			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

		}
		else if (!ui[4].install) {
			ui_bool_game_pause = FALSE;
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		//- 4 EXIT
		if (ui[5].install) {
			PostQuitMessage(0);
		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	//case에서 정의되지 않은 메시지는 커널이 처리하도록 메시지 전달
}

