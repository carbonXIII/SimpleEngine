/*
 * main.cpp
 *
 *  Created on: Jul 29, 2017
 *      Author: Jordan Richards
 */
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "PhysicsEngine.h"
#include "CharacterComponent.h"
#include "Sprite.h"
#include "Scene.h"
#include "Stage.h"
#include "Text.h"

#include <windows.h>

using namespace std;

SDL_Window* win;
SDL_Renderer* renderer;

void OpenConsole(){
	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
}

void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void drawTexture(TextureRegion texture, Pixel color){
	int pitch;
	Pixel* data;

	SDL_Rect r = {0,0,texture.getROI().w,texture.getROI().h};

	SDL_LockTexture(texture.getParent(), &r, (void**)&data, &pitch);

	for(int i = 0; i < texture.getROI().w*texture.getROI().h; i++){
		data[i] = color;
	}

	SDL_UnlockTexture(texture.getParent());
}

int main(int argc, char** argv){
#ifdef DEBUG
	OpenConsole();
#endif
	ShowConsoleCursor(false);

	Stage stage("This was a triumph.",640,480,Pixel(255,255,255));
	auto controller_id = stage.addController();
	auto horiz = stage.registerAxis(controller_id, SDLK_LEFT, SDLK_RIGHT);
	auto space = stage.registerInput(controller_id, SDLK_SPACE);

	TextureRegion graphic = stage.getTextureManager().create("player",{50,50});
	drawTexture(graphic,Pixel(255,0,0,255));

	TextureRegion ground = stage.getTextureManager().create("ground", {640, 10});
	drawTexture(ground,Pixel(0,0,0,255));

	Scene scene(.02F,9.8f,Transform(),&stage);
	stage.connectScene(&scene);
	CameraComponent* camera = static_cast<CameraComponent*>(scene.addComponent<CameraComponent>(SDL_Point{640,480}));

	Sprite sprite(graphic, Transform(Vector(20.,20.)), &scene);
	sprite.addComponent<PhysicsComponent>(Collider(50,50),1.f,1.f,false,true);

	CharacterComponentParams params;
	params.jumpAccel = 75;
	params.jumpAccelDuration = .1;
	params.maxVelocity = 20.;
	CharacterComponent* controller = static_cast<CharacterComponent*>(sprite.addComponent<CharacterComponent>(params));
	camera->setTarget(&sprite);

	AnimatedSprite block(stage.getTextureManager().load("link.png"),24,32,20,Transform(Vector(400.,20.),0,2.), &scene);
	block.addAnimation("walking_right", {0,1}, 12, 1);
	block.extendAnimation("walking_right", {0,5}, 12, 1);
	block.addComponent<PhysicsComponent>(Collider(48,50,0,14),1.f,.3f);

	Sprite floor(ground, Transform(Vector(0.,400.)), &scene);
	floor.addComponent<PhysicsComponent>(Collider(640,10),1.f,.3f,true);

	Sprite more_floor(ground, Transform(Vector(700.,475.)), &scene);
	more_floor.addComponent<PhysicsComponent>(Collider(640,10),1.f,.3f,true);

	return stage.exec(60, [&controller, &controller_id, &horiz, &space, &stage](double dt){
		controller->move(stage.getAxis(controller_id, horiz));
		if(stage.getButton(controller_id, space))controller->jump();

		return 0;
	});
}
