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

// Global Constants
constexpr int   SCREEN_WIDTH  = 1600,
                SCREEN_HEIGHT = 900,
                FPS           = 60,
                PADDLE_SPEED  = 200;
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
    ballSpeed    = 200;

Vector2 gPaddle1Position = { 50, SCREEN_HEIGHT / 2},
        gPaddle2Position = { 50, SCREEN_HEIGHT / 2},
        gPaddle1Movement = { 0.0f, 0.0f },
        gPaddle2Movement = { 0.0f, 0.0f };

std::vector<Vector2> ballsPositions = {ORIGIN},
                     ballsMovements =   {Vector2{0, 0}};

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); 
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Variations Jan Borowski");

    // Load Textures

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
            if(IsKeyPressed(KEY_A)) gPaddle1Movement.x -= 1;  
            if(IsKeyPressed(KEY_D)) gPaddle1Movement.x += 1;
            if(IsKeyPressed(KEY_S)) gPaddle1Movement.y -= 1;
            if(IsKeyPressed(KEY_W)) gPaddle1Movement.y += 1;  
            
            if(playersNum == 2){
                if(IsKeyPressed(KEY_LEFT)) gPaddle2Movement.x -= 1;  
                if(IsKeyPressed(KEY_RIGHT)) gPaddle2Movement.x += 1;
                if(IsKeyPressed(KEY_UP)) gPaddle2Movement.y -= 1;
                if(IsKeyPressed(KEY_DOWN)) gPaddle2Movement.y += 1;    
            }
            Normalise(&gPaddle1Movement);
            Normalise(&gPaddle2Movement);

            break;

        case GAME_OVER_MENU:
            if(IsKeyPressed(KEY_ENTER)) gAppStatus = SETTINGS_MENU;
            break;
    }
}

Vector2 randomDirection(){
    int s = std::rand();
    return Vector2{cos(s), sin(s)};
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
        int ballDiff = ballsNum - (sizeof(ballsPositions) / sizeof(ballsPositions[0]));
        if(ballDiff != 0){
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
            };
        }

        // Check for collisions
        for(int i = 0; i < sizeof(ballsPositions) / sizeof(ballsPositions[0]); i++){
            if(isColliding(&gPaddle1Position, &PADDLE_SIZE, &ballsPositions[i], &BALL_SIZE)) ballsMovements[i].x = -ballsMovements[i].x;
            if(isColliding(&gPaddle2Position, &PADDLE_SIZE, &ballsPositions[i], &BALL_SIZE)) ballsMovements[i].x = -ballsMovements[i].x;
        }

        // Update Balls Positions
        for(int i = 0; i < sizeof(ballsPositions) / sizeof(ballsPositions[0]); i++) ballsPositions[i] += ballsMovements[i] * ballSpeed * deltaTime; 

        // Check if point is Scored
        for(int i = 0; i < sizeof(ballsPositions) / sizeof(ballsPositions[0]); i++){
            if(ballsPositions[i].x <= 0 || ballsPositions[i].x >= SCREEN_WIDTH){
                if(ballsPositions[i].x <= 0) player1Score += 1;
                if(ballsPositions[i].x >= SCREEN_WIDTH) player2Score +=1;
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

    switch (gAppStatus)
    {
        case START_MENU:
            break;
        case GAME_RUNNING:
            break;
        case SETTINGS_MENU:
            break;
        case GAME_OVER_MENU:
            break;
        default:
            break;
    }

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
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
