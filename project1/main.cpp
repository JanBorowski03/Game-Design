/**

* Author: Jan Borowski

* Assignment: 2D scene

* Date due: 02/14/2026

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

* sure :)

**/

// Used an AI generated python script to get bg images with pulsing brightness
// Obtained graphics from
// https://luizmelo.itch.io/wizard-pack
// https://www.forgotten-adventures.net/product/battlemaps/dungeon-encounters-pack-1/

#include "CS3113/cs3113.h"
#include <math.h>

// Global Constants
constexpr int   SCREEN_WIDTH  = 1600,
                SCREEN_HEIGHT = 900,
                FPS           = 60;
//constexpr float PI = 3.14159265f;
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gTime     = 0.0f,
          gPreviousTicks = 0.0f;


// Background 
int gBackgroundPulseTime = 10;
Texture2D gBGTextures[60];  


// Wizard
float gWizardPathTime = 30;
Vector2   gWizardPosition;
const Vector2 gWizardCheckpoints[] = {
    {500.0f, 700.0f},
    {200.0f, 200.0f},
    {1300.0f, 400.0f}
};


Texture2D gWizardTexture;


// Magic orbs
const float ORB_PULSE_TIME = 2,
            ORB_PULSE_FACTOR = 0.2,
            ORB_ROTATION_TIME = 4,
            ORB_ORBIT_RADIUS = 40,
            ORB_ORBIT_TIME = 7;

float       gOrb1Pulse,
            gOrb2Pulse,
            gOrbPulsePhase,
            gOrb1Rotation,
            gOrb2Rotation,
            gOrbPhase;

Vector2     gOrb1Position,
            gOrb2Position;

Texture2D   gOrbTexture;




// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// More functions
void renderBackground();
void renderWizard();
void renderOrbs();
void updateWizardPosition();
void updateOrbPositions();

Vector2 operator+(const Vector2 vec1, const Vector2 vec2){
    return Vector2{vec1.x + vec2.x, vec1.y + vec2.y};
}

Vector2 operator*(const float t, const Vector2 vec){
    return Vector2{vec.x * t, vec.y* t};
}

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2D Scene Jan Borowski");

    // Load all textures
    char LINK_FP[] = "assets/dungeon_bg_00.jpg";
    for(int i = 0; i < 60; i++){
        LINK_FP[18] = static_cast<char>('0' + (i / 10));
        LINK_FP[19] = static_cast<char>('0' + (i % 10));
        gBGTextures[i] = LoadTexture(LINK_FP);
    }

    char wiz_LINK_FP[] = "assets/wizard.png";
    gWizardTexture = LoadTexture(wiz_LINK_FP);

    char orb_LINK_FP[] = "assets/orb.png";
    gOrbTexture = LoadTexture(orb_LINK_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void updateWizardPosition()
{
    Vector2 start, end;
    int phase = floor(fmodf(gTime, gWizardPathTime) / (gWizardPathTime / 4));

    switch (phase)
    {
    case 0:
        start = gWizardCheckpoints[0];
        end = gWizardCheckpoints[1];
        break;
    case 1:
        start = gWizardCheckpoints[1];
        end = gWizardCheckpoints[2];
        break;
    case 2:
        start = gWizardCheckpoints[2];
        end = gWizardCheckpoints[1];
        break;
    case 3:
        start = gWizardCheckpoints[1];
        end = gWizardCheckpoints[0];
        break;
    }
    float t = fmodf(gTime, gWizardPathTime / 4) / (gWizardPathTime / 4);

    gWizardPosition = (1 - t) * start + t * end;
}

void updateOrbPositions()
{
    gOrbPhase = fmodf(gTime, ORB_ORBIT_TIME) * (2 * PI / ORB_ORBIT_TIME);
    gOrb1Position = gWizardPosition + Vector2{-7, -33} + ORB_ORBIT_RADIUS * Vector2{cosf(gOrbPhase), sinf(gOrbPhase)};
    
    gOrb2Position = gWizardPosition + Vector2{-7, -33} + ORB_ORBIT_RADIUS * Vector2{cosf(gOrbPhase + PI), sinf(gOrbPhase + PI)};

    gOrb1Rotation = fmodf(gTime, ORB_ROTATION_TIME) * (360 / ORB_ROTATION_TIME);
    gOrb2Rotation = - fmodf(gTime, ORB_ROTATION_TIME) * (360 / ORB_ROTATION_TIME);

    gOrbPulsePhase = fmodf(gTime, ORB_PULSE_TIME) * (2 * PI / ORB_PULSE_TIME);
    gOrb1Pulse = 1 + ORB_PULSE_FACTOR * cosf(gOrbPulsePhase);
    gOrb2Pulse = 1 - ORB_PULSE_FACTOR * sinf(gOrbPulsePhase);
}

void update() 
{
    // Upadte delta time
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gTime += 1.0f * deltaTime;

    // Update objects
    updateWizardPosition();
    updateOrbPositions();
}

void renderBackground()
{
    int phase = (static_cast<int>(gTime) % gBackgroundPulseTime) * (60 / gBackgroundPulseTime);
    Texture2D BGTexture = gBGTextures[phase];

    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(BGTexture.width),
        static_cast<float>(BGTexture.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        ORIGIN.x, ORIGIN.y,

        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(SCREEN_WIDTH) / 2.0f,
        static_cast<float>(SCREEN_HEIGHT) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        BGTexture, 
        textureArea, 
        destinationArea, 
        objectOrigin, 
        0.0f, 
        WHITE
    );

}

void renderWizard()
{
    // Whole texture (UV coordinates)
    Rectangle textureArea = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gWizardTexture.width),
        static_cast<float>(gWizardTexture.height)
    };

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        gWizardPosition.x, gWizardPosition.y,

        static_cast<float>(gWizardTexture.width * 2),
        static_cast<float>(gWizardTexture.height * 2)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(gWizardTexture.width) / 2.0f,
        static_cast<float>(gWizardTexture.height) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        gWizardTexture,
        textureArea,
        destinationArea,
        objectOrigin,
        0.0f,
        WHITE
    );
}

void renderOrbs()
{
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(gOrbTexture.width),
        static_cast<float>(gOrbTexture.height)
    };

    Rectangle destinationAreaOrb1 = {
        gOrb1Position.x, gOrb1Position.y,

        static_cast<float>(gOrbTexture.width) * gOrb1Pulse,
        static_cast<float>(gOrbTexture.height) * gOrb1Pulse
    };

    Rectangle destinationAreaOrb2 = {
        gOrb2Position.x, gOrb2Position.y,

        static_cast<float>(gOrbTexture.width) * gOrb2Pulse,
        static_cast<float>(gOrbTexture.height) * gOrb2Pulse
    };

    Vector2 objectOrigin = {
        static_cast<float>(gOrbTexture.width) / 2.0f,
        static_cast<float>(gOrbTexture.height) / 2.0f
    };

    DrawTexturePro(
        gOrbTexture, 
        textureArea, 
        destinationAreaOrb1, 
        objectOrigin, 
        gOrb1Rotation, 
        WHITE
    );

    DrawTexturePro(
        gOrbTexture, 
        textureArea, 
        destinationAreaOrb2, 
        objectOrigin, 
        gOrb2Rotation, 
        WHITE
    );
}

void render()
{
    BeginDrawing();

    renderBackground();

    renderOrbs();

    renderWizard();

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow(); 
    for(Texture2D& BGTexture : gBGTextures) UnloadTexture(BGTexture);
    UnloadTexture(gWizardTexture);
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
