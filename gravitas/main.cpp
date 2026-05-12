#include "raylib.h"
#include <vector>
#include <cstdlib>
#include<time.h>
using namespace std;

struct Ball {
    float x, y; // pos
    float vx, vy; // velocity
    Color color;
    float rad; // radius
};
    
int main() {
    InitWindow(800, 600, "Gravitas");
    SetTargetFPS(60);
    srand(time(NULL));  
    vector<Ball> balls;

    for(int i = 0; i < 10; i++) {
    Ball b;
    b.x = rand() % 800; // random
    b.y = rand()%300;
    b.vx = (rand() % 7) - 3;// random
    b.vy = 0;
    b.rad = rand()%25;
    b.color = ColorFromHSV(i * (360 / 10), 1, 1);
    balls.push_back(b);
}

    
    
    
    while (!WindowShouldClose()) {
        for(int i = 0; i<10; i++){
            balls[i].vy += 0.5, balls[i].y += balls[i].vy;
            if(balls[i].y > 580) {
                balls[i].y = 580;
                balls[i].vy = -balls[i].vy * 0.8; // restitution number is 0.8, meaning it loses 20% of its velocity on each bounce
            }
        }
        BeginDrawing();
            // ClearBackground(BLACK);
            // to get a trailing effect
            DrawRectangle(0,0 ,800, 600, (Color){0,0,0,25});
            // DrawCircle(400, y, 20, WHITE); // the center is at 580 when it hits the ground :)
            for(int i = 0; i<10; i++){
                DrawCircle(balls[i].x, balls[i].y, balls[i].rad, balls[i].color);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}