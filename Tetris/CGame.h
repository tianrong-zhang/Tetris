#pragma once
#include <vector>
#include <graphics.h>
#include "CBlock.h"

class CGame
{
public:

	//��Ϸ�������������������߽硢�ϱ߽硢�����С
	CGame(int rows, int cols, int left, int top, int blockSize);

	//��ʼ��
	void init();

	//��ʼ��Ϸ
	void play();

private:

	//��Ⱦ���
	int delay;

	//��Ⱦ��־
	bool update;

	// 0: �հף�û�з���
	// i: ��i�ַ���
	std::vector<std::vector<int>> map;
	
	int rows;

	int cols;

	int left;

	int top;

	int blockSize;

	//��ǰ����
	int score;

	//��߷�
	int highestScore;

	//��ǰ�ؿ�
	int level;

	//��ǰ����������
	int lineCount;

	//��Ϸ������־
	bool gameOver;

	IMAGE imgBg;

	IMAGE imgOver;

	IMAGE imgWin;

	CBlock* curBlock;

	//Ԥ�淽��
	CBlock* nextBlock;

	//��ǰ���齵������У�����������һ���Ϸ�λ��
	CBlock backBlock;

	//�����û�����
	void keyEvent();

	//��Ⱦ��Ϸ����
	void updateWindow();

	//���ؾ�����һ�ε��øú���������˶���ʱ�䣨ms��
	//��һ�ε��øú���������0
	int getDelay();

	void drop();

	void clearLine();

	void move(int offset);

	//��ת
	void rotate();

	//���Ƶ�ǰ����
	void draw();

	//�����Ϸ�Ƿ����
	void checkOver();

	//������߷�
	void saveScore();

	//������Ϸ��������
	void displayOver();
};

