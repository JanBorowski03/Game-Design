/**

* Author: Jan Borowski

* Assignment: Pong Variations

* Date due: 02/28/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

* sure :)

**/

#include "CS3113/cs3113.h"
#include <math.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
Vector2 randomDirection();


// Global Constants
constexpr int   SCREEN_WIDTH  = 1600,
                SCREEN_HEIGHT = 900,
                FPS           = 60,
                PADDLE_SPEED  = 200,
                BALL_SPEED    = 150;
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                  BALL_SIZE   = { 50, 50},
                  PADDLE_SIZE = { 10, 100};

// Global Variables
AppStatus gAppStatus     = START_MENU;
float     gTime          = 0.0f,
          gPreviousTicks = 0.0f;

int maxScore     = 5,
    player1Score = 0,
    player2Score = 0,
    playersNum  = 2,
    ballsNum    = 1,
    ballSpeed   = 1;

Vector2 gPaddle1Position = { 50, SCREEN_HEIGHT / 2},
        gPaddle2Position = { SCREEN_WIDTH - 50, SCREEN_HEIGHT / 2},
        gPaddle1Movement = { 0.0f, 0.0f },
        gPaddle2Movement = { 0.0f, 0.0f },
        objectOrigin;

std::vector<Vector2> ballsPositions = {ORIGIN},
                     ballsMovements =   {randomDirection()};


Texture gHockeyPuckTexture,
        gPaddleTexture;

Rectangle textureArea,
          destinationArea;




// Function Definitions
void initialise()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); 
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Variations Jan Borowski");

    // Load Textures
    gHockeyPuckTexture = LoadTexture("assets/hockey_puck.png");
    gPaddleTexture = LoadTexture("assets/paddle.png");


    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
    if(IsKeyPressed(KEY_ESCAPE)) gAppStatus = TERMINATED;

    switch (gAppStatus)
    {
        case START_MENU: 
            if(IsKeyPressed(KEY_ENTER)) gAppStatus = SETTINGS_MENU;
            break;

        case SETTINGS_MENU: 
            if(IsKeyPressed(KEY_P)) playersNum = playersNum % 2 + 1;
            if(IsKeyPressed(KEY_S)) ballSpeed = ballSpeed % 4 + 1;
            if(IsKeyPressed(KEY_T)) ballsNum = ballsNum % 3 + 1;
            if(IsKeyPressed(KEY_W)) maxScore = maxScore % 10 + 1;
            if(IsKeyPressed(KEY_ENTER)) gAppStatus = GAME_RUNNING;
            break;
        
        case GAME_RUNNING: 
            if(IsKeyPressed(KEY_T)){
                gAppStatus = SETTINGS_MENU;
                break;
            }
            
            gPaddle1Movement = { 0.0f, 0.0f };
            gPaddle2Movement = { 0.0f, 0.0f };
            if(IsKeyPressed(KEY_A) || IsKeyDown(KEY_A)) gPaddle1Movement.x -= 1;  
            if(IsKeyPressed(KEY_D) || IsKeyDown(KEY_D)) gPaddle1Movement.x += 1;
            if(IsKeyPressed(KEY_S) || IsKeyDown(KEY_S)) gPaddle1Movement.y += 1;
            if(IsKeyPressed(KEY_W) || IsKeyDown(KEY_W)) gPaddle1Movement.y -= 1;  
            
            if(playersNum == 2){
                if(IsKeyPressed(KEY_LEFT) || IsKeyDown(KEY_LEFT)) gPaddle2Movement.x -= 1;  
                if(IsKeyPressed(KEY_RIGHT) || IsKeyDown(KEY_RIGHT)) gPaddle2Movement.x += 1;
                if(IsKeyPressed(KEY_UP) || IsKeyDown(KEY_UP)) gPaddle2Movement.y -= 1;
                if(IsKeyPressed(KEY_DOWN) || IsKeyDown(KEY_DOWN)) gPaddle2Movement.y += 1;    
            }
            Normalise(&gPaddle1Movement);
            Normalise(&gPaddle2Movement);
            break;
        
        case GAME_OVER_MENU: 
            if(IsKeyPressed(KEY_ENTER)) gAppStatus = SETTINGS_MENU;
            break;

        }
}

void update() 
{
    // Upadte delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gTime += 1.0f * deltaTime;

    
    if(gAppStatus == GAME_RUNNING){
        // Update Paddles
        gPaddle1Position += gPaddle1Movement * PADDLE_SPEED * deltaTime;
        gPaddle2Position += gPaddle2Movement * PADDLE_SPEED * deltaTime;

        // Create or Delete Balls
        int ballDiff = ballsNum - static_cast<int>(ballsPositions.size());
        if(ballDiff != 0)
        {
            switch (ballDiff)
            {
                case  1 :
                    ballsPositions.push_back(ORIGIN);
                    ballsMovements.push_back(randomDirection());
                    break;
                case  2 :
                    ballsPositions.push_back(ORIGIN + Vector2{0, 50});
                    ballsPositions.push_back(ORIGIN + Vector2{0, -50});
                    ballsMovements.push_back(randomDirection());
                    ballsMovements.push_back(randomDirection());
                    break;
                case  -1 :
                    ballsPositions.pop_back();
                    ballsMovements.pop_back();
                    break;
                case  -2 :
                    ballsPositions.pop_back();
                    ballsPositions.pop_back();
                    ballsMovements.pop_back();
                    ballsMovements.pop_back();
            break;
            }
        }

        // Update Balls Positions
        for(int i = 0; i < static_cast<int>(ballsPositions.size()); i++) ballsPositions[i] += ballsMovements[i] * ballSpeed * BALL_SPEED * deltaTime; 

        // Ensure Paddles stay in the frame
        if(gPaddle1Position.x <= 20) gPaddle1Position.x = 20;
        if(gPaddle1Position.x >= 200) gPaddle1Position.x = 200;
        if(gPaddle1Position.y <= BALL_SIZE.y) gPaddle1Position.y = BALL_SIZE.y;
        if(gPaddle1Position.y >= SCREEN_HEIGHT - BALL_SIZE.y) gPaddle1Position.y = SCREEN_HEIGHT - BALL_SIZE.y;

        if(gPaddle2Position.x <= SCREEN_WIDTH - 200) gPaddle2Position.x = SCREEN_WIDTH - 200;
        if(gPaddle2Position.x >= SCREEN_WIDTH - 20) gPaddle2Position.x = SCREEN_WIDTH - 20;
        if(gPaddle2Position.y <= BALL_SIZE.y) gPaddle2Position.y = BALL_SIZE.y;
        if(gPaddle2Position.y >= SCREEN_HEIGHT - BALL_SIZE.y) gPaddle2Position.y = SCREEN_HEIGHT - BALL_SIZE.y;

        // Ensure Balls stay in the frame
        for(int i = 0; i < static_cast<int>(ballsPositions.size()); i++){
        if(ballsPositions[i].y <= BALL_SIZE.y || ballsPositions[i].y >= SCREEN_HEIGHT - BALL_SIZE.y) ballsMovements[i].y = -ballsMovements[i].y;
        }

        // Check for collisions
        for(int i = 0; i < static_cast<int>(ballsPositions.size()); i++){
            if(isColliding(&gPaddle1Position, &PADDLE_SIZE, &ballsPositions[i], &BALL_SIZE)) ballsMovements[i].x = -ballsMovements[i].x;
            if(isColliding(&gPaddle2Position, &PADDLE_SIZE, &ballsPositions[i], &BALL_SIZE)) ballsMovements[i].x = -ballsMovements[i].x;
        }

        // Check if point is Scored
        for(int i = 0; i < static_cast<int>(ballsPositions.size()); i++){
            if(ballsPositions[i].x <= 0 || ballsPositions[i].x >= SCREEN_WIDTH){
                if(ballsPositions[i].x <= BALL_SIZE.x) player1Score += 1;
                if(ballsPositions[i].x >= SCREEN_WIDTH - BALL_SIZE.x) player2Score +=1;
                ballsPositions[i] = ORIGIN;
                ballsMovements[i] = randomDirection();
            }
        }
        // Check if game has ended
        if(player1Score >= maxScore || player2Score >= maxScore)  gAppStatus = GAME_OVER_MENU;  
    }
}

void render()
{
    BeginDrawing();

    ClearBackground(ColorFromHex("#00000"));
    switch (gAppStatus)
    {   
               
        case START_MENU: {
            DrawText("PRESS ENTER TO CONTINUE", 100, ORIGIN.y, 40, DARKGRAY);
            break;
        }
        case GAME_RUNNING: {
            DrawText((std::to_string(player1Score) + " : " + std::to_string(player2Score)).c_str(), ORIGIN.x, 100, 40, DARKGRAY);

            for(const Vector2& pos : ballsPositions)
            {
                textureArea = {
                    0.0f, 0.0f,
                    static_cast<float>(gHockeyPuckTexture.width),
                    static_cast<float>(gHockeyPuckTexture.height)
                };

                destinationArea = {
                    pos.x, pos.y,

                    BALL_SIZE.x,
                    BALL_SIZE.y
                };

                objectOrigin = {
                    static_cast<float>(BALL_SIZE.x) / 2.0f,
                    static_cast<float>(BALL_SIZE.y) / 2.0f
                };

                DrawTexturePro(
                    gHockeyPuckTexture, 
                    textureArea, 
                    destinationArea, 
                    objectOrigin, 
                    0.0f, 
                    WHITE
                );
            }

            // Paddle 1
            textureArea = {
                0.0f, 0.0f,
                static_cast<float>(gPaddleTexture.width),
                static_cast<float>(gPaddleTexture.height)
            };

            destinationArea = {
                gPaddle1Position.x, gPaddle1Position.y,

                PADDLE_SIZE.x,
                PADDLE_SIZE.y
            };

            objectOrigin = {
                static_cast<float>(PADDLE_SIZE.x) / 2.0f,
                static_cast<float>(PADDLE_SIZE.y) / 2.0f
            };

            DrawTexturePro(
                gPaddleTexture, 
                textureArea, 
                destinationArea, 
                objectOrigin, 
                0.0f, 
                WHITE
            );


            // Paddle 2
            textureArea = {
                0.0f, 0.0f,
                static_cast<float>(gPaddleTexture.width),
                static_cast<float>(gPaddleTexture.height)
            };

            destinationArea = {
                gPaddle2Position.x, gPaddle2Position.y,

                PADDLE_SIZE.x,
                PADDLE_SIZE.y
            };

            objectOrigin = {
                static_cast<float>(PADDLE_SIZE.x) / 2.0f,
                static_cast<float>(PADDLE_SIZE.y) / 2.0f
            };

            DrawTexturePro(
                gPaddleTexture, 
                textureArea, 
                destinationArea, 
                objectOrigin, 
                0.0f, 
                WHITE
            );
            break;
        }
        case SETTINGS_MENU: {
            DrawText(("Press P TO CHANGE THE NUMBER OF PLAYERS: " + std::to_string(playersNum)).c_str(), 100, ORIGIN.y - 100, 40, DARKGRAY);
            DrawText(("PRESS S TO CHANGE THE GAME SPEED: " + std::to_string(ballSpeed)).c_str(), 100, ORIGIN.y - 50, 40, DARKGRAY);
            DrawText(("PRESS T TO CHANGE THE NUMBER OF BALLS: " + std::to_string(ballsNum)).c_str(), 100, ORIGIN.y, 40, DARKGRAY);
            DrawText(("PRESS W TO CHANGE THE POINTS WIN CONDITION:  " + std::to_string(maxScore)).c_str(), 100, ORIGIN.y + 50, 40, DARKGRAY);
            DrawText("PRESS ENTER TO CONTINUE", 100, ORIGIN.y + 100, 40, DARKGRAY);
            break;
        }
        case GAME_OVER_MENU: {
            DrawText("GAME OVER", ORIGIN.x, 50, 80, DARKGRAY);
            DrawText((std::to_string(player1Score) + " : " + std::to_string(player2Score)).c_str(), ORIGIN.x, 100, 60, DARKGRAY);
            DrawText("PRESS ENTER TO CONTINUE", 100, ORIGIN.y, 40, DARKGRAY);
            break;
        }
    }

    EndDrawing();
}

void shutdown() 
{ 
    UnloadTexture(gHockeyPuckTexture);
    UnloadTexture(gPaddleTexture);
    CloseWindow(); 
}

Vector2 randomDirection(){
    float s = std::rand();
    float x = cosf(s), y = sinf(s);
    while(x > 2 * y || y > 2 * x){
        float s = std::rand();
        float x = cosf(s), y = sinf(s);
    }
    return Vector2{x, y};
}

int main(void)
{
    initialise();

    while (gAppStatus != TERMINATED)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}


