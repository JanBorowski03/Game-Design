#ifndef CS3113_H
#define VECTOR_OPERATIONS_H

#include "raylib.h"


Vector2 operator+(const Vector2 vec1, const Vector2 vec2);
Vector2& operator+=(Vector2& vec1, const Vector2 vec2);

Vector2 operator*(const float t, const Vector2 vec);
Vector2 operator*(const int t, const Vector2 vec);
Vector2 operator*(const Vector2 vec, const float t);
Vector2 operator*(const Vector2 vec, const int t);


#endif // CS3113_H