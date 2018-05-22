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

#define MAX_QUEUE_SIZE 20
#define MAX_SIZE_X 600
//큐를 위한 선언 블록 이미지를 넣고 다음 인덱스를 보여주려면 큐 구조가 좋을것같아! 선입 선출~~
typedef int Element;
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
	int down_count = 0;
};
struct BACK {
	int save_sel = -1;
	int save_y;
	int save_x;
	BOOL Draw_true = FALSE; //충돌하면 그리는지 안그리는지 체크

};
static int que_save;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(윈도우 핸들, 처리해야 할 메시지의 값,입력장치의 값1,2)
{
	PAINTSTRUCT ps;
	HDC hdc, boardDC;
	//보드판 사이즈를 결정해주는 함수
	static int Board_sizeX, Board_sizeY, All_sizeX, All_sizeY;
	static int now_size;
	//비트맵
	static HBITMAP hBlock_image[6], next_block, fake_block; //블록 함수 
	static int put_num;
	static BLOCK Block[100];
	static BACK back2[40][20];
	static BACK fake[20]; //체크하는 페이크 함수 한줄밖에 없으니 한줄만 선언해준다.
	static BOOL fake_bk;

	static int b_count = 0; //블럭 카운트 

							//떨어졌는지 안떨어졌는지 체크
	static BOOL Fall;

	switch (iMessage) //메시지의 번호
	{
	case WM_CREATE:
		//블록에 이미지를 넣어주자!
		for (int i = 0; i < 6; i++) {
			hBlock_image[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(101 + i));
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
		Block[b_count].select = dequeue(); // 블럭에 select 이미지를 저장한다.
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
		}


		SetTimer(hWnd, 1, 200, NULL);
		break;
	case WM_KEYDOWN:
		//left
		if (wParam == VK_LEFT) {
			Block[b_count].lf = TRUE;
			Block[b_count].ri = FALSE;

			Block[b_count].move_x -= (All_sizeX / Board_sizeX);
		}
		//right
		else if (wParam == VK_RIGHT) {
			Block[b_count].ri = TRUE;
			Block[b_count].lf = FALSE;
			Block[b_count].move_x += (All_sizeX / Board_sizeX);

		}

		//down

		Block[b_count].dw = TRUE;
		InvalidateRect(hWnd, NULL, TRUE);

		break;
	case WM_CHAR:
		//보드 사이즈를 결정해 준다.
		//우선 중 사이즈만 해주자!
		if (wParam == 'M' || wParam == 'm') {
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;

			//페이크 블록 정의
			for (int i = 0; i < 20; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}


			//키를 누르면 처음부터 다시 시작하기
		}
		else if (wParam == 'L' || wParam == 'l') {
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 4; //값을 쉽게 주기 위해서 2의 배수로 했습니다
						  //페이크 블록 정의
			for (int i = 0; i < 20; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}

		}
		else if (wParam == 's' || wParam == 'S') {
			Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
			//페이크 블록 정의
			for (int i = 0; i < 20; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}


		}


		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			//down
			Block[b_count].move_y += (All_sizeY / Board_sizeY);
			Block[b_count].down_count++; //내려갈때마다 처리가 된다. // ->> 정상적으로 출력이 가능하다.
										 //충돌 체크를 해주자

			for (int i = 0; i < now_size * 5; i++) { //x 좌표
				if (fake[i].save_x == Block[b_count].move_x && fake[i].save_y == Block[b_count].move_y) {
					//만약 내려오는 블록과 가짜 블록이 충돌하면
					Block[b_count].Draw_true = FALSE; //이미지를 꺼주고

													  //이미지를 출력하기 위해 처리를 해준다. (조심 !! down)
					back2[Block[b_count].down_count][i].save_sel = Block[b_count].select; //이미지 저장
																						  //--위치를 출력
					back2[Block[b_count].down_count][i].save_x = fake[i].save_x; //너는 정상이고
					back2[Block[b_count].down_count][i].save_y = fake[i].save_y; //이게 문제인것 같은데...
					back2[Block[b_count].down_count][i].Draw_true = TRUE;
					//b_count 터지는걸 막아보자
					if (b_count > 100) { //떨어지는 블록이 100개 이상이면
						b_count = 0;
					}
					else {
						b_count++;
					}
					//	Block[b_count].down_count = 0;


					//fake 블록을 위로 올린다.
					fake[i].save_y -= (All_sizeY / Board_sizeY);

					//값을 집어넣는다.
					Block[b_count].select = put_num; // 블럭은 이전 que에서 받아온 값을 읽는다.
					put_num = dequeue();
					que_save = rand() % 5; //이미지를 넣었으면 바로 빼도록 하자!
					enqueue(que_save);

				}
			}


			InvalidateRect(hWnd, NULL, TRUE);
			break;


			//바닥하고 충돌하면
			//그전에 바다을 정의하자

		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		//화면 오른쪽에 출력되는친구들이 있어야겠지?? //시스템 설정도 해두자구~!
		for (int i = 0; i < Board_sizeX + 1; ++i) {
			MoveToEx(hdc, (All_sizeX / Board_sizeX) * i, 0, NULL);
			LineTo(hdc, (All_sizeX / Board_sizeX) * i, All_sizeY + 0);
		}//세로 칸
		for (int i = 0; i < Board_sizeY + 1; ++i) {
			MoveToEx(hdc, 0, (All_sizeY / Board_sizeY) * i, NULL);
			LineTo(hdc, All_sizeX, (All_sizeY / Board_sizeY) * i);
		}//가로 칸 이거 총 사이즈는 500 입니다!

		 //오른쪽에 나오는 다음을 알려주는 모양
		boardDC = CreateCompatibleDC(hdc);

		if (Block[b_count].Draw_true) {
			SelectObject(boardDC, hBlock_image[Block[b_count].select]);
			StretchBlt(hdc, Block[b_count].move_x, Block[b_count].move_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, SRCCOPY);
		}

		//--오른쪽 이미지 출력
		SelectObject(boardDC, hBlock_image[put_num]);
		BitBlt(hdc, MAX_SIZE_X + 100, 100, 50, 50, boardDC, 0, 0, SRCCOPY);


		//--save image 출력
		for (int i = 0; i < now_size * 10; i++) {
			for (int j = 0; j < now_size * 5; j++) {
				if (back2[i][j].Draw_true) {
					SelectObject(boardDC, hBlock_image[back2[i][j].save_sel]);
					StretchBlt(hdc, back2[i][j].save_x, back2[i][j].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, SRCCOPY);
				}
				//신비의 코드 얏호 히히히하하하ㅏ하하ㅏ		
			}
		}


		SelectObject(boardDC, fake_block);
		//충돌체크 연습을 위한 이미지 출력
		for (int i = 0; i < now_size * 5; i++) {
			StretchBlt(hdc, fake[i].save_x, fake[i].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, SRCCOPY);
		}

		DeleteDC(boardDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	//case에서 정의되지 않은 메시지는 커널이 처리하도록 메시지 전달
}

