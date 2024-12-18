#ifndef HELPERS_H
#define HELPERS_H

#include <raylib.h>

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

#endif