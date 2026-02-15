#include "cs3113.h"

Color ColorFromHex(const char *hex)
{
    // Skip leading '#', if present
    if (hex[0] == '#') hex++;

    // Default alpha = 255 (opaque)
    unsigned int r = 0, 
                 g = 0, 
                 b = 0, 
                 a = 255;

    // 6‑digit form: RRGGBB
    if (sscanf(hex, "%02x%02x%02x", &r, &g, &b) == 3) {
        return (Color){ (unsigned char) r,
                        (unsigned char) g,
                        (unsigned char) b,
                        (unsigned char) a };
    }

    // 8‑digit form: RRGGBBAA
    if (sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a) == 4) {
        return (Color){ (unsigned char) r,
                        (unsigned char) g,
                        (unsigned char) b,
                        (unsigned char) a };
    }

    // Fallback – return white so you notice something went wrong
    return RAYWHITE;
}

/**
 * @brief Calculates and returns the magnitude of a 2D vector.
 * 
 * @param vector Any 2D raylib vector.
 */
float GetLength(const Vector2 *vector)
{
    return sqrtf(
        pow(vector->x, 2) + pow(vector->y, 2)
    );
}

/**
 * @brief Mutates two dimensional vector to become its unit vector counterpart,
 * also known as a direction vector, retains the original vector’s orientation
 * but has a standardised length.
 * 
 * @see https://hogonext.com/how-to-normalize-a-vector/
 * 
 * @param vector Any 2D raylib vector.
 */
void Normalise(Vector2 *vector)
{
    float magnitude = GetLength(vector);

    vector->x /= magnitude;
    vector->y /= magnitude;
}

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 * 
 * @see 
 * 
 * @param postionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(postionA->x - positionB->x) - 
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(postionA->y - positionB->y) - 
                      ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}


Vector2 operator+(const Vector2 vec1, const Vector2 vec2){
    return Vector2{vec1.x + vec2.x, vec1.y + vec2.y};
}

Vector2 operator+=(Vector2 vec1, const Vector2 vec2){
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