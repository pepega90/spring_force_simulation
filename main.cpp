#include <raylib.h>
#include <math.h>
#include <vector>
#include <iostream>

const int WIDTH = 840;
const int HEIGHT = 480;

struct Particle
{
    Vector2 pos;
    Vector2 acc;
    Vector2 vel;
    Vector2 sumForces;
    float mass;
    float radius;

    void AddForce(const Vector2 &force)
    {
        sumForces += force;
    };
    void ClearForces()
    {
        sumForces = Vector2(0.0, 0.0);
    };

    void Integrate(float dt)
    {

        acc = sumForces * mass;

        vel += acc * dt;

        pos += vel * dt;

        ClearForces();
    };
};

Vector2 GenerateDragForce(const Particle &particle, float k)
{
    Vector2 dragForce = Vector2(0, 0);
    if (particle.vel.MagnitudeSquared() > 0)
    {

        Vector2 dragDirection = particle.vel.UnitVector() * -1.0;

        float dragMagnitude = k * particle.vel.MagnitudeSquared();

        dragForce = dragDirection * dragMagnitude;
    }
    return dragForce;
}

Vector2 GenerateSpringForce(Particle &particle, Vector2 &anchor, float restLength, float k)
{
    Vector2 d = particle.pos - anchor;

    float displacement = d.Magnitude() - restLength;

    Vector2 springDirection = d.UnitVector();
    float springMagnitude = -k * displacement;

    Vector2 springForce = springDirection * springMagnitude;
    return springForce;
};

int main()
{
    InitWindow(WIDTH, HEIGHT, "Spring Force");
    SetTargetFPS(60);

    Vector2 anchor = Vector2(GetScreenWidth() / 2.0f, 20);

    std::vector<Particle> particles;
    int NUM_PARTICLES = 5;

    float k = 100;
    float restLength = 50;
    bool press = false;
    Vector2 mousePos;
    int lastParticle = NUM_PARTICLES - 1;

    for (int i = 0; i < NUM_PARTICLES; i++)
    {
        Particle particle;
        particle.pos = Vector2(anchor.x, anchor.y + (i * restLength));
        particle.mass = 2.0;
        particle.radius = 10;
        particles.push_back(particle);
    }

    while (!WindowShouldClose())
    {

        ClearBackground(BLACK);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            mousePos = GetMousePosition();
            press = true;
        }

        if (IsMouseButtonUp(MOUSE_BUTTON_LEFT) && press)
        {
            press = false;
            Vector2 impulseDir = (particles[lastParticle].pos - mousePos).UnitVector();
            float impulseMagnitude = (particles[lastParticle].pos - mousePos).Magnitude() * 20.0;
            particles[lastParticle].vel = impulseDir * impulseMagnitude;
        }

        float dt = GetFrameTime();

        for (auto &particle : particles)
        {
            Vector2 drag = GenerateDragForce(particle, 0.001);
            particle.AddForce(drag);

            particle.AddForce(Vector2(0.0, particle.mass * 9.8 * 50));
        }

        Vector2 spring = GenerateSpringForce(particles[0], anchor, restLength, k);
        particles[0].AddForce(spring);

        for (int i = 1; i < NUM_PARTICLES; i++)
        {
            Vector2 spring = GenerateSpringForce(particles[i], particles[i - 1].pos, restLength, k);
            particles[i].AddForce(spring);
            particles[i - 1].AddForce(-spring);
        }

        for (auto &particle : particles)
        {
            particle.Integrate(dt);
        }

        BeginDrawing();

        if (press)
        {
            DrawLineV(particles[lastParticle].pos, mousePos, YELLOW);
        }

        DrawCircleV(anchor, 5.0, RED);
        DrawLineEx(anchor, particles[0].pos, 3.0, RED);

        for (int i = 0; i < NUM_PARTICLES - 1; i++)
        {
            DrawLineEx(particles[i].pos, particles[i + 1].pos, 3.0, RED);
        }

        for (auto &particle : particles)
        {
            DrawCircleV(particle.pos, particle.radius, WHITE);
        }

        EndDrawing();
    }
    CloseWindow();

    return 0;
}