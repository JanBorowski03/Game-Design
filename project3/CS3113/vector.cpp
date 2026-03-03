#include "vector.h"

Vector2 operator+(const Vector2 vec1, const Vector2 vec2){
    return Vector2{vec1.x + vec2.x, vec1.y + vec2.y};
}

Vector2& operator+=(Vector2& vec1, const Vector2 vec2){
    vec1.x += vec2.x; vec1.y += vec2.y;
    return vec1;
}

Vector2 operator*(const float t, const Vector2 vec){
    return Vector2{vec.x * t, vec.y* t};
}

Vector2 operator*(const int t, const Vector2 vec){
    return Vector2{vec.x * t, vec.y* t};
}
Vector2 operator*(const Vector2 vec, const float t){
    return Vector2{vec.x * t, vec.y* t};
}
Vector2 operator*(const Vector2 vec, const int t){
    return Vector2{vec.x * t, vec.y* t};
}
