#ifndef SHAPES_H
#define SHAPES_H

#include <raylib.h>

bool CheckCollisionRecs(Rectangle rect1, Vector2 rect1pivot, float rect1rotation, Rectangle rect2, Vector2 rect2pivot, float rect2rotation);
Rectangle GetRectOuterRect(Rectangle rect, Vector2 pivot, float rotation);

#endif