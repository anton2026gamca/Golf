#ifndef SPRITE_H
#define SPRITE_H

#include <raylib.h>

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

};

Sprite LoadSprite(const char* path, Vector2 pivot, Vector2 position, float scale); // Loads Texture at path and creates Sprite

bool GetSpriteCollision(Sprite* sprite1, Sprite* sprite2); // Checks collision between 2 sprites
bool GetSpritePixelCollision(Sprite* sprite1, Sprite* sprite2, bool drawCollision = false); // Check pixel perfect collision between 2 sprites

#endif