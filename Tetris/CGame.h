#pragma once
#include <vector>
#include <graphics.h>
#include "CBlock.h"

class CGame
{
public:

	//游戏界面的行数、列数、左边界、上边界、方块大小
	CGame(int rows, int cols, int left, int top, int blockSize);

	//初始化
	void init();

	//开始游戏
	void play();

private:

	//渲染间隔
	int delay;

	//渲染标志
	bool update;

	// 0: 空白，没有方块
	// i: 第i种方块
	std::vector<std::vector<int>> map;
	
	int rows;

	int cols;

	int left;

	int top;

	int blockSize;

	//当前分数
	int score;

	//最高分
	int highestScore;

	//当前关卡
	int level;

	//当前消除的行数
	int lineCount;

	//游戏结束标志
	bool gameOver;

	IMAGE imgBg;

	IMAGE imgOver;

	IMAGE imgWin;

	CBlock* curBlock;

	//预告方块
	CBlock* nextBlock;

	//当前方块降落过程中，用来备份上一个合法位置
	CBlock backBlock;

	//接受用户输入
	void keyEvent();

	//渲染游戏画面
	void updateWindow();

	//返回距离上一次调用该函数，间隔了多少时间（ms）
	//第一次调用该函数，返回0
	int getDelay();

	void drop();

	void clearLine();

	void move(int offset);

	//旋转
	void rotate();

	//绘制当前分数
	void draw();

	//检查游戏是否结束
	void checkOver();

	//保存最高分
	void saveScore();

	//绘制游戏结束画面
	void displayOver();
};

