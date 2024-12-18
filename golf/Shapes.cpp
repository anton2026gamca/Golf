#include "Shapes.h"
#include <raylib.h>
#include <raymath.h>
#include "Helpers.h"
#include <iostream>

using namespace std;

bool CheckCollisionRecs(Rectangle rect1, Vector2 rect1pivot, float rect1rotation, Rectangle rect2, Vector2 rect2pivot, float rect2rotation)
{
    float rect1_x = rect1.x - rect1pivot.x;
    float rect1_y = rect1.y - rect1pivot.y;
    float rect2_x = rect2.x - rect2pivot.x;
    float rect2_y = rect2.y - rect2pivot.y;

    if (rect1rotation == 0 && rect2rotation == 0)
    {
        Rectangle new_rect1 = {rect1_x, rect1_y, rect1.width, rect1.height};
        Rectangle new_rect2 = {rect2_x, rect2_y, rect2.width, rect2.height};
        //DrawRectangleLinesEx(new_rect1, 2, RED);
        //DrawRectangleLinesEx(new_rect2, 2, RED);
        return CheckCollisionRecs(new_rect1, new_rect2);
    }

    //Rectangle outer_rect1 = GetRectOuterRect(rect1, rect1pivot, rect1rotation);
    //Rectangle outer_rect2 = GetRectOuterRect(rect2, rect2pivot, rect2rotation);
    //DrawRectangleLinesEx(outer_rect1, 1, BLUE);
    //DrawRectangleLinesEx(outer_rect2, 1, BLUE);
    //if (!CheckCollisionRecs(outer_rect1, outer_rect2))
    //    return false;

    Vector2 topLeft = (Vector2){rect2_x, rect2_y};
    Vector2 topRight = (Vector2){rect2_x + rect2.width, rect2_y};
    Vector2 bottomLeft = (Vector2){rect2_x, rect2_y + rect2.height};
    Vector2 bottomRight = (Vector2){rect2_x + rect2.width, rect2_y + rect2.height};

    Vector2 rTopLeft = RotatePoint(topLeft, {rect2.x, rect2.y}, rect2rotation);
    Vector2 rTopRight = RotatePoint(topRight, {rect2.x, rect2.y}, rect2rotation);
    Vector2 rBottomLeft = RotatePoint(bottomLeft, {rect2.x, rect2.y}, rect2rotation);
    Vector2 rBottomRight = RotatePoint(bottomRight, {rect2.x, rect2.y}, rect2rotation);
    
    rTopLeft = RotatePoint(rTopLeft, {rect1.x, rect1.y}, -rect1rotation);
    rTopRight = RotatePoint(rTopRight, {rect1.x, rect1.y}, -rect1rotation);
    rBottomLeft = RotatePoint(rBottomLeft, {rect1.x, rect1.y}, -rect1rotation);
    rBottomRight = RotatePoint(rBottomRight, {rect1.x, rect1.y}, -rect1rotation);

    Vector2 otherRectPoints[5];
    otherRectPoints[0] = rTopLeft;
    otherRectPoints[1] = rTopRight;
    otherRectPoints[2] = rBottomRight;
    otherRectPoints[3] = rBottomLeft;
    otherRectPoints[4] = rTopLeft;

    //DrawLine(otherRectPoints[0].x, otherRectPoints[0].y, otherRectPoints[1].x, otherRectPoints[1].y, RED);
    //DrawLine(otherRectPoints[1].x, otherRectPoints[1].y, otherRectPoints[2].x, otherRectPoints[2].y, RED);
    //DrawLine(otherRectPoints[2].x, otherRectPoints[2].y, otherRectPoints[3].x, otherRectPoints[3].y, RED);
    //DrawLine(otherRectPoints[3].x, otherRectPoints[3].y, otherRectPoints[4].x, otherRectPoints[4].y, RED);

    //DrawRectangleLines(rect1_x, rect1_y, rect1.width, rect1.height, RED);

    for (int y = 0; y < rect1.height; y++)
    {
        for (int x = 0; x < rect1.width; x++)
        {
            if (CheckCollisionPointPoly((Vector2){rect1_x + (float)x, rect1_y + (float)y}, otherRectPoints, 5))
            {
                //Vector2 point = (Vector2){rect1_x + x, rect1_y + y};
                //Vector2 rPoint = RotatePoint(point, {rect1.x, rect1.y}, rect1rotation);
                //DrawRectangle(point.x, point.y, 1, 1, RED);
                //DrawRectangle(rPoint.x, rPoint.y, 1, 1, BLUE);

                return true;
            }
        }
    }

    return false;
}

Rectangle GetRectOuterRect(Rectangle rect, Vector2 pivot, float rotation)
{
    Vector2 topLeft = Vector2{-rect.width * pivot.x, -rect.height * pivot.y};
    Vector2 topRight = Vector2{rect.width * pivot.x, -rect.height * pivot.y};
    Vector2 bottomLeft = Vector2{-rect.width * pivot.x, rect.height * pivot.y};
    Vector2 bottomRight = Vector2{rect.width * pivot.x, rect.height * pivot.y};

    Vector2 rTopLeft = RotatePoint(topLeft, pivot, rotation);
    Vector2 rTopRight = RotatePoint(topRight, pivot, rotation);
    Vector2 rBottomLeft = RotatePoint(bottomLeft, pivot, rotation);
    Vector2 rBottomRight = RotatePoint(bottomRight, pivot, rotation);

    float minX = min(min(rTopLeft.x, rTopRight.x), min(rBottomLeft.x, rBottomRight.x));
    float maxX = max(max(rTopLeft.x, rTopRight.x), max(rBottomLeft.x, rBottomRight.x));
    float minY = min(min(rTopLeft.y, rTopRight.y), min(rBottomLeft.y, rBottomRight.y));
    float maxY = max(max(rTopLeft.y, rTopRight.y), max(rBottomLeft.y, rBottomRight.y));

    return {minX, minY, maxX - minX, maxY - minY};
}
