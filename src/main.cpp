// Author: Ross Adamson
//
// The dharma-world program lets the user navigate a 3d scene with the keyboard or game controller.
//

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include "RzColor3f.h"
#include "Debugger.h"
#include "Utils.h"
#include "matrixmath.h"
#include "floor.h"
#include "wall1.h"
#include "wall2.h"
#include "wall3.h"
#include "wall4.h"
#include "ceiling.h"
#include "gltypes.h"
#include "dharmacube.h"

using namespace std;

#define DEFAULT_WINDOW_WIDTH 700
#define DEFAULT_WINDOW_HEIGHT 700

// Game metrics
#define UNIT_PER_METER 1.0f

#define METER_TO_UNIT(m) ((float)m * UNIT_PER_METER)
#define UNIT_TO_METER(u) ((float)u / UNIT_PER_METER)
#define MS_TO_SEC(ms) ((float)ms / 1000.0f)

#define PLAYER_EYE_HEIGHT 1.0f // meters

#define PLAYER_WALK_SPEED 1.0f // meters per second
#define PLAYER_MAX_SPEED 3.0f // meters per second
#define PLAYER_MIN_SPEED 0.0f // meters per second
#define SLOW_TURN_SPEED 72.0f // degrees per second
#define MAX_TURN_SPEED 180.0f // degrees per second
#define MIN_TURN_SPEED 0.0f // degrees per second

bool fullscreen = false;

float player_move_speed = 0; // meters per second
float player_turn_speed = 0; // degrees per second

float dharma_turn_speed = 25.0f; // degrees per second
float dharma_rotation = 0.0; // angle of dharma cube

// Game controller thresholds
int jaxis_0_min_neg = -3200;
int jaxis_0_min_pos = -jaxis_0_min_neg;
int jaxis_1_rest_min = jaxis_0_min_neg;
int jaxis_1_rest_max = jaxis_0_min_pos;

int jaxis_0_max_neg = -32768;
int jaxis_0_max_pos = -jaxis_0_max_neg;
int jaxis_1_max_neg = jaxis_0_max_neg;
int jaxis_1_max_pos = jaxis_0_max_pos;

Uint32 loop_duration, current_ticks, loop_start;

// Bird sound audio data
Uint8 *bird_buffer = NULL;
// Position of the playing bird sound within the buffer
Uint8 *bird_position = NULL;
// Length of the bird sound
Uint32 bird_length = 0;

// Texture handles
GLuint floor_texture;
GLuint dharma_texture;

// Keydown booleans
bool key[321];

typedef struct LINE_SEGMENT_3D {
    VERTEX_DATA_3D data1;
    VERTEX_DATA_3D data2;
} LINE_SEGMENT_3D, *LINE_SEGMENT_3D_PTR;

// Wall boundaries for checking collisions
LINE_SEGMENT_3D boundaries[] = {
        // wall1
        { wall1VertexData[9], wall1VertexData[11] },
        { wall1VertexData[12], wall1VertexData[17] },
        // wall2
        { wall2VertexData[0], wall2VertexData[1] },
        // wall3
        { wall3VertexData[9], wall3VertexData[11] },
        { wall3VertexData[12], wall3VertexData[17] },
        // wall4
        { wall4VertexData[0], wall4VertexData[1] },
};
#define numberOfBoundaries 6

int window_width = DEFAULT_WINDOW_WIDTH;
int window_height = DEFAULT_WINDOW_HEIGHT;

// Light source details
POINT4D light_pos = {METER_TO_UNIT(0), METER_TO_UNIT(5), METER_TO_UNIT(0), 1.0f};
RzColor3f light_color;
RzColor3f ambient_light;
RzColor3f specular_highlight;

int quitButton = 0;

// Camera details
POINT3D camera_position = {METER_TO_UNIT(0), METER_TO_UNIT(PLAYER_EYE_HEIGHT), METER_TO_UNIT(0)};
POINT3D camera_target = {METER_TO_UNIT(0), METER_TO_UNIT(PLAYER_EYE_HEIGHT), METER_TO_UNIT(-1)};
VECTOR3D camera_up = {0.0f, 1.0f, 0.0f};
float near_z = 0.1;
float far_z = 100;
float field_of_view = 45; // degrees

// v1 will point to what v2 used to point to and vice versa.
void swapPointers(void **v1, void **v2) {
    void *tmp_v = *v1;
    *v1 = *v2;
    *v2 = tmp_v;
}

// Play a sound when user collides with wall boundary
void doCollision() {
    Debugger::getInstance().print("wall collision\n");
    
    SDL_LockAudio();
    
    // Play the bird sound if it isn't already playing
    if (bird_position == NULL) {
        bird_position = bird_buffer;
    }

    SDL_UnlockAudio();
}

// Reshape the window when it's moved or resized
void reshapeGL(int width, int height) {
    // Reset the current viewport
    glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));

    // Select the projection matrix
    glMatrixMode(GL_PROJECTION);

    // Reset the projection matrix
    glLoadIdentity();

    // Calculate the aspect ratio of the window
    gluPerspective(field_of_view, (GLfloat)width/(GLfloat)height, near_z, far_z);

    // Select the modelview matrix
    glMatrixMode(GL_MODELVIEW);

    // reset the modelview matrix
    glLoadIdentity();

    window_width = width;
    window_height = height;
}

// Process audio
// @param userdata Special data passed to the callback.
// @param stream The audio stream pointer.
// @param len The length of audio data to read during this call.
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
    // Continue playing the bird sound if it has started
    if (bird_position != NULL) {
        Uint32 length_to_go = bird_buffer + bird_length - bird_position;

        if (length_to_go == 0) {
            bird_position = NULL;
        } else {
            // Mix as much data as possible
            len = (len > length_to_go ? length_to_go : len);
            SDL_MixAudio(stream, bird_position, len, SDL_MIX_MAXVOLUME);
            bird_position += len;
        }
    }
}

// Rotate the camera target to the right (negative angle) or left (positive angle)
// @param my_angle The angle to rotate in radians
void rotateView(float my_angle) {
    POINT4D target, result_target;
    MATRIX4X4 transform_matrix, temp_matrix, result_4x4;
    float cos_rot, sin_rot;

    // Setup the target point
    VECTOR4D_INITXYZ(&target, camera_target.x, camera_target.y, camera_target.z);

    // Create the transform matrix, beginning by translating to the origin
    Mat_Init_4X4(&transform_matrix,
            1, 0, 0, -camera_position.x,
            0, 1, 0, -camera_position.y,
            0, 0, 1, -camera_position.z,
            0, 0, 0, 1);

    cos_rot = cos(my_angle);
    sin_rot = sin(my_angle);

    // Rotate around origin
    Mat_Init_4X4(&temp_matrix,
        cos_rot, 0, sin_rot, 0,
        0, 1, 0, 0,
        -sin_rot, 0, cos_rot, 0,
        0, 0, 0, 1);

    Mat_Mul_4X4(&temp_matrix, &transform_matrix, &result_4x4);
    MAT_COPY_4X4(&result_4x4, &transform_matrix);

    // Translate back to original position
    Mat_Init_4X4(&temp_matrix,
            1, 0, 0, camera_position.x,
            0, 1, 0, camera_position.y,
            0, 0, 1, camera_position.z,
            0, 0, 0, 1);

    Mat_Mul_4X4_VECTOR4D(&transform_matrix, &target, &result_target);

    // Set the camera target position to the new location
    VECTOR3D_INITXYZ(&camera_target, result_target.x, METER_TO_UNIT(PLAYER_EYE_HEIGHT), result_target.z);

    // Make sure the vector from camera to target is normal (doesn't get too big or small)
    VECTOR3D target_vector = { camera_target.x - camera_position.x,
            camera_target.y - camera_position.y,
            camera_target.z - camera_position.z };

    VECTOR3D_Normalize(&target_vector);
    VECTOR3D_Add(&target_vector, &camera_target, &camera_target);
}

// Move camera by amount
// @param amount Amount to move (in game units). Position is forward, negative is backward.
void moveByAmount(float amount) {
    // Get the camera target vector
    VECTOR3D target_vector = { camera_target.x - camera_position.x,
            camera_target.y - camera_position.y,
            camera_target.z - camera_position.z };

    VECTOR3D_Normalize(&target_vector);

    // Create the safe vector for determining collision. This movement
    // vector is a little longer than the actual movement vector to prevent
    // accidental penetration of boundaries do to math errors.
    VECTOR3D safe_vector;
    float safe_amount = METER_TO_UNIT(0.2);
    if (amount < 0) {
        safe_amount = -safe_amount;
    }

    VECTOR3D_Scale(amount + safe_amount, &target_vector, &safe_vector);

    // Calculate the new position
    VECTOR3D new_position;
    VECTOR3D_Add(&safe_vector, &camera_position, &new_position);

    // Figure out if we're crossing an important boundary
    for (int i = 0; i < numberOfBoundaries; ++i) {
        float temp_float;

        GLfloat *x_min_vertex = boundaries[i].data1.vertex;
        GLfloat *x_max_vertex = boundaries[i].data2.vertex;
        // Sort vertices by x value
        if (x_min_vertex[0] > x_max_vertex[0]) {
            swapPointers((void**)&x_min_vertex, (void**)&x_max_vertex);
        }
        float x_min_boundary = x_min_vertex[0];
        float x_max_boundary = x_max_vertex[0];
        // If difference in x values is zero, nudge one vertex a little
        if (x_min_boundary == x_max_boundary) {
            x_max_boundary += EPSILON_E3;
        }
        // Calculate slope of boundary line (z2 - z1)/(x2 - x1)
        float m_boundary = (x_max_vertex[2] - x_min_vertex[2]) /
                (x_max_boundary - x_min_boundary);
        // Calculate z intersect
        float b_boundary = (x_max_vertex[2]
                - (m_boundary * x_max_boundary));

        // Calculate movement vector
        float x_min_movement = camera_position.x;
        float x_max_movement = new_position.x;
        if (x_min_movement > x_max_movement) {
            SWAP(x_min_movement, x_max_movement, temp_float);
        }
        // Calculate movement slope and intersect
        float m_movement = (safe_vector.z / safe_vector.x);
        float b_movement = (new_position.z - m_movement * new_position.x);

        // Calculate intersection of movement line and boundary line
        float x_intersect = ((b_movement - b_boundary) / (m_boundary - m_movement));

        // Check if the movement segment and boundary intersect
        if (x_intersect >= x_min_movement && x_intersect <= x_max_movement &&
                x_intersect >= x_min_boundary && x_intersect <= x_max_boundary) {
                // Housten, we have a collision
            doCollision();
            
            // Don't actually perform the movement
            return;
        }
    }

    // Calculate the movement vector
    VECTOR3D_Scale(amount, &target_vector);

    // Make the move
    VECTOR3D_Add(&target_vector, &camera_position, &camera_position);
    VECTOR3D_Add(&target_vector, &camera_target, &camera_target);

    // Keep the camera from drifting vertically
    camera_position.y = METER_TO_UNIT(PLAYER_EYE_HEIGHT);
    camera_target.y = METER_TO_UNIT(PLAYER_EYE_HEIGHT);
}

// The main game loop.
void main_loop_function() {
    loop_start = SDL_GetTicks();

    SDL_Event event;

    while(true) {
        // Check for SDL event
        if (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    key[event.key.keysym.sym] = true;
                    switch(event.key.keysym.sym) {
                        case SDLK_UP:
                            player_move_speed = PLAYER_WALK_SPEED;
                            break;
                        case SDLK_DOWN:
                            player_move_speed = -PLAYER_WALK_SPEED;
                            break;
                        case SDLK_RIGHT:
                            player_turn_speed = -SLOW_TURN_SPEED;
                            break;
                        case SDLK_LEFT:
                            player_turn_speed = SLOW_TURN_SPEED;
                            break;
                        case SDLK_ESCAPE:
                            return;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    key[event.key.keysym.sym] = false;
                    switch(event.key.keysym.sym) {
                        case SDLK_UP:
                            player_move_speed = 0;
                            break;
                        case SDLK_DOWN:
                            player_move_speed = 0;
                            break;
                        case SDLK_RIGHT:
                            player_turn_speed = 0;
                            break;
                        case SDLK_LEFT:
                            player_turn_speed = 0;
                            break;
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    if (event.jaxis.axis == 0) {
                        // Move the camera angle left or right
                        if (event.jaxis.value > jaxis_0_min_pos) {
                                // Right motion
                            if (event.jaxis.value > jaxis_0_max_pos) {
                                player_turn_speed = -MAX_TURN_SPEED;
                            } else {
                                player_turn_speed = -((float)(event.jaxis.value - jaxis_0_min_pos) / (float)(jaxis_0_max_pos - jaxis_0_min_pos) *
                                    (float)(MAX_TURN_SPEED - MIN_TURN_SPEED));
                            }
                        } else if (event.jaxis.value >= jaxis_0_min_neg) {
                                // At rest
                            player_turn_speed = 0;
                        } else {
                                // Right motion
                            if (event.jaxis.value < jaxis_0_max_neg) {
                                player_turn_speed = MAX_TURN_SPEED;
                            } else {
                                player_turn_speed = ((float)(event.jaxis.value - jaxis_0_min_neg) /
                                        (float)(jaxis_0_max_neg - jaxis_0_min_neg) *
                                        (float)(MAX_TURN_SPEED - MIN_TURN_SPEED));

                            }
                        }
                    } else if (event.jaxis.axis == 1) {
                        // Move the camera forward or backward
                        if (event.jaxis.value > jaxis_1_rest_max) {
                                // Backward motion
                            if (event.jaxis.value > jaxis_1_max_pos) {
                                player_move_speed = -PLAYER_MAX_SPEED;
                            } else {
                                player_move_speed = -((float)(event.jaxis.value - jaxis_1_rest_max) / (float)(jaxis_1_max_pos - jaxis_1_rest_max) *
                                    (float)(PLAYER_MAX_SPEED - PLAYER_MIN_SPEED));
                            }
                        } else if (event.jaxis.value >= jaxis_1_rest_min) {
                                // At rest
                            player_move_speed = 0;
                        } else {
                                // Forward motion
                            if (event.jaxis.value < jaxis_1_max_neg) {
                                player_move_speed = PLAYER_MAX_SPEED;
                            } else {
                                player_move_speed = ((float)(event.jaxis.value - jaxis_1_rest_min) /
                                        (float)(jaxis_1_max_neg - jaxis_1_rest_min) *
                                        (float)(PLAYER_MAX_SPEED - PLAYER_MIN_SPEED));
                            }
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    if (event.jbutton.button == 0) {
                    }
                    break;
                case SDL_JOYBUTTONUP:
                    if (event.jbutton.button == quitButton) {
                        return;
                    }
                    break;
                case SDL_JOYHATMOTION:
                    break;
                case SDL_QUIT:
                    return;
                case SDL_VIDEORESIZE:
                default:
                    break;
            }
        }

        // Rotate view if turning
        if (player_turn_speed != 0) {
            rotateView(DEG_TO_RAD(player_turn_speed * MS_TO_SEC(loop_duration)));
        }

        // Move camera if advancing or retreating
        if (player_move_speed != 0) {
            moveByAmount(METER_TO_UNIT(player_move_speed * MS_TO_SEC(loop_duration)));
        }

        // Rotate dharma cubes
        dharma_rotation += (dharma_turn_speed * MS_TO_SEC(loop_duration));

        // Reset the loop timers
        current_ticks = SDL_GetTicks();
        loop_duration = current_ticks - loop_start;
        loop_start = current_ticks;

        // Draw the scene
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera_position.x, camera_position.y, camera_position.z,
                camera_target.x, camera_target.y, camera_target.z,
                camera_up.x, camera_up.y, camera_up.z);

        // Add objects to the scene
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        // Floor
        glBindTexture(GL_TEXTURE_2D, floor_texture);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &floorVertexData[0].vertex);
        glNormalPointer(GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &floorVertexData[0].normal);
        glTexCoordPointer(2, GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &floorVertexData[0].texCoord);
        glDrawArrays(GL_TRIANGLES, 0, floorNumberOfVertices);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        // Wall1
        glColor3f(0.5, 0.0, 0.0);
        glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall1VertexData[0].vertex);
        glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall1VertexData[0].normal);
        glDrawArrays(GL_TRIANGLES, 0, wall1NumberOfVertices);

        // Wall2
        glColor3f(0.0, 0.5, 0.0);
        glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall2VertexData[0].vertex);
        glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall2VertexData[0].normal);
        glDrawArrays(GL_TRIANGLES, 0, wall2NumberOfVertices);

        // Wall3
        glColor3f(0.0, 0.0, 1.0);
        glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall3VertexData[0].vertex);
        glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall3VertexData[0].normal);
        glDrawArrays(GL_TRIANGLES, 0, wall3NumberOfVertices);

        // Wall4
        glColor3f(0.5, 0.0, 0.5);
        glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall4VertexData[0].vertex);
        glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall4VertexData[0].normal);
        glDrawArrays(GL_TRIANGLES, 0, wall4NumberOfVertices);

        // Ceiling
        glColor3f(0.4, 0.4, 0.4);
        glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &ceilingVertexData[0].vertex);
        glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &ceilingVertexData[0].normal);
        glDrawArrays(GL_TRIANGLES, 0, ceilingNumberOfVertices);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        // Add dharma cubes, spaced out evenly in a line
        int numCubes = 10;
        for (int i = 0; i < numCubes; ++i) {
            glPushMatrix();
            glTranslatef(METER_TO_UNIT(((numCubes / 2) - i) * METER_TO_UNIT(5)), METER_TO_UNIT(-1.5), METER_TO_UNIT(0));
            glRotatef(dharma_rotation * ((numCubes / 2) - i), 0, 0, 1);
            glScalef(0.7, 0.7, 0.7);
            glBindTexture(GL_TEXTURE_2D, dharma_texture);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &dharmaCubeVertexData[0].vertex);
            glNormalPointer(GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &dharmaCubeVertexData[0].normal);
            glTexCoordPointer(2, GL_FLOAT, sizeof(TEXTURED_VERTEX_DATA_3D), &dharmaCubeVertexData[0].texCoord);
            glDrawArrays(GL_TRIANGLES, 0, dharmaCubeNumberOfVertices);
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_NORMAL_ARRAY);
            glPopMatrix();
        }
        SDL_GL_SwapBuffers();
    }
}

// Initialize OpenGL perspective matrix
void GL_Setup(int width, int height)
{
    // Load Bitmaps
    SDL_Surface* bmpFile = SDL_LoadBMP("data/floor.bmp");

    // Standard OpenGL texture creation code
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create textures
    glGenTextures(1, &floor_texture);
    glBindTexture(GL_TEXTURE_2D, floor_texture);

    if (gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bmpFile->w,
            bmpFile->h, GL_BGR_EXT,
            GL_UNSIGNED_BYTE, bmpFile->pixels) != 0) {
        throw "error building mipmaps";
    }
    // Free surface after using it
    SDL_FreeSurface(bmpFile);

    bmpFile = SDL_LoadBMP("data/dharmacube.bmp");

    glGenTextures(1, &dharma_texture);
    glBindTexture(GL_TEXTURE_2D, dharma_texture);

    if (gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bmpFile->w,
            bmpFile->h, GL_BGR_EXT,
            GL_UNSIGNED_BYTE, bmpFile->pixels) != 0) {
        throw "error building mipmaps";
    }
    SDL_FreeSurface(bmpFile);

    // Change the texture matrix to flip and mirror
    glMatrixMode(GL_TEXTURE);
    glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    glScalef(-1.0f, 1.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);

    // Setup light source
    light_color.setRed(1.0f);
    light_color.setGreen(1.0f);
    light_color.setBlue(1.0f);

    ambient_light.setRed(0.0f);
    ambient_light.setGreen(0.0f);
    ambient_light.setBlue(0.0f);

    specular_highlight.setRed(1.0f);
    specular_highlight.setGreen(1.0f);
    specular_highlight.setBlue(1.0f);

    GLfloat main_light_color[] = {light_color.getRed(), light_color.getGreen(), light_color.getBlue(), 1.0f};
    GLfloat main_ambient_light[] = {ambient_light.getRed(), ambient_light.getGreen(), ambient_light.getBlue(), 1.0f};
    GLfloat main_specular_highlight[] = {specular_highlight.getRed(), specular_highlight.getGreen(), specular_highlight.getBlue(), 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos.M);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, main_light_color);
    glLightfv(GL_LIGHT0, GL_AMBIENT, main_ambient_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, main_specular_highlight);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);

    GLfloat material_shininess[] = { 4.0f };
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);

    reshapeGL(width, height);
}

int main(int argc, char *argv[]) {
    
    // Initialize SDL audio, video, input
    stringstream ss;

    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8 *wav_buffer = NULL;

    SDL_AudioCVT wav_cvt;
    int ret;

    SDL_AudioSpec *desired, *obtained;
    SDL_AudioSpec *hardware_spec = NULL;
    SDL_Joystick *joystick = NULL;

    try {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

        // Allocate a desired SDL_AudioSpec
        desired = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));

        // Allocate space for the obtained SDL_AudioSpec
        obtained = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));

        desired->freq=44100;

        // 16-bit signed audio
        desired->format = AUDIO_S16SYS;

        desired->channels = 2;

        // Large audio buffer reduces risk of dropouts but increases response time
        desired->samples = 2048;

        // Audio callback function
        desired->callback = my_audio_callback;

        desired->userdata = NULL;

        // Open the audio device
        if (SDL_OpenAudio(desired, obtained) < 0) {
            ss.clear();
            ss << "Couldn't open audio: " << SDL_GetError() << endl;
            throw ss.str();
        }
        // Desired spec is no longer needed
        free(desired);

        hardware_spec = obtained;
        Debugger::getInstance().print("Audio hardware:\nfreq: " + Utils::getInstance().itos(hardware_spec->freq) +
                "\n" +
                "channels: " + Utils::getInstance().itos(hardware_spec->channels) +
                "\n" +
                "samples: " + Utils::getInstance().itos(hardware_spec->samples) +
                "\n"
                );

        // Load the WAV
        if (SDL_LoadWAV("data/bird2.wav", &wav_spec, &wav_buffer, &wav_length) == NULL) {
            ss.clear();
            ss << "Could not open test.wav: " << SDL_GetError() << endl;
            throw ss.str();
        }

        // Build AudioCVT
        ret = SDL_BuildAudioCVT(&wav_cvt,
                                wav_spec.format, wav_spec.channels, wav_spec.freq,
                                hardware_spec->format, hardware_spec->channels, hardware_spec->freq);

        // Check that the convert was built
        if (ret == -1) {
            ss.clear();
            ss << "Couldn't build converter!" << endl;
            throw ss.str();
        }

        // Setup for conversion, copy original data to new buffer
        wav_cvt.buf = (Uint8*)malloc(wav_length * wav_cvt.len_mult);
        wav_cvt.len = wav_length;
        memcpy(wav_cvt.buf, wav_buffer, wav_length);

        // We can delete to original WAV data now
        SDL_FreeWAV(wav_buffer);

        // And now we're ready to convert
        SDL_ConvertAudio(&wav_cvt);
        bird_buffer = wav_cvt.buf;
        bird_length = wav_cvt.len;

        // Start audio
        SDL_PauseAudio(0);

        ss.clear();
        ss << SDL_NumJoysticks() << " joysticks were found" << endl;
        Debugger::getInstance().print(ss.str());
        Debugger::getInstance().print("The names of the joysticks are:\n");

        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            ss.clear();
            ss << SDL_JoystickName(i) << endl;
            Debugger::getInstance().print(ss.str());
        }

        SDL_JoystickEventState(SDL_ENABLE);
        if (SDL_NumJoysticks() > 0) {
            joystick = SDL_JoystickOpen(0);
        }

        const SDL_VideoInfo* info = SDL_GetVideoInfo();
        int vidFlags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER;
        if (fullscreen) {
            vidFlags |= SDL_RESIZABLE;
            vidFlags |= SDL_FULLSCREEN;
            window_width = info->current_w;
            window_height = info->current_h;
        }

        if (info->hw_available) {
            vidFlags |= SDL_HWSURFACE;
            Debugger::getInstance().print("using hardware surface");
        } else {
            vidFlags |= SDL_SWSURFACE;
            Debugger::getInstance().print("using software surface");
        }
        int bpp = info->vfmt->BitsPerPixel;
        SDL_SetVideoMode(window_width, window_height, bpp, vidFlags);
        GL_Setup(window_width, window_height);

        // Start main loop
        main_loop_function();
    } catch (std::string exc) {
        Debugger::getInstance().print(exc);
    }

    // Clean up
    if (joystick != NULL) {
        SDL_JoystickClose(joystick);
    }
    SDL_CloseAudio();
    if (bird_buffer != NULL) {
        SDL_FreeWAV(bird_buffer);
    }
    if (hardware_spec != NULL) {
        free(hardware_spec);
    }
    //
    // Delete textures
    glDeleteTextures(1, &floor_texture);
    glDeleteTextures(1, &dharma_texture);

    return 0;
}
