#include "slider.h"

void DrawSlider(float x, float y, float width, const char *label, float *value, float min, float max)
{
    DrawRectangle(x, y, width, 4, DARKGRAY);
    float t = (*value - min) / (max - min);
    float handleX = x + t * width;
    DrawCircle(handleX, y + 2, 8, WHITE);
    DrawText(label, x, y - 18, 12, WHITE);
    DrawText(TextFormat("%.2f", *value), x + width + 10, y - 8, 12, WHITE);

    // interaction code
    // roughly: get mouse position
    // if mouse left button is down AND mouse is inside hit area:
    //     calculate t from mouseX
    //     clamp t to 0..1
    //     write new value
    Vector2 mousePos = GetMousePosition();
    bool hovering = CheckCollisionPointRec(mousePos, (Rectangle){x - 8, y - 8, width + 16, 16});
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && hovering)
    {
        float dragT = (mousePos.x - x) / width;
        if (dragT < 0)
            dragT = 0;
        if (dragT > 1)
            dragT = 1;
        *value = min + dragT * (max - min);
    }
}