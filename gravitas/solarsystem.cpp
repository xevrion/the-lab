#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <time.h>
#include <math.h>
using namespace std;

const float G = 0.5f;
const float EPSILON = 15.0f;

struct Ball
{
    float x, y;   // pos
    float vx, vy; // velocity
    Color color;
    float rad;                               // radius
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

    float impulse = (1.0f + 0.5f) * dot / totalMass;
    // inelastic collision impulse with mass weighting and restitution
    a.vx -= impulse * mb * nx;
    a.vy -= impulse * mb * ny;
    b.vx += impulse * ma * nx;
    b.vy += impulse * ma * ny;
}

int main()
{
    InitWindow(800, 600, "Solar System");
    SetTargetFPS(144);
    srand(time(NULL));
    vector<Ball> balls;

    Ball sun;
    sun.x = 400;
    sun.y = 300;
    sun.vx = 0;
    sun.vy = 0;
    sun.rad = 40;
    sun.color = YELLOW;
    balls.push_back(sun);

    for (int i = 0; i < 9; i++)
    {
        Ball b;
        b.rad = rand() % 8 + 3; // 3 to 10px instead of 10 to 35
        float angle = (rand() % 628) / 100.0f; // 0 to 2π
        float r = 100 + rand() % 200;          // distance from sun: 100 to 300px
        b.x = sun.x + r * cos(angle);
        b.y = sun.y + r * sin(angle);

        // tangential velocity for circular orbit: v = sqrt(G * M / r)
        float dx = b.x - sun.x;
        float dy = b.y - sun.y;
        float dist = sqrt(dx * dx + dy * dy);
        float v = sqrt(G * sun.mass() / dist);
        // set velocity perpendicular to radius vector for circular orbit
        b.vx = -v * dy / dist; // perpendicular direction
        b.vy = v * dx / dist;
        b.color = ColorFromHSV(i * (360 / 10), 1, 1);
        balls.push_back(b);
    }

    while (!WindowShouldClose())
    {
        for (int i = 0; i < 10; i++)
        {
            balls[i].fx = 0;
            balls[i].fy = 0;
        } // we did this to reset the force accumulators before calculating new forces

        // 1st loop , accumulate n body forces
        for (int i = 0; i < 10; i++)
        {
            for (int j = i + 1; j < 10; j++)
            {
                float dx = balls[j].x - balls[i].x;
                float dy = balls[j].y - balls[i].y;
                float dist2 = dx * dx + dy * dy;        // real squared distance
                float distSq = dist2 + EPSILON * EPSILON; // softened, for force magnitude only
                float dist = sqrt(dist2);               // real distance, for normalization only
                if(dist < 0.001f) continue; // prevent singularity and extreme forces  
                float force = G * balls[i].mass() * balls[j].mass() / distSq;
                float fx = force * dx / dist;
                float fy = force * dy / dist;
                balls[i].fx += fx;
                balls[i].fy += fy;
                balls[j].fx -= fx; // Newton's 3rd law
                balls[j].fy -= fy;
            }
        }

        // 2nd -. apply forces to velocities
        for (int i = 0; i < 10; i++)
        {
            // apply accumulated force to velocity here
            // then move: x += vx, y += vy
            if(i == 0) continue; // sun is static for simplicity
            balls[i].vx += balls[i].fx / balls[i].mass();
            balls[i].vy += balls[i].fy / balls[i].mass();
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
        }

        // 3rd -> collisions and speed cap
        for (int i = 0; i < 10; i++)
        {
            // resolveCollision
            for (int j = i + 1; j < 10; j++)
            {
                resolveCollision(balls[i], balls[j]);
            }
            // speed cap
            float curr_speed = sqrt(balls[i].vx * balls[i].vx + balls[i].vy * balls[i].vy);
            if(curr_speed > 50.0f) // cap max speed to prevent tunneling
            {
                float scale = 50.0f / curr_speed;
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
