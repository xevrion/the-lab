#include "raylib.h"

int main() {
    InitWindow(800, 600, "Gravitas");
    SetTargetFPS(60);

    float y = 300, vy = 0, gravity = 0.5;

    while (!WindowShouldClose()) {
        vy += gravity;
        y += vy;
        if(y > 580) {
            y = 580;
            vy = -vy * 0.8; // restitution number is 0.8, meaning it loses 20% of its velocity on each bounce
        }
        BeginDrawing();
            ClearBackground(BLACK);
            DrawCircle(400, y, 20, WHITE); // the center is at 580 when it hits the ground :)
        EndDrawing();
    }

    CloseWindow();
    return 0;
}