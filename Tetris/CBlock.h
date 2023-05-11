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

	//方块向下降落
	void drop();

	//方块左右移动
	void move(int offset);

	//方块旋转
	void rotate();

	//生成方块的位置
	void draw(int left, int top);

	static IMAGE** getImages();

	CBlock& operator = (const CBlock& other);

	bool blockInMap(const std::vector<std::vector<int>>& map);

	void solidify(std::vector<std::vector<int>>& map);

	int getBlockType();

private:

	//方块类型
	int blockType;

	//每个方块中的小方块
	Point smallBlocks[4];

	IMAGE* img;

	//七种方块的图像指针数组
	static IMAGE* imgs[7];
	//图像大小
	static int size;
};

