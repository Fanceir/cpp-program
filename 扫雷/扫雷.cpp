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

class MinesweeperGame {
private:
    int map[ROW][COL];    // 地图数组
    int flagCount;        // 标记计数器
    bool isOver;          // 游戏是否结束
    bool isVictorious;    // 是否获胜

public:
    MinesweeperGame() {
        flagCount = 0;
        isOver = false;
        isVictorious = false;
        initgraph(COL * IMGW, ROW * IMGW, EX_SHOWCONSOLE);   // 初始化图形界面
        initMap();                                           // 初始化地图
    }

    void initMap() {
        memset(map, 0, ROW * COL * sizeof(int));             // 将地图数组全部设置为0
        srand(static_cast<unsigned>(time(NULL)));            // 以当前时间作为随机数种子
        for (int i = 0; i < 10;) {                            // 随机放置10个地雷
            int r = rand() % ROW;
            int c = rand() % COL;
            if (map[r][c] == 0) {
                map[r][c] = -1;                              // 地雷标记为-1
                i++;
            }
        }
        for (int i = 0; i < ROW; i++) {                       // 计算每个格子周围的地雷数量
            for (int j = 0; j < COL; j++) {
                if (map[i][j] == -1) {
                    for (int num1 = i - 1; num1 <= i + 1; num1++) {
                        for (int num2 = j - 1; num2 <= j + 1; num2++) {
                            if (num1 >= 0 && num1 < ROW && num2 >= 0 && num2 < COL && map[num1][num2] != -1) {
                                map[num1][num2]++;
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

    void drawMap(IMAGE img[]) {
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (map[i][j] >= 0 && map[i][j] <= 8) {
                    putimage(j * IMGW, i * IMGW, img + map[i][j]);
                }
                else if (map[i][j] == -1) {
                    putimage(j * IMGW, i * IMGW, img + 9);
                }
                else if (map[i][j] >= 19 && map[i][j] <= 28 || map[i][j] == -100) {
                    putimage(j * IMGW, i * IMGW, img + 10);
                }
                else if (map[i][j] == 100) {
                    putimage(j * IMGW, i * IMGW, img + 11);
                }
            }
        }
    }

    void openNull(int row, int col) {
        if (map[row][col] == 0) {
            for (int i = row - 1; i <= row + 1; i++) {
                for (int j = col - 1; j <= col + 1; j++) {
                    if ((i >= 0 && i < ROW && j >= 0 && j < COL) && map[i][j] >= 19 && map[i][j] <= 28) {
                        map[i][j] -= 20;
                        openNull(i, j);
                    }
                }
            }
        }
    }

    void judge(int row, int col) {
        if (map[row][col] == -1) {
            for (int i = 0; i < ROW; i++) {
                for (int j = 0; j < COL; j++) {
                    if (map[i][j] == 19) {
                        map[i][j] -= 20;
                    }
                }
            }
            isOver = true;
        }
    }

    void victory() {
        if (ROW * COL - flagCount == 10) {                    // 当标记的数量等于地雷数量时获胜
            isVictorious = true;
        }
    }

    void handleMouseEvent() {
        ExMessage msg;
        if (peekmessage(&msg, EX_MOUSE)) {
            int r = msg.y / IMGW;
            int c = msg.x / IMGW;
            if (msg.message == WM_LBUTTONDOWN) {
                if (map[r][c] >= 19 && map[r][c] <= 28) {
                    map[r][c] -= 20;
                    openNull(r, c);
                    judge(r, c);
                    victory();
                }
            }
            else if (msg.message == WM_RBUTTONDOWN) {
                if (map[r][c] == 19) {
                    map[r][c] = 100;
                    flagCount++;
                }
                else if (map[r][c] == 100) {
                    map[r][c] = 19;
                    flagCount--;
                }
                else if (map[r][c] >= 0 && map[r][c] <= 8) {
                    // 在未打开的方块上点击右键，进行标记
                    if (map[r][c] >= 20 && map[r][c] <= 28) {
                        map[r][c] -= 20;
                    }
                    else {
                        map[r][c] += 20;
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
                    initMap();
                    isOver = false;
                    isVictorious = false;
                    flagCount = 0;
                }
                else if (ret == IDCANCEL) {
                    exit(666);
                }
            }
            else if (isVictorious) {
                int ret = MessageBox(GetHWnd(), "胜利，要再来一次吗", "提示", MB_OKCANCEL);
                if (ret == IDOK) {
                    initMap();
                    isOver = false;
                    isVictorious = false;
                    flagCount = 0;
                }
                else if (ret == IDCANCEL) {
                    exit(666);
                }
            }
        }
    }
};

int main() {
    MinesweeperGame game;
    game.gameLoop();

    return 0;
}
