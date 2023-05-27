# c++实验项目报告——扫雷游戏
# easyx
这次的项目主要是基于easyx的图形库[easyx的下载](https://easyx.cn/)<br>
easyx的一个教程[Easyx库基础用法教程](https://blog.csdn.net/qq_61797201/article/details/122827689)<br>
## 项目报告：扫雷游戏
### 1. 项目介绍

扫雷游戏是一款经典的单人益智游戏，玩家需要根据数字提示避开地雷并揭开所有非地雷方块。本项目使用了 `easyx` 图形库来实现图形界面和用户交互。

### 2. 设计思路和实现方法

在本项目中，设计了 `MinesweeperGame` 类来管理游戏的逻辑和状态。主要成员变量包括 `map` 类型的地图对象、标记计数器、游戏结束标志和获胜标志等。<br>
使用了 `initMap` 函数来初始化地图，随机生成地雷并计算每个格子周围地雷的数量。绘制地图使用了 `drawMap` 函数，根据地图数组的值绘制对应的图像。<br>
通过 `handleMouseEvent` 函数处理鼠标事件，包括左键点击打开方块和右键点击标记雷等操作。游戏循环使用 `gameLoop` 函数，不断处理鼠标事件和绘制地图，判断游戏是否结束或获胜。
### 3. 关键算法和功能

1. 初始化地图和地雷：在 `Map` 类的构造函数中，通过随机生成地雷的位置来初始化地图。使用 `srand` 函数设置随机数种子，<br>
然后使用 `rand` 函数生成随机数来确定地雷的位置。生成的地雷位置被标记为 -1，其他格子被初始化为 0。随后，通过遍历地图数组，<br>
```C++
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
        for (int i = 0; i < ROW; i++) {          // 将地雷数量加上20，用于在图形界面上显示不同的图片
            for (int j = 0; j < COL; j++) {
                map[i][j] += 20;
            }
        }}
```
2. 绘制地图：`MinesweeperGame` 类的 `drawMap` 方法用于绘制游戏地图。根据地图中的值，选择不同的图片进行绘制，包括数字、地雷、未打开的方块和旗子。
```C++
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
```

3. 打开空白格子：`MinesweeperGame` 类的 `openNull` 方法用于递归地打开空白格子及其周围的格子。当点击一个空白格子时，该方法会将周围的空白格子递归地打开。
```c++
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
```
4. 判断是否踩雷：`MinesweeperGame` 类的 `judge` 方法用于判断玩家是否踩雷。当点击一个格子时，如果该格子是地雷，游戏失败，所有地雷格子被打开并显示在图形界面上。
```c++
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
```
5. 判断是否获胜：`MinesweeperGame` 类的 `victory` 方法用于判断玩家是否获胜。当玩家正确标记的雷的数量等于地雷总数时，游戏胜利。
```c++
    void victory() {
        if (flagCount == 10) {                    // 当标记的数量等于地雷数量时获胜
            isVictorious = true;
        }
    }
```
6. 处理鼠标事件：`MinesweeperGame` 类的 `handleMouseEvent` 方法用于处理鼠标事件。根据鼠标按键和点击的位置，执行相应的操作，包括打开格子和标记雷。
```c++
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
```
7. 游戏循环：`MinesweeperGame` 类的 `gameLoop` 方法是游戏的主循环。在循环中，不断处理鼠标事件、绘制地图，并根据游戏状态判断游戏是否结束或获胜。
```c++

    void gameLoop() {// 游戏循环
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
```
8. 主函数
```c++
    int main() {
    Map map;// 创建地图对象
    MinesweeperGame game(map);// 创建游戏对象
    game.gameLoop();// 进入游戏循环

    return 0;
    }
```    
### 4. 图形界面和交互设计

使用 `easyx` 图形库实现图形界面和用户交互。绘制地图时，根据地图数组的值选择合适的图像进行绘制，包括数字方块、地雷和标记的雷等。通过监听鼠标事件，处理左键点击打开方块和右键点击标记雷的操作。

### 5. 功能演示和测试

经过测试，扫雷游戏实现了以下功能：

- 鼠标左键点击打开方块，右键点击标记雷。
- 点击到雷时游戏结束，弹出提示框，点击确定重新开始，点击取消退出游戏。
- 标记正确的雷数等于地雷数时游戏胜利，弹出提示框，点击确定重新开始，点击取消退出游戏。
- 点击到空白方块时，自动打开周围的方块。
- 点击到数字方块时，显示数字。

### 6. 改进

扫雷游戏在功能实现上已经较为完善，但还可以进一步改进和扩展，例如：

- 添加计时功能，记录游戏时间。
- 提供不同难度的游戏模式，例如初级、中级和高级，难度级别可以影响地雷数量和地图大小。
- 添加音效和背景音乐，增加游戏的乐趣和氛围。

