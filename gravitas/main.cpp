#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <time.h>
#include <math.h>
using namespace std;

struct Ball
{
    float x, y;   // pos
    float vx, vy; // velocity
    Color color;
    float rad; // radius
    float restitution; // bounciness
    float mass() const { return rad * rad; } // mass proportional to area
    
    // two force accumulators for gravity
    float fx = 0; // force in x direction
    float fy = 0; // force in y direction
};

void resolveCollision(Ball &a, Ball &b)
{
    float dist = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    if (dist == 0) // to prevent division by zero
        return;
    if (dist >= a.rad + b.rad)
        return;

    // normalized collision normal
    float nx = (a.x - b.x) / dist;
    float ny = (a.y - b.y) / dist;

    // separate overlapping balls proportional to mass (r^2)
    float overlap = a.rad + b.rad - dist;
    float ma = a.rad * a.rad;
    float mb = b.rad * b.rad;
    float totalMass = ma + mb;
    a.x += overlap * (mb / totalMass) * nx;
    a.y += overlap * (mb / totalMass) * ny;
    b.x -= overlap * (ma / totalMass) * nx;
    b.y -= overlap * (ma / totalMass) * ny;

    // relative velocity along the normal
    float dvx = a.vx - b.vx;
    float dvy = a.vy - b.vy;
    float dot = dvx * nx + dvy * ny;
    if (dot >= 0) // already separating, skip impulse
        return;

    // elastic collision impulse with mass weighting
    // float impulse = (2.0f * dot) / totalMass;
    
    // inelastic collision impulse with mass weighting and restitution
    float e = (a.restitution + b.restitution) / 2.0f;
    float impulse = (1.0f + e) * dot / totalMass;
    a.vx -= impulse * mb * nx;
    a.vy -= impulse * mb * ny;
    b.vx += impulse * ma * nx;
    b.vy += impulse * ma * ny;
}

int main()
{
    InitWindow(800, 600, "Gravitas");
    SetTargetFPS(144);
    srand(time(NULL));
    vector<Ball> balls;

    for (int i = 0; i < 10; i++)
    {
        Ball b;
        b.x = rand() % 800; // random
        b.y = rand() % 300;
        b.vx = (rand() % 7) - 3; // random
        b.vy = 0;
        b.rad = rand() % 25 + 10;
        b.color = ColorFromHSV(i * (360 / 10), 1, 1);
        b.restitution =   0.5f + (rand() % 50) / 100.0f;
        balls.push_back(b);
    }

    while (!WindowShouldClose())
    {
        for(int i = 0; i < 10; i++)
        {
            balls[i].fx = 0;
            balls[i].fy = 0;
        } // we did this to reset the force accumulators before calculating new forces


        for (int i = 0; i < 10; i++)
        {
            balls[i].vy += 0.5, balls[i].y += balls[i].vy;
            balls[i].x += balls[i].vx;
            float r = balls[i].rad;
            if (balls[i].x - r < 0 || balls[i].x + r > 800)
            {
                
                if (balls[i].x - r < 0)
                    balls[i].x = r;
                else
                    balls[i].x = 800 - r;
                balls[i].vx = -balls[i].vx * 0.8; // restitution 0.8, loses 20% on bounce
                balls[i].vy *= 0.98f; // friction
            }
            if (balls[i].y - r < 0 || balls[i].y + r > 600)
            {   
                if (balls[i].y - r < 0)
                balls[i].y = r;
                else
                balls[i].y = 600 - r;
                balls[i].vy = -balls[i].vy * 0.8; // restitution 0.8, loses 20% on bounce
                balls[i].vx *= 0.98f; // friction
            }
            for (int j = i + 1; j < 10; j++)
            {
                resolveCollision(balls[i], balls[j]);
            }
            float curr_speed = sqrt(balls[i].vx * balls[i].vx + balls[i].vy * balls[i].vy);
            if (curr_speed < 0.1f)
            {
                balls[i].vx = 0;
                balls[i].vy = 0;
            }
            if(curr_speed > 15.0f) // cap max speed to prevent tunneling
            {
                float scale = 15.0f / curr_speed;
                balls[i].vx *= scale;
                balls[i].vy *= scale;
            }
        }
        BeginDrawing();
        // ClearBackground(BLACK);
        // to get a trailing effect
        DrawRectangle(0, 0, 800, 600, (Color){0, 0, 0, 25});
        // DrawCircle(400, y, 20, WHITE); // the center is at 580 when it hits the ground :)
        for (int i = 0; i < 10; i++)
        {
            DrawCircle(balls[i].x, balls[i].y, balls[i].rad, balls[i].color);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


// now research for the gravitational force attraction part thingie:
// standard order T_T
// 1. accumulate all forces -> update velocities
// 2. update positions using new velocities
// 3. resolve collisions (they correct positions and velocities)
// 4. draw

// and we'll also face the singularity problem , in which in F = G * m1 * m2 / r^2, if r is very small, F becomes huge and causes instability. To prevent this, we can add a small softening factor to the denominator, like F = G * m1 * m2 / (r^2 + epsilon^2), where epsilon is a small constant. This way, when r is close to zero, the force will not become infinite and will remain manageable.
// epsillon is the softening length.
// When balls are far apart: r² is large, ε² is negligible. Formula behaves exactly like real gravity. No difference.
// When balls are close: r² shrinks toward zero, but ε² is still there. The denominator never reaches zero. Force stays bounded.


