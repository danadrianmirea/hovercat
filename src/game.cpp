#include <vector>
#include <utility>
#include <string>
#include <cmath>  // For sqrtf
#include <algorithm> // For std::remove_if
#include <fstream>

#include "raylib.h"
#include "globals.h"
#include "game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

bool Game::isMobile = false;

Game::Game(int width, int height)
{
    firstTimeGameStart = true;

    // Initialize Flappy Bird variables
    birdSize = 30.0f;
    birdX = width / 4;
    birdY = height / 2;
    birdVelocity = 0.0f;
    gravity = 800.0f;
    jumpForce = -400.0f;
    pipeWidth = 80.0f;
    pipeGap = 200.0f;
    pipeSpeed = 200.0f;
    pipeSpawnTimer = 0.0f;
    pipeSpawnInterval = 2.0f;

    // Initialize score
    score = 0;
    LoadHighScore();

#ifdef __EMSCRIPTEN__
    // Check if we're running on a mobile device
    isMobile = EM_ASM_INT({
        return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
    });
#endif

    targetRenderTex = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(targetRenderTex.texture, TEXTURE_FILTER_BILINEAR);

    font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    this->width = width;
    this->height = height;
    InitGame();
}

Game::~Game()
{
    UnloadRenderTexture(targetRenderTex);
    UnloadFont(font);
}

void Game::InitGame()
{
    isInExitMenu = false;
    paused = false;
    lostWindowFocus = false;
    gameOver = false;

    screenScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
}

void Game::Reset()
{
    InitGame();
    // Reset bird position and velocity
    birdX = width / 4;
    birdY = height / 2;
    birdVelocity = 0.0f;
    // Clear all pipes
    pipes.clear();
    pipeSpawnTimer = 0.0f;
    // Reset score
    score = 0;
}

void Game::Update(float dt)
{
    if (dt == 0)
    {
        return;
    }

    screenScale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);
    UpdateUI();

    bool running = (firstTimeGameStart == false && paused == false && lostWindowFocus == false && isInExitMenu == false && gameOver == false);

    if (running)
    {
        HandleInput();
        
        // Update bird physics
        birdVelocity += gravity * dt;
        birdY += birdVelocity * dt;

        // Check for collisions with screen boundaries
        if (birdY - birdSize/2 < 0 || birdY + birdSize/2 > height) {
            gameOver = true;
            if (score > highScore) {
                highScore = score;
                SaveHighScore();
            }
        }

        // Update pipes
        pipeSpawnTimer += dt;
        if (pipeSpawnTimer >= pipeSpawnInterval) {
            pipeSpawnTimer = 0.0f;
            float gapCenter = GetRandomValue(pipeGap/2, height - pipeGap/2);
            pipes.push_back({(float)width, gapCenter, false});
        }

        // Move pipes and check collisions
        for (auto& pipe : pipes) {
            pipe.x -= pipeSpeed * dt;

            // Check if bird has passed the pipe
            if (birdX > pipe.x + pipeWidth && !pipe.scored) {
                score++;
                pipe.scored = true;
                if (score > highScore) {
                    highScore = score;
                    SaveHighScore();
                }
            }

            // Check collision with pipe
            if (!gameOver) {
                // Check if bird is within pipe's x range
                if (birdX + birdSize/2 > pipe.x && birdX - birdSize/2 < pipe.x + pipeWidth) {
                    // Check if bird is outside the gap
                    if (birdY - birdSize/2 < pipe.gapCenter - pipeGap/2 || 
                        birdY + birdSize/2 > pipe.gapCenter + pipeGap/2) {
                        gameOver = true;
                        if (score > highScore) {
                            highScore = score;
                            SaveHighScore();
                        }
                    }
                }
            }
        }

        // Remove pipes that are off screen
        pipes.erase(std::remove_if(pipes.begin(), pipes.end(), 
            [this](const auto& pipe) { return pipe.x < -this->pipeWidth; }), 
            pipes.end());
    }
}

void Game::HandleInput()
{
    if(!isMobile) { // desktop and web controls
        if(IsKeyPressed(KEY_SPACE)) {
            birdVelocity = jumpForce;
        }
    } 
    else // mobile controls
    {
        if(IsGestureDetected(GESTURE_TAP)) {
            birdVelocity = jumpForce;
        }
    }
}

void Game::UpdateUI()
{
#ifndef EMSCRIPTEN_BUILD
    if (WindowShouldClose() || (IsKeyPressed(KEY_ESCAPE) && exitWindowRequested == false))
    {
        exitWindowRequested = true;
        isInExitMenu = true;
        return;
    }

    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {
        if (fullscreen)
        {
            fullscreen = false;
            ToggleBorderlessWindowed();
        }
        else
        {
            fullscreen = true;
            ToggleBorderlessWindowed();
        }
    }
#endif

    if(firstTimeGameStart) {
        if(isMobile) {
            if(IsGestureDetected(GESTURE_TAP)) {
                firstTimeGameStart = false;
            }
        }
        else if(IsKeyDown(KEY_ENTER)) {
            firstTimeGameStart = false;
        }
    }

    if (exitWindowRequested)
    {
        if (IsKeyPressed(KEY_Y))
        {
            exitWindow = true;
        }
        else if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE))
        {
            exitWindowRequested = false;
            isInExitMenu = false;
        }
    }

    if (IsWindowFocused() == false)
    {
        lostWindowFocus = true;
    }
    else
    {
        lostWindowFocus = false;
    }

#ifndef EMSCRIPTEN_BUILD
    if (exitWindowRequested == false && lostWindowFocus == false && gameOver == false && IsKeyPressed(KEY_P))
#else
    if (exitWindowRequested == false && lostWindowFocus == false && gameOver == false && (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)))
#endif
    {
        paused = !paused;
    }

    // Handle game over restart
    if (gameOver) {
        if (isMobile) {
            if (IsGestureDetected(GESTURE_TAP)) {
                Reset();
            }
        } else if (IsKeyPressed(KEY_ENTER)) {
            Reset();
        }
    }
}

void Game::Draw()
{
    // render everything to a texture
    BeginTextureMode(targetRenderTex);
    ClearBackground(SKYBLUE);

    // Draw pipes
    for (const auto& pipe : pipes) {
        // Top pipe
        DrawRectangle(pipe.x, 0, pipeWidth, pipe.gapCenter - pipeGap/2, GREEN);
        // Bottom pipe
        DrawRectangle(pipe.x, pipe.gapCenter + pipeGap/2, pipeWidth, height - (pipe.gapCenter + pipeGap/2), GREEN);
    }

    // Draw bird
    DrawRectangle(birdX - birdSize/2, birdY - birdSize/2, birdSize, birdSize, YELLOW);

    DrawUI();

    EndTextureMode();

    // render the scaled frame texture to the screen
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(targetRenderTex.texture, (Rectangle){0.0f, 0.0f, (float)targetRenderTex.texture.width, (float)-targetRenderTex.texture.height},
                   (Rectangle){(GetScreenWidth() - ((float)gameScreenWidth * screenScale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * screenScale)) * 0.5f, (float)gameScreenWidth * screenScale, (float)gameScreenHeight * screenScale},
                   (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();
}

void Game::DrawUI()
{
    float screenX = 0.0f;
    float screenY = 0.0f;

    DrawTextEx(font, "Flappy Square", {300, 10}, 44, 2, BLACK);

    // Draw score on the right side
    std::string scoreText = "Score: " + std::to_string(score);
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    int scoreWidth = MeasureText(scoreText.c_str(), 20);
    int highScoreWidth = MeasureText(highScoreText.c_str(), 20);
    int rightPadding = 20;
    
    DrawText(scoreText.c_str(), width - scoreWidth - rightPadding, 20, 20, BLACK);
    DrawText(highScoreText.c_str(), width - highScoreWidth - rightPadding, 50, 20, BLACK);

    if (exitWindowRequested)
    {
        DrawRectangleRounded({screenX + (float)(gameScreenWidth / 2 - 250), screenY + (float)(gameScreenHeight / 2 - 20), 500, 60}, 0.76f, 20, BLACK);
        DrawText("Are you sure you want to exit? [Y/N]", screenX + (gameScreenWidth / 2 - 200), screenY + gameScreenHeight / 2, 20, yellow);
    }
    else if (firstTimeGameStart)
    {
        DrawRectangleRounded({screenX + (float)(gameScreenWidth / 2 - 250), screenY + (float)(gameScreenHeight / 2 - 20), 500, 80}, 0.76f, 20, BLACK);
        if (isMobile) {
            DrawText("Tap to play", screenX + (gameScreenWidth / 2 - 60), screenY + gameScreenHeight / 2 + 10, 20, yellow);
        } else {
#ifndef EMSCRIPTEN_BUILD            
            DrawText("Press Enter to play", screenX + (gameScreenWidth / 2 - 100), screenY + gameScreenHeight / 2 - 10, 20, yellow);
            DrawText("Alt+Enter: toggle fullscreen", screenX + (gameScreenWidth / 2 - 120), screenY + gameScreenHeight / 2 + 30, 20, yellow);
#else
            DrawText("Press Enter to play", screenX + (gameScreenWidth / 2 - 100), screenY + gameScreenHeight / 2 + 10, 20, yellow);
#endif
        }
    }
    else if (paused)
    {
        DrawRectangleRounded({screenX + (float)(gameScreenWidth / 2 - 250), screenY + (float)(gameScreenHeight / 2 - 20), 500, 60}, 0.76f, 20, BLACK);
#ifndef EMSCRIPTEN_BUILD
        DrawText("Game paused, press P to continue", screenX + (gameScreenWidth / 2 - 200), screenY + gameScreenHeight / 2, 20, yellow);
#else
        if (isMobile) {
            DrawText("Game paused, tap to continue", screenX + (gameScreenWidth / 2 - 200), screenY + gameScreenHeight / 2, 20, yellow);
        } else {
            DrawText("Game paused, press P or ESC to continue", screenX + (gameScreenWidth / 2 - 200), screenY + gameScreenHeight / 2, 20, yellow);
        }
#endif
    }
    else if (lostWindowFocus)
    {
        DrawRectangleRounded({screenX + (float)(gameScreenWidth / 2 - 250), screenY + (float)(gameScreenHeight / 2 - 20), 500, 60}, 0.76f, 20, BLACK);
        DrawText("Game paused, focus window to continue", screenX + (gameScreenWidth / 2 - 200), screenY + gameScreenHeight / 2, 20, yellow);
    }
    else if (gameOver)
    {
        DrawRectangleRounded({screenX + (float)(gameScreenWidth / 2 - 250), screenY + (float)(gameScreenHeight / 2 - 20), 500, 100}, 0.76f, 20, BLACK);
        std::string gameOverText = "Game Over! Score: " + std::to_string(score);
        DrawText(gameOverText.c_str(), screenX + (gameScreenWidth / 2 - 150), screenY + gameScreenHeight / 2 - 20, 20, yellow);
        if (isMobile) {
            DrawText("Tap to play again", screenX + (gameScreenWidth / 2 - 100), screenY + gameScreenHeight / 2 + 20, 20, yellow);
        } else {
            DrawText("Press Enter to play again", screenX + (gameScreenWidth / 2 - 120), screenY + gameScreenHeight / 2 + 20, 20, yellow);
        }
    }
}

std::string Game::FormatWithLeadingZeroes(int number, int width)
{
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    numberText = std::string(leadingZeros, '0') + numberText;
    return numberText;
}

void Game::Randomize()
{
}

void Game::LoadHighScore()
{
#ifndef __EMSCRIPTEN__
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
        highScore = 0;
    }
#else
    highScore = 0;
#endif
}

void Game::SaveHighScore()
{
#ifndef __EMSCRIPTEN__
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
#endif
}