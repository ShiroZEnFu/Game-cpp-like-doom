#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <Windows.h>

class Player {
public:
    float x, y, angle;

    Player(float startX, float startY, float startAngle) : x(startX), y(startY), angle(startAngle) {}

    void turn(float speed, float elapsedTime) {
        angle += speed * elapsedTime;
    }
    void move(float speed, float elapsedTime) {
        float newX = x + sinf(angle) * speed * elapsedTime;
        float newY = y + cosf(angle) * speed * elapsedTime;
        x = newX;
        y = newY;
    }

    void revertMove(float speed, float elapsedTime) {
        x -= sinf(angle) * speed * elapsedTime;
        y -= cosf(angle) * speed * elapsedTime;
    }
};

class Map {
public:
    int width;
    int height;
    std::wstring layout;

    Map() : width(16), height(16) {
        layout += L"################";
        layout += L"#..............#";
        layout += L"#..............#";
        layout += L"#...#..........#";
        layout += L"#..............#";
        layout += L"#......#.......#";
        layout += L"#..............#";
        layout += L"#..............#";
        layout += L"#..............#";
        layout += L"#..............#";
        layout += L"#..............#";
        layout += L"#.........######";
        layout += L"#..............#";
        layout += L"#..............#";
        layout += L"#..............#";
        layout += L"################";
    }

    bool isWall(float x, float y) const {
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        if (ix < 0 || ix >= width || iy < 0 || iy >= height) {
            return true;
        }
        return layout[iy * width + ix] == '#';
    }
};

class Game {
private:

    int nScreenWidth;
    int nScreenHeight;
    wchar_t* screen;
    HANDLE hConsole;
    DWORD dwBytesWritten;

    Player player;
    Map gameMap;

    float fFOV;
    float fDepth;

    std::chrono::system_clock::time_point tp1, tp2;

public:
    Game()
        : nScreenWidth(120), nScreenHeight(40),
        player(8.0f, 8.0f, 0.0f),
        fFOV(3.14159f / 4.0f), fDepth(16.0f),
        dwBytesWritten(0)
    {
        screen = new wchar_t[nScreenWidth * nScreenHeight];
        hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
        SetConsoleActiveScreenBuffer(hConsole);

        COORD coord = { (short)nScreenWidth, (short)nScreenHeight };
        SetConsoleScreenBufferSize(hConsole, coord);
        SMALL_RECT rect = { 0, 0, (short)nScreenWidth - 1, (short)nScreenHeight - 1 };
        SetConsoleWindowInfo(hConsole, TRUE, &rect);
    }

    ~Game() {
        delete[] screen;
        CloseHandle(hConsole);
    }

    void run() {
        tp1 = std::chrono::system_clock::now();
        tp2 = std::chrono::system_clock::now();

        while (true) {

            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsedTime = tp2 - tp1;
            tp1 = tp2;
            float fElapsedTime = elapsedTime.count();

            processInput(fElapsedTime);
            render(fElapsedTime);
        }
    }

private:

    void processInput(float fElapsedTime) {
        float moveSpeed = 5.0f;
        float turnSpeed = 1.0f;

        if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
            player.turn(-turnSpeed, fElapsedTime);
        }
        if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
            player.turn(turnSpeed, fElapsedTime);
        }
        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            player.move(moveSpeed, fElapsedTime);
            if (gameMap.isWall(player.x, player.y)) {
                player.revertMove(moveSpeed, fElapsedTime);
            }
        }
        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            player.move(-moveSpeed, fElapsedTime);
            if (gameMap.isWall(player.x, player.y)) {
                player.revertMove(-moveSpeed, fElapsedTime);
            }
        }
    }

    void render(float fElapsedTime) {

        for (int x = 0; x < nScreenWidth; x++) {
            float fRayAngle = (player.angle - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistanceToWall = 0;
            bool bHitWall = false;
            bool bBoundary = false;

            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fDepth) {
                fDistanceToWall += 0.1f;

                int nTestX = (int)(player.x + fEyeX * fDistanceToWall);
                int nTestY = (int)(player.y + fEyeY * fDistanceToWall);

                if (nTestX < 0 || nTestX >= gameMap.width || nTestY < 0 || nTestY >= gameMap.height) {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else {
                    if (gameMap.layout[nTestY * gameMap.width + nTestX] == '#') {
                        bHitWall = true;
                        std::vector<std::pair<float, float>> p;
                        for (int tx = 0; tx < 2; tx++) {
                            for (int ty = 0; ty < 2; ty++) {
                                float vy = (float)nTestY + ty - player.y;
                                float vx = (float)nTestX + tx - player.x;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back({ d, dot });
                            }
                        }
                        sort(p.begin(), p.end());
                        if (acos(p.at(0).second) < 0.01) bBoundary = true;
                        if (acos(p.at(1).second) < 0.01) bBoundary = true;
                    }
                }
            }


            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / fDistanceToWall;
            int nFloor = nScreenHeight - nCeiling;

            short nShade = ' ';
            if (fDistanceToWall <= fDepth / 4.0f)      nShade = 0x2588;
            else if (fDistanceToWall < fDepth / 3.0f) nShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f) nShade = 0x2592;
            else if (fDistanceToWall < fDepth)        nShade = 0x2591;
            else                                      nShade = ' ';

            if (bBoundary) nShade = ' ';

            for (int y = 0; y < nScreenHeight; y++) {
                if (y <= nCeiling) {
                    screen[y * nScreenWidth + x] = ' ';
                }
                else if (y > nCeiling && y <= nFloor) {
                    screen[y * nScreenWidth + x] = nShade; 
                }
                else {
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    short ShadeFloor = ' ';
                    if (b < 0.25)      ShadeFloor = '#';
                    else if (b < 0.5)  ShadeFloor = 'x';
                    else if (b < 0.75) ShadeFloor = '.';
                    else if (b < 0.9)  ShadeFloor = '-';
                    else               ShadeFloor = ' ';
                    screen[y * nScreenWidth + x] = ShadeFloor;
                }
            }
        }

        drawOverlay(fElapsedTime);

        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    void drawOverlay(float fElapsedTime) {
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f", player.x, player.y, player.angle, 1.0f / fElapsedTime);

        for (int nx = 0; nx < gameMap.width; nx++) {
            for (int ny = 0; ny < gameMap.height; ny++) {
                screen[(ny + 1) * nScreenWidth + nx] = gameMap.layout[ny * gameMap.width + nx];
            }
        }
        screen[((int)player.y + 1) * nScreenWidth + (int)player.x] = 'P';
    }
};

int main()
{
    Game console3DGame;
    console3DGame.run();
    return 0;

}
