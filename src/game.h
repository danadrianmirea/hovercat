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
    bool UpdateUI();

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

    // Game variables
    float playerX;
    float playerY;
    float playerSize;
    float playerVelocity;
    const float defaultGravity = 1200.0f;
    const float defaultJumpForce = -400.0f;
    const float defaultPipeSpeed = 400.0f;
    const float defaultPipeSpawnInterval = 1.5f;
    const float defaultPipeWidth = 80.0f;
    const float defaultPipeGap = 230.0f;
    float gravity;
    float jumpForce;
    float pipeWidth;
    float pipeGap;
    float pipeSpeed;
    float basePipeSpeed;  // Store the initial pipe speed
    float initialPipeDistance;  // Store the initial distance between pipes
    int speedLevel;       // Track the current speed level
    std::vector<Pipe> pipes;
    float pipeSpawnTimer;
    float pipeSpawnInterval;

    // Sound variables
    Music gameMusic;
    Sound flySound;
    Sound hitSound;
    Sound scoreSound;
    bool musicPlaying;
    bool musicManuallyDisabled;

    const float pipeSpeedIncrease = 10.0f;  // Speed increase per second
    const float maxGapHeightDifference = 150.0f;  // Maximum allowed vertical distance between consecutive pipe gaps
    void UpdatePipeSpeed(float dt);  // Add function to update pipe speed

    // Background scrolling
    Texture2D backgroundTexture;
    float backgroundScrollX;
    float backgroundScrollSpeed;

    Texture2D playerTexture;
    Texture2D playerTextureEyesClosed;
    float playerEyesClosedTimer; // Time left to display eyes closed
    const float playerEyesClosedDuration = 0.33f; // Duration in seconds

    float gameOverDelayTimer; // Time left before allowing input after game over
    const float gameOverDelayDuration = 0.5f; // Duration in seconds

    float playerCollisionWidthRatio; 
    float playerCollisionHeightRatio;

    Texture2D pipeTexture;
};