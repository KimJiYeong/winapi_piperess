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
//ť�� ���� ���� ��� �̹����� �ְ� ���� �ε����� �����ַ��� ť ������ �����Ͱ���! ���� ����~~
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



int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) //h�� �ǹ�? = �ڵ�
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hinst = hinstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //����
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW); //���α׷� ������ ���̴� Ŀ��
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //������ 
	WndClass.hInstance = hinstance; //���� ����ǰ� �ִ� ��ü�� �ڵ�
	WndClass.lpfnWndProc = WndProc; //���ν����Լ��� �̸� 
	WndClass.lpszClassName = lpszClass; //������ Ŭ���� ������ �̸� 
	WndClass.lpszMenuName = NULL; // �޴��� �̸�
	WndClass.style = CS_HREDRAW | CS_VREDRAW; //������ ��� ��Ÿ��
											  // ������� WNDCLASS����ü�� �����鿡 ���� ����

	RegisterClass(&WndClass); // ������ Ŭ������ �ü���� ���

	hWnd = CreateWindow(lpszClass, "Windows Program 1-1", WS_OVERLAPPEDWINDOW, 100, 0,900, 1100, NULL, (HMENU)NULL, hinstance, NULL);
	//CreateWindow(Ŭ���� �̸�,Ÿ��Ʋ �̸�,������ ��Ÿ��, ������ ��ġ��ǥx , y,������ ����ũ��, ������ ����ũ��, �θ� ������ �ڵ�, �޴� �ڵ�, �������α׷� �ν��Ͻ�, ���� ������ ����(NULL))
	ShowWindow(hWnd, nCmdShow); //�������� ȭ�� ���
	UpdateWindow(hWnd); //OS�� WM_PAINT�޽��� ����
	while (GetMessage(&Message, NULL, 0, 0))
	{
		//������ ���ν������� PostQuitMessage()�� ȣ���Ҷ� �����.
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}//�޽��������� ����.
	return (int)Message.wParam;
}
struct BLOCK {
	int select;
	int move_y;
	int move_x;
	BOOL lf, ri, up, dw = FALSE;//������ �˷��ִ� BOOL
};
struct BACK {
	int save_sel;
	int save_y;
	int save_x;
};
static int que_save;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(������ �ڵ�, ó���ؾ� �� �޽����� ��,�Է���ġ�� ��1,2)
{
	PAINTSTRUCT ps;
	HDC hdc , boardDC;
	//������ ����� �������ִ� �Լ�
	static int Board_sizeX , Board_sizeY , All_sizeX , All_sizeY;
	static int now_size;
	//��Ʈ��
	static HBITMAP hBlock_image[6] , next_block; //��� �Լ� 
	static int put_num;
	static BLOCK Block[10];
	static BACK back1[20];
	static BACK fake[20]; //üũ�ϴ� ����ũ �Լ�

	static int b_count = 0; //�� ī��Ʈ 

	//���������� �ȶ��������� üũ
	static BOOL Fall;

	switch (iMessage) //�޽����� ��ȣ
	{
	case WM_CREATE:
		//��Ͽ� �̹����� �־�����!
		for (int i = 0; i < 6; i++) {
			hBlock_image[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(101 + i));
		}
		//��� 1 , 2 -> ���ں��
		//ť���ٰ� ����־�����!
		for (int i = 0; i < 10; i++) {
			que_save = rand() % 5;
			enqueue(que_save); //ť���ٰ� �ʱ� ���� 10�� ����ִ´�.
		}
		//���� �� ũ�� ����
		All_sizeX = MAX_SIZE_X;
		All_sizeY = 1000;
		Board_sizeX = 10;
		Board_sizeY = 20;
		now_size = 2;
		Fall = FALSE;
		//�̹����� ��������� �� ������ �־�߰���?? �ϴ� ť���� �̹����� �޾ƿ���!
		Block[b_count].select = dequeue(); // ���� select �̹����� �����Ѵ�.
		que_save = rand() % 5; //�̹����� �־����� �ٷ� ������ ����!
		enqueue(que_save);
		//--������ �����ִ� �̹���
		put_num = dequeue();
		que_save = rand() % 5; //�̹����� �־����� �ٷ� ������ ����!
		enqueue(que_save);

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
		//���� ����� ������ �ش�.
		//�켱 �� ����� ������!
		if (wParam == 'M' || wParam == 'm') {
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;
		//Ű�� ������ ó������ �ٽ� �����ϱ�
		}
		else if (wParam == 'L' || wParam == 'l') {
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 3;
		}
		else if (wParam == 's' || wParam == 'S') {
			Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
		
		}


		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			//down
			if (Block[b_count].dw) {

				Block[b_count].move_y += (All_sizeY / Board_sizeY);
			}
			else {
				Block[b_count].move_y += (All_sizeY / Board_sizeY);
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		
		
		//�ٴ��ϰ� �浹�ϸ�
		//������ �ٴ��� ��������
		
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
	
		//ȭ�� �����ʿ� ��µǴ�ģ������ �־�߰���?? //�ý��� ������ �ص��ڱ�~!
		for (int i = 0; i < Board_sizeX + 1; ++i) {
			MoveToEx(hdc, (All_sizeX / Board_sizeX) * i, 0, NULL);
			LineTo(hdc, (All_sizeX / Board_sizeX) * i, All_sizeY + 0);
		}//���� ĭ
		for (int i = 0; i < Board_sizeY + 1; ++i) {
			MoveToEx(hdc, 0, (All_sizeY / Board_sizeY) * i, NULL);
			LineTo(hdc, All_sizeX, (All_sizeY / Board_sizeY) * i);
		}//���� ĭ �̰� �� ������� 500 �Դϴ�!

		//�����ʿ� ������ ������ �˷��ִ� ���
		boardDC = CreateCompatibleDC(hdc);

		SelectObject(boardDC, hBlock_image[Block[b_count].select]);
		StretchBlt(hdc, Block[b_count].move_x, Block[b_count].move_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, SRCCOPY);
		
		//--�̹��� ���
		SelectObject(boardDC, hBlock_image[put_num]);
		BitBlt(hdc, MAX_SIZE_X + 100, 100, 50, 50, boardDC, 0, 0, SRCCOPY);
		
		
		DeleteDC(boardDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	//case���� ���ǵ��� ���� �޽����� Ŀ���� ó���ϵ��� �޽��� ����
}

