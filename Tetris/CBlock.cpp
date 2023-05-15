#include <stdlib.h>
#include "CBlock.h"

IMAGE* CBlock::imgs[7] = { NULL };
int CBlock::size = 36;

CBlock::CBlock()
{
    if (imgs[0] == NULL) {
        IMAGE imgTmp;
        //加载方块图片
        loadimage(&imgTmp, "res/tiles.png");
        //设置绘图目标为imgTmp对象
        SetWorkingImage(&imgTmp);

        for (int i = 0; i < 7; ++i) {
            imgs[i] = new IMAGE;
            //切割方块
            getimage(imgs[i], i * size, 0, size, size);
        }
        //设置绘图目标为绘图窗口
        SetWorkingImage();
    }

    int blocks[7][4] = {
        {1, 3, 5, 7},   // I
        {2, 4, 5, 7},   // Z 1型
        {3, 5, 4, 6},   // Z 2型
        {3, 5, 4, 7},   // T
        {2, 3, 5, 7},   // L
        {3, 5, 7, 6},   // J
        {2, 3, 4, 5}    // 田
    };

    //随机生成一种俄罗斯方块
    blockType = rand() % 7 + 1;

    //初始化smallBlocks
    for (int i = 0; i < 4; ++i) {
        int value = blocks[blockType - 1][i];
        smallBlocks[i].row = value / 2;
        smallBlocks[i].col = value % 2;
    }

    img = imgs[blockType - 1];
}

void CBlock::drop()
{
    for (auto& block : smallBlocks) {
        ++block.row;
    }
}

void CBlock::move(int offset)
{
    for (int i = 0; i < 4; ++i) {
        smallBlocks[i].col += offset;
    }
}

void CBlock::rotate()
{
    Point p = smallBlocks[1];

    for (int i = 0; i < 4; ++i) {
        Point tmp = smallBlocks[i];
        smallBlocks[i].col = p.col - tmp.row + p.row;
        smallBlocks[i].row = p.row + tmp.col - p.col;
    }
}

void CBlock::draw(int left, int top)
{
    for (int i = 0; i < 4; ++i) {
        int x = left + smallBlocks[i].col * size;
        int y = top + smallBlocks[i].row * size;
        putimage(x, y, img);
    }
}

IMAGE** CBlock::getImages()
{
    return imgs;
}

CBlock& CBlock::operator=(const CBlock& other)
{
    if (this == &other) {
        return *this;
    }

    this->blockType = other.blockType;

    for (int i = 0; i < 4; ++i) {
        this->smallBlocks[i] = other.smallBlocks[i];
    }

    return *this;
}

bool CBlock::blockInMap(const std::vector<std::vector<int>>& map)
{
    int rows = map.size();
    int cols = map[0].size();

    for (int i = 0; i < 4; ++i) {
        if (smallBlocks[i].col < 0 || smallBlocks[i].col >= cols ||
            smallBlocks[i].row < 0 || smallBlocks[i].row >= rows ||
            map[smallBlocks[i].row][smallBlocks[i].col] != 0) {

            return false;
        }
    }
    return true;
}

void CBlock::solidify(std::vector<std::vector<int>>& map)
{
    for (int i = 0; i < 4; ++i) {
        //设置标记，固化对应的位置
        map[smallBlocks[i].row][smallBlocks[i].col] = blockType;
    }
}

int CBlock::getBlockType()
{
    return blockType;
}
