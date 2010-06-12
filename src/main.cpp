#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include "RzPolygonGroupCollection.h"
#include "RzPolygonGroup.h"
#include "RzPolygon.h"
#include "RzTriangle.h"
#include "RzColor3f.h"
#include "RzVertex3f.h"
#include "Debugger.h"
#include "Utils.h"
#include "matrixmath.h"
#include "RzMatrix.h"
#include "RzVector.h"
#include "floor.h"
#include "wall1.h"
#include "wall2.h"
#include "wall3.h"
#include "wall4.h"
#include "ceiling.h"

using namespace std;

#define DEFAULT_WINDOW_WIDTH 700
#define DEFAULT_WINDOW_HEIGHT 700

//#define UNIT_PER_METER 1.5f
#define UNIT_PER_METER 1.0f

#define METER_TO_UNIT(m) ((float)m * UNIT_PER_METER)
#define UNIT_TO_METER(u) ((float)u / UNIT_PER_METER)
#define MS_TO_SEC(ms) ((float)ms / 1000.0f)

#define PLAYER_EYE_HEIGHT 1.5f // meters

#define PLAYER_WALK_SPEED 1.0f // meters per second
#define PLAYER_MAX_SPEED 3.0f // meters per second
#define PLAYER_MIN_SPEED 0.0f // meters per second
#define SLOW_TURN_SPEED 72.0f // degrees per second
#define MAX_TURN_SPEED 180.0f // degrees per second
#define MIN_TURN_SPEED 0.0f // degrees per second

bool fullscreen = true;

float player_move_speed = 0; // meters per second
float player_turn_speed = 0; // degrees per second

//int jaxis_0_min_neg = -4000;
int jaxis_0_min_neg = -3200;
int jaxis_0_min_pos = -jaxis_0_min_neg;
int jaxis_1_rest_min = jaxis_0_min_neg;
int jaxis_1_rest_max = jaxis_0_min_pos;

int jaxis_0_max_neg = -32768;
int jaxis_0_max_pos = -jaxis_0_max_neg;
int jaxis_1_max_neg = jaxis_0_max_neg;
int jaxis_1_max_pos = jaxis_0_max_pos;

Uint32 loop_duration, current_ticks, loop_start;

// Keydown booleans
bool key[321];

int window_width = DEFAULT_WINDOW_WIDTH;
int window_height = DEFAULT_WINDOW_HEIGHT;

//POINT4D light_pos = {METER_TO_UNIT(0), METER_TO_UNIT(5), METER_TO_UNIT(0), 1.0f};
POINT4D light_pos = {METER_TO_UNIT(0), METER_TO_UNIT(2.5), METER_TO_UNIT(0), 1.0f};
RzColor3f light_color;
RzColor3f ambient_light;
RzColor3f specular_highlight;
int quitButton = 0;

//float eye_z, near_z, far_z, other_factor;
POINT3D camera_position = {METER_TO_UNIT(0), METER_TO_UNIT(PLAYER_EYE_HEIGHT), METER_TO_UNIT(0)};
POINT3D camera_target = {METER_TO_UNIT(0), METER_TO_UNIT(PLAYER_EYE_HEIGHT), METER_TO_UNIT(-1)};
VECTOR3D camera_up = {0.0f, 1.0f, 0.0f};
float near_z = 0.1;
float far_z = 100;
float field_of_view = 45; // degrees

void reshapeGL(int width, int height) // reshape the window when it's moved or resized
{
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));
    // reset the current viewport
	glMatrixMode(GL_PROJECTION);
	// select the projection matrix
	glLoadIdentity();
	// reset the projection matrix
	gluPerspective(field_of_view, (GLfloat)(width)/(GLfloat)(height), near_z, far_z);
	// calculate the aspect ratio of the window
	glMatrixMode(GL_MODELVIEW);
	// select the modelview matrix
	glLoadIdentity();
	// reset the modelview matrix

	window_width = width;
	window_height = height;
	return;
}

void my_audio_callback(void *userdata, Uint8 *stream, int len) {
}

// in radians
void rotateView(float my_angle) {
	POINT4D target, result_vector;
	MATRIX4X4 transform_matrix, temp_matrix, result_4x4;
	float cos_rot, sin_rot;

	VECTOR4D_INITXYZ(&target, camera_target.x, camera_target.y, camera_target.z);
	MAT_IDENTITY_4X4(&transform_matrix);

	// translate the eye to origin to do the rotation
	Mat_Init_4X4(&temp_matrix,
			1, 0, 0, -camera_position.x,
			0, 1, 0, -camera_position.y,
			0, 0, 1, -camera_position.z,
			0, 0, 0, 1);

	Mat_Mul_4X4(&temp_matrix, &transform_matrix, &result_4x4);
	MAT_COPY_4X4(&result_4x4, &transform_matrix);

	cos_rot = cos(my_angle);
	sin_rot = sin(my_angle);

	// rotate around origin
	Mat_Init_4X4(&temp_matrix,
		cos_rot, 0, sin_rot, 0,
		0, 1, 0, 0,
		-sin_rot, 0, cos_rot, 0,
		0, 0, 0, 1);

	Mat_Mul_4X4(&temp_matrix, &transform_matrix, &result_4x4);
	MAT_COPY_4X4(&result_4x4, &transform_matrix);

	// translate back to original position
	Mat_Init_4X4(&temp_matrix,
			1, 0, 0, camera_position.x,
			0, 1, 0, camera_position.y,
			0, 0, 1, camera_position.z,
			0, 0, 0, 1);

	Mat_Mul_4X4_VECTOR4D(&transform_matrix, &target, &result_vector);

	//VECTOR3D_INITXYZ(&camera_target, result_vector.x, result_vector.y, result_vector.z);
	VECTOR3D_INITXYZ(&camera_target, result_vector.x, METER_TO_UNIT(PLAYER_EYE_HEIGHT), result_vector.z);

	// make sure the vector from camera to target is normal (doesn't get too big or small)
	VECTOR3D target_vector = { camera_target.x - camera_position.x,
			camera_target.y - camera_position.y,
			camera_target.z - camera_position.z };

	VECTOR3D_Normalize(&target_vector);
	VECTOR3D_Add(&target_vector, &camera_target, &camera_target);
}

void moveByAmount(float amount) {
	VECTOR3D target_vector = { camera_target.x - camera_position.x,
			camera_target.y - camera_position.y,
			camera_target.z - camera_position.z };

	VECTOR3D_Normalize(&target_vector);
	VECTOR3D_Scale(amount, &target_vector);

	// figure out if we're crossing an important boundary


	// make the move
	VECTOR3D_Add(&target_vector, &camera_position, &camera_position);
	VECTOR3D_Add(&target_vector, &camera_target, &camera_target);

	// stay on the floor
	camera_position.y = METER_TO_UNIT(PLAYER_EYE_HEIGHT);
	camera_target.y = METER_TO_UNIT(PLAYER_EYE_HEIGHT);
}

void main_loop_function()
{
	loop_start = SDL_GetTicks();

	stringstream ss;
	SDL_Event event;

	while(true)
	{
		if (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_VIDEORESIZE:

			case SDL_KEYDOWN :
				key[event.key.keysym.sym] = true;
				switch(event.key.keysym.sym) {
				case SDLK_UP:
					// figure out how far the player should go
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
			case SDL_KEYUP   :
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
			case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
		    	if (event.jaxis.axis == 0) {
					// move the camera toward the camera target
			    	if (event.jaxis.value > jaxis_0_min_pos) {
			    		if (event.jaxis.value > jaxis_0_max_pos) {
			    			player_turn_speed = -MAX_TURN_SPEED;
			    		} else {
			    			player_turn_speed = -((float)(event.jaxis.value - jaxis_0_min_pos) / (float)(jaxis_0_max_pos - jaxis_0_min_pos) *
								(float)(MAX_TURN_SPEED - MIN_TURN_SPEED));
			    		}
			    	} else {
			    		if (event.jaxis.value >= jaxis_0_min_neg) {
			    			// at rest
			    			player_turn_speed = 0;
			    		} else {
			    			if (event.jaxis.value < jaxis_0_max_neg) {
			    				player_turn_speed = MAX_TURN_SPEED;
			    			} else {
				    			player_turn_speed = ((float)(event.jaxis.value - jaxis_0_min_neg) /
						    			(float)(jaxis_0_max_neg - jaxis_0_min_neg) *
										(float)(MAX_TURN_SPEED - MIN_TURN_SPEED));

			    			}

			    		}
			    	}
			    } else if (event.jaxis.axis == 1) {
					// move the camera toward the camera target
			    	if (event.jaxis.value > jaxis_1_rest_max) {
			    		if (event.jaxis.value > jaxis_1_max_pos) {
			    			player_move_speed = -PLAYER_MAX_SPEED;
			    		} else {
			    			player_move_speed = -((float)(event.jaxis.value - jaxis_1_rest_max) / (float)(jaxis_1_max_pos - jaxis_1_rest_max) *
								(float)(PLAYER_MAX_SPEED - PLAYER_MIN_SPEED));
			    		}
			    	} else {
			    		if (event.jaxis.value >= jaxis_1_rest_min) {
			    			// at rest
			    			player_move_speed = 0;
			    		} else {
			    			if (event.jaxis.value < jaxis_1_max_neg) {
			    				player_move_speed = PLAYER_MAX_SPEED;
			    			} else {
				    			player_move_speed = ((float)(event.jaxis.value - jaxis_1_rest_min) /
						    			(float)(jaxis_1_max_neg - jaxis_1_rest_min) *
										(float)(PLAYER_MAX_SPEED - PLAYER_MIN_SPEED));

			    			}

			    		}
			    	}
				}
		    	/*
		    	Debugger::getInstance().print("axis: " + Utils::getInstance().itos(event.jaxis.axis)
		    			+ ", value: " + Utils::getInstance().itos(event.jaxis.value));
		    			*/
			    break;
			case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
			    if ( event.jbutton.button == 0 )
			    {
			        /* code goes here */
			    }
			    /*
			    Debugger::getInstance().print("button down: " + Utils::getInstance().itos(event.jbutton.button));
			    */
			    break;
			case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */
			    if ( event.jbutton.button == quitButton )
			    {
			        /* code goes here */
			    	return;
			    }
			    /*
			    Debugger::getInstance().print("button up: " + Utils::getInstance().itos(event.jbutton.button));
			    */
			    break;
			case SDL_JOYHATMOTION:  /* Handle Hat Motion */
				/*
			    if ( event.jhat.value & SDL_HAT_UP )
			    {
			    	Debugger::getInstance().print("joystick hat up");
			    }
			    if ( event.jhat.value & SDL_HAT_LEFT )
			    {
			    	Debugger::getInstance().print("joystick hat left");
			    }
			    if ( event.jhat.value & SDL_HAT_RIGHTDOWN )
			    {
			    }
			    Debugger::getInstance().print("hat motion: " + Utils::getInstance().itos(event.jhat.value));
			    */
			    break;
			case SDL_QUIT    : return; break;
			}
		}

		if (player_turn_speed != 0) {
			rotateView(DEG_TO_RAD(player_turn_speed * MS_TO_SEC(loop_duration)));
		}

		if (player_move_speed != 0) {
			moveByAmount(METER_TO_UNIT(player_move_speed * MS_TO_SEC(loop_duration)));
		}

		current_ticks = SDL_GetTicks();
		loop_duration = current_ticks - loop_start;
		loop_start = current_ticks;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(camera_position.x, camera_position.y, camera_position.z,
				camera_target.x, camera_target.y, camera_target.z,
				camera_up.x, camera_up.y, camera_up.z);

		// draw the scene

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		// floor
		glColor3f(0.4, 0.4, 0.4);
		glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &floorVertexData[0].vertex);
		glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &floorVertexData[0].normal);
		glDrawArrays(GL_TRIANGLES, 0, floorNumberOfVertices);

		// wall1
		glColor3f(0.5, 0.0, 0.0);
		glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall1VertexData[0].vertex);
		glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall1VertexData[0].normal);
		glDrawArrays(GL_TRIANGLES, 0, wall1NumberOfVertices);

		// wall2
		glColor3f(0.0, 0.5, 0.0);
		glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall2VertexData[0].vertex);
		glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall2VertexData[0].normal);
		glDrawArrays(GL_TRIANGLES, 0, wall2NumberOfVertices);

		// wall3
		glColor3f(0.0, 0.0, 1.0);
		glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall3VertexData[0].vertex);
		glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall3VertexData[0].normal);
		glDrawArrays(GL_TRIANGLES, 0, wall3NumberOfVertices);

		// wall4
		glColor3f(0.5, 0.0, 0.5);
		glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall4VertexData[0].vertex);
		glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &wall4VertexData[0].normal);
		glDrawArrays(GL_TRIANGLES, 0, wall4NumberOfVertices);

		// ceiling
		glColor3f(0.4, 0.4, 0.4);
		glVertexPointer(3, GL_FLOAT, sizeof(VERTEX_DATA_3D), &ceilingVertexData[0].vertex);
		glNormalPointer(GL_FLOAT, sizeof(VERTEX_DATA_3D), &ceilingVertexData[0].normal);
		glDrawArrays(GL_TRIANGLES, 0, ceilingNumberOfVertices);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		/*
		// the floor
		glBegin(GL_QUADS);
		glColor3f(0.4, 0.4, 0.4);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-5, 0, -5);
		glVertex3f(5, 0, -5);
		glVertex3f(5, 0, 5);
		glVertex3f(-5, 0, 5);
		glEnd();
		*/

		SDL_GL_SwapBuffers();
	}
}

// Initialze OpenGL perspective matrix
void GL_Setup(int width, int height)
{
	//Load Bitmaps

	/* Standard OpenGL texture creation code */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//change the texture matrix to flip and mirror
	glMatrixMode(GL_TEXTURE);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glScalef(-1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);

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
	//glColorMaterial(GL_FRONT, GL_DIFFUSE);

    //GLfloat material_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    //glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);

    GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);

	GLfloat material_shininess[] = { 4.0f };
    glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	//glClearColor(0.3, 0.3, 1.0, 0.0);
    glClearColor(0, 0, 0, 0);
    glShadeModel(GL_SMOOTH);

    reshapeGL(width, height);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

int main(int argc, char *argv[]) {
	// Initialize SDL with best video mode
	stringstream ss;

	SDL_AudioSpec wav_spec;
	Uint32 wav_length;
	Uint8 *wav_buffer = NULL;

	SDL_AudioCVT  wav_cvt;
	int ret;

	/* Open the audio device */
	SDL_AudioSpec *desired, *obtained;
	SDL_AudioSpec *hardware_spec = NULL;

    SDL_Joystick *joystick = NULL;

	try {
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

		/* Allocate a desired SDL_AudioSpec */
		desired = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));

		/* Allocate space for the obtained SDL_AudioSpec */
		obtained = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));

		//desired->freq=22050;
		desired->freq=44100;

		/* 16-bit signed audio */
		desired->format=AUDIO_S16SYS;

		/* Mono */
		desired->channels=2;

		/* Large audio buffer reduces risk of dropouts but increases response time */
		//desired->samples=8192;
		desired->samples=2048;

		/* Our callback function */
		desired->callback=my_audio_callback;

		desired->userdata=NULL;

		/* Open the audio device */
		if (SDL_OpenAudio(desired, obtained) < 0) {
			ss.clear();
			ss << "Couldn't open audio: " << SDL_GetError() << endl;
			throw ss.str();
		}
		/* desired spec is no longer needed */
		free(desired);

		hardware_spec=obtained;
		Debugger::getInstance().print("Audio hardware:\nfreq: " + Utils::getInstance().itos(hardware_spec->freq) +
				"\n" +
				"channels: " + Utils::getInstance().itos(hardware_spec->channels) +
				"\n" +
				"samples: " + Utils::getInstance().itos(hardware_spec->samples) +
				"\n"
				);

		/* Start playing */
		SDL_PauseAudio(0);

		ss.clear();
		ss << SDL_NumJoysticks() << " joysticks were found" << endl;
		Debugger::getInstance().print(ss.str());
	    Debugger::getInstance().print("The names of the joysticks are:\n");

	    for(int i=0; i < SDL_NumJoysticks(); i++ )
	    {
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


		main_loop_function();
	} catch (std::string exc) {
		Debugger::getInstance().print(exc);
	}

	if (joystick != NULL) {
		SDL_JoystickClose(joystick);
	}
	SDL_CloseAudio();
	if (hardware_spec != NULL) {
		free(hardware_spec);
	}
	// delete textures

	return 0;
}
