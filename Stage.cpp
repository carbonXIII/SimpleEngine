/*
 * Stage.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: Jordan Richards
 */

#include "Stage.h"

#include <iostream>
#include <windows.h>

using namespace std;

void GameController::set(KeyInput key, bool down){
	key.type == KeyInput::Type::BUTTON ? setButton(key.id, down) : setAxis(key.id, key.type, down);
}

void GameController::setButton(uint64_t mask, bool down){
	if(down)buttons |= mask;
	else buttons &= ~mask;
}

void GameController::setAxis(uint64_t id, KeyInput::Type type, bool down){
	double& axis = axes[id];

	if((type == KeyInput::Type::AXIS_NEG && down) || (type == KeyInput::Type::AXIS_POS && !down))axis -= 1;
	else axis += 1;

	if(axis < -1)axis = -1;
	if(axis > 1)axis = 1;
}

uint64_t Stage::addController(){
	auto ret = controllers.size();
	controllers.emplace_back();
	return ret;
}

uint64_t Stage::registerInput(uint64_t controller, SDL_Keycode code){
	KeyInput k;
	k.type = KeyInput::Type::BUTTON;
	k.controller_id = controller;
	k.id = controllers[controller].getNextButton();

	key_controls[code] = k;

	return k.id;
}

uint64_t Stage::registerAxis(uint64_t controller, SDL_Keycode neg, SDL_Keycode pos){
	KeyInput kNeg;
	kNeg.controller_id = controller;
	kNeg.id = controllers[controller].getNextAxis();
	KeyInput kPos = kNeg;

	kNeg.type = KeyInput::Type::AXIS_NEG;
	kPos.type = KeyInput::Type::AXIS_POS;

	key_controls[neg] = kNeg;
	key_controls[pos] = kPos;

	return kNeg.id;
}

bool Stage::getButton(uint64_t controller, uint64_t mask){
	return controllers[controller].getButton(mask);
}

double Stage::getAxis(uint64_t controller, uint64_t id){
	return controllers[controller].getAxis(id);
}

void Stage::handleInput(SDL_Keycode& key, bool down){
	auto it = key_controls.find(key);
	if(it != key_controls.end()){
		controllers[it->second.controller_id].set(it->second,down);
	}
}

int Stage::exec(double desiredFPS, std::function<int(double)> customUpdate){
	init();

	double lastTime = SDL_GetTicks();
	double dt = 0;
	while(state != State::STOP){
		if(state == State::PAUSE)dt = 0;

		SDL_RenderClear(renderer);
		draw(renderer);
		SDL_RenderPresent(renderer);

		SDL_Event e;
		while(SDL_PollEvent(&e)){
			switch(e.type){
			case SDL_QUIT:
				quit();
				break;
			case SDL_KEYUP:
				handleInput(e.key.keysym.sym, false);
				break;
			case SDL_KEYDOWN:
				handleInput(e.key.keysym.sym, true);
				break;
			case SDL_WINDOWEVENT:
				if(e.window.event == SDL_WINDOWEVENT_RESIZED){
					for(Scene* scene : scenes)scene->findComponent<CameraComponent>()->resize({e.window.data1, e.window.data2});
				}break;
			}
		}

		customUpdate(dt);
		update(dt);

		double time = SDL_GetTicks();
		dt = (time - lastTime) / 1000.;

		if(dt < (1./desiredFPS)){
			Sleep(((1./desiredFPS) - dt)*1000.);//windows only, linux use msleep
			dt = 1./desiredFPS;
		}

		lastTime = time;
	}

	SDL_Quit();
	return 0;
}

