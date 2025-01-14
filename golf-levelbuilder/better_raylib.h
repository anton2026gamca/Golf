#include "raylib.h"

// Sprite
#ifndef BETTER_RAYLIB_SPRITE_H
#define BETTER_RAYLIB_SPRITE_H

typedef struct {
    int width;
    int height;
    bool* pixels;
} CollisionMask;

class Sprite
{
    public:
        Sprite();                                   // Creates Empty Sprite
        Sprite(Image _image);                       // Creates Sprite
        Sprite(Image _image, Vector2 _pivot, Vector2 _position, float _scale); // Creates Sprite
        void Delete();                              // Delete Sprite

        // Position, rotation, scale

        bool visible;                               // Is Sprite visible on screen
        Vector2 pivot;                              // Sprite Pivot: (0, 0) = top-left; (1, 1) = bottom-right
        Vector2 position;                           // Sprite Position
        float rotation;                             // Sprite Rotation
        float scale;                                // Sprite Scale
        Color color;                                // Sprite Color

        void Move(Vector2 move);                    // Move Sprite
        void Rotate(float rotateBy);                // Rotate Sprite

        // Rects

        float GetWidth();                           // Get Sprite width
        float GetHeight();                          // Get Sprite Height
        Rectangle GetBackgroundRect();              // Get Sprite rectangle
        float GetOuterRectWidth();                  // Get Sprite outer rectangle width
        float GetOuterRectHeight();                 // Get Sprite outer rectangle height
        Rectangle GetOuterRect();                   // Get Sprite outer Rectangle
        Vector2 GetPivotWorld();                    // Get Sprite Pivot in world cordinates
        void SetSourceRect(Rectangle rect);         // Sets 'sourceRect' if the animation is not set
        Rectangle GetSourceRect();                  // Gets 'sourceRect'

        // Collision

        bool GetCollision(Sprite* otherSprite);     // Check collision between Sprite and otherSprite
        bool GetCollisionRect(Rectangle rect, Vector2 rectPivot, float rectRotation); // Check collision between sprite and rectangle
        bool GetCollisionPoint(Vector2 point); // Checl collision between sprite and point
        CollisionMask mask;                         // Collision mask
        bool CreateCollisionMask();                 // Create collision mask
        void CreateCollisionMaskTexture();          // Create collision mask texture to draw in DrawCollisionMask()

        // Draw

        void Draw();                                // Draw Sprite to screen if visible
        void DrawDebugRects(Color rectColor, Color outerRectColor); // Draw Debug rectangles if visible
        void DrawCollisionMask();                   // Draw Sprite collision mask

        // Animation

        void SetAnimation(int frameWidth, int frameHeight); // Cuts Sprite into frames (Animation will start on the 1st frame and end on the last)
        void SetAnimation(int frameWidth, int frameHeight, int startFrame, int frameCount); // Cuts Sprite into frames
        void NextFrame();                           // Displays the next frame of the animation
        void AnimationUpdate(float fps);            // Updates the animation

        Texture texture = {0};                      // Sprite Texture
        Image image = {0};                          // Sprite Image
    private:
        Rectangle sourceRect;                       // What to draw from the texture

        Image collisionMaskImage = {0};
        Texture collisionMaskTexture = {0};

        // Sprite Animation

        // true if the animation is set
        // NOTE: if true SetSourceRect() will not work
        bool isAnimationSet;                        // ...
        int animationFrameWidth;                    // Animation frame width
        int animationFrameHeight;                   // Animation frame height
        int animationStartFrame;                    // The first frame of the animation
        int animationFramesCount;                   // Max Animation frames
        int animationCurrentFrame;                  // Current Animation frame
        float timeFromLastUpdate;                   // The time elapsed from the last animation update

};

Sprite LoadSprite(const char* path, Vector2 pivot, Vector2 position, float scale); // Loads Texture at path and creates Sprite

bool GetSpriteCollision(Sprite* sprite1, Sprite* sprite2); // Checks collision between 2 sprites
bool GetSpritePixelCollision(Sprite* sprite1, Sprite* sprite2, bool drawCollision = false); // Check pixel perfect collision between 2 sprites

#endif /* BETTER_RAYLIB_SPRITE_H */

// Shapes
#ifndef BETTER_RAYLIB_SHAPES_H
#define BETTER_RAYLIB_SHAPES_H

bool CheckCollisionRecs(Rectangle rect1, Vector2 rect1pivot, float rect1rotation, Rectangle rect2, Vector2 rect2pivot, float rect2rotation);
Rectangle GetRectOuterRect(Rectangle rect, Vector2 pivot, float rotation);

#endif /* BETTER_RAYLIB_SHAPES_H */

// Helpers
#ifndef BETTER_RAYLIB_HELPERS_H
#define BETTER_RAYLIB_HELPERS_H

bool Button(Rectangle rect, const char* text, int fontSize, Color normalColor, Color hoverColor, Color pressColor);
bool Toggle(bool value, Rectangle rect, const char* text, int fontSize, Color normalColor, Color hoverColor, Color pressColor);
class Slider
{
    public:
        Slider(Rectangle rect, float min, float max, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor);
        Slider(Rectangle rect, float min, float max, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor, bool intSlider);
        Slider(Rectangle rect, float min, float max, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor, const char* label, int fontSize);
        Slider(Rectangle rect, float min, float max, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor, bool intSlider, const char* label, int fontSize);

        void Draw();

        float value;
    private:
        bool _intSlider = false;
        bool drawLabel;
        const char* _label;
        int _fontSize;

        Rectangle _rect;
        float _min;
        float _max;
        Color _backgroundColor;
        Color _normalColor;
        Color _hoverColor;
        Color _pressColor;

        bool dragging = false;
        float progress = 0.5;
};
class InputBox
{
    public:
        InputBox(Rectangle rect, Color background_color, Color outline_color, Color text_color, float font_size);

        void Draw();
        const char* GetText();
    private:
        char *text = {0};
        Rectangle _rect;
        Color _background_color;
        Color _outline_color;
        Color _text_color;
        float _font_size;
};
void DrawText(const char *text, int posX, const int posY, int fontSize, float originX, Color color);

Vector2 RotatePoint(Vector2 point, Vector2 origin, float rotation);
float Vector2Angle(Vector2 v1, Vector2 v2, Vector2 origin);

#endif /* BETTER_RAYLIB_HELPERS_H */

#ifdef BETTER_RAYLIB_IMPLEMENTATION

#include <raylib.h>
#include <raymath.h>
#include <iostream>

using namespace std;

/* SPRITE */

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

    return CheckCollisionRecs(rect1, {pivot.x * rect1.width, pivot.y * rect1.height}, rotation, rect2, {otherSprite->pivot.x * rect2.width, otherSprite->pivot.y * rect2.height}, otherSprite->rotation);
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
    if (!isAnimationSet)
        return;

    animationCurrentFrame++;
    if (animationCurrentFrame >= animationFramesCount)
        animationCurrentFrame = 0;

    int numFramesFitX = texture.width / animationFrameWidth;
    sourceRect = Rectangle{(float)((animationStartFrame + animationCurrentFrame) % numFramesFitX) * animationFrameWidth, (float)((animationStartFrame + animationCurrentFrame) / numFramesFitX) * animationFrameHeight, (float)animationFrameWidth, (float)animationFrameHeight};
}

void Sprite::AnimationUpdate(float fps)
{
    if (!isAnimationSet)
        return;

    timeFromLastUpdate += GetFrameTime();
    const float targetTime = 1 / fps;

    if (timeFromLastUpdate > targetTime)
    {
        NextFrame();
        timeFromLastUpdate -= targetTime;
    }
}

/* SHAPES */

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

    ///DrawLine(otherRectPoints[0].x, otherRectPoints[0].y, otherRectPoints[1].x, otherRectPoints[1].y, RED);
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
                //DrawRectangle(point.x, point.y, 1, 1, RED);

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

/* HELPERS */

/// @brief              Draws a button to screen
/// @param rect         Button Rectangle
/// @param text         Text to display in button
/// @param fontSize     Text font size
/// @return             True if the button is pressed
bool Button(Rectangle rect, const char* text, int fontSize, Color normalColor, Color hoverColor, Color pressColor)
{
    if (CheckCollisionPointRec(GetMousePosition(), rect))
    {
        if (IsMouseButtonDown(0))
            DrawRectangle(rect.x, rect.y, rect.width, rect.height, pressColor);
        else
            DrawRectangle(rect.x, rect.y, rect.width, rect.height, hoverColor);
    }
    else
    {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, normalColor);
    }
    
    DrawText(text, rect.x + (rect.width - MeasureText(text, fontSize)) / 2, rect.y + (rect.height - fontSize) / 2, fontSize, RAYWHITE);

    return IsMouseButtonReleased(0) && CheckCollisionPointRec(GetMousePosition(), rect);
}

/// @brief              Draws a toggle to screen
/// @param value        Toggle value
/// @param rect         Toggle rectangle
/// @param text         Text to display in toggle
/// @param fontSize     Text font size
/// @return             Toggle value
bool Toggle(bool value, Rectangle rect, const char* text, int fontSize, Color normalColor, Color hoverColor, Color pressColor)
{
    float toggleRectMargin = 7;
    Rectangle toggleRect = Rectangle{rect.x + toggleRectMargin, rect.y + toggleRectMargin, rect.height - toggleRectMargin * 2, rect.height - toggleRectMargin * 2};

    if (CheckCollisionPointRec(GetMousePosition(), rect))
    {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, IsMouseButtonDown(0) ? pressColor : hoverColor);
    }
    else
    {
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, normalColor);
    }

    DrawText(text, rect.x + 10 + rect.height, rect.y + (rect.height - fontSize), fontSize, RAYWHITE);

    if (value)
    {
        DrawRectangle(rect.x, rect.y, rect.height, rect.height, GREEN);
        DrawRectangle(toggleRect.x, toggleRect.y, toggleRect.width, toggleRect.height, BLACK);
    }
    else
    {
        DrawRectangle(rect.x, rect.y, rect.height, rect.height, RED);
        DrawRectangleLinesEx(toggleRect, 5, BLACK);
    }

    return IsMouseButtonPressed(0) && CheckCollisionPointRec(GetMousePosition(), rect) ? !value : value;
}

/// @brief              Creates a slider
/// @param rect         Slider rectangle
/// @param minVal       minimum value
/// @param maxVal       maximum value
/// @param backgroundColor color
/// @param normalColor  Handle default color
/// @param hoverColor   Handle hovered color
/// @param pressColor   Handle pressed color
Slider::Slider(Rectangle rect, float minVal, float maxVal, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor)
{
    drawLabel = false;

    _rect = rect;
    _min = min(minVal, maxVal);
    _max = max(minVal, maxVal);
    _backgroundColor = backgroundColor;
    _normalColor = normalColor;
    _hoverColor = hoverColor;
    _pressColor = pressColor;

    progress = (defaultValue - minVal) / (maxVal - minVal);
    value = defaultValue;
}

Slider::Slider(Rectangle rect, float minVal, float maxVal, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor, bool intSlider)
{
    _intSlider = intSlider;
    
    drawLabel = false;

    _rect = rect;
    _min = min(minVal, maxVal);
    _max = max(minVal, maxVal);
    _backgroundColor = backgroundColor;
    _normalColor = normalColor;
    _hoverColor = hoverColor;
    _pressColor = pressColor;

    progress = (defaultValue - minVal) / (maxVal - minVal);
    value = defaultValue;
}

Slider::Slider(Rectangle rect, float minVal, float maxVal, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor, const char* label, int fontSize)
{
    drawLabel = true;
    _label = label;
    _fontSize = fontSize;

    _rect = Rectangle{rect.x, rect.y + rect.height / 2, rect.width, rect.height / 2};
    _min = min(minVal, maxVal);
    _max = max(minVal, maxVal);
    _backgroundColor = backgroundColor;
    _normalColor = normalColor;
    _hoverColor = hoverColor;
    _pressColor = pressColor;

    progress = (defaultValue - minVal) / (maxVal - minVal);
    value = defaultValue;
}

Slider::Slider(Rectangle rect, float minVal, float maxVal, float defaultValue, Color backgroundColor, Color normalColor, Color hoverColor, Color pressColor, bool intSlider, const char* label, int fontSize)
{
    _intSlider = intSlider;
    
    drawLabel = true;
    _label = label;
    _fontSize = fontSize;

    _rect = Rectangle{rect.x, rect.y + rect.height / 2, rect.width, rect.height / 2};
    _min = min(minVal, maxVal);
    _max = max(minVal, maxVal);
    _backgroundColor = backgroundColor;
    _normalColor = normalColor;
    _hoverColor = hoverColor;
    _pressColor = pressColor;

    progress = (defaultValue - minVal) / (maxVal - minVal);
    value = defaultValue;
}

/// @brief              Updates and Draws slider to screen 
void Slider::Draw()
{
    float range = _max - _min;

    int lineHeight = 5;
    int lineMargin = 15;

    int sliderMargin = 5;
    float sliderWidth = 20;
    Rectangle sliderRect = Rectangle{_rect.x + sliderMargin, _rect.y + sliderMargin, sliderWidth, _rect.height - sliderMargin * 2};
    if (progress > 1)
    {
        sliderRect.x += (_rect.width - sliderWidth - sliderMargin * 2);
    }
    else if (progress > 0)
    {
        if (_intSlider)
        {
            sliderRect.x += (round(progress * range) / range) * (_rect.width - sliderWidth - sliderMargin * 2);
        }
        else
        {
            sliderRect.x += progress * (_rect.width - sliderWidth - sliderMargin * 2);
        }
    }

    // Draw Background
    DrawRectangle(_rect.x, _rect.y, _rect.width, _rect.height, _backgroundColor);
    // Draw line
    DrawRectangle(_rect.x + lineMargin, _rect.y + (_rect.height - lineHeight) / 2, _rect.width - lineMargin * 2, lineHeight, RAYWHITE);
    // Draw slider
    if (dragging)
    {
        DrawRectangle(sliderRect.x, sliderRect.y, sliderRect.width, sliderRect.height, _pressColor);

        float move = GetMouseDelta().x;
        progress += move / (_rect.width - sliderWidth - sliderMargin * 2);

        float value01 = progress;
        if (progress > 1)
            value01 = 1;
        else if (progress < 0)
            value01 = 0;

        if (_intSlider)
            value = round(_min + (value01 * range));
        else
            value = _min + (value01 * range);

        if (!IsMouseButtonDown(0))
        {
            dragging = false;

            if (progress < 0) progress = 0;
            else if (progress > 1) progress = 1;
        }
    }
    else if (CheckCollisionPointRec(GetMousePosition(), sliderRect))
    {
        DrawRectangle(sliderRect.x, sliderRect.y, sliderRect.width, sliderRect.height, IsMouseButtonDown(0) ? _pressColor : _hoverColor);

        if (IsMouseButtonPressed(0))
        {
            dragging = true;
        }
    }
    else
    {
        DrawRectangle(sliderRect.x, sliderRect.y, sliderRect.width, sliderRect.height, _normalColor);

        progress = value / range;
    }

    // Draw Label
    if (drawLabel)
    {
        DrawRectangle(_rect.x, _rect.y - _rect.height, _rect.width, _rect.height, _backgroundColor);

        DrawText(_label, _rect.x + 5, _rect.y - _rect.height + 5, _fontSize, RAYWHITE);

        char valueStr[50];
        if (_intSlider)
            sprintf(valueStr, "%d", (int)value);
        else
            sprintf(valueStr, "%f", value);
        DrawText(valueStr, _rect.x + _rect.width - MeasureText(valueStr, _fontSize) - 5, _rect.y - _rect.height + 5, _fontSize, RAYWHITE);
    }
}

InputBox::InputBox(Rectangle rect, Color background_color, Color outline_color, Color text_color, float font_size)
{
    _rect = rect;
    _background_color = background_color;
    _outline_color = outline_color;
    _text_color = text_color;
    _font_size = font_size;
}

void InputBox::Draw()
{
    DrawRectangle(_rect.x, _rect.y, _rect.width, _rect.height, _background_color);
    DrawRectangleLines(_rect.x, _rect.y, _rect.width, _rect.height, _outline_color);
    DrawText(text, _rect.x + 5, _rect.y + (_rect.height - _font_size) / 2, _font_size, _text_color);
}

/// @brief              Draws text
/// @param text         Text to draw
/// @param posX         X position of the text
/// @param posY         Y position of the text
/// @param fontSize     The font size
/// @param originX      From where to draw the text (0 is left, 1 is right)
/// @param color        The color of the text
void DrawText(const char *text, int posX, const int posY, int fontSize, float originX, Color color)
{
    DrawText(text, posX - MeasureText(text, fontSize) * originX, posY, fontSize, color);
}

/// @brief              Rotates point
/// @param point        Point to rotate
/// @param origin       Rotate point around
/// @param rotation     Rotation (deg) to rotate
/// @return             Rotated point
Vector2 RotatePoint(Vector2 point, Vector2 origin, float rotation)
{
    Vector2 pointFromOrigin = (Vector2){point.x - origin.x, point.y - origin.y};

    float x = origin.x + cos(rotation * DEG2RAD) * pointFromOrigin.x - sin(rotation * DEG2RAD) * pointFromOrigin.y;
    float y = origin.y + cos(rotation * DEG2RAD) * pointFromOrigin.y - sin(rotation * DEG2RAD) * -pointFromOrigin.x;

    return (Vector2){x, y};
}

float Vector2Angle(Vector2 v1, Vector2 v2, Vector2 origin)
{
    Vector2 v1fo = {v1.x - origin.x, v1.y - origin.y};
    Vector2 v2fo = {v2.x - origin.x, v2.y - origin.y};
    return Vector2Angle(v1fo, v2fo);
}

#endif