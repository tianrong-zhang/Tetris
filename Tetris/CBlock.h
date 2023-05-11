#pragma once
#include <graphics.h>
#include <vector>

struct Point {
	int row;
	int col;
};

class CBlock
{
public:

	CBlock();

	//�������½���
	void drop();

	//���������ƶ�
	void move(int offset);

	//������ת
	void rotate();

	//���ɷ����λ��
	void draw(int left, int top);

	static IMAGE** getImages();

	CBlock& operator = (const CBlock& other);

	bool blockInMap(const std::vector<std::vector<int>>& map);

	void solidify(std::vector<std::vector<int>>& map);

	int getBlockType();

private:

	//��������
	int blockType;

	//ÿ�������е�С����
	Point smallBlocks[4];

	IMAGE* img;

	//���ַ����ͼ��ָ������
	static IMAGE* imgs[7];
	//ͼ���С
	static int size;
};

