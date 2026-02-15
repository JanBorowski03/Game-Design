#ifndef CS3113_H
#define CS3113_H
#define LOG(argument) std::cout << argument << '\n'

#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

enum AppStatus { TERMINATED, START_MENU, GAME_RUNNING, SETTINGS_MENU, GAME_OVER_MENU };

Color ColorFromHex(const char *hex);
void Normalise(Vector2 *vector);
float GetLength(const Vector2 *vector);
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

Vector2 operator+(const Vector2 vec1, const Vector2 vec2);
Vector2 operator+=(Vector2 vec1, const Vector2 vec2);

Vector2 operator*(const float t, const Vector2 vec);
Vector2 operator*(const int t, const Vector2 vec);
Vector2 operator*(const Vector2 vec, const float t);
Vector2 operator*(const Vector2 vec, const int t);

#endif // CS3113_H