

#include "raylib.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    Camera3D cam = { 0 };
    cam.fovy = 45.0f;
    cam.position = Vector3{0.0f, 0.0f, 0.0f};
    cam.target = Vector3{0.0f, 1.0f, 0.0f};
    cam.projection = CAMERA_FREE;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60); 
    while (!WindowShouldClose())   
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

            BeginMode3D(cam);
            DrawCube({0.0f, 0.0f, 0.0f}, 1,1,1,GREEN);
            EndMode3D();

        DrawFPS(20,20);
        EndDrawing();
       
    }
    CloseWindow(); 

    return 0;
}