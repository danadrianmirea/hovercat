#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "raylib.h"

struct Pipe {
    float x;
    float gapCenter;
    bool scored;
};

class Game
{
public:
    Game(int width, int height);
    ~Game();
    void InitGame();
    void Reset();
    void Update(float dt);
    void HandleInput();
    void UpdateUI();

    void Draw();
    void DrawUI();
    std::string FormatWithLeadingZeroes(int number, int width);
    void Randomize();

    static bool isMobile;

private:
    bool firstTimeGameStart;
    bool isInExitMenu;
    bool paused;
    bool lostWindowFocus;
    bool gameOver;

    float screenScale;
    RenderTexture2D targetRenderTex;
    Font font;

    int width;
    int height;

    // Score system
    int score;
    int highScore;
    void LoadHighScore();
    void SaveHighScore();

    float ballX;
    float ballY;
    int ballRadius;
    float ballSpeed;
    Color ballColor;

    // Flappy Bird variables
    float birdX;
    float birdY;
    float birdSize;
    float birdVelocity;
    float gravity;
    float jumpForce;
    float pipeWidth;
    float pipeGap;
    float pipeSpeed;
    float basePipeSpeed;  // Store the initial pipe speed
    int speedLevel;       // Track the current speed level
    std::vector<Pipe> pipes;
    float pipeSpawnTimer;
    float pipeSpawnInterval;

    void UpdatePipeSpeed();  // Add function to update pipe speed
};