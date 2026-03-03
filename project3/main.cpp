/**
* Author: Jan Borowski
* Assignment: Lunar Lander
* Date due: March 14th
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "CS3113/Entity.h"
#include "string.h"
#include "CS3113/vector.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr int   NUMBER_OF_TILES         = 20;
constexpr float ACCELERATION_OF_GRAVITY = 1.0f,
                LANDER_ANGULAR_SPEED    = 1.0f,
                LANDER_ENGINE_POWER     = 1.5f,
                FIXED_TIMESTEP          = 1.0f / 60.0f;


constexpr char BG_COLOUR[]    = "#111111";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
          GRAVITY_VECTOR = {0, ACCELERATION_OF_GRAVITY};

constexpr float tileSize = static_cast<float>(SCREEN_WIDTH) / NUMBER_OF_TILES;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f,
      gLanderAngle     = 0.0f;
    
bool landedOnPeak = false; 
     
int   gLanderFuel      = 20000;

Entity *gLunarLander = nullptr;
std::vector<Entity*> ground = {};
Entity *gLandingPeaks[2] = { nullptr, nullptr };


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void generateLandscape(){
    constexpr float minTileHeight = tileSize * 0.5f;
    constexpr float maxTileHeight = SCREEN_HEIGHT - (tileSize * 0.5f);
    float tileHeight = maxTileHeight - tileSize;

    for (int i = 0; i < NUMBER_OF_TILES; i++)
    {
        ground.push_back(new Entity(
            {(i * tileSize) + (tileSize * 0.5f), tileHeight},
            {tileSize, tileSize},
            "assets/land.png"
        ));
            
        tileHeight += GetRandomValue(0, 1) == 0 ? -tileSize : tileSize;
        tileHeight = Clamp(tileHeight, minTileHeight, maxTileHeight);
    }

};

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander Jan Borowski");

    // Lunar Lander

    std::map<Engine, std::vector<int>> animationAtlas = {
        {OFF,  {  0,  1,  2,  3,  4,  5}},
        {ON ,  {  6,  7,  8,  9, 10, 11}}
    };

    float sizeRatio  = 48.0f / 64.0f;

    gLunarLander = new Entity(
        {SCREEN_WIDTH / 2, 50}, 
        {100.0f * sizeRatio, 100.0f},  
        "assets/lander_sheet.png",           
        ATLAS,                         
        {2, 6},                      
        animationAtlas                 
    );

    gLunarLander->setColliderDimensions({
        gLunarLander->getScale().x * 0.4f ,
        gLunarLander->getScale().y * 0.4f 
    });

    gLunarLander->setAcceleration({0, ACCELERATION_OF_GRAVITY});


    // Landscape
    generateLandscape();

    // Platforms
    gLandingPeaks[0] = ground[GetRandomValue(0, NUMBER_OF_TILES - 1)];
    do{gLandingPeaks[1] = ground[GetRandomValue(0, NUMBER_OF_TILES - 1)];}
    while(gLandingPeaks[0] == gLandingPeaks[1]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    Vector2 landerAcceleration = GRAVITY_VECTOR;

    gLunarLander->setDirection(OFF);
    if (gLanderFuel > 0)
    {
        if (IsKeyDown(KEY_A))
        {
            gLanderAngle -= LANDER_ANGULAR_SPEED;
            gLanderFuel -= 2;
        }
        if (IsKeyDown(KEY_D))
        {
            gLanderAngle += LANDER_ANGULAR_SPEED;
            gLanderFuel -= 2;
        }

        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_SPACE))
        {
            float landerAngleRadians = DEG2RAD * gLanderAngle;
            landerAcceleration += {
                sinf(landerAngleRadians) * LANDER_ENGINE_POWER,
                -cosf(landerAngleRadians) * LANDER_ENGINE_POWER
            };
            gLanderFuel -= 5;
            gLunarLander->setDirection(ON);
        }

        gLanderFuel = std::max(gLanderFuel, 0);
    }

    gLunarLander->setAcceleration(landerAcceleration);
    gLunarLander->setAngle(gLanderAngle);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

}

void update()
{
    // Delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gLunarLander->update(FIXED_TIMESTEP, ground);
        if (gLunarLander->isCollidingBottom() || gLunarLander->getPosition().x > SCREEN_HEIGHT) 
        {
            for (Entity *peak : gLandingPeaks)
            {
                const float xDistance = fabsf(gLunarLander->getPosition().x - peak->getPosition().x);
                const float xLimit = gLunarLander->getColliderDimensions().x * 0.3f;

                if (xDistance <= xLimit) landedOnPeak = true;
            }

            gAppStatus = landedOnPeak ? GAMEWON : GAMEOVER;
        }
        deltaTime -= FIXED_TIMESTEP;
    }
}

void render()
{
    BeginDrawing();

    ClearBackground(ColorFromHex(BG_COLOUR));

    gLunarLander->render();
    for(Entity* land : ground) land->render();

    for (Entity *peak : gLandingPeaks)
    {
        DrawText(
            "->  <-",
            static_cast<int>(peak->getPosition().x - 25),
            static_cast<int>(peak->getPosition().y - peak->getScale().y + 10),
            20,
            RED
        );
    }

    if(gAppStatus == RUNNING)
    {
    // Control Panel
    DrawText(("FUEL: " + std::to_string(gLanderFuel)).c_str(), SCREEN_WIDTH - 300, 40, 40, WHITE);
    }

    else
    {
    DrawText(gAppStatus == GAMEWON ? "YOU WIN" : "GAME OVER", SCREEN_WIDTH - 300, 40, 40, WHITE);
    }
    EndDrawing();
}

void shutdown() 
{ 
    for (Entity *land : ground) delete land;
    ground.clear();
    delete gLunarLander;
    gLunarLander = nullptr;
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus != TERMINATED)
    {
        processInput();
        if(gAppStatus == RUNNING) update();
        render();
    }

    shutdown();

    return 0;
}
