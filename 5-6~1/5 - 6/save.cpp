//<**�ϱ���**>>�ؾ��� �� : ������ ���� �浹üũ // �ѹ��� ���� ������°� üũ�ϱ� 
//�������� ��µǴ� �ִϸ��̼��� 2���� �迭�� 
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
//ť�� ���� ���� ��� �̹����� �ְ� ���� �ε����� �����ַ��� ť ������ �����Ͱ���! ���� ����~~
//typedef int Element;
typedef int Element;
//������ ť �ڵ�
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

	hWnd = CreateWindow(lpszClass, "Windows Program 1-1", WS_OVERLAPPEDWINDOW, 100, 0, 900, 1100, NULL, (HMENU)NULL, hinstance, NULL);
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
	BOOL Draw_true = TRUE; //�浹�ϸ� �׸����� �ȱ׸����� üũ
	int down_count = 0; //���� �¿�� �����̴°� üũ
	int move_count = 0; //���� ��ġ�� �� ǥ��
};
struct BACK {
	int save_sel = -1;
	int save_y;
	int save_x;
	BOOL Draw_true = FALSE; //�浹�ϸ� �׸����� �ȱ׸����� üũ
	int count_x;
	int count_y;
};
//UI ����
struct UI {
	RECT re;
	POINT pt;
	BOOL install;
	int UI_r, UI_g, UI_b;
	HBRUSH UI_button, oldUI_button; //�̷��� �ȵɰͰ��� �ѵ�;;;
};
static int que_save; //���� ���� �˷��ش�.
					 //------------------�浹 �Լ� ����

					 //�浹üũ�� ���� ť�� ���� �����Ѵ�.
					 //�̷��� �����ص� �Ƿ�����
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

//�� ���
static BACK back2[40][20];
static BACK fake[20]; //üũ�ϴ� ����ũ �Լ� ���ٹۿ� ������ ���ٸ� �������ش�.
static int Cull_save_x[50];//üũ�Լ�
static int Cull_save_y[50];//üũ�Լ�

static POINT _save;
static int Count = 0;
//����Ŭ ���(ȸ�� �ߴ°� ���ߴ°�??)
static BOOL Cycle = TRUE;
//���� �̻��� 
static char cycle_num[100];
static char all_check[10][100];
static BOOL Sele = FALSE;
int direct;//����¯


		   //�ִϸ��̼� �Լ�
static HBITMAP hAni_fly[4], hAni_Bomb[3];
static int Ani_timer, Ani_fly_count, Ani_bomb_count, Ani_save, Ani_return_count;
static BOOL Ani_bool;//�ִϸ��̼�
static BACK Ani[7];


BOOL _find(int posY, int posX, HWND hWnd) { //��ǥ�� �޾ƿ´�.

											//Ž���Լ��� ������.
											//�ð� �������� ȸ���Ѵ�. 
											//Ani_save = 0;
	Cull_save_x[Count] = posX;
	Cull_save_y[Count] = posY;
	if (_save.x == Cull_save_x[Count] && _save.y == Cull_save_y[Count] && Count != 0) { // :(
																						//�޾ư��� �����Ѵ�.
		for (int i = 0; i < Count; ++i) { //���� ������� �� �迭�ϰ� ��ǥ���� ���� ���ϴ� ������� �Ф�

			Ani[Ani_save].count_x = Cull_save_x[i]; //���° �迭���� [19][20] //x = 950
			Ani[Ani_save].count_y = Cull_save_y[i];
			Ani_save += 1;//ũ�Ⱑ Ŀ���鼭 save�� ����
		}
		Ani_return_count = Count;
		return TRUE;
	}
	else {
		Ani_save = 0;

	}
	if (Cycle == TRUE) {//����Ŭ�� Ʈ���϶���
		if (back2[posY][posX].save_sel == 2)
		{
			if (direct == 0) {
				posX -= 1;
			}
			else {
				posY -= 1;
			}

			if (back2[posY][posX].save_sel == -1)	//����
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 3 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 4) && direct == 0)//�� ���
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
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 4) && direct == 1)//�� ���
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

			if (back2[posY][posX].save_sel == -1)	//����
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 2) && direct == 2)//�� ���
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
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 4) && direct == 1)//�� ���
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
			if (back2[posY][posX].save_sel == -1)	//����
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 2) && direct == 2)//�� ���
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
			else if ((back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 2 || back2[posY][posX].save_sel == 3) && direct == 3)//�� ���
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
			if (back2[posY][posX].save_sel == -1)	//����
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 3 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 4) && direct == 0)//�� ���
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
			else if ((back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 2 || back2[posY][posX].save_sel == 3) && direct == 3)//�� ���
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

			if (back2[posY][posX].save_sel == -1)	//����
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 4) && direct == 1)//�� ���
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
			else if ((back2[posY][posX].save_sel == 0 || back2[posY][posX].save_sel == 2 || back2[posY][posX].save_sel == 3) && direct == 3)//�� ���
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
			if (back2[posY][posX].save_sel == -1)	//����
			{
				Cycle = FALSE;
			}
			else if ((back2[posY][posX].save_sel == 3 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 4) && direct == 0)//�� ���
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
			else if ((back2[posY][posX].save_sel == 5 || back2[posY][posX].save_sel == 1 || back2[posY][posX].save_sel == 2) && direct == 2)//�� ���
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
		else if (back2[posY][posX].save_sel == -1) { // save_sel�� -1 �̸�
			Cycle = FALSE;
			Sele = FALSE;
		}



	}//cyclie ��
	else {
		//Sele = TRUE;
		return FALSE;
	}
	return FALSE;
}

//background image
struct UI_BACK_IMAGE //��׶��� ����ϱ�
{
	int _back_R; //����
	int _back_G; //�ʷ�
	int _back_B; //�Ķ�

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(������ �ڵ�, ó���ؾ� �� �޽����� ��,�Է���ġ�� ��1,2)
{
	PAINTSTRUCT ps;
	HDC hdc, boardDC, aniDC;
	//������� ó��
	HDC memdc, memdc2; //������� ó��
	HBITMAP hDouble, hOld_db;
	//������ ����� �������ִ� �Լ�
	static int Board_sizeX, Board_sizeY, All_sizeX, All_sizeY;
	static int now_size;//ĭ ��
	static int  x_cungdol;
	//��Ʈ��
	static HBITMAP hBlock_image[6], next_block, fake_block; //��� �Լ� 
	static int put_num;
	static BLOCK Block;
	static BACK fake[20]; //üũ�ϴ� ����ũ �Լ� ���ٹۿ� ������ ���ٸ� �������ش�.
	static BOOL fake_bk;//����ũ�Լ� �Ǵ�
	static HBRUSH hFake, oldFake;//����ũ�Լ� �׸���
	static int b_count = 0; //�� ī��Ʈ
							//�ð� ����
	static int All_time_count;//��ü�ð�
	static int Set_time = 200;//������ ���ǵ�
	static char show_count[100];//ī��Ʈ ���
	static int stop = 0; //�Ͻ�����
	static int move_cc;//
					   //UI�Լ�
	static BOOL ui_bool_game_over, ui_bool_game_pause = FALSE;
	static POINT mouse, end_mouse;
	static BOOL Small_board, Middle_board, Large_board, Re_start = FALSE;
	static UI ui[6];
	static char text[][20] = { "Small 5 X 10" , "Middle 10 X 20" ,"Large 20 X 40" ,"Reset","Stop" ,"EXIT" }; //UI ���
	static UI_BACK_IMAGE ui_back[6];
	static int BG_UI_count, BG_UI_fake_count;//����̹��� ī����
	static HBRUSH hui_back_Brush, hui_back_old_Brush;


	//----------------------------

	//���������� �ȶ��������� üũ
	static BOOL Fall;

	switch (iMessage) //�޽����� ��ȣ
	{
	case WM_CREATE:
		//��Ͽ� �̹����� �־�����!
		for (int i = 0; i < 6; i++) {
			hBlock_image[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(101 + i));
		}
		for (int i = 0; i < 4; i++) {
			hAni_fly[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(201 + i));//����ٴϴ� �����
		}
		for (int i = 0; i < 3; i++) {
			hAni_Bomb[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(205 + i));//��ź ������
		}
		fake_block = LoadBitmap(g_hinst, MAKEINTRESOURCE(107));

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
		Block.select = 2; // ���� select �̹����� �����Ѵ�.
		que_save = rand() % 5; //�̹����� �־����� �ٷ� ������ ����!
		enqueue(que_save);
		//--������ �����ִ� �̹���
		put_num = dequeue();
		que_save = rand() % 5; //�̹����� �־����� �ٷ� ������ ����!
		enqueue(que_save);
		//����ũ ��� ����
		for (int i = 0; i < 20; i++) {
			fake[i].save_x = (All_sizeX / Board_sizeX) *i;
			fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			//�ִϸ��̼� ��� ����	

		}

		//����ũ ��� ��Ÿ���°� ���� ����
		Block.move_count = 0;
		//UI �ʱⰪ
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

		//ui ��׶��� �̹���
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

		//�ִϸ��̼� �ʱ�ȭ
		Ani_fly_count = 0;
		//�׷� ���� ����!
		SetTimer(hWnd, 1, Set_time, NULL);
		SetTimer(hWnd, 2, 100, NULL);//�ð��� ���� �������°� ����
		break;
	case WM_KEYUP:
		//left
		if (!ui_bool_game_over) { //���ߴ� 2�� ������ FALSE�̸�
			if (!ui_bool_game_pause) { //���ߴ� 2�� ������ FALSE�̸�

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
				else if (wParam == VK_UP) { //�����̵� ��� �Ϸ�
					Block.ri = FALSE;
					Block.lf = FALSE;
					//�����̵� ��!
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
		//���� ����� ������ �ش�.
		//�켱 �� ����� ������!
		if (wParam == 'w' || wParam == 'W') {
			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;
			//�ð� �ʱ�ȭ
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


				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�� �ʱ�ȭ

					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;
				}
			}
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);


			//Ű�� ������ ó������ �ٽ� �����ϱ� //�ʱ�ȭ ����
		}
		else if (wParam == 'E' || wParam == 'e') {
			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;
			//�ð� �ʱ�ȭ
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 4; //���� ���� �ֱ� ���ؼ� 2�� ����� �߽��ϴ�
			x_cungdol = 1;
			//-----------------�ʱⰪ���� ��������------------
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {


				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�� �ʱ�ȭ

					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;

				}
			}
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		else if (wParam == 'q' || wParam == 'Q') {
			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;

			//�ð� �ʱ�ȭ
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
			x_cungdol = 3;
			//�ʱⰪ���� ��������
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {
				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�� �ʱ�ȭ

					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;
				}
			}
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		else if (wParam == 'O' || wParam == 'o') {
			PostQuitMessage(0);
		}//������
		 //�Ͻ�����
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
		else if (wParam == 'r' || wParam == 'R') { //����

			if (ui_bool_game_over) {
				ui_bool_game_over = FALSE;
			}
			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;

			//�ð� �ʱ�ȭ
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

				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�� �ʱ�ȭ

					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;

				}
			}
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			//down
			//�浹 üũ�� ������
			Block.move_y += (All_sizeY / Board_sizeY);
			Block.down_count++; //������������ ó���� �ȴ�. // ->> ���������� ����� �����ϴ�.

			for (int i = 0; i < now_size * 5; i++) { //x ��ǥ
													 //��� �̹����� �������ش�.
				if (fake[i].save_y == 0) {
					ui_bool_game_over = TRUE;
					KillTimer(hWnd, 1);
					KillTimer(hWnd, 2);

				}


				if (fake[i].save_x == Block.move_x && fake[i].save_y == Block.move_y) {
					//���� �������� ��ϰ� ��¥ ����� �浹�ϸ�
					//����ũ�� ���� �ö󰡸� false ����

					//�̹����� ����ϱ� ���� ó���� ���ش�. (���� !! down)
					back2[Block.down_count][i].save_sel = Block.select; //�̹��� ����
																		//--��ġ�� ���
					back2[Block.down_count][i].save_x = fake[i].save_x; //�ʴ� �����̰�
					back2[Block.down_count][i].save_y = fake[i].save_y; //�̰� �����ΰ� ������...
					back2[Block.down_count][i].Draw_true = TRUE;

					fake[i].save_y -= (All_sizeY / Board_sizeY);
					//b_count �����°� ���ƺ���
					//ť �ʱ�ȭ
					for (int i = 0; i < 50; i++) {
						Cull_save_x[i] = NULL;
						Cull_save_y[i] = NULL;
					}
					Cycle = TRUE;
					Count = 0;
					Sele = FALSE;

					//�ʱ� ��ġ���� ����������.
					_save.x = i;
					_save.y = Block.down_count;

					//�𷺼��� ���غ���
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
					//--------�����ϴ� �˰���
					Sele = _find(Block.down_count, i, hWnd);//
															//while �� �ٽ��ѹ� ���� ����� �����̴�
															//----------------------------------
					if (Sele) {

						SetTimer(hWnd, 3, 100, NULL);

					}

					//�� �� ���--------------------------------------------------------------
					//���� ����ִ´�.
					Block.select = put_num; // ���� ���� que���� �޾ƿ� ���� �д´�.
					put_num = dequeue();
					que_save = rand() % 5; //�̹����� �־����� �ٷ� ������ ����!
					enqueue(que_save);


					//�ʱⰪ���� ��������
					Block.move_x = 0;
					Block.move_y = 0;
					Block.move_count = 0;
					Block.down_count = 0;

				}
			}
			//���� �����϶��� üũ������!
			break;
		case 2:
			if (All_time_count % 500 == 0) { //�ð��� ������ ���� �ӵ��� ��������. //50�� ���� �ѹ���
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
				Ani_fly_count = 0; //�ִ�ī��Ʈ 0���� �ʱ�ȭ
			}

			InvalidateRect(hWnd, NULL, FALSE); //Ÿ�̸� ����Ʈ �θ��� �Լ�
			break;
		case 3:
			Ani_bool = TRUE;//�ִϸ��̼� ��� �Ϸ�!

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


				InvalidateRect(hWnd, NULL, FALSE); //Ÿ�̸� ����Ʈ �θ��� �Լ�
			}
			break;
		}//switch ��ȣ
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		hDouble = CreateCompatibleBitmap(hdc, 900, 1100);
		hOld_db = (HBITMAP)SelectObject(memdc, hDouble);
		PatBlt(memdc, 0, 0, 900, 1100, WHITENESS);

		//�̰��� ����Դϴ�!
		hui_back_Brush = CreateSolidBrush(RGB(ui_back[BG_UI_count]._back_R, ui_back[BG_UI_count]._back_G, ui_back[BG_UI_count]._back_B));
		hui_back_old_Brush = (HBRUSH)SelectObject(memdc, hui_back_Brush);
		Rectangle(memdc, 0, 0, All_sizeX, All_sizeY);
		SelectObject(memdc, hui_back_old_Brush);
		DeleteObject(hui_back_Brush);

		//ȭ�� �����ʿ� ��µǴ�ģ������ �־�߰���?? //�ý��� ������ �ص��ڱ�~!
		for (int i = 0; i <= Board_sizeX; ++i) {
			MoveToEx(memdc, (All_sizeX / Board_sizeX) * i, 0, NULL);
			LineTo(memdc, (All_sizeX / Board_sizeX) * i, All_sizeY);
		}//���� ĭ
		for (int i = 0; i <= Board_sizeY; ++i) {
			MoveToEx(memdc, 0, (All_sizeY / Board_sizeY) * i, NULL);
			LineTo(memdc, All_sizeX, (All_sizeY / Board_sizeY) * i);
		}//���� ĭ �̰� �� ������� 500 �Դϴ�!

		 //�������� ��
		boardDC = CreateCompatibleDC(memdc);
		//�ִϸ��̼� ���

		SelectObject(boardDC, hBlock_image[Block.select]);
		TransparentBlt(memdc, Block.move_x, Block.move_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, RGB(255, 255, 255));

		//--������ �̹��� ���
		SelectObject(boardDC, hBlock_image[put_num]);
		BitBlt(memdc, MAX_SIZE_X + 100, 100, 50, 50, boardDC, 0, 0, SRCCOPY);

		//--save image ���
		for (int i = 0; i < now_size * 10; i++) {
			for (int j = 0; j < now_size * 5; j++) {
				if (back2[i][j].Draw_true) {
					//���� �ش� y ��ǥ�� �ش��ϴ°� �ΰ��� �ƴϸ� 
					//���� ��� 4����� ��Ų�Ŀ�
					//-----------���� ���� ���� �ٸ��� ��� (���)
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
					//�޿� �ϴ� �ϼ�

					SelectObject(boardDC, hBlock_image[back2[i][j].save_sel]);
					TransparentBlt(memdc, back2[i][j].save_x, back2[i][j].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, RGB(255, 255, 255));


				}
				//�ִϸ��̼� ���
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
		//�浹üũ ������ ���� �̹��� ���
		SelectObject(boardDC, hBlock_image[Block.select]);
		StretchBlt(memdc, fake[Block.move_count].save_x, fake[Block.move_count].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, MERGECOPY);

		DeleteDC(boardDC);
		SelectObject(memdc, oldFake);
		DeleteObject(hFake);
		//UI ���

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

		//UI�� ���� �۾�
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
				//stop�� ���� ó�� ���ش�.
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
		//install ����
		if (ui[0].install) {

			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;

			//�ð� �ʱ�ȭ
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
			x_cungdol = 3;
			//�ʱⰪ���� ��������
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {


				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�� �ʱ�ȭ
					//�ִϸ��̼� ��� ����	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;

				}
			}
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);
		}
		//-1. middle
		else if (ui[1].install) {

			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;

			//�ð� �ʱ�ȭ
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

				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�� �ʱ�ȭ
					//�ִϸ��̼� ��� ����	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;

				}
			}
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		//-2 large
		else if (ui[2].install) {
			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;

			//�ð� �ʱ�ȭ
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);

			All_time_count = 0;
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 4; //���� ���� �ֱ� ���ؼ� 2�� ����� �߽��ϴ�
			x_cungdol = 1;
			//-----------------�ʱⰪ���� ��������------------
			Block.move_x = 0;
			Block.move_y = 0;
			Block.move_count = 0;
			Block.down_count = 0;
			Set_time = 200;

			for (int j = 0; j < Board_sizeY; j++) {

				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�� �ʱ�ȭ
					//�ִϸ��̼� ��� ����	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;

				}
			}
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		//����¯?
		else if (ui[3].install) {

			//��� �̹��� �ʱ�ȭ
			BG_UI_count = 1;

			//�ð� �ʱ�ȭ
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
				//����ũ ��� ����
				for (int i = 0; i < now_size * 5; i++) {
					fake[i].save_x = (All_sizeX / Board_sizeX) *i;
					fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
					//�ִϸ��̼� ��� ����	
					Ani[i].save_x = (All_sizeX / Board_sizeX) *i;
					Ani[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);

					//�� �ʱ�ȭ
					back2[j][i].save_sel = -1; //�̹��� ����
					back2[j][i].Draw_true = FALSE;

				}
			}
			ui_bool_game_over = FALSE;
			//���� ��� ���� �Ŀ� �ٽ� ����
			SetTimer(hWnd, 1, Set_time, NULL);
			SetTimer(hWnd, 2, 100, NULL);

		}
		//- 4 �Ͻ�����
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
	//case���� ���ǵ��� ���� �޽����� Ŀ���� ó���ϵ��� �޽��� ����
}

