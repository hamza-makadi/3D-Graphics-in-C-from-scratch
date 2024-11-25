#include <SDL.h>
#include <stdio.h>
#include <math.h>

// Define the screen dimensions
int screenWidth = 800;
int screenHeight = 600;

// Define a 2D vector
typedef struct {
    float x;
    float y;
} Vector2D;

// Define a 3D vector
typedef struct {
    float x;
    float y;
    float z;
} Vector3D;

// Define a triangle
typedef struct {
    Vector3D p[3];
} Triangle;

// Define a mesh
typedef struct{
    int numTris;
    Triangle *tris;
} Mesh;

// Create the triangles for a cube
Triangle cubeTriangles[] = {
        // SOUTH
        { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } } },
        { { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } } },

        // EAST
        { { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } } },
        { { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } } },

        // NORTH
        { { { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } } },
        { { { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } } },

        // WEST
        { { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } } },
        { { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },

        // TOP
        { { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } } },
        { { { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } } },

        // BOTTOM
        { { { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } } },
        { { { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } } },
    };

// Create a Mesh and assign the triangles
Mesh meshCube;

// Define a 4 by 4 matrix struct
typedef struct{
    float m[4][4];
}mat4x4;

// Function to multiply a matrix and a vector
void MultiplyMatrixVector(const Vector3D *i, Vector3D *o, const mat4x4 *m) {
    // Perform the matrix-vector multiplication
    o->x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0];
    o->y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1];
    o->z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2];
    float w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][3];

    // Handle perspective division
    if (w != 0.0f) {
        o->x /= w;
        o->y /= w;
        o->z /= w;
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // Set up the screen (640x480 window, 32-bit color depth)
    SDL_Surface* screen = SDL_SetVideoMode(screenWidth, screenHeight, 32, SDL_SWSURFACE);
    if (!screen) {
        printf("SDL_SetVideoMode failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Set the window title
    SDL_WM_SetCaption("Basic SDL 1.2 Program", NULL);

    // Fill the screen with a blue color (RGB: 0, 0, 255)
    Uint32 blue = SDL_MapRGB(screen->format, 0, 0, 0);

    // assign the triangles of the cube to the mesh
    meshCube.numTris = sizeof(cubeTriangles) / sizeof(Triangle);
    meshCube.tris = cubeTriangles;

    // Projection matrix
    mat4x4 matProj = { 0 }; // All elements initialized to 0

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)screenHeight / (float)screenWidth;
    float fFovRad = 1.0f / tan(fFov * 0.5f / 180.0f * 3.14159f); //Convert from degree to radian

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;


    // Wait for the user to close the window
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }
        SDL_FillRect(screen, NULL, blue);

        // Update the screen
        SDL_Flip(screen);
    }

    // Clean up and quit SDL
    SDL_Quit();
    return 0;
}
