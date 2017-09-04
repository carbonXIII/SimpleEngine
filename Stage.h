/*
 * Stage.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Jordan Richards
 */

#ifndef STAGE_H_
#define STAGE_H_

#include <functional>
#include <unordered_map>

#include "structures.h"
#include "TextureRegion.h"
#include "Text.h"
#include "Scene.h"

#include <SDL2/SDL.h>

struct KeyInput{
	uint64_t id;
	uint64_t controller_id;
	enum class Type: char {BUTTON, AXIS_POS, AXIS_NEG} type;
};

struct GameController{
	bool getButton(uint64_t mask){
		return !!(buttons & mask);
	}

	double getAxis(uint64_t id){
		return axes[id];
	}

	uint64_t getNextAxis(){
		auto ret = axes.size();
		axes.push_back(0);
		return ret;
	}

	uint64_t getNextButton(){
		auto ret = nextMask;
		nextMask <<= 1;
		return ret;
	}

	void set(KeyInput key, bool down);

private:
	std::vector<double> axes;
	uint64_t buttons = 0;
	uint64_t nextMask = 1;

	void setButton(uint64_t mask, bool down);
	void setAxis(uint64_t id, KeyInput::Type type, bool down);
};

class Stage: public Object {
public:
	Stage(const char* title, int width, int height, Pixel clearColor, Transform transform = Transform(), Object* parent = nullptr): Object(transform, parent), width(width), height(height), clearColor(clearColor) {
		SDL_Init(SDL_INIT_EVERYTHING);
		atexit(SDL_Quit);

		SDL_CreateWindowAndRenderer(width,height,SDL_WINDOW_RESIZABLE,&win,&renderer);
		SDL_SetWindowTitle(win, title);
		SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);

		m_textureManager = static_cast<TextureManager*>(addComponent<TextureManager>());
		addComponent<FontManager>();
	};

	int exec(double desiredFPS, std::function<int(double)> customUpdate);

	void pause() { state = State::PAUSE; }
	void play() { state = State::PLAY; }
	void quit() { state = State::STOP; }

	uint64_t addController();

	uint64_t registerInput(uint64_t controller, SDL_Keycode code);
	uint64_t registerAxis(uint64_t controller, SDL_Keycode neg, SDL_Keycode pos);

	bool getButton(uint64_t controller, uint64_t mask);
	double getAxis(uint64_t controller, uint64_t id);

	void connectScene(Scene* scene) { scenes.push_back(scene); }

	SDL_Renderer* getRenderer() { return renderer; }
	TextureManager& getTextureManager() { return *m_textureManager; }

protected:
	enum class State {PLAY, PAUSE, STOP} state = State::PLAY;

	std::unordered_map<int32_t, KeyInput> key_controls;

	std::vector<GameController> controllers;

	TextureManager* m_textureManager;

private:
	void handleInput(SDL_Keycode& key, bool down);

	std::vector<Scene*> scenes;

	SDL_Window* win = nullptr;
	SDL_Renderer* renderer = nullptr;

	int width, height;
	Pixel clearColor;
};

#endif /* STAGE_H_ */
