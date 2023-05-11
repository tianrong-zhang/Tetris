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

	//�����������
	srand(time(NULL));

	//������Ϸ����
	initgraph(938, 896);

	//���ر���ͼƬ
	loadimage(&imgBg, "res/bg2.png");
	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	//��ʼ����Ϸ���е�����
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

	//��ʼ����߷�
	std::ifstream file(RECORDED_FILE);
	if (!file.is_open()) {
		std::cout << RECORDED_FILE << "��ʧ��" << std::endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	//�ر��ļ�
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
		//�����û�����
		keyEvent();
		
		timer += getDelay();
		if (timer > delay) {
			timer = 0;
			drop();
			//��Ⱦ��Ϸ����
			update = true;
		}

		if (update) {
			update = false;
			//������Ϸ����
			updateWindow();

			//������Ϸ����
			clearLine();
		}

		if (gameOver) {
			//�������
			saveScore();

			//������Ϸ��������
			displayOver();

			system("pause");

			//���¿���
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

		//������·���������Զ����������ַ�
		//����������Ϸ���������Ⱥ󷵻�: 224 72
		//����������·���������Ⱥ󷵻�: 224 80
		//�������������������Ⱥ󷵻�: 224 75
		//����������ҷ���������Ⱥ󷵻�: 224 77
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
		//ʵ�������ƶ�
		move(dx);
		update = true;
	}

}

void CGame::updateWindow()
{
	//���Ʊ���ͼƬ
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

	//���Ʒ���
	draw();

	EndBatchDraw();

}


//���ؾ�����һ�ε��øú���������˶���ʱ�䣨ms��
//��һ�ε��øú���������0
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
		//���������̻�
		backBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new CBlock;

		//�����Ϸ�Ƿ����
		checkOver();
	}

	delay = SPEED_NORMAL[level-1];
}

void CGame::clearLine()
{
	int lines = 0;

	//�洢���ݵ�����
	int k = rows - 1;

	for (int i = rows - 1; i >= 0; --i) {
		//����i���Ƿ�����
		int count = 0;
		for (int j = 0; j < cols; ++j) {
			if (map[i][j]) {
				++count;
			}
			//һ��ɨ��һ�ߴ洢
			map[k][j] = map[i][j];
		}

		if (count < cols) {
			//��������
			--k;
		}
		else {
			//����
			++lines;
		}
	}

	if (lines > 0) {
		//����÷�
		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];

		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);
		update = true;

		//ÿ100��һ������ 0-100 ��һ�� 101-200 �ڶ���
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
	//���Ʒ���
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);

	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	//��ȡ��ǰ������
	gettextstyle(&f);
	
	f.lfHeight = 60;
	f.lfWidth = 30;
	//��������Ϊ�����Ч��
	f.lfQuality = ANTIALIASED_QUALITY;
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), "Segoe UI Black");
	
	settextstyle(&f);
	//����ı�������Ϊ͸��Ч��
	setbkmode(TRANSPARENT);

	outtextxy(670, 727, scoreText);

	//��������������
	char lineText[32];
	sprintf_s(lineText, sizeof(lineText), "%d", lineCount);

	gettextstyle(&f);

	int xPos = 224 - f.lfWidth * strlen(lineText);
	outtextxy(xPos, 817, lineText);

	//���ƹؿ��ȼ�
	char levelText[32];
	sprintf_s(levelText, sizeof(levelText), "%d", level);

	outtextxy(224 - 30, 727, levelText);

	//������߷�
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

	//ʤ������������ʧ�ܽ���
	if (level <= MAX_LEVEL) {
		putimage(262, 361, &imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);
	}
}
