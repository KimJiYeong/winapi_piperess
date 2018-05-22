//<**�ϱ���**>>�ؾ��� �� : �ڱ� // ������ ���� �浹üũ // ������ // �ѹ��� ���� ������°� üũ�ϱ� //�ʱ�ȭ ���ִ°� ����

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
	BOOL Draw_true = TRUE; //�浹�ϸ� �׸����� �ȱ׸����� üũ
	int down_count =0; //���� �¿�� �����̴°� üũ
	int move_count = 0; //���� ��ġ�� �� ǥ��
};
struct BACK {
	int save_sel = -1;
	int save_y;
	int save_x;
	BOOL Draw_true = FALSE; //�浹�ϸ� �׸����� �ȱ׸����� üũ
	
};
//UI ����
struct UI {
	RECT re;
	POINT pt;
	BOOL install;
	int UI_r, UI_g, UI_b;
	HBRUSH UI_button, oldUI_button; //�̷��� �ȵɰͰ��� �ѵ�;;;
};
static int que_save;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(������ �ڵ�, ó���ؾ� �� �޽����� ��,�Է���ġ�� ��1,2)
{
	PAINTSTRUCT ps;
	HDC hdc , boardDC;
	//������ ����� �������ִ� �Լ�
	static int Board_sizeX , Board_sizeY , All_sizeX , All_sizeY;
	static int now_size, x_cungdol;
	//��Ʈ��
	static HBITMAP hBlock_image[6] , next_block , fake_block; //��� �Լ� 
	static int put_num;
	static BLOCK Block;
	static BACK back2[40][20];
	static BACK fake[20]; //üũ�ϴ� ����ũ �Լ� ���ٹۿ� ������ ���ٸ� �������ش�.
	static BOOL fake_bk;
	static HBRUSH hFake , oldFake;
	static int b_count = 0; //�� ī��Ʈ
	//�ð� ����
	static int All_time_count;
	static int Set_time = 200;
	static char show_count[100];
	static int stop =0; //�Ͻ�����

	//UI�Լ�
	static POINT mouse , end_mouse;
	static BOOL Small_board, Middle_board, Large_board , Re_start = FALSE;
	static UI ui[6];
	static char text[][20] = { "Small 5 X 10" , "Middle 10 X 20" ,"Large 20 X 40" ,"Reset","Stop" ,"EXIT"};
	
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
		Block.select = dequeue(); // ���� select �̹����� �����Ѵ�.
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
		}
		//����ũ ��� ��Ÿ���°� ���� ����
		Block.move_count = 0;
		//UI �ʱⰪ
		x_cungdol = 2;

		for (int i = 0; i < 6; i++) {
			ui[i].re.left = MAX_SIZE_X + 100;
			ui[i].re.right = MAX_SIZE_X + 200;
			//----------------------------------
			ui[i].re.top = 400 + 70 *i;
			ui[i].re.bottom = 450 + 70 * i;
			ui[i].install = FALSE;
				ui[i].UI_r = 128;
				ui[i].UI_g = 176;
				ui[i].UI_b = 120;
		}


		Middle_board = TRUE;
		SetTimer(hWnd, 1, Set_time, NULL);
		SetTimer(hWnd, 2, 100, NULL);//�ð��� ���� �������°� ����
		break;
	case WM_KEYUP:
		//left
		if (wParam == VK_LEFT) {
			if (Block.move_x > 0 ) {
				Block.lf = TRUE;
				Block.ri = FALSE;

				Block.move_x -= (All_sizeX / Board_sizeX);
				Block.move_count--;
			
			}
			
		}
		//right
		else if (wParam == VK_RIGHT) {
				if (Block.move_x <=  MAX_SIZE_X -  50 * x_cungdol) {
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
		
		/*
		#define IDB_BITMAP1                     103 //���� + ��
#define IDB_BITMAP4                     104 // ���� + ��
#define IDB_BITMAP5                     105 // ���� + �Ʒ�
#define IDB_BITMAP6                     106 // ���� + �Ʒ�
#define IDB_BITMAP7                     107 //fake

		*/
		//down

			Block.dw = TRUE;
		InvalidateRect(hWnd, NULL, TRUE);
		
		break;
	case WM_CHAR:
		//���� ����� ������ �ش�.
		//�켱 �� ����� ������!
		if (wParam == 'w' || wParam == 'W') {
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;
			x_cungdol = 2;

			//����ũ ��� ����
			for (int i = 0; i < now_size * 5; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}
			

		//Ű�� ������ ó������ �ٽ� �����ϱ� //�ʱ�ȭ ����
		}
		else if (wParam == 'E' || wParam == 'e') {
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 4; //���� ���� �ֱ� ���ؼ� 2�� ����� �߽��ϴ�
			x_cungdol = 1;
						  //����ũ ��� ����
			for (int i = 0; i < now_size * 5; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}
			//

		}
		else if (wParam == 'q' || wParam == 'Q') {
			Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
			x_cungdol = 3;
			//����ũ ��� ����
			for (int i = 0; i < now_size * 5; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}

		
		}
		else if (wParam == 'q' || wParam == 'Q') {
			PostQuitMessage(0);
		}
		//�Ͻ�����
		else if (wParam == 's' || wParam == 'S') {
			stop++;
			if (stop % 2 == 1) {
				KillTimer(hWnd, 1);

			}
			else {
				SetTimer(hWnd, 1, Set_time, NULL);
			}
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			//down
			//�浹 üũ�� ������
			Block.move_y += (All_sizeY / Board_sizeY);
			Block.down_count++; //������������ ó���� �ȴ�. // ->> ���������� ����� �����ϴ�.

				for (int i = 0; i < now_size * 5; i++) { //x ��ǥ
					if (fake[i].save_x == Block.move_x && fake[i].save_y == Block.move_y) {
						//���� �������� ��ϰ� ��¥ ����� �浹�ϸ�
						
						 //�̹����� ����ϱ� ���� ó���� ���ش�. (���� !! down)
						back2[Block.down_count][i].save_sel = Block.select; //�̹��� ����
						//--��ġ�� ���
						back2[Block.down_count][i].save_x = fake[i].save_x; //�ʴ� �����̰�
						back2[Block.down_count][i].save_y = fake[i].save_y; //�̰� �����ΰ� ������...
						back2[Block.down_count][i].Draw_true = TRUE;
							//b_count �����°� ���ƺ���
						

							//fake ����� ���� �ø���.
							fake[i].save_y -= (All_sizeY / Board_sizeY);

							//���� ����ִ´�.
							Block.select = put_num; // ���� ���� que���� �޾ƿ� ���� �д´�.
							put_num = dequeue();
							que_save = rand() % 5; //�̹����� �־����� �ٷ� ������ ����!
							enqueue(que_save);
					//�ʱⰪ���� ��������
							Block.move_x = 0;
							Block.move_y = 0;
							Block.move_count = 0;
							Block.down_count =0;

					}
				}
			//���� �����϶��� üũ������!
				
				InvalidateRect(hWnd, NULL, TRUE);

			break;
		case 2:

			All_time_count++;
			if (All_time_count % 500 == 0) { //�ð��� ������ ���� �ӵ��� ��������.
				Set_time -= 2;
			}
			break;
		
		
		}//switch ��ȣ
	
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		//ȭ�� �����ʿ� ��µǴ�ģ������ �־�߰���?? //�ý��� ������ �ص��ڱ�~!
		for (int i = 0; i <= Board_sizeX; ++i) {
			MoveToEx(hdc, (All_sizeX / Board_sizeX) * i, 0, NULL);
			LineTo(hdc, (All_sizeX / Board_sizeX) * i, All_sizeY );
		}//���� ĭ
		for (int i = 0; i <= Board_sizeY ; ++i) {
			MoveToEx(hdc, 0, (All_sizeY / Board_sizeY) * i, NULL);
			LineTo(hdc, All_sizeX, (All_sizeY / Board_sizeY) * i);
		}//���� ĭ �̰� �� ������� 500 �Դϴ�!

		//�������� ��
		boardDC = CreateCompatibleDC(hdc);

			SelectObject(boardDC, hBlock_image[Block.select]);
			StretchBlt(hdc, Block.move_x, Block.move_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, SRCCOPY);
		
		//--������ �̹��� ���
		SelectObject(boardDC, hBlock_image[put_num]);
		BitBlt(hdc, MAX_SIZE_X + 100, 100, 50, 50, boardDC, 0, 0, SRCCOPY);
		
		
		//--save image ���
		for (int i = 0; i < now_size * 10; i++) {
			for (int j = 0; j < now_size * 5; j++) {
				if (back2[i][j].Draw_true) {
					SelectObject(boardDC, hBlock_image[back2[i][j].save_sel]);
					StretchBlt(hdc, back2[i][j].save_x, back2[i][j].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, SRCCOPY);
				
				}
		//�ź��� �ڵ� ��ȣ �����������Ϥ����Ϥ�		
			}
		}

		hFake = CreateSolidBrush(RGB(81, 238, 81));
		oldFake = (HBRUSH)SelectObject(hdc, hFake);
		//SelectObject(boardDC, fake_block);
		//�浹üũ ������ ���� �̹��� ���
		//	StretchBlt(hdc, fake[Block.move_count].save_x, fake[Block.move_count].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, SRCCOPY);
		SelectObject(boardDC, hBlock_image[Block.select]);
		StretchBlt(hdc, fake[Block.move_count].save_x, fake[Block.move_count].save_y, (All_sizeX / Board_sizeX), (All_sizeY / Board_sizeY), boardDC, 0, 0, 50, 50, MERGECOPY);

		DeleteDC(boardDC);
		SelectObject(hdc, oldFake);
		DeleteObject(hFake);
		//UI ���
		
		for (int i = 0; i < 6; i++) {
			ui[i].UI_button = CreateSolidBrush(RGB(ui[i].UI_r, ui[i].UI_g, ui[i].UI_b));
			ui[i].oldUI_button = (HBRUSH)SelectObject(hdc, ui[i].UI_button);

			Rectangle(hdc, ui[i].re.left, ui[i].re.top , ui[i].re.right, ui[i].re.bottom);
			SetBkColor(hdc , RGB(ui[i].UI_r, ui[i].UI_g, ui[i].UI_b));
			DrawText(hdc, text[i], strlen(text[i]), &ui[i].re, DT_CENTER |DT_VCENTER | DT_SINGLELINE);
			SelectObject(hdc, ui[i].oldUI_button);
			DeleteObject(ui[i].UI_button);

		}
		wsprintf(show_count, "Time : %d.%d Speed: %d ", All_time_count /10 , All_time_count % 10,  100 - Set_time);
		TextOut(hdc, MAX_SIZE_X + 100, 300, show_count, strlen(show_count));
		//UI �Ǵ�
		//-0. small
		if (ui[0].install) {
				Board_sizeX = 5;
			Board_sizeY = 10;
			now_size = 1;
			x_cungdol = 3;
			//����ũ ��� ����
			for (int i = 0; i < now_size * 5; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}
		}
		//-1. middle
		else if (ui[1].install) {
			Board_sizeX = 10;
			Board_sizeY = 20;
			now_size = 2;
			x_cungdol = 2;

			//����ũ ��� ����
			for (int i = 0; i < now_size * 5; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}

		}
		//-2 large
		else if (ui[2].install) {
			Board_sizeX = 20;
			Board_sizeY = 40;
			now_size = 4; //���� ���� �ֱ� ���ؼ� 2�� ����� �߽��ϴ�
			x_cungdol = 1;
			//����ũ ��� ����
			for (int i = 0; i < now_size * 5; i++) {
				fake[i].save_x = (All_sizeX / Board_sizeX) *i;
				fake[i].save_y = All_sizeY - (All_sizeY / Board_sizeY);
			}
			//
		}
		//-3 rescet
		else if (ui[3].install) {
			KillTimer(hWnd, 1);
			//�ʱ�ȭ ���ִ°� ����

			SetTimer(hWnd, 1, 200, NULL);

		}
		//- 4 EXIT
		else if (ui[4].install) {
	
			KillTimer(hWnd, 1);
		
		}
		else if (!ui[4].install) {
		
			SetTimer(hWnd, 1, Set_time, NULL);
		
		}
		//- 4 EXIT
		if (ui[5].install) {
			PostQuitMessage(0);
		}
		EndPaint(hWnd, &ps);
		break;

	//UI�� ���� �۾�
	case WM_MOUSEMOVE:
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);

		for (int i = 0; i < 6; i++) {
		/*	if (PtInRect(&ui[i].re, end_mouse)) {
				ui[i].UI_r = 255;
				ui[i].UI_g = 78;
				ui[i].UI_b = 78;
			}*/
		//	else {
				if (PtInRect(&ui[i].re, mouse)) {
					ui[i].UI_r = 205;
					ui[i].UI_g = 155;
					ui[i].UI_b = 155;
					
				}
				else {
					ui[i].UI_r = 128;
					ui[i].UI_g = 176;
					ui[i].UI_b = 120;
				}
				ui[i].install = FALSE;
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
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	//case���� ���ǵ��� ���� �޽����� Ŀ���� ó���ϵ��� �޽��� ����
}

