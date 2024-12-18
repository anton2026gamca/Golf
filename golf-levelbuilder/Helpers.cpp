#include "Helpers.h"
#include <raylib.h>
#include <raymath.h>
#include <iostream>

using namespace std;

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