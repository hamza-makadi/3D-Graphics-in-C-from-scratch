#include <SDL.h>
#include <SDL_gfxPrimitives.h>
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
void MultiplyMatrixVector(Vector3D *i, Vector3D *o, mat4x4 *m) {
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


// Function to draw a triangle
void DrawTriangle(SDL_Surface* screen, int x1, int y1, int x2, int y2, int x3, int y3){
    lineRGBA(screen, x1, y1, x2, y2, 255, 255, 255, 255);
    lineRGBA(screen, x2, y2, x3, y3, 255, 255, 255, 255);
    lineRGBA(screen, x3, y3, x1, y1, 255, 255, 255, 255);
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

    freopen("CON", "w", stdout);  // redirects stdout to console
    freopen("CON", "w", stderr);  // redirects stderr to console

    // Set the window title
    SDL_WM_SetCaption("Basic SDL 1.2 Program", NULL);

    // Fill the screen with a blue color (RGB: 0, 0, 255)
    Uint32 blue = SDL_MapRGB(screen->format, 0, 0, 0);

    // assign the triangles of the cube to the mesh
    meshCube.numTris = sizeof(cubeTriangles) / sizeof(Triangle);
    meshCube.tris = cubeTriangles;

    // Matrix Transformers
    // Projection matrix
    mat4x4 matProj = { 0 }; // All elements initialized to 0
    // Rotation Matrix
    mat4x4 matRotZ;
    mat4x4 matRotX;

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


    Uint32 lastTime = SDL_GetTicks();
    float fTheta = 0.0f;
    // Wait for the user to close the window
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // ms to seconds
        lastTime = currentTime;

        SDL_FillRect(screen, NULL, blue);
        mat4x4 matRotZ, matRotX;
		fTheta += deltaTime;

        // Rotation Z
        matRotZ.m[0][0] = cos(fTheta);
        matRotZ.m[0][1] = sin(fTheta);
        matRotZ.m[1][0] = -sin(fTheta);
        matRotZ.m[1][1] = cos(fTheta);
        matRotZ.m[2][2] = 1.0f;
        matRotZ.m[3][3] = 1.0f;

        // Rotation X
        matRotX.m[0][0] = 1.0f;
        matRotX.m[1][1] = cos(fTheta * 0.5f);
        matRotX.m[1][2] = sin(fTheta * 0.5f);
        matRotX.m[2][1] = -sin(fTheta * 0.5f);
        matRotX.m[2][2] = cos(fTheta * 0.5f);
        matRotX.m[3][3] = 1.0f;
        int i;
        for(i = 0; i < meshCube.numTris; i++){
            Triangle triPojected, triTranslated, triRotatedZ, triRotatedZX;

            MultiplyMatrixVector(&meshCube.tris[i].p[0], &triRotatedZ.p[0], &matRotZ);
            MultiplyMatrixVector(&meshCube.tris[i].p[1], &triRotatedZ.p[1], &matRotZ);
            MultiplyMatrixVector(&meshCube.tris[i].p[2], &triRotatedZ.p[2], &matRotZ);

            MultiplyMatrixVector(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
            MultiplyMatrixVector(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
            MultiplyMatrixVector(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

            // Translate the cube in the z-axis
            triTranslated = triRotatedZX ;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;


            MultiplyMatrixVector(&triTranslated.p[0], &triPojected.p[0], &matProj);
            MultiplyMatrixVector(&triTranslated.p[1], &triPojected.p[1], &matProj);
            MultiplyMatrixVector(&triTranslated.p[2], &triPojected.p[2], &matProj);

            // Scale and center coordinates to screen space
            triPojected.p[0].x = (triPojected.p[0].x + 1.0f) * 0.5f * screenWidth;
            triPojected.p[0].y = (triPojected.p[0].y + 1.0f) * 0.5f * screenHeight;

            triPojected.p[1].x = (triPojected.p[1].x + 1.0f) * 0.5f * screenWidth;
            triPojected.p[1].y = (triPojected.p[1].y + 1.0f) * 0.5f * screenHeight;

            triPojected.p[2].x = (triPojected.p[2].x + 1.0f) * 0.5f * screenWidth;
            triPojected.p[2].y = (triPojected.p[2].y + 1.0f) * 0.5f * screenHeight;




            DrawTriangle(screen, triPojected.p[0].x, triPojected.p[0].y,
                                triPojected.p[1].x, triPojected.p[1].y,
                                triPojected.p[2].x, triPojected.p[2].y
                         );
        }
        // Update the screen
        SDL_Flip(screen);
    }

    // Clean up and quit SDL
    SDL_Quit();
    return 0;
}
