#ifndef BUTTONS_H
#define BUTTONS_H

#include "raylib.h"
#include "string.h"


class Button{
    private:
        const Vector2 size, position;
        std::string text;


    public:
        Button(const Vector2 size, const Vector2 position, const string& text)

        void render();
};



#endif