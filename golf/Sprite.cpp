#include "Sprite.h"
#include <raylib.h>
#include <raymath.h>
#include "Helpers.h"
#include "Shapes.h"
#include <iostream>

using namespace std;

/// @brief              Creates Sprite
/// @param _image       Sprite Image
/// @param _pivot       Sprite pivot:           (0, 0) = top-left  (0.5, 0.5) = center  (1, 1) = bottom-right
/// @param _position    Sprite position
/// @param _scale       Sprite scale
Sprite::Sprite(Image _image, Vector2 _pivot, Vector2 _position, float _scale)
{
    // Set Sprite properties
    visible = true;
    texture = LoadTextureFromImage(_image);
    image = _image;
    pivot = _pivot;
    position = _position;
    rotation = 0;
    scale = _scale;
    color = WHITE;
    sourceRect = Rectangle{0, 0, (float)texture.width, (float)texture.height};

    CreateCollisionMask();
}

/// @brief              Creates Empty Sprite
Sprite::Sprite()
{
    // Create Sprite
    Sprite({ 0 }, Vector2{0, 0}, Vector2{0, 0}, 1);
}

/// @brief              Creates Sprite
/// @param _image       Sprite Image
Sprite::Sprite(Image _image)
{
    // Create Sprite
    Sprite(_image, Vector2{0.5, 0.5}, Vector2{0, 0}, 1);
}

/// @brief              Delete Sprite
void Sprite::Delete()
{
    // Unload Sprite Texture from GPU memory (VRAM)
    UnloadTexture(texture);

    // Unload Sprite image from memory (RAM)
    UnloadImage(image);
}

/// @brief              Loads Texture at path and Creates Sprite
/// @param path         Texture path
/// @param _pivot       Sprite pivot:           (0, 0) = top-left  (0.5, 0.5) = center  (1, 1) = bottom-right
/// @param _position    Sprite position
/// @param _scale       Sprite scale
/// @return             Created Sprite
Sprite LoadSprite(const char* path, Vector2 pivot, Vector2 position, float scale)
{
    //Load Image to memory (RAM)
    Image image = LoadImage(path);

    // Create Sprite
    return Sprite(image, pivot, position, scale);
}

/// @brief              Get Sprite width
/// @return             Sprite width
float Sprite::GetWidth()
{
    // Return width
    return sourceRect.width * scale;
}

/// @brief              Get Sprite height
/// @return             Sprite height
float Sprite::GetHeight()
{
    // Return height
    return sourceRect.height * scale;
}

Rectangle Sprite::GetBackgroundRect()
{
    // Get width and height
    float width = GetWidth();
    float height = GetHeight();

    // Get pivot in world cordinates
    Vector2 wPivot = GetPivotWorld();

    // Get Sprites Top Left Corner
    Vector2 topLeftCorener = Vector2{position.x - wPivot.x, position.y - wPivot.y};

    // Return rectangle
    return Rectangle{topLeftCorener.x, topLeftCorener.y, width, height};
}

/// @brief              Gets Sprite Width on screen
/// @return             Sprite Width on screen
float Sprite::GetOuterRectWidth()
{
    // Rectangle Corners relative to pivot withot rotation
    Vector2 topLeft = Vector2{-sourceRect.width * scale * pivot.x, -sourceRect.height * scale * pivot.y};
    Vector2 topRight = Vector2{sourceRect.width * scale * pivot.x, -sourceRect.height * scale * pivot.y};
    Vector2 bottomLeft = Vector2{-sourceRect.width * scale * pivot.x, sourceRect.height * scale * pivot.y};
    Vector2 bottomRight = Vector2{sourceRect.width * scale * pivot.x, sourceRect.height * scale * pivot.y};

    // Rotate corners X
    float c1x = (sourceRect.x + sourceRect.width * pivot.x) + cos(rotation * DEG2RAD) * topLeft.x - sin(rotation * DEG2RAD) * topLeft.y;
    float c2x = (sourceRect.x + sourceRect.width * pivot.x) + cos(rotation * DEG2RAD) * topRight.x - sin(rotation * DEG2RAD) * topRight.y;
    float c3x = (sourceRect.x + sourceRect.width * pivot.x) + cos(rotation * DEG2RAD) * bottomLeft.x - sin(rotation * DEG2RAD) * bottomLeft.y;
    float c4x = (sourceRect.x + sourceRect.width * pivot.x) + cos(rotation * DEG2RAD) * bottomRight.x - sin(rotation * DEG2RAD) * bottomRight.y;
    // Get the lowest and highest
    float minX = min(min(c1x, c2x), min(c3x, c4x));
    float maxX = max(max(c1x, c2x), max(c3x, c4x));

    // Return the width
    return maxX - minX;
}

/// @brief              Gets Sprite Height on screen
/// @return             Sprite Height on screen
float Sprite::GetOuterRectHeight()
{
    // Rectangle Corners relative to pivot withot rotation
    Vector2 topLeft = Vector2{-sourceRect.width * scale * pivot.x, -sourceRect.height * scale * pivot.y};
    Vector2 topRight = Vector2{sourceRect.width * scale * pivot.x, -sourceRect.height * scale * pivot.y};
    Vector2 bottomLeft = Vector2{-sourceRect.width * scale * pivot.x, sourceRect.height * scale * pivot.y};
    Vector2 bottomRight = Vector2{sourceRect.width * scale * pivot.x, sourceRect.height * scale * pivot.y};

    // Rotate corners Y
    float c1y = (sourceRect.y + sourceRect.height * pivot.y) + cos(rotation * DEG2RAD) * topLeft.y - sin(rotation * DEG2RAD) * topLeft.x;
    float c2y = (sourceRect.y + sourceRect.height * pivot.y) + cos(rotation * DEG2RAD) * topRight.y - sin(rotation * DEG2RAD) * topRight.x;
    float c3y = (sourceRect.y + sourceRect.height * pivot.y) + cos(rotation * DEG2RAD) * bottomLeft.y - sin(rotation * DEG2RAD) * bottomLeft.x;
    float c4y = (sourceRect.y + sourceRect.height * pivot.y) + cos(rotation * DEG2RAD) * bottomRight.y - sin(rotation * DEG2RAD) * bottomRight.x;
    // Get the lowest and highest
    float minY = min(min(c1y, c2y), min(c3y, c4y));
    float maxY = max(max(c1y, c2y), max(c3y, c4y));

    // Return the height
    return maxY - minY;
}

/// @brief              Gets Sprite rectangle on screen
/// @return             Sprite rectangle
Rectangle Sprite::GetOuterRect()
{
    // Get width and height
    float width = GetOuterRectWidth();
    float height = GetOuterRectHeight();

    // Get Sprites Top Left Corner
    Vector2 topLeftCorener = Vector2{position.x - width * pivot.x, position.y - height * pivot.y};

    // Return rectangle
    return Rectangle{topLeftCorener.x, topLeftCorener.y, width, height};
}

/// @brief              Get Pivot in world cordinates
/// @return             Sprite pivot in world cordinates
Vector2 Sprite::GetPivotWorld()
{
    return Vector2{pivot.x * sourceRect.width * scale, pivot.y * sourceRect.height * scale};
}

/// @brief              Moves Sprite
/// @param move         Where to move
void Sprite::Move(Vector2 move)
{
    // Add 'move' to sprites position
    position = Vector2{position.x + move.x, position.y + move.y};
}

/// @brief              Rotate Sprite
/// @param rotateBy     Rotate Sprite by degrees
void Sprite::Rotate(float rotateBy)
{
    rotation += rotateBy;
    if (rotation > 360)
        rotation -= 360;
    if (rotation < 0)
        rotation += 360;
}

bool Sprite::CreateCollisionMask()
{
    mask.width = image.width;
    mask.height = image.height;
    int total = mask.width * mask.height;

    mask.pixels = (bool*) malloc(total);
    if (!mask.pixels)
    {
        cout << "ERROR: Failed to allocate mask memory" << endl;
        return false;
    }

    Color* colors = LoadImageColors(image);
    Color* c = colors;

    for (int i = 0; i < total; i++)
    {
        mask.pixels[i] = c->a == 0 ? 0 : 1;
        ++c;
    }
    
    UnloadImageColors(colors);

    return true;
}

/// @brief              Checks collision between Sprite and otherSprite
/// @param otherSprite  Other Sprite to check collision with
/// @return             true if they collided
bool Sprite::GetCollision(Sprite* otherSprite)
{
    Rectangle rect1 = GetBackgroundRect();
    Rectangle rect2 = otherSprite->GetBackgroundRect();

    return CheckCollisionRecs(rect1, position, rotation, rect2, otherSprite->position, otherSprite->rotation);
    /*
    if (!CheckCollisionRecs(GetOuterRect(), otherSprite->GetOuterRect()))
        return false;

    float rotationDiff = otherSprite->rotation - rotation;

    Rectangle rect1 = GetBackgroundRect();
    Rectangle rect2 = otherSprite->GetBackgroundRect();

    Vector2 topLeft = (Vector2){rect2.x, rect2.y};
    Vector2 topRight = (Vector2){rect2.x + rect2.width, rect2.y};
    Vector2 bottomLeft = (Vector2){rect2.x, rect2.y + rect2.height};
    Vector2 bottomRight = (Vector2){rect2.x + rect2.width, rect2.y + rect2.height};

    Vector2 rTopLeft = RotatePoint(topLeft, position, rotationDiff);
    Vector2 rTopRight = RotatePoint(topRight, position, rotationDiff);
    Vector2 rBottomLeft = RotatePoint(bottomLeft, position, rotationDiff);
    Vector2 rBottomRight = RotatePoint(bottomRight, position, rotationDiff);

    Vector2 otherSpritePoints[5];
    otherSpritePoints[0] = rTopLeft;
    otherSpritePoints[1] = rTopRight;
    otherSpritePoints[2] = rBottomRight;
    otherSpritePoints[3] = rBottomLeft;
    otherSpritePoints[4] = rTopLeft;

    //DrawLine(otherSpritePoints[0].x, otherSpritePoints[0].y, otherSpritePoints[1].x, otherSpritePoints[1].y, RED);
    //DrawLine(otherSpritePoints[1].x, otherSpritePoints[1].y, otherSpritePoints[2].x, otherSpritePoints[2].y, RED);
    //DrawLine(otherSpritePoints[2].x, otherSpritePoints[2].y, otherSpritePoints[3].x, otherSpritePoints[3].y, RED);
    //DrawLine(otherSpritePoints[3].x, otherSpritePoints[3].y, otherSpritePoints[4].x, otherSpritePoints[4].y, RED);

    //DrawRectangleLines(rect1.x, rect1.y, rect1.width, rect1.height, RED);

    for (int y = 0; y < rect1.height; y++)
    {
        for (int x = 0; x < rect1.width; x++)
        {
            if (CheckCollisionPointPoly((Vector2){rect1.x + (float)x, rect1.y + (float)y}, otherSpritePoints, 5))
            {
                //Vector2 point = (Vector2){rect1.x + x, rect1.y + y};
                //Vector2 rPoint = RotatePoint(point, position, rotation);
                //DrawRectangle(point.x, point.y, 1, 1, RED);
                //DrawRectangle(rPoint.x, rPoint.y, 1, 1, BLUE);

                return true;
            }
        }
    }

    return false;*/
}

bool Sprite::GetCollisionRect(Rectangle rect, Vector2 rectPivot, float rectRotation)
{
    Rectangle spriteRect = GetBackgroundRect();

    return CheckCollisionRecs({position.x, position.y, spriteRect.width, spriteRect.height}, {spriteRect.width * pivot.x, spriteRect.height * pivot.y}, rotation, rect, rectPivot, rectRotation);
}

bool Sprite::GetCollisionPoint(Vector2 point)
{
    Rectangle rect = GetBackgroundRect();

    Vector2 topLeft = (Vector2){rect.x, rect.y};
    Vector2 topRight = (Vector2){rect.x + rect.width, rect.y};
    Vector2 bottomLeft = (Vector2){rect.x, rect.y + rect.height};
    Vector2 bottomRight = (Vector2){rect.x + rect.width, rect.y + rect.height};

    Vector2 pivot = GetPivotWorld();
    Vector2 rTopLeft = RotatePoint(topLeft, pivot, rotation);
    Vector2 rTopRight = RotatePoint(topRight, pivot, rotation);
    Vector2 rBottomLeft = RotatePoint(bottomLeft, pivot, rotation);
    Vector2 rBottomRight = RotatePoint(bottomRight, pivot, rotation);

    Vector2 poly[5];
    poly[0] = rTopLeft;
    poly[1] = rTopRight;
    poly[2] = rBottomRight;
    poly[3] = rBottomLeft;
    poly[4] = rTopLeft;

    return CheckCollisionPointPoly(point, poly, 5);
}

/// @brief              Checks collision between 2 sprites
/// @param sprite1      1st sprite to check collision with
/// @param sprite2      2nd sprite to check collision with
/// @return             true if they collided
bool GetSpriteCollision(Sprite* sprite1, Sprite* sprite2)
{
    // Get if the sprites collided
    return sprite1->GetCollision(sprite2);
}

/// @brief              Checks pixel perfect collision between 2 sprites
/// @param sprite1      1st sprite to check collision with
/// @param sprite2      2nd sprite to check collision with
/// @return             true if they collided
bool GetSpritePixelCollision(Sprite* sprite1, Sprite* sprite2, bool drawCollision)
{
    if (!CheckCollisionRecs(sprite1->GetOuterRect(), sprite2->GetOuterRect()))
        return false;

    Rectangle collision = GetCollisionRec(sprite1->GetOuterRect(), sprite2->GetOuterRect());

    int columns = collision.width;
    int rows = collision.height;

    Rectangle sprite1Rect = sprite1->GetBackgroundRect();
    Vector2 sprite1Shift = {collision.x - sprite1Rect.x, collision.y - sprite1Rect.y};
    Vector2 sprite1Pivot = sprite1->GetPivotWorld();

    Rectangle sprite2Rect = sprite2->GetBackgroundRect();
    Vector2 sprite2Shift = {collision.x - sprite2Rect.x, collision.y - sprite2Rect.y};
    Vector2 sprite2Pivot = sprite2->GetPivotWorld();
    
    bool IsColliding = false;

    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            Vector2 sprite1wPoint = (Vector2){(sprite1Shift.x + x) / sprite1->scale + sprite1->GetSourceRect().x, (sprite1Shift.y + y) / sprite1->scale + sprite1->GetSourceRect().y};
            Vector2 sprite1point = RotatePoint(sprite1wPoint, (Vector2){sprite1Pivot.x / sprite1->scale, sprite1Pivot.y / sprite1->scale}, -sprite1->rotation);
            bool sprite1IsInPoly = CheckCollisionPointRec(Vector2Multiply(sprite1point, (Vector2){sprite1->scale, sprite1->scale}), (Rectangle){0, 0, sprite1->GetWidth(), sprite1->GetHeight()});
            bool sprite1p = sprite1IsInPoly && sprite1->mask.pixels[(int)(sprite1point.y) * sprite1->mask.width + (int)(sprite1point.x)];

            Vector2 sprite2wPoint = (Vector2){(sprite2Shift.x + x) / sprite2->scale + sprite2->GetSourceRect().x, (sprite2Shift.y + y) / sprite2->scale + sprite2->GetSourceRect().y};
            Vector2 sprite2point = RotatePoint(sprite2wPoint, (Vector2){sprite2Pivot.x / sprite2->scale, sprite2Pivot.y / sprite2->scale}, -sprite2->rotation);
            bool sprite2IsInPoly = CheckCollisionPointRec(Vector2Multiply(sprite2point, (Vector2){sprite2->scale, sprite2->scale}), (Rectangle){0, 0, sprite2->GetWidth(), sprite2->GetHeight()});
            bool sprite2p = sprite2IsInPoly && sprite2->mask.pixels[(int)(sprite2point.y) * sprite1->mask.width + (int)(sprite2point.x)];

            if (sprite1p && sprite2p)
            {
                if (drawCollision)
                {
                    IsColliding = true;

                    DrawRectangle(collision.x + x, collision.y + y, 1, 1, BLUE);
                }
                else
                {
                    return true;
                }
            }
            else if (drawCollision && (!sprite1IsInPoly || !sprite2IsInPoly))
            {
                DrawRectangle(collision.x + x, collision.y + y, 1, 1, RED);
            }
        }
    }

    return IsColliding;
}

/// @brief              Set what to draw from sprites texture (does not work if an animation is set)
/// @param rect         The rectangle on texture
void Sprite::SetSourceRect(Rectangle rect)
{
    if (isAnimationSet)
        return;

    sourceRect = rect;
}

/// @brief              Gets source rectangle
/// @return             Source Rectangle
Rectangle Sprite::GetSourceRect()
{
    return sourceRect;
}

/// @brief              Draws Sprite to screen if visible
void Sprite::Draw()
{
    // if Sprite is not visible => don't draw
    if (!visible)
        return;

    // Where draw the source rectangle on screen
    Rectangle dest = (Rectangle){position.x, position.y, sourceRect.width * scale, sourceRect.height * scale};
    // The sprite is rotated around the pivot
    Vector2 drawPivot = (Vector2){pivot.x * dest.width, pivot.y * dest.height};
    // Draw Sprite texture
    DrawTexturePro(texture, sourceRect, dest, drawPivot, rotation, color);
}

/// @brief              Draws Sprite and Debug rectangles to screen if visible
/// @param rectColor    Sprite background rectangle color
/// @param outerRectColor Sprite outer rectangle color
void Sprite::DrawDebugRects(Color backColor, Color outerRectColor)
{
    // if Sprite is not visible => don't draw
    if (!visible)
        return;
    
    // Draw Sprite outer rectangle on screen
    Rectangle outerRect = GetOuterRect();
    DrawRectangle(outerRect.x, outerRect.y, outerRect.width, outerRect.height, outerRectColor);

    // Draw sprite background rectangle
    DrawRectanglePro(Rectangle{position.x, position.y, GetWidth(), GetHeight()}, GetPivotWorld(), rotation, backColor);
}

/// @brief              Creates collision mask texture that is drawn in DrawCollisionMask() 
void Sprite::CreateCollisionMaskTexture()
{
    // Unload old if any
    if (collisionMaskTexture.id > 0)
    {
        UnloadTexture(collisionMaskTexture);
    }
    if (collisionMaskImage.data != NULL)
    {
        UnloadImage(collisionMaskImage);
        collisionMaskImage.data = NULL;
    }

    // Create new image from Sprite image
    collisionMaskImage = GenImageColor(mask.width, mask.height, BLANK);
    
    // Loop through all the pixels
    for (int y = 0; y < mask.height; y++)
    {
        for (int x = 0; x < mask.width; x++)
        {
            // Draw the pixel
            if (mask.pixels[y * mask.width + x])
                ImageDrawPixel(&collisionMaskImage, x, y,  WHITE);
        }
    }

    // Load Texture
    collisionMaskTexture = LoadTextureFromImage(collisionMaskImage);
}

/// @brief              Draws collision mask if visible
void Sprite::DrawCollisionMask()
{
    // if Sprite is not visible => don't draw
    if (!visible)
        return;

    // Where draw the source rectangle on screen
    Rectangle dest = (Rectangle){position.x, position.y, sourceRect.width * scale, sourceRect.height * scale};
    // The sprite is rotated around the pivot
    Vector2 drawPivot = (Vector2){pivot.x * dest.width, pivot.y * dest.height};

    // Draw Collision Mask texture
    DrawTexturePro(collisionMaskTexture, sourceRect, dest, drawPivot, rotation, WHITE);
}

/// @brief              Cuts sprite into frames (Animation will start on the 1st frame and end on the last)
/// @param frameWidth   Single frame width
/// @param frameHeight  Single frame height
void Sprite::SetAnimation(int frameWidth, int frameHeight)
{
    animationFrameWidth = frameWidth;
    animationFrameHeight = frameHeight;

    int numFramesFitX = texture.width / frameWidth;
    int numFramesFitY = texture.height / frameHeight;
    int numFramesFit = numFramesFitX * numFramesFitY;

    animationFramesCount = numFramesFit;
    animationStartFrame = 0;
    animationCurrentFrame = 0;

    sourceRect = Rectangle{0, 0, (float)frameWidth, (float)frameHeight};

    isAnimationSet = true;
}

/// @brief              Cuts sprite into frames
/// @param frameWidth   Single frame width
/// @param frameHeight  Single frame height
/// @param startFrame   First animation frame
/// @param frameCount   Max frames count
void Sprite::SetAnimation(int frameWidth, int frameHeight, int startFrame, int frameCount)
{
    animationFrameWidth = frameWidth;
    animationFrameHeight = frameHeight;

    int numFramesFitX = texture.width / frameWidth;
    int numFramesFitY = texture.height / frameHeight;
    int numFramesFit = numFramesFitX * numFramesFitY;

    if (numFramesFit < frameCount)
        animationFramesCount = numFramesFit;
    else
        animationFramesCount = frameCount;
    animationStartFrame = 0;
    animationCurrentFrame = 0;

    sourceRect = Rectangle{(float)startFrame * frameWidth, 0, (float)frameWidth, (float)frameHeight};

    isAnimationSet = true;
}

/// @brief Displays next frame of the animation
void Sprite::NextFrame()
{
    animationCurrentFrame++;
    if (animationCurrentFrame >= animationFramesCount)
        animationCurrentFrame = 0;

    int numFramesFitX = texture.width / animationFrameWidth;
    sourceRect = Rectangle{(float)((animationStartFrame + animationCurrentFrame) % numFramesFitX) * animationFrameWidth, (float)((animationStartFrame + animationCurrentFrame) / numFramesFitX) * animationFrameHeight, (float)animationFrameWidth, (float)animationFrameHeight};
}