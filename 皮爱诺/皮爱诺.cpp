#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include<graphics.h>
#include<Windows.h>
#include<mmsystem.h>
#include<time.h>
#include"曲谱资源.h"
#include <iostream>
using namespace std;
#pragma comment(lib,"winmm.lib")

//-----------------------------宏定义-------------------------------------
#define KEYWIDTH      55	//琴键宽度
#define KEYHEIGHT    300	 //格子高度
#define BLOCKWIDTH  55	//方块宽度
#define BLOCKHEIGHT  70	//方块高度
#define KEYNUM	 21	//白键列数
#define GAMEWIDTH   (KEYWIDTH*KEYNUM)	//游戏框宽度
#define GAMEHEIGHT  (KEYHEIGHT*2)	//游戏框高度
//---------------------------------------------------------------------------

//-----------------------------全局变量声明---------------------------------
char music[1999]; //字符乐谱
int Music[1999]; //数字乐谱
int idx = 0; //音乐数组下表索引
int eofm; //谱子的最后一个音符下标

bool success = TRUE; //欢迎界面的循环退出条件
int decision; //进入的模式编号

int manual_flag; //手动模式选谱标志
int auto_flag; //自动模式选谱标志
bool manual_choice = TRUE; //手动模式退出选谱菜单
bool auto_choice = TRUE; //自动模式退出选谱菜单
bool manual_success = TRUE; //手动模式退出播放
bool auto_success = TRUE; //自动模式退出播放
bool manual_goon = TRUE; //手动模式判断是否已点击谱字
bool auto_goon = TRUE; //自动模式判断是否已点击谱字
bool manual_judge = TRUE; //手动模式判断游戏是否已结束
bool auto_judge = TRUE; //自动模式判断游戏是否已结束

IMAGE white1, white2, white3, black, grey1, grey2, grey3, blackgrey; //图片变量
HWND hWnd; //弹窗句柄

//描述字母，包含字母属性
struct letter
{
	int x;
	int y;
	char target;
};
struct letter charbuff[1999];
//-----------------------------------------------------------------------------------------

//------------------------------------函数原型--------------------------------------------
void loadresource(); //加载图片信息
void drawMenu(); //画主菜单
int menuRespond(); //主菜单响应

void manualMode(); //手动模式
void autoMode(); //自动模式
void gameIntroduction(); //游戏介绍

void initialize(); //初始化琴键图片界面
void playPIANO(int i); //弹出音乐并产生按动特效

void musicMenu(); //选择音乐函数
int translate(char); //将谱中字符翻译成整型方便对应不同轨道
void initchar(struct letter array[], int[], int curPos); //初始化滑块信息
void outtextxy_img(int, int, char); //生成响应的滑块

int getX(int); //滑块横坐标匹配
int getY(int, int); //滑块纵坐标匹配
char getTarget(int); //滑块字母匹配
//----------------------------------------------------------------------------------------------


int main()
{
	//加载资源
	loadresource();

	//创建菜单窗口并将选择的模式记录
	decision = menuRespond();
	closegraph();

	//进入游戏初始化界面
	//进入手动模式
	if (decision == 1)
	{
		//进入手动模式
		manualMode();
	}
	//进入自动模式
	else if (decision == 2)
	{
		autoMode();
	}
	//进入手动模式
	else if (decision == 3)
	{
		gameIntroduction();
	}

	closegraph();
	return 0;
}

void loadresource()
{
	loadimage(&white1, "../picture/white1.bmp");
	loadimage(&white2, "../picture/white2.bmp");
	loadimage(&white3, "../picture/white3.bmp");
	loadimage(&black, "../picture/black.bmp");
	loadimage(&grey1, "../picture/grey1.bmp");
	loadimage(&grey2, "../picture/grey2.bmp");
	loadimage(&grey3, "../picture/grey3.bmp");
	loadimage(&blackgrey, "../picture/blackgrey.bmp");
}

void drawMenu()
{
	mciSendString("open ../resource/Background.mp3 alias m", 0, 0, 0);
	mciSendString("play m repeat", 0, 0, 0);

	setbkcolor(WHITE);							//窗口颜色设置为白色
	cleardevice();									//刷新才能显示

	//画背景格子
	setlinecolor(RGB(100, 200, 255));
	for (int i = 1; i < 46; i++)
	{
		line(10 * i, 0, 10 * i, 600);
	}
	for (int i = 1; i < 61; i++)
	{
		line(0, 10 * i, 450, 10 * i);
	}

	//按钮绘制
	setfillcolor(RGB(249, 205, 173));					//设置填充颜色为浅粉色
	fillroundrect(50, 400, 190, 450, 20, 20);
	fillroundrect(50, 500, 190, 550, 20, 20);
	fillroundrect(260, 400, 400, 450, 20, 20);
	fillroundrect(260, 500, 400, 550, 20, 20);

	//文字处理
	settextcolor(BLACK);
	settextstyle(90, 0, "微软雅黑", 0, 0, 0, false, false, false, 0, 0, 0, PROOF_QUALITY, 0);
	setbkmode(TRANSPARENT);
	outtextxy(95, 0, "皮 爱 诺");

	settextstyle(35, 0, "微软雅黑", 0, 0, 0, false, false, false, 0, 0, 0, PROOF_QUALITY, 0);
	setbkmode(TRANSPARENT);
	outtextxy(66, 407, "手动模式");
	outtextxy(66, 507, "自动模式");
	outtextxy(276, 407, "游戏说明");
	outtextxy(276, 507, "退出游戏");

	settextstyle(25, 0, "微软雅黑", 0, 0, 0, false, false, false, 0, 0, 0, PROOF_QUALITY, 0);
	outtextxy(80, 570, "Fernando & Owen  Edition 3.0");

	//桌面特效
	//画白键
	setfillcolor(RGB(255, 255, 255));
	setlinecolor(RGB(0, 0, 0));
	for (int i = 0; i < 7; i++)
		fillrectangle(50 + i * 50, 150, 50 + (i + 1) * 50, 350);

	//画黑键
	setfillcolor(RGB(0, 0, 0));
	for (int i = 0; i < 5; i++)
	{
		if (i != 0 && i != 3)
			fillrectangle(50 + i * 50 - 15, 150, 50 + i * 50 + 15, 275);
	}

	//循环放特效
	do
	{
		for (int i = 0; i < 7 && success; i++)
		{
			if (MouseHit())
				success = false;
			setfillcolor(RGB(144, 144, 144));
			fillrectangle(50 + i * 50, 150, 50 + (i + 1) * 50, 350);
			setfillcolor(RGB(0, 0, 0));
			fillrectangle(85, 150, 115, 275);
			fillrectangle(135, 150, 165, 275);
			fillrectangle(235, 150, 265, 275);
			fillrectangle(285, 150, 315, 275);
			fillrectangle(335, 150, 365, 275);

			Sleep(100);

			setfillcolor(RGB(255, 255, 255));
			fillrectangle(50 + i * 50, 150, 50 + (i + 1) * 50, 350);
			setfillcolor(RGB(0, 0, 0));
			fillrectangle(85, 150, 115, 275);
			fillrectangle(135, 150, 165, 275);
			fillrectangle(235, 150, 265, 275);
			fillrectangle(285, 150, 315, 275);
			fillrectangle(335, 150, 365, 275);
			Sleep(200);
		}
	} while (success);
}

int menuRespond()
{
	initgraph(450, 600);
	drawMenu();

	//鼠标交互
	MOUSEMSG m;  //存储鼠标的值
	while (1)
	{
		m = GetMouseMsg();	//获取鼠标的值

		//判断鼠标是否在需响应区域
		//手动模式
		if (m.x >= 50 && m.x <= 190 && m.y >= 400 && m.y <= 450)
		{
			setlinecolor(RGB(249, 205, 173));
			roundrect(45, 395, 195, 455, 25, 25);
			//鼠标点击处理
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString("close m", 0, 0, 0);
				return 1;
			}
		}
		//自动模式
		else if (m.x >= 50 && m.x <= 190 && m.y >= 500 && m.y <= 550)
		{
			setlinecolor(RGB(249, 205, 173));
			roundrect(45, 495, 195, 555, 25, 25);
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				mciSendString("close m", 0, 0, 0);
				return 2;
			}
		}
		//游戏说明
		else if (m.x >= 260 && m.x <= 400 && m.y >= 400 && m.y <= 450)
		{
			setlinecolor(RGB(249, 205, 173));
			roundrect(255, 395, 405, 455, 25, 25);
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				return 3;
			}
		}
		//退出游戏
		else if (m.x >= 260 && m.x <= 500 && m.y >= 400 && m.y <= 550)
		{
			setlinecolor(RGB(249, 205, 173));
			roundrect(255, 495, 405, 555, 25, 25);
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				exit(0);
			}
		}
		else//鼠标不在以上范围时
		{
			setlinecolor(WHITE);
			roundrect(45, 395, 195, 455, 25, 25);
			roundrect(45, 495, 195, 555, 25, 25);
			roundrect(255, 395, 405, 455, 25, 25);
			roundrect(255, 495, 405, 555, 25, 25);
		}
	}
}

void initchar(struct letter array[], int copy[], int curPos)
{
	//根据动态数组的值来完成三个值的初始化
	array[curPos].x = getX(copy[curPos]);
	array[curPos].y = getY(copy[curPos], curPos);
	array[curPos].target = getTarget(copy[curPos]);
}

void outtextxy_img(int x, int y, char target)
{
	if (target == '1' || target == '2' || target == '3' || target == '4' || target == '5'
		|| target == '6' || target == '7' || target == 'A' || target == 'S' || target == 'D'
		|| target == 'F' || target == 'G' || target == 'H' || target == 'J' || target == 'Y'
		|| target == 'U' || target == 'I' || target == 'O' || target == 'P' || target == '['
		|| target == ']')
	{
		setfillcolor(LIGHTBLUE);
		setbkmode(TRANSPARENT);
		settextstyle(30, 0, "微软雅黑", 0, 0, 0, true, false, false, 0, 0, 0, PROOF_QUALITY, 0);
		settextcolor(WHITE);
		fillrectangle(x, y, x + 55, y + 70);
		outtextxy(x + 15, y + 20, target);
	}
	else if (target == '/')
	{
		setfillcolor(BLACK);
		fillrectangle(x, y, x + 0.1, y + 0.1);
	}
	else if (target == ' ' || target == '.')
	{
		fillrectangle(x, y, x + 0.1, y + 0.1);
	}
	else
	{
		setfillcolor(LIGHTRED);
		setbkmode(TRANSPARENT);
		settextstyle(30, 0, "微软雅黑", 0, 0, 0, true, false, false, 0, 0, 0, PROOF_QUALITY, 0);
		settextcolor(BLACK);
		fillrectangle(x, y, x + 41, y + 70);
		outtextxy(x + 13, y + 18, target);
	}
}

void gameIntroduction()
{
	initgraph(GAMEWIDTH, GAMEHEIGHT);

	setbkcolor(WHITE);						//窗口颜色设置为白色
	cleardevice();								 //刷新才能显示
	setfillcolor(RGB(167, 220, 224));	//设置填充颜色为蓝色

	fillroundrect(1025, 15, 1145, 65, 20, 20);
	settextstyle(30, 0, "微软雅黑", 0, 0, 0, false, false, false, 0, 0, 0, PROOF_QUALITY, 0);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	outtextxy(1060, 24, "返回");

	settextstyle(25, 0, "微软雅黑", 0, 0, 0, false, false, false, 0, 0, 0, PROOF_QUALITY, 0);
	outtextxy(110, 50, "核心玩法：");
	outtextxy(150, 100, "这是一款音乐类游戏，玩家在游戏中可以伴随音乐节奏根据音符提示敲击键盘以获");
	outtextxy(110, 150, "得畅快的游戏体验，游戏过程中不仅有音乐带来的听觉享受，更有“演奏”音乐带来的");
	outtextxy(110, 200, "爽快感。");
	outtextxy(110, 250, "游戏模式：");
	outtextxy(150, 300, "目前玩家在游戏中可选择手动模式以及自动模式。手动模式下，玩家需要自己敲击");
	outtextxy(110, 350, "键盘来打击掉落下来的音符获得分数，每个音符都敲对可以演奏出一段美妙的音乐哦。");
	outtextxy(110, 400, "自动模式下，玩家可自行操控钢琴键盘，随心所欲的弹奏自己喜欢的歌曲。");
	outtextxy(150, 450, "祝您玩的愉快。");

	settextstyle(20, 0, "微软雅黑", 0, 0, 0, false, false, false, 0, 0, 0, PROOF_QUALITY, 0);
	outtextxy((GAMEWIDTH - textwidth("Fernando & Owen  Edition 3.0")) / 2, 550, "Fernando & Owen  Edition 3.0");

	MOUSEMSG m;
	while (1)
	{
		m = GetMouseMsg();  //获取鼠标的值
		//判断鼠标是否在需响应区域
		//移到返回区域
		if (m.x >= 1025 && m.x <= 1145 && m.y >= 15 && m.y <= 65)
		{
			setlinecolor(RGB(167, 220, 224));
			roundrect(1020, 10, 1150, 70, 25, 25);
			//鼠标点击处理
			if (m.uMsg == WM_LBUTTONDOWN)
			{
				success = TRUE;
				main();
			}
		}
		else//鼠标不在以上范围时
		{
			setlinecolor(WHITE);
			roundrect(1020, 10, 1150, 70, 25, 25);
		}
	}
}

//播放音乐并产生特效
void playPIANO(int i)
{
	char buff[256] = { 0 };
	sprintf_s(buff, "open ../resource/%d.MP3 alias m", i);
	printf("buff:%s\n", buff);

	//打开音乐文件
	mciSendString(buff, NULL, NULL, NULL);
	mciSendString("play m", NULL, NULL, NULL);

	//第三音域 白色琴键
	if (i == 121)
	{
		putimage(0, 300, &grey1);
		Sleep(100);
		putimage(0, 300, &white1);
	}
	if (i == 117)
	{
		putimage(55, 300, &grey2);
		Sleep(100);
		putimage(55, 300, &white2);
	}
	if (i == 105)
	{
		putimage(110, 300, &grey3);
		Sleep(100);
		putimage(110, 300, &white3);
	}
	if (i == 111)
	{
		putimage(165, 300, &grey1);
		Sleep(100);
		putimage(165, 300, &white1);
	}
	if (i == 112)
	{
		putimage(220, 300, &grey2);
		Sleep(100);
		putimage(220, 300, &white2);
	}
	if (i == 91)
	{
		putimage(275, 300, &grey2);
		Sleep(100);
		putimage(275, 300, &white2);
	}
	if (i == 93)
	{
		putimage(330, 300, &grey3);
		Sleep(100);
		putimage(330, 300, &white3);
	}

	//第三音域 黑色琴键
	if (i == 8)
	{
		putimage(35, 300, &blackgrey);
		Sleep(100);
		putimage(35, 300, &black);
	}
	if (i == 9)
	{
		putimage(90, 300, &blackgrey);
		Sleep(100);
		putimage(90, 300, &black);
	}

	if (i == 0)
	{
		putimage(200, 300, &blackgrey);
		Sleep(100);
		putimage(200, 300, &black);
	}
	if (i == 45)
	{
		putimage(255, 300, &blackgrey);
		Sleep(100);
		putimage(255, 300, &black);
	}
	if (i == 61)
	{
		putimage(310, 300, &blackgrey);
		Sleep(100);
		putimage(310, 300, &black);
	}

	//第四音域 白色琴键
	if (i == 1)
	{
		putimage(385, 300, &grey1);
		Sleep(100);
		putimage(385, 300, &white1);
	}
	if (i == 2)
	{
		putimage(440, 300, &grey2);
		Sleep(100);
		putimage(440, 300, &white2);
	}
	if (i == 3)
	{
		putimage(495, 300, &grey3);
		Sleep(100);
		putimage(495, 300, &white3);
	}
	if (i == 4)
	{
		putimage(550, 300, &grey1);
		Sleep(100);
		putimage(550, 300, &white1);
	}
	if (i == 5)
	{
		putimage(605, 300, &grey2);
		Sleep(100);
		putimage(605, 300, &white2);
	}
	if (i == 6)
	{
		putimage(660, 300, &grey2);
		Sleep(100);
		putimage(660, 300, &white2);
	}
	if (i == 7)
	{
		putimage(715, 300, &grey3);
		Sleep(100);
		putimage(715, 300, &white3);
	}

	//第四音域 黑色琴键
	if (i == 113)
	{
		putimage(420, 300, &blackgrey);
		Sleep(100);
		putimage(420, 300, &black);
	}
	if (i == 119)
	{
		putimage(475, 300, &blackgrey);
		Sleep(100);
		putimage(475, 300, &black);
	}
	if (i == 101)
	{
		putimage(585, 300, &blackgrey);
		Sleep(100);
		putimage(585, 300, &black);
	}
	if (i == 114)
	{
		putimage(640, 300, &blackgrey);
		Sleep(100);
		putimage(640, 300, &black);
	}
	if (i == 116)
	{
		putimage(695, 300, &blackgrey);
		Sleep(100);
		putimage(695, 300, &black);
	}

	//第五音域 白色琴键
	if (i == 97)
	{
		putimage(770, 300, &grey1);
		Sleep(100);
		putimage(770, 300, &white1);
	}
	if (i == 115)
	{
		putimage(825, 300, &grey2);
		Sleep(100);
		putimage(825, 300, &white2);
	}
	if (i == 100)
	{
		putimage(880, 300, &grey3);
		Sleep(100);
		putimage(880, 300, &white3);
	}
	if (i == 102)
	{
		putimage(935, 300, &grey1);
		Sleep(100);
		putimage(935, 300, &white1);
	}
	if (i == 103)
	{
		putimage(990, 300, &grey2);
		Sleep(100);
		putimage(990, 300, &white2);
	}
	if (i == 104)
	{
		putimage(1045, 300, &grey2);
		Sleep(100);
		putimage(1045, 300, &white2);
	}
	if (i == 106)
	{
		putimage(1100, 300, &grey3);
		Sleep(100);
		putimage(1100, 300, &white3);
	}

	//第五音域 黑色琴键
	if (i == 122)
	{
		putimage(805, 300, &blackgrey);
		Sleep(100);
		putimage(805, 300, &black);
	}
	if (i == 120)
	{
		putimage(860, 300, &blackgrey);
		Sleep(100);
		putimage(860, 300, &black);
	}
	if (i == 99)
	{
		putimage(970, 300, &blackgrey);
		Sleep(100);
		putimage(970, 300, &black);
	}
	if (i == 118)
	{
		putimage(1025, 300, &blackgrey);
		Sleep(100);
		putimage(1025, 300, &black);
	}
	if (i == 98)
	{
		putimage(1080, 300, &blackgrey);
		Sleep(100);
		putimage(1080, 300, &black);
	}
	Sleep(800);
	mciSendString("close m", NULL, NULL, NULL);
}

void initialize()
{
	//贴类型1白键
	for (int i = 0; i < 21; i++)
	{
		if (i % 7 == 0 || i % 7 == 3)
		{
			putimage(i * KEYWIDTH, GAMEHEIGHT - KEYHEIGHT, &white1);
		}
		else
			continue;
	}

	//贴类型2白键
	for (int i = 0; i < 21; i++)
	{
		if (i % 7 == 1 || i % 7 == 4 || i % 7 == 5)
		{
			putimage(i * KEYWIDTH, GAMEHEIGHT - KEYHEIGHT, &white2);
		}
		else
			continue;
	}

	//贴类型3白键
	for (int i = 0; i < 21; i++)
	{
		if (i % 7 == 2 || i % 7 == 6)
		{
			putimage(i * KEYWIDTH, GAMEHEIGHT - KEYHEIGHT, &white3);
		}
		else
			continue;
	}

	//贴类型3白键
	for (int i = 0; i < 21; i++)
	{
		if (i % 7 == 2 || i % 7 == 6)
		{
			putimage(i * KEYWIDTH, GAMEHEIGHT - KEYHEIGHT, &white3);
		}
		else
			continue;
	}

	//贴黑键
	for (int i = 0; i < 21; i++)
	{
		if (i % 7 != 0 && i % 7 != 3)
		{
			putimage(i * KEYWIDTH - 20, GAMEHEIGHT - KEYHEIGHT, &black);
		}
		else
			continue;
	}
}

void musicMenu()
{
	setbkcolor(BLACK);
	cleardevice();
	initialize();

	setfillcolor(RGB(160, 191, 124));
	solidrectangle(20, 20, 50, 25);
	solidrectangle(20, 28, 50, 33);
	solidrectangle(20, 36, 50, 41);

	settextstyle(30, 0, "微软雅黑", 0, 0, 100, false, false, false, 0, 0, 0, PROOF_QUALITY, 0);
	setbkmode(TRANSPARENT);
	solidroundrect(1040, 15, 1150, 65, 20, 20);
	outtextxy(1073, 25, "返回");

	//根据不同模式分流处理
	if (decision == 1)
	{
		while (manual_choice)
		{
			//鼠标交互
			MOUSEMSG t;
			t = GetMouseMsg();			//获取鼠标的值
			//判断鼠标是否在需响应区域

			if (t.x >= 20 && t.x <= 50 && t.y >= 20 && t.y <= 41)
			{
				setfillcolor(RGB(160, 191, 124));
				rectangle(15, 15, 55, 46);

				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setfillcolor(RGB(160, 191, 124));
					rectangle(15, 15, 55, 46);

					manual_goon = FALSE;

					setfillcolor(RGB(160, 191, 124));
					settextstyle(23, 0, "微软雅黑", 0, 0, 0, true, false, false, 0, 0, 0, PROOF_QUALITY, 0);
					setbkmode(TRANSPARENT);

					solidrectangle(70, 20, 200, 50);
					outtextxy(80, 22, "串烧");
					Sleep(25);
					solidrectangle(70, 55, 200, 85);
					outtextxy(80, 57, "小星星");
					Sleep(25);
					solidrectangle(70, 90, 200, 120);
					outtextxy(80, 92, "超级马里奥");
					Sleep(25);
					solidrectangle(70, 125, 200, 155);
					outtextxy(80, 127, "My Soul");
					Sleep(25);
					solidrectangle(70, 160, 200, 190);
					outtextxy(80, 162, "One Thing");
					Sleep(25);
					solidrectangle(70, 195, 200, 225);
					outtextxy(80, 197, "青花瓷");
					Sleep(25);
					solidrectangle(70, 230, 200, 260);
					outtextxy(80, 232, "月半小夜曲");
				}
			}
			else
			{
				setlinecolor(BLACK);
				rectangle(15, 15, 55, 41);
				roundrect(1038, 13, 1152, 67, 25, 25);
			}

			if (t.x >= 70 && t.x <= 200 && t.y >= 20 && t.y <= 50 && !manual_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 18, 202, 52);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 18, 202, 52);
					manual_flag = 1;
					manual_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 55 && t.y <= 85 && !manual_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 53, 202, 87);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 53, 202, 87);
					manual_flag = 2;
					manual_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 90 && t.y <= 120 && !manual_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 88, 202, 122);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 88, 202, 122);
					manual_flag = 3;
					manual_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 125 && t.y <= 155 && !manual_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 123, 202, 157);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 123, 202, 157);
					manual_flag = 4;
					manual_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 160 && t.y <= 190 && !manual_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 158, 202, 192);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 158, 202, 192);
					manual_flag = 5;
					manual_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 195 && t.y <= 225 && !manual_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 193, 202, 227);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 193, 202, 227);
					manual_flag = 6;
					manual_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 230 && t.y <= 260 && !manual_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 228, 202, 262);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 228, 202, 262);
					manual_flag = 7;
					manual_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 1040 && t.x <= 1150 && t.y >= 15 && t.y <= 65)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(1038, 13, 1152, 67);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					success = TRUE;
					manual_flag = 0;
					manual_choice = TRUE;
					manual_success = TRUE;
					main();
					break;
				}
			}
			else
			{
				setlinecolor(BLACK);
				rectangle(15, 15, 55, 41);
				rectangle(68, 18, 202, 52);
				rectangle(68, 53, 202, 87);
				rectangle(68, 88, 202, 122);
				rectangle(68, 123, 202, 157);
				rectangle(68, 158, 202, 192);
				rectangle(68, 193, 202, 227);
				rectangle(68, 228, 202, 262);
				roundrect(1038, 13, 1152, 67, 25, 25);
			}
		}

		switch (manual_flag)
		{
		case 0:
			success = TRUE;
			main();
			break;
		case 1:
			eofm = 1143;
			for (int a = 0; a < eofm; ++a)
				music[a] = ChuanShao[a];
			break;
		case 2:
			eofm = 91;
			for (int a = 0; a < eofm; ++a)
				music[a] = littleStar[a];
			break;
		case 3:
			eofm = 481;
			for (int a = 0; a < eofm; ++a)
				music[a] = Super_Mario[a];
			break;
		case 4:
			eofm = 278;
			for (int a = 0; a < eofm; ++a)
				music[a] = My_Soul[a];
			break;
		case 5:
			eofm = 626;
			for (int a = 0; a < eofm; ++a)
				music[a] = One_Thing[a];
			break;
		case 6:
			eofm = 182;
			for (int a = 0; a < eofm; ++a)
				music[a] = QingHuaCi[a];
			break;
		case 7:
			eofm = 738;
			for (int a = 0; a < eofm; ++a)
				music[a] = YueBan[a];
			break;
		}
	}

	else
	{
		while (auto_choice)
		{
			//鼠标交互
			MOUSEMSG t;
			t = GetMouseMsg();			//获取鼠标的值
			//判断鼠标是否在需响应区域

			if (t.x >= 20 && t.x <= 50 && t.y >= 20 && t.y <= 41)
			{
				setfillcolor(RGB(160, 191, 124));
				rectangle(15, 15, 55, 46);

				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setfillcolor(RGB(160, 191, 124));
					rectangle(15, 15, 55, 46);

					auto_goon = FALSE;

					setfillcolor(RGB(160, 191, 124));
					settextstyle(23, 0, "微软雅黑", 0, 0, 0, true, false, false, 0, 0, 0, PROOF_QUALITY, 0);
					setbkmode(TRANSPARENT);

					solidrectangle(70, 20, 200, 50);
					outtextxy(80, 22, "串烧");
					Sleep(25);
					solidrectangle(70, 55, 200, 85);
					outtextxy(80, 57, "小星星");
					Sleep(25);
					solidrectangle(70, 90, 200, 120);
					outtextxy(80, 92, "超级马里奥");
					Sleep(25);
					solidrectangle(70, 125, 200, 155);
					outtextxy(80, 127, "My Soul");
					Sleep(25);
					solidrectangle(70, 160, 200, 190);
					outtextxy(80, 162, "One Thing");
					Sleep(25);
					solidrectangle(70, 195, 200, 225);
					outtextxy(80, 197, "青花瓷");
					Sleep(25);
					solidrectangle(70, 230, 200, 260);
					outtextxy(80, 232, "月半小夜曲");
				}
			}
			else
			{
				setlinecolor(BLACK);
				rectangle(15, 15, 55, 41);
				roundrect(1038, 13, 1152, 67, 25, 25);
			}

			if (t.x >= 70 && t.x <= 200 && t.y >= 20 && t.y <= 50 && !auto_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 18, 202, 52);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 18, 202, 52);
					auto_flag = 1;
					auto_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 55 && t.y <= 85 && !auto_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 53, 202, 87);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 53, 202, 87);
					auto_flag = 2;
					auto_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 90 && t.y <= 120 && !auto_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 88, 202, 122);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 88, 202, 122);
					auto_flag = 3;
					auto_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 125 && t.y <= 155 && !auto_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 123, 202, 157);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 123, 202, 157);
					auto_flag = 4;
					auto_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 160 && t.y <= 190 && !auto_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 158, 202, 192);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 158, 202, 192);
					auto_flag = 5;
					auto_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 195 && t.y <= 225 && !auto_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 193, 202, 227);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 193, 202, 227);
					auto_flag = 6;
					auto_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 70 && t.x <= 200 && t.y >= 230 && t.y <= 260 && !auto_goon)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(68, 228, 202, 262);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					setlinecolor(BLACK);
					rectangle(68, 228, 202, 262);
					auto_flag = 7;
					auto_choice = FALSE;
					break;
				}
			}
			else if (t.x >= 1040 && t.x <= 1150 && t.y >= 15 && t.y <= 65)
			{
				setlinecolor(RGB(160, 191, 124));
				rectangle(1038, 13, 1152, 67);
				if (t.uMsg == WM_LBUTTONDOWN)
				{
					success = TRUE;
					auto_flag = 0;
					auto_choice = TRUE;
					auto_success = TRUE;
					main();
					break;
				}
			}
			else
			{
			setlinecolor(BLACK);
			rectangle(15, 15, 55, 41);
			rectangle(68, 18, 202, 52);
			rectangle(68, 53, 202, 87);
			rectangle(68, 88, 202, 122);
			rectangle(68, 123, 202, 157);
			rectangle(68, 158, 202, 192);
			rectangle(68, 193, 202, 227);
			rectangle(68, 228, 202, 262);
			roundrect(1038, 13, 1152, 67, 25, 25);
			}
		}

		switch (auto_flag)
		{
		case 0:
			success = TRUE;
			main();
			break;
		case 1:
			eofm = 1143;
			for (int a = 0; a < eofm; ++a)
				music[a] = ChuanShao[a];
			break;
		case 2:
			eofm = 91;
			for (int a = 0; a < eofm; ++a)
				music[a] = littleStar[a];
			break;
		case 3:
			eofm = 481;
			for (int a = 0; a < eofm; ++a)
				music[a] = Super_Mario[a];
			break;
		case 4:
			eofm = 278;
			for (int a = 0; a < eofm; ++a)
				music[a] = My_Soul[a];
			break;
		case 5:
			eofm = 626;
			for (int a = 0; a < eofm; ++a)
				music[a] = One_Thing[a];
			break;
		case 6:
			eofm = 182;
			for (int a = 0; a < eofm; ++a)
				music[a] = QingHuaCi[a];
			break;
		case 7:
			eofm = 738;
			for (int a = 0; a < eofm; ++a)
				music[a] = YueBan[a];
			break;
		}
	}
}

void manualMode()
{
	//创建新窗口
	initgraph(GAMEWIDTH, GAMEHEIGHT);

	do
	{
		int count = 0; //结束标志

		//完成曲谱选择
		musicMenu();

		//把music中的字符数据翻译成整型数据放进新数组中
		for (int i = 0; i < eofm; ++i)
		{
			Music[i] = translate(music[i]);
		}

		//滑块各就各位
		for (int j = 0; j < eofm; j++)
		{
			initchar(charbuff, Music, j);		//初始化曲谱结构数组信息
		}

		while (manual_judge)
		{
			BeginBatchDraw();				//双缓冲防闪屏
			setbkcolor(BLACK);
			cleardevice();
			initialize();

			if (count >= eofm)
			{
				manual_judge = FALSE;
			}

			//让上一循环中的变量都下降一定像素值
			for (int i = 0; i < eofm; i++)
			{
				//若触及底线则判断是否结束标志，若不是则该滑块消失
				if (charbuff[i].y == KEYHEIGHT - 70)
				{
					charbuff[i].target = '/';
					count++;
				}
				//所有滑块下滑10个像素
				charbuff[i].y += 10;
			}

			for (int i = 0; i < eofm; i++)
			{
				outtextxy_img(charbuff[i].x, charbuff[i].y, charbuff[i].target);
			}

			//判断是否有按键
			if (_kbhit())
			{
				char userKey = _getch();
				switch (userKey)
				{
				case 48: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)0, NULL, NULL);
					break;
				case 49: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)1, NULL, NULL);
					break;
				case 50: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)2, NULL, NULL);
					break;
				case 51: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)3, NULL, NULL);
					break;
				case 52: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)4, NULL, NULL);
					break;
				case 53: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)5, NULL, NULL);
					break;
				case 54: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)6, NULL, NULL);
					break;
				case 55: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)7, NULL, NULL);
					break;
				case 56: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)8, NULL, NULL);
					break;
				case 57: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)9, NULL, NULL);
					break;
				case 'q': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'q', NULL, NULL);
					break;
				case 'w': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'w', NULL, NULL);
					break;
				case 'e': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'e', NULL, NULL);
					break;
				case 'r': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'r', NULL, NULL);
					break;
				case 't': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'t', NULL, NULL);
					break;
				case 'a': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'a', NULL, NULL);
					break;
				case 's': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'s', NULL, NULL);
					break;
				case 'd': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'d', NULL, NULL);
					break;
				case 'f': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'f', NULL, NULL);
					break;
				case 'g': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'g', NULL, NULL);
					break;
				case 'h': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'h', NULL, NULL);
					break;
				case 'j': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'j', NULL, NULL);
					break;
				case 'z': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'z', NULL, NULL);
					break;
				case 'x': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'x', NULL, NULL);
					break;
				case 'c': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'c', NULL, NULL);
					break;
				case 'v': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'v', NULL, NULL);
					break;
				case 'b': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'b', NULL, NULL);
					break;
				case '-': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'-', NULL, NULL);
					break;
				case '=': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'=', NULL, NULL);
					break;
				case 'y': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'y', NULL, NULL);
					break;
				case 'u': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'u', NULL, NULL);
					break;
				case 'i': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'i', NULL, NULL);
					break;
				case 'o': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'o', NULL, NULL);
					break;
				case 'p': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'p', NULL, NULL);
					break;
				case '[': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'[', NULL, NULL);
					break;
				case ']': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)']', NULL, NULL);
					break;
				}
			}
			Sleep(30);
			EndBatchDraw();
		}

		int ID = MessageBox(hWnd, "重新选谱？", "", MB_YESNO);
		if (ID == IDNO)
		{
			manual_success = FALSE;
		}
		else
		{
			//初始化所有控制变量
			manual_goon = TRUE;
			manual_success = TRUE;
			manual_judge = TRUE;
			manual_choice = TRUE;
			manual_flag = 0;
			for (int i = 0; i < eofm; i++)
				music[i] = 0;
		}

	} while (manual_success);
}

void autoMode()
{
	//创建新窗口
	initgraph(GAMEWIDTH, GAMEHEIGHT);

	do
	{
		int count = 0; //结束标志

		//完成曲谱选择
		musicMenu();

		//把music中的字符数据翻译成整型数据放进新数组中
		for (int i = 0; i < eofm; ++i)
		{
			Music[i] = translate(music[i]);
		}

		//滑块各就各位
		for (int j = 0; j < eofm; j++)
		{
			initchar(charbuff, Music, j);		//初始化曲谱结构数组信息
		}

		while (auto_judge)
		{
			BeginBatchDraw();				//双缓冲防闪屏
			setbkcolor(BLACK);
			cleardevice();
			initialize();

			if (count >= eofm)
			{
				auto_judge = FALSE;
			}

			//让上一循环中的变量都下降一定像素值
			for (int i = 0; i < eofm; i++)
			{
				//若触及底线则判断是否结束标志，若不是则该滑块消失
				if (charbuff[i].y == KEYHEIGHT - 70)
				{
					switch (charbuff[i].target)
					{
					case '1': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)1, NULL, NULL);
						break;
					case '2': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)2, NULL, NULL);
						break;
					case '3': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)3, NULL, NULL);
						break;
					case '4': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)4, NULL, NULL);
						break;
					case '5': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)5, NULL, NULL);
						break;
					case '6': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)6, NULL, NULL);
						break;
					case '7': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)7, NULL, NULL);
						break;
					case 'q':
					case'Q':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'q', NULL, NULL);
						break;
					case 'w':
					case 'W':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'w', NULL, NULL);
						break;
					case 'e':
					case 'E':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'e', NULL, NULL);
						break;
					case 'r':
					case 'R':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'r', NULL, NULL);
						break;
					case 't':
					case 'T':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'t', NULL, NULL);
						break;
					case 'a':
					case'A':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'a', NULL, NULL);
						break;
					case 's':
					case 'S':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'s', NULL, NULL);
						break;
					case 'd':
					case 'D':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'d', NULL, NULL);
						break;
					case 'f':
					case 'F':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'f', NULL, NULL);
						break;
					case 'g':
					case 'G':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'g', NULL, NULL);
						break;
					case 'h':
					case'H':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'h', NULL, NULL);
						break;
					case 'j':
					case'J':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'j', NULL, NULL);
						break;
					case 'z':
					case 'Z':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'z', NULL, NULL);
						break;
					case 'x':
					case 'X':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'x', NULL, NULL);
						break;
					case 'c':
					case 'C':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'c', NULL, NULL);
						break;
					case 'v':
					case 'V':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'v', NULL, NULL);
						break;
					case 'b':
					case 'B':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'b', NULL, NULL);
						break;
					case 'y':
					case 'Y':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'y', NULL, NULL);
						break;
					case 'u':
					case 'U':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'u', NULL, NULL);
						break;
					case 'i':
					case 'I':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'i', NULL, NULL);
						break;
					case 'o':
					case 'O':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'o', NULL, NULL);
						break;
					case 'p':
					case 'P':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'p', NULL, NULL);
						break;
					case '[':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'[', NULL, NULL);
						break;
					case ']':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)']', NULL, NULL);
						break;
					case '8':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)8, NULL, NULL);
						break;
					case '9':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)9, NULL, NULL);
						break;
					case '0':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)0, NULL, NULL);
						break;
					case '-':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'-', NULL, NULL);
						break;
					case '=':
						CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'=', NULL, NULL);
						break;
					}
					charbuff[i].target = '/';
					count++;
				}
				//所有滑块下滑10个像素
				charbuff[i].y += 10;
			}

			for (int i = 0; i < eofm; i++)
			{
				outtextxy_img(charbuff[i].x, charbuff[i].y, charbuff[i].target);
			}

			//判断是否有按键
			if (_kbhit())
			{
				char userKey = _getch();
				switch (userKey)
				{
				case 48: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)0, NULL, NULL);
					break;
				case 49: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)1, NULL, NULL);
					break;
				case 50: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)2, NULL, NULL);
					break;
				case 51: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)3, NULL, NULL);
					break;
				case 52: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)4, NULL, NULL);
					break;
				case 53: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)5, NULL, NULL);
					break;
				case 54: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)6, NULL, NULL);
					break;
				case 55: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)7, NULL, NULL);
					break;
				case 56: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)8, NULL, NULL);
					break;
				case 57: CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)9, NULL, NULL);
					break;
				case 'q': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'q', NULL, NULL);
					break;
				case 'w': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'w', NULL, NULL);
					break;
				case 'e': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'e', NULL, NULL);
					break;
				case 'r': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'r', NULL, NULL);
					break;
				case 't': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'t', NULL, NULL);
					break;
				case 'a': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'a', NULL, NULL);
					break;
				case 's': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'s', NULL, NULL);
					break;
				case 'd': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'d', NULL, NULL);
					break;
				case 'f': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'f', NULL, NULL);
					break;
				case 'g': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'g', NULL, NULL);
					break;
				case 'h': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'h', NULL, NULL);
					break;
				case 'j': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'j', NULL, NULL);
					break;
				case 'z': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'z', NULL, NULL);
					break;
				case 'x': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'x', NULL, NULL);
					break;
				case 'c': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'c', NULL, NULL);
					break;
				case 'v': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'v', NULL, NULL);
					break;
				case 'b': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'b', NULL, NULL);
					break;
				case '-': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'-', NULL, NULL);
					break;
				case '=': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'=', NULL, NULL);
					break;
				case 'y': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'y', NULL, NULL);
					break;
				case 'u': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'u', NULL, NULL);
					break;
				case 'i': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'i', NULL, NULL);
					break;
				case 'o': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'o', NULL, NULL);
					break;
				case 'p': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'p', NULL, NULL);
					break;
				case '[': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)'[', NULL, NULL);
					break;
				case ']': CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)playPIANO, (LPVOID)']', NULL, NULL);
					break;
				}
			}
			Sleep(13);
			EndBatchDraw();
		}

		int ID = MessageBox(hWnd, "重新选谱？", "", MB_YESNO);
		if (ID == IDNO)
		{
			auto_success = FALSE;
		}
		else
		{
			//初始化所有控制变量
			auto_goon = TRUE;
			auto_success = TRUE;
			auto_judge = TRUE;
			auto_choice = TRUE;
			auto_flag = 0;
			for (int i = 0; i < eofm; i++)
				music[i] = 0;
		}

	} while (auto_success);
}

//使不同的字母对应不同的起始坐标
int getX(int p)
{
	const int xValue[] = { 0, 55, 110, 165, 220, 275, 330, 35, 90, 200, 255, 310,385, 440, 495, 550, 605, 660, 715, 420, 475, 585, 640, 695,
										770, 825, 880, 935, 990, 1045, 1100, 805, 860, 970, 1025, 1080, 0, 0 };
	return xValue[p];
}

int getY(int p, int current)
{
	return -70 * current;
}

char getTarget(int p)
{
	const char literal[] = { 'Y','U','I','O','P','[',']','8','9','0','-','=', '1', '2', '3', '4', '5', '6', '7', 'Q', 'W', 'E', 'R', 'T',
										'A','S','D','F','G','H','J','Z','X','C','V','B','.',' ' };
	return literal[p];
}

int translate(char y)
{
	switch (y)
	{
	case'1':
		return 12;
		break;
	case'2':
		return 13;
		break;
	case'3':
		return 14;
		break;
	case'4':
		return 15;
		break;
	case'5':
		return 16;
		break;
	case'6':
		return 17;
		break;
	case'7':
		return 18;
		break;
	case'q':
		return 19;
		break;
	case'w':
		return 20;
		break;
	case'e':
		return 21;
		break;
	case'r':
		return 22;
		break;
	case't':
		return 23;
		break;
	case'a':
		return 24;
		break;
	case's':
		return 25;
		break;
	case'd':
		return 26;
		break;
	case'f':
		return 27;
		break;
	case'g':
		return 28;
		break;
	case'h':
		return 29;
		break;
	case'j':
		return 30;
		break;
	case'z':
		return 31;
		break;
	case'x':
		return 32;
		break;
	case'c':
		return 33;
		break;
	case'v':
		return 34;
		break;
	case'b':
		return 35;
		break;
	case'y':
		return 0;
		break;
	case'u':
		return 1;
		break;
	case'i':
		return 2;
		break;
	case'o':
		return 3;
		break;
	case'p':
		return 4;
		break;
	case'[':
		return 5;
		break;
	case']':
		return 6;
		break;
	case'8':
		return 7;
		break;
	case'9':
		return 8;
		break;
	case'0':
		return 9;
		break;
	case'-':
		return 10;
		break;
	case'=':
		return 11;
		break;
	case'.':
		return 36;
		break;
	case' ':
		return 37;
		break;
	}
}