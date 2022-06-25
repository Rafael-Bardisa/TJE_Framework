/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "camera.h"
#include "utils.h"
#include "renderer.h"
#include "synth.h"
#include "stage.h"

#define MAX_LATITUDE 22.45f
#define BYTE_TO_SHADER (1.f/255.f)

#define NUM_STAGES 2

//forward declaration
class Texture;
class Image;
class Mesh;
class FBO;
class Shader;


//class definition
class Game
{
public:
    
    enum eCameraRotation {
        NORMAL,
        CENTER
    };
    
	static Game* instance;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;
    Matrix44 sun_position;
    Vector3 season_offset;
    
    float angle = 0;
    float mouse_speed = 100.0f;
    FBO* fbo = NULL;
    const float season_angle = tan(MAX_LATITUDE * DEG2RAD);

    Mesh* mesh = NULL;
    Shader* shader = NULL;
    
    Texture* day_texture = NULL;
    Texture* night_texture = NULL;
    Texture* regions_texture = NULL;

    //copy in cpu to get information
    Image* day_image = NULL;
    Image* night_image = NULL;
    Image* regions_image = NULL;

	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)
    eCameraRotation rotation_mode;
    
    Color selected_color;
    
    STAGE_ID current_stage;
    std::vector<Stage*> stages;
    
    //sound
    Synth synth;

	Game( int window_width, int window_height, SDL_Window* window );
    
	//main functions
	void render( void );
	void update( double dt );

    
    //stage things
    void initStages();
    Stage* getStage(STAGE_ID id);
    void setStage(STAGE_ID id);
    Stage* getCurrentStage(){return stages[current_stage];};
    
    
    
    
    //audio
    void enableAudio(int device = -1);
    void onAudio(float* buffer, unsigned int len, double time, SDL_AudioSpec &audio_spec); //called constantly to fill the audio buffer

    
	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

};


#endif 
