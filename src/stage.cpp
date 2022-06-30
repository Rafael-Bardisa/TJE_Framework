//
// Created by Rafael Bardisa Rodes on 24/4/22.
//

#include "stage.h"
#include "game.h"
#include "shader.h"
#include "mesh.h"
#include "utils.h"
#include "fbo.h"
#include "texture.h"

#include "extra/coldet/coldet.h"

#define MAX_LATITUDE 22.45f
#define BYTE_TO_SHADER (1.f/255.f)
// renders and updates

Stage::Stage(Game* game) {
    this->reference = game;
};


// renders
void GeoStage::render() {
    //get data from game instance
    
    //pointers
    auto* camera = reference->camera;
    
    auto* day_texture = reference->day_texture;
    auto* night_texture = reference->night_texture;
    auto* regions_texture = reference->regions_texture;
    
    auto* window = reference->window;
    
    auto* mesh = reference->mesh;
    auto* shader = reference->shader;
    
    //data
    auto time = reference->time;
    
    auto sun_position = reference->sun_position;
    
    auto season_offset = reference->season_offset;
    
    auto selected_color = reference->selected_color;
    
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
    shader = Shader::Get("data/shaders/basic.vs", "data/shaders/geoscape.fs");
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
    SDL_GL_SwapWindow(window);
}

void BattleStage::render() {
    return;
}

// updates
STAGE_ID GeoStage::update(float seconds_elapsed) {
    //get data from game instance
    
    //references
    auto* camera = reference->camera;
    auto* regions_image = reference->regions_image;
    
    //data
    auto mouse_speed = reference->mouse_speed;

    auto season_angle = reference->season_angle;
    
    auto window_width = reference->window_width;
    auto window_height = reference->window_height;
    
    auto selected_color = reference->selected_color;
    
    auto mouse_locked = reference->mouse_locked;
    auto rotation_mode = reference->rotation_mode;
    auto time = reference->time;
    
    //these will get updated
    /*
    auto sun_position = reference->sun_position;
    auto season_offset = reference->season_offset;
     */
    
    float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

    //example
    //angle += (float)seconds_elapsed * 10.0f;
    if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT))
    reference->sun_position.rotateGlobal(seconds_elapsed, Vector3(0,1,0));
    reference->season_offset.y = season_angle*cos(time/3);

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
            reference->selected_color = regions_image->getPixelLatLon(polar_coords.x, polar_coords.y);
            std::cout << collision_point << std::endl;
            std::cout << polar_coords << std::endl;
            std::cout << polar_coords.fromPolar() << std::endl;
        }
        
        
        
        
        
        
        
        switch (rotation_mode){
            case (Game::eCameraRotation::NORMAL):
                //std::cout << "normal\n";
                camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
                camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
                break;
            case (Game::eCameraRotation::CENTER):
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
    
    return GEOSCAPE;
}

STAGE_ID BattleStage::update(float seconds_elapsed) {
    return BATTLESCAPE;
}
