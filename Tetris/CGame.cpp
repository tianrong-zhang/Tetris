#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include <mmsystem.h>
#include <fstream>
#include "CGame.h"
#include "CBlock.h"
#pragma comment(lib, "winmm.lib")

#define MAX_LEVEL 5
#define RECORDED_FILE "recorder.txt"

const int SPEED_NORMAL[MAX_LEVEL] = { 500, 300, 150, 100, 80 };
const int SPEED_QUICK = 50;		//ms

CGame::CGame(int rows, int cols, int left, int top, int blockSize)
{
	this->rows = rows;
	this->cols = cols;
	this->left = left;
	this->top = top;
	this->blockSize = blockSize;
	
	for (int i = 0; i < rows; ++i) {
		std::vector<int> row;
		for (int j = 0; j < cols; ++j) {
			row.emplace_back(0);
		}
		map.emplace_back(row);
	}
}

void CGame::init()
{
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	delay = SPEED_NORMAL[0];

	//配置随机种子
	srand(time(NULL));

	//创建游戏窗口
	initgraph(938, 896);

	//加载背景图片
	loadimage(&imgBg, "res/bg2.png");
	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	//初始化游戏区中的数据
	char data[20][10];
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			map[i][j] = 0;
		}
	}

	score = 0;
	level = 1;
	lineCount = 0;
	gameOver = false;

	//初始化最高分
	std::ifstream file(RECORDED_FILE);
	if (!file.is_open()) {
		std::cout << RECORDED_FILE << "打开失败" << std::endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	//关闭文件
	file.close();
}

void CGame::play()
{
	init();

	nextBlock = new CBlock;

	curBlock = nextBlock;

	nextBlock = new CBlock;

	int timer = 0;

	while (1) {
		//接受用户输入
		keyEvent();
		
		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();
			//渲染游戏画面
			update = true;
		}

		if (update) {
			update = false;
			//更新游戏画面
			updateWindow();

			//更新游戏数据
			clearLine();
		}

		if (gameOver) {
			//保存分数
			saveScore();

			//更新游戏结束界面
			displayOver();

			system("pause");

			//重新开局
			init();
		}

	}
}

void CGame::keyEvent()
{
	unsigned char ch;
	bool rotateFlag = false;
	int dx = 0;
	
	if (_kbhit()) {
		ch = _getch();

		//如果按下方向键，会自动返回两个字符
		//如果按下向上方向键，会先后返回: 224 72
		//如果按下向下方向键，会先后返回: 224 80
		//如果按下向左方向键，会先后返回: 224 75
		//如果按下向右方向键，会先后返回: 224 77
		if (ch == 224) {
			ch = _getch();
			switch (ch) {
			case 72:
				rotateFlag = true;
				break;
			case 80:
				delay = SPEED_QUICK;
				break;
			case 75:
				dx = -1;
				break;
			case 77:
				dx = 1;
				break;
			default:
				break;
			}
		}
	}

	if (rotateFlag) {
		rotate();
		update = true;
	}

	if (dx != 0) {
		//实现左右移动
		move(dx);
		update = true;
	}

}

void CGame::updateWindow()
{
	//绘制背景图片
	putimage(0, 0, &imgBg);

	IMAGE** imgs = CBlock::getImages();

	BeginBatchDraw();

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (map[i][j] != 0) {
				int x = j * blockSize + left;
				int y = i * blockSize + top;
				putimage(x, y, imgs[map[i][j] - 1]);
			}
		}
	}

	curBlock->draw(left, top);

	nextBlock->draw(689, 150);

	//绘制分数
	draw();

	EndBatchDraw();

}


//返回距离上一次调用该函数，间隔了多少时间（ms）
//第一次调用该函数，返回0
int CGame::getDelay()
{
	static unsigned long long lastTime = 0;

	unsigned long long currentTime =  GetTickCount64();

	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void CGame::drop()
{
	backBlock = *curBlock;
	curBlock->drop();

	if (!curBlock->blockInMap(map)) {
		//把这个方块固化
		backBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new CBlock;

		//检查游戏是否结束
		checkOver();
	}

	delay = SPEED_NORMAL[level-1];
}

void CGame::clearLine()
{
	int lines = 0;

	//存储数据的行数
	int k = rows - 1;

	for (int i = rows - 1; i >= 0; --i) {
		//检查第i行是否满行
		int count = 0;
		for (int j = 0; j < cols; ++j) {
			if (map[i][j]) {
				++count;
			}
			//一边扫描一边存储
			map[k][j] = map[i][j];
		}

		if (count < cols) {
			//不是满行
			--k;
		}
		else {
			//满行
			++lines;
		}
	}

	if (lines > 0) {
		//计算得分
		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];

		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
		update = true;

		//每100分一个级别 0-100 第一关 101-200 第二关
		level = (score + 99) / 100;
		if (level > MAX_LEVEL) {
			gameOver = true;
		}

		lineCount += lines;
	}
}

void CGame::move(int offset)
{
	backBlock = *curBlock;
	curBlock->move(offset);

	if (!curBlock->blockInMap(map)) {
		*curBlock = backBlock;
	}
}

void CGame::rotate()
{
	if (curBlock->getBlockType() == 7) {
		return;
	}
	backBlock = *curBlock;
	curBlock->rotate();

	if (!curBlock->blockInMap(map)) {
		*curBlock = backBlock;
	}
}

void CGame::draw()
{
	//绘制分数
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);

	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	//获取当前的字体
	gettextstyle(&f);
	
	f.lfHeight = 60;
	f.lfWidth = 30;
	//设置字体为抗锯齿效果
	f.lfQuality = ANTIALIASED_QUALITY;
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), "Segoe UI Black");
	
	settextstyle(&f);
	//字体的背景设置为透明效果
	setbkmode(TRANSPARENT);

	outtextxy(670, 727, scoreText);

	//绘制消除的行数
	char lineText[32];
	sprintf_s(lineText, sizeof(lineText), "%d", lineCount);

	gettextstyle(&f);

	int xPos = 224 - f.lfWidth * strlen(lineText);
	outtextxy(xPos, 817, lineText);

	//绘制关卡等级
	char levelText[32];
	sprintf_s(levelText, sizeof(levelText), "%d", level);

	outtextxy(224 - 30, 727, levelText);

	//绘制最高分
	char highestScoreText[32];
	sprintf_s(highestScoreText, sizeof(highestScoreText), "%d", highestScore);

	outtextxy(670, 817, highestScoreText);

}

void CGame::checkOver()
{
	gameOver = curBlock->blockInMap(map) == false;
}

void CGame::saveScore()
{
	if (score > highestScore) {
		highestScore = score;

		std::ofstream file(RECORDED_FILE);
		file << highestScore;
		file.close();
	}
}

void CGame::displayOver()
{
	mciSendString("stop res/bg.mp3", 0, 0, 0);

	//胜利结束，还是失败结束
	if (level <= MAX_LEVEL) {
		putimage(262, 361, &imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}
