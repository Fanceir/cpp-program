#include <iostream>
#include <ctime>
#include <iomanip>
#include <easyx.h>       // 用于图形和用户界面的库
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
using namespace std;

#define IMGW 40          // 图片宽度
#define ROW 10           // 行数
#define COL 10           // 列数

class Map {
private:
    int map[ROW][COL];    // 地图数组

public:
    Map() {
        memset(map, 0, ROW * COL * sizeof(int));             // 将地图数组全部设置为0
        srand(static_cast<unsigned>(time(NULL)));            // 以当前时间作为随机数种子
        for (int i = 0; i < 10;) {                            // 随机放置10个地雷
            int r = rand() % ROW;
            int c = rand() % COL;
            if (map[r][c] == 0) {
                map[r][c] = -1;// 地雷标记为-1
                i++;
            }
        }
        for (int i = 0; i < ROW; i++) {                       // 计算每个格子周围的地雷数量
            for (int j = 0; j < COL; j++) {
                if (map[i][j] == -1) {
                    for (int num1 = i - 1; num1 <= i + 1; num1++) {
                        for (int num2 = j - 1; num2 <= j + 1; num2++) {
                            if (num1 >= 0 && num1 < ROW && num2 >= 0 && num2 < COL && map[num1][num2] != -1) {
                                map[num1][num2]++;// 周围的格子地雷数量加1
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < ROW; i++) {                       // 将地雷数量加上20，用于在图形界面上显示不同的图片
            for (int j = 0; j < COL; j++) {
                map[i][j] += 20;
            }
        }
    }

    int getValue(int row, int col) {
        return map[row][col];
    }

    void setValue(int row, int col, int value) {
        map[row][col] = value;
    }
};

class  MinesweeperGame {
private:
    Map map;              // 地图对象
    int flagCount;        // 标记计数器
    bool isOver;          // 游戏是否结束
    bool isVictorious;    // 是否获胜
    bool isFirstClick;

public:
    MinesweeperGame(Map& map) : map(map) {
        flagCount = 0;
        isOver = false;
        isVictorious = false;
        isFirstClick = true;

        initgraph(COL * IMGW, ROW * IMGW, EX_SHOWCONSOLE);
    }

    void drawMap(IMAGE img[]) {// 绘制地图
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                int value = map.getValue(i, j);
                if (value >= 0 && value <= 8) {
                    putimage(j * IMGW, i * IMGW, img + value);// 绘制数字
                }
                else if (value == -1) {
                    putimage(j * IMGW, i * IMGW, img + 9);// 绘制地雷
                }
                else if (value >= 19 && value <= 28) {
                    putimage(j * IMGW, i * IMGW, img + 10);// 绘制未打开的方块
                }
                else if (value >= 99) {
                    putimage(j * IMGW, i * IMGW, img + 11);
                }   // 绘制旗子
            }
        }
    }

    void openNull(int row, int col) {
        if (map.getValue(row, col) == 0) {
            for (int i = row - 1; i <= row + 1; i++) {
                for (int j = col - 1; j <= col + 1; j++) {
                    if ((i >= 0 && i < ROW && j >= 0 && j < COL) && map.getValue(i, j) >= 19 && map.getValue(i, j) <= 28) {
                        map.setValue(i, j, map.getValue(i, j) - 20);
                        openNull(i, j);// 递归打开周围的格子
                    }
                }
            }
        }
    }

    void judge(int row, int col) {
        if (map.getValue(row, col) == -1) {
            for (int i = 0; i < ROW; i++) {
                for (int j = 0; j < COL; j++) {
                    if (map.getValue(i, j) == 19) {
                        map.setValue(i, j, map.getValue(i, j) - 20);
                    }
                }
            }
            isOver = true;
        }
    }// 判断是否踩雷

    void victory() {
        if (flagCount == 10) {                    // 当标记的数量等于地雷数量时获胜
            isVictorious = true;
        }
    }
    void handleMouseEvent() {
        ExMessage msg;
        if (peekmessage(&msg, EX_MOUSE)) {
            int r = msg.y / IMGW;
            int c = msg.x / IMGW;
            if (msg.message == WM_LBUTTONDOWN) {
                if (map.getValue(r, c) >= 19 && map.getValue(r, c) <= 28) {
                    if (map.getValue(r, c) == 19 && isFirstClick) { // 添加 isFirstClick 的判断
                        while (map.getValue(r, c) == 19) {
                            //std::cout << "第一次雷" << std::endl;
                            map = Map();
                            int r = msg.y / IMGW;
                            int c = msg.x / IMGW;
                            

                        }
                        isFirstClick = false;
                    }
                    isFirstClick = false;
                    map.setValue(r, c, map.getValue(r, c) - 20);
                    openNull(r, c);
                    judge(r, c);
                    victory(); // 检查是否获胜
                }
            }
            else if (msg.message == WM_RBUTTONDOWN) {
                if (map.getValue(r, c) >= 19 && map.getValue(r, c) <= 28) {
                    map.setValue(r, c, map.getValue(r, c) + 80);
                    if (map.getValue(r, c) == 99)
                        flagCount++;
                }
                else if (map.getValue(r, c) >= 99) { // 取消标记
                    map.setValue(r, c, map.getValue(r, c) - 80);

                    if (map.getValue(r, c) == 19) {
                        flagCount--;
                    }
                }
            }
        }
    }


    void gameLoop() {
        IMAGE img[12];
        for (int i = 0; i < 12; i++) {
            char filename[50];
            sprintf_s(filename, "./images/%d.jpg", i);
            loadimage(img + i, filename, IMGW, IMGW);
        }
        while (true) {
            handleMouseEvent();
            drawMap(img);
            if (isOver) {
                int ret = MessageBox(GetHWnd(), "失败，再试一次吗", "提示", MB_OKCANCEL);
                if (ret == IDOK) {
                    map = Map();
                    isOver = false;
                    isVictorious = false;
                    flagCount = 0; //记录扫出来雷的数目清零
                    isFirstClick = true;

                }
                else if (ret == IDCANCEL) {
                    exit(1);
                }
            }
            else if (isVictorious) {
                int ret = MessageBox(GetHWnd(), "胜利，要再来一次吗", "提示", MB_OKCANCEL);
                if (ret == IDOK) {
                    map = Map();
                    isOver = false;
                    isVictorious = false;             
                    flagCount = 0; //记录扫出来雷的数目清零
                    isFirstClick = true;
                }
                else if (ret == IDCANCEL) {
                    exit(1);
                }
            }
        }
    }
};

int main() {
    Map map;
    MinesweeperGame game(map);
    game.gameLoop();

    return 0;
}
