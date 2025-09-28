#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <cmath>

#define msize 10

float heights[msize][msize];

void Perlin(int i, float pp){
    float tmp[msize][msize] = {0};
    for(int z = i; z < msize-i;z++){
        for(int x = i; x < msize-i;x++){
            tmp[z][x] = heights[z-1][x-1]   + heights[z][x-1]   + heights[z+1][x-1] +
                        heights[z-1][x]     + heights[z][x]     + heights[z+1][x] +
                        heights[z-1][x+1]   + heights[z][x+1]   + heights[z+1][x+1];
            tmp[z][x] = tmp[z][x]/9.0f;
        }
    }
    for(int z = 0; z < msize;z++){
        for(int x = 0; x < msize;x++){
            heights[z][x] = powf(tmp[z][x], pp);
        }
    }
}

// pick a color based on height
Color GetHeightColor(int h) {
    if (h < 5) return BLUE;         // water
    else if (h < 15) return GREEN;  // grass
    else if (h < 25) return BROWN;  // hills
    else return RAYWHITE;           // snow
}

Mesh GenerateHeightMesh(int width, int height, float data[msize][msize])
{
    Mesh mesh = {0};
    mesh.triangleCount = (width - 1) * (height - 1) * 2;
    mesh.vertexCount   = mesh.triangleCount * 3;

    mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float)); // x,y,z
    mesh.normals  = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float)); // nx,ny,nz
    mesh.colors   = (unsigned char*)MemAlloc(mesh.vertexCount * 4 * sizeof(unsigned char)); // r,g,b,a

    int vIdx = 0, nIdx = 0, cIdx = 0;

    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            Vector3 v0 = { (float)x,     (float)data[x][y],     (float)y     };
            Vector3 v1 = { (float)(x+1), (float)data[x+1][y],   (float)y     };
            Vector3 v2 = { (float)(x+1), (float)data[x+1][y+1], (float)(y+1) };
            Vector3 v3 = { (float)x,     (float)data[x][y+1],   (float)(y+1) };

            // ---- Use winding that is CCW when viewed from +Y (top) ----
            // Triangle A: v0, v2, v1
            // Triangle B: v0, v3, v2
            Vector3 tris[6] = { v0, v2, v1,  v0, v3, v2 };

            // process two triangles
            for (int tri = 0; tri < 2; ++tri) {
                Vector3 a = tris[tri*3 + 0];
                Vector3 b = tris[tri*3 + 1];
                Vector3 c = tris[tri*3 + 2];

                // edges
                Vector3 e1 = { b.x - a.x, b.y - a.y, b.z - a.z };
                Vector3 e2 = { c.x - a.x, c.y - a.y, c.z - a.z };

                // normal = cross(e1, e2)
                Vector3 normal = {
                    e1.y * e2.z - e1.z * e2.y,
                    e1.z * e2.x - e1.x * e2.z,
                    e1.x * e2.y - e1.y * e2.x
                };

                // normalize
                float len = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
                if (len > 1e-6f) {
                    normal.x /= len; normal.y /= len; normal.z /= len;
                }

                // safety: if normal points downward, flip it
                if (normal.y < 0.0f) {
                    normal.x = -normal.x;
                    normal.y = -normal.y;
                    normal.z = -normal.z;
                }

                // write 3 verts for this triangle
                for (int v = 0; v < 3; ++v) {
                    Vector3 vert = tris[tri*3 + v];

                    // vertex
                    mesh.vertices[vIdx++] = vert.x;
                    mesh.vertices[vIdx++] = vert.y;
                    mesh.vertices[vIdx++] = vert.z;

                    // normal (flat shading: same normal for all 3 verts)
                    mesh.normals[nIdx++] = normal.x;
                    mesh.normals[nIdx++] = normal.y;
                    mesh.normals[nIdx++] = normal.z;

                    // color (example: simple height-based)
                    Color col = GetHeightColor((int)vert.y);
                    mesh.colors[cIdx++] = col.r;
                    mesh.colors[cIdx++] = col.g;
                    mesh.colors[cIdx++] = col.b;
                    mesh.colors[cIdx++] = col.a;
                }
            }
        }
    }

    UploadMesh(&mesh, false);
    return mesh;
}

int main(void) {
    InitWindow(800, 600, "Raylib Heightmap Mesh");
    SetTargetFPS(200);
    DisableCursor();

    srand(time(NULL));
    for (int x = 0; x < msize; x++) {
        for (int y = 0; y < msize; y++) {
            heights[x][y] = (rand() % 50)/50.0f; // random heights 0–29
        }
    }
    //for(int i = 0; i < 4; i++) Perlin(4, 0.8f);

    Mesh mesh = GenerateHeightMesh(msize, msize, heights);
    Model model = LoadModelFromMesh(mesh);

    mesh.vertices[1] = 25.0f;
    UpdateMeshBuffer(mesh, 0, mesh.vertices, mesh.vertexCount*3*sizeof(float), 0);

    Shader shader = LoadShader("terrain.vs", "terrain.fs");
    int lightDirLoc = GetShaderLocation(shader, "lightDir");
    int lightColorLoc = GetShaderLocation(shader, "lightColor");
    int ambientLoc = GetShaderLocation(shader, "ambient");

    // Example sun
    Vector3 sunDir = { -0.3f, -1.0f, -0.2f };
    Vector3 sunColor = { 1.0f, 0.95f, 0.8f };
    Vector3 ambient = { 0.2f, 0.25f, 0.3f };

    SetShaderValue(shader, lightDirLoc, &sunDir, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, lightColorLoc, &sunColor, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC3);

    model.materials[0].shader = shader;

    Camera3D camera = {0};
    camera.position = (Vector3){ float(msize/2), 5.0f, float(msize/2) };
    camera.target = (Vector3){ 25.0f, 0.0f, 25.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 90.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(BLACK);
       
        DrawCircleGradient(GetScreenWidth()/2, (camera.target.y)*(GetScreenHeight())/4, 8000, Color{175,175,255,255},BLUE );    // SKY

        BeginMode3D(camera);
        DrawSphere(Vector3{0.0f, 100.0f,0.0f}, 5.0f, YELLOW);
        DrawModel(model, (Vector3){0,0,0}, 1.0f, WHITE);
        EndMode3D();

        DrawFPS(20,20);
        EndDrawing();
    }

    UnloadModel(model);
    CloseWindow();
    return 0;
}
