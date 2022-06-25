#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "extra/coldet/coldet.h"

#include <cmath>

//some globals
/*
Mesh* mesh = NULL;
//Texture* texture = NULL;
Texture* day_texture = NULL;
Texture* night_texture = NULL;
Texture* regions_texture = NULL;

//copy in cpu to get information
Image* day_image = NULL;
Image* night_image = NULL;
Image* regions_image = NULL;
*/

Animation* anim = NULL;
/*
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;
const float season_angle = tan(MAX_LATITUDE * DEG2RAD);*/

Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
    this->initStages();
    this->setStage(GEOSCAPE);
    
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;
    
    // simulate time
    sun_position.setTranslation(1, 0, 0);
    season_offset = Vector3(0, 0, 0);

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,3.f, 3.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective
    rotation_mode = CENTER;
    selected_color = Color(0,0,0,0);
    
    enableAudio();
    synth.playSample("data/sound/bg/mus-play2.wav", 1, true);
    
    //synth.osc1.amplitude = 0.3;

    //images are flipped idk why
#warning hack feo
    day_image = new Image();
    day_image->loadTGA("data/textures/earth_day.tga");
    day_image->flipY();
    night_image = new Image();
    night_image->loadTGA("data/textures/earth_night.tga");
    night_image->flipY();
    regions_image = new Image();
    regions_image->loadTGA("data/textures/test_borders_6.tga");
    regions_image->flipY();
    
    //load one texture without using the Texture Manager (Texture::Get would use the manager)
    day_texture = new Texture();
    day_texture->load("data/textures/earth_day.tga");
    night_texture = new Texture();
    night_texture->load("data/textures/earth_night.tga");
    regions_texture = new Texture();
    regions_texture->load("data/textures/test_borders_6.tga");
	// example of loading Mesh from Mesh Manager
#warning LOAD MESH
	mesh = Mesh::Get("data/sphere.obj");
    

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/geoscape.fs");

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw

//main functions
#warning TODO migrate to renderer for clean code
void Game::render(void)
{
    /*
    
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
   
	//create model matrix for cube
	Matrix44 m;
	//m.rotate(cos(time)*20.0f*DEG2RAD, Vector3(1, 0, 0));
//#warning rescale to normalize
    //m.scale(40, 40, 40);
	if(shader)
	{
		//enable shader
		shader->enable();
		//upload uniforms
		shader->setUniform("u_color", Vector4(1,1,1,1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix );
        
		shader->setUniform("u_day_texture", day_texture, 0);
        shader->setUniform("u_night_texture", night_texture, 1);
        shader->setUniform("u_regions_texture", regions_texture, 2);
        
		shader->setUniform("u_model", m);
		shader->setUniform("u_time", time);
        shader->setUniform("u_sun_direction", normalize(sun_position.getTranslation() + season_offset));
        
        
#warning FLOATS OR DEAD AS HELL
        shader->setUniform("u_selected_region_color", (selected_color.toVector4()*BYTE_TO_SHADER));
        // pass political color to shader to highlight the selected country

		//do the draw call
		mesh->render( GL_TRIANGLES );

		//disable shader
		shader->disable();
	}

	//Draw the floor grid
	drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
     */
    this->getCurrentStage()->render();
}

#warning STATES
void Game::update(double seconds_elapsed)
{
    /*
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	//angle += (float)seconds_elapsed * 10.0f;
    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
    sun_position.rotateGlobal(seconds_elapsed, Vector3(0,1,0));
    season_offset.y = season_angle*cos(time/3);

	//mouse input to rotate the cam
#warning TODO a medias :un clasico:
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
        // cast ray to detect collision with earth
        Vector2 mouse_position = Input::mouse_position;
        Vector3 direction = camera->getRayDirection(mouse_position.x, mouse_position.y, window_width, window_height);
        
        //Y IS HEIGHT
        Vector3 collision_point = Vector3(0, 0, 0);

        // world is always at 0,0,0 with radius 1

        // only update color if collision
        if (SphereRayCollision(Vector3().v, 1.f, camera->eye.v, direction.v, collision_point.v)){
            //convert world coordinates of collision to uvs
            Vector2 polar_coords = collision_point.toPolar();
            
            // query the color of countries texture at uv
            selected_color = regions_image->getPixelLatLon(polar_coords.x, polar_coords.y);
            //std::cout << "|-|" << (int)selected_color.a << "|-|";
        }
        
        
        
        
        
        
        
        switch (rotation_mode){
            case NORMAL:
                //std::cout << "normal\n";
                camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
                camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
                break;
            case CENTER:
                //std::cout << "center\n";
                camera->rotateView(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
                camera->rotateView(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
                break;
            default: break;
        }
	}

	//async input to move the camera around
	if(Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f,0.0f, 0.0f) * speed);
    if (Input::isKeyPressed(SDL_SCANCODE_E)){
        camera->fov = camera->fov - speed;
        camera->updateProjectionMatrix();
    }
    if (Input::isKeyPressed(SDL_SCANCODE_Q)){
        camera->fov = camera->fov + speed;
        camera->updateProjectionMatrix();
    }
        
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
     */
    this->getCurrentStage()->update(seconds_elapsed);
}


void Game::initStages(){
    stages.clear();
    stages.reserve(NUM_STAGES);
    stages.push_back(new GeoStage(this));
    stages.push_back(new BattleStage(this));
}

Stage* Game::getStage(STAGE_ID id){
    return stages[id];
}

// set new stage as current. Store time of change and set local stage time to 0
void Game::setStage(STAGE_ID id){
    current_stage = id;
    //getStage(id)->epoch = time;
    //getStage(id)->localFrame = 0;
}

//AUDIO STUFF ********************

SDL_AudioSpec audio_spec;

void AudioCallback(void*  userdata,
    Uint8* stream,
    int    len)
{
    static double audio_time = 0;

    memset(stream, 0, len);//clear
    if (!Game::instance)
        return;

    Game::instance->onAudio((float*)stream, len / sizeof(float), audio_time, audio_spec);
    audio_time += len / (double)audio_spec.freq;
}

void Game::enableAudio(int device)
{
    const char* selected_device_name = NULL;
    if (device != -1)
    {
        int i, count = SDL_GetNumAudioDevices(0);
        if (device >= count)
            device = 0; //default device

        for (i = 0; i < count; ++i) {
            const char* name = SDL_GetAudioDeviceName(i, 0);
            if (i == device)
                selected_device_name = name;
            std::cout << (i == device ? " *" : " -") << " Audio device " << i << ": " << name << std::endl;
        }
    }

    SDL_memset(&audio_spec, 0, sizeof(audio_spec)); /* or SDL_zero(want) */
    audio_spec.freq = 48000;
    audio_spec.format = AUDIO_F32;
    audio_spec.channels = 1;
    audio_spec.samples = 1024;
    audio_spec.callback = AudioCallback; /* you wrote this function elsewhere. */

    SDL_AudioDeviceID dev;
    
    if(selected_device_name)
        dev = SDL_OpenAudioDevice(selected_device_name, 0, &audio_spec, &audio_spec, 0);
    else
        dev = SDL_OpenAudio(&audio_spec, &audio_spec);

    if (dev < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        return;
    }
    SDL_PauseAudio(0);
}

void Game::onAudio(float *buffer, unsigned int len, double time, SDL_AudioSpec& audio_spec)
{
    //fill the audio buffer using our custom retro synth
    synth.generateAudio(buffer, len, audio_spec);
}



//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break;
//#warning DEBUG STATEMENT(?)
        case SDLK_c:
            std::cout <<"pressed\n";
            rotation_mode = rotation_mode == NORMAL ? CENTER : NORMAL; break;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

