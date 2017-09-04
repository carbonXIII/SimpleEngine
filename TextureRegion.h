/*
 * TextureRegion.h
 *
 *  Created on: Aug 8, 2017
 *      Author: Jordan Richards
 */

#ifndef TEXTUREREGION_H_
#define TEXTUREREGION_H_

#include "structures.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>
#include <unordered_map>

class TextureRegion{
public:
	TextureRegion(): parent(nullptr) {};
	TextureRegion(SDL_Texture* texture, const SDL_Rect& rect, const SDL_Point& origin = {0,0});
	TextureRegion(SDL_Texture* texture, int x, int y, int width, int height, int originX, int originY);
	TextureRegion(SDL_Texture* texture);
	TextureRegion(const TextureRegion& other);

	SDL_Texture* getParent(){
		return parent;
	}

	const SDL_Rect& getROI(){
		return region;
	}

	const SDL_Point& getOrigin(){
		return origin;
	}

	void renderCopy(SDL_Renderer* renderer, Transform transform);

	std::vector<TextureRegion> split(int width, int height, int nX = -1, int nY = -1, const SDL_Point& offset = {0,0});

private:
	SDL_Texture* parent;
	SDL_Rect region;
	SDL_Point origin;
};

class TextureManager: public Component{
public:
	TextureManager(Object* parent);
	virtual void init() {}
	virtual void update(double) {}
	virtual void destroy(Object*) {}

	TextureRegion load(const std::string& file, const SDL_Rect& rect = {0,0,0,0}, const SDL_Point& origin = {0,0});
	TextureRegion create(const std::string& name, const SDL_Point& size);

private:
	std::unordered_map<std::string, SDL_Texture*> textures;

	SDL_Renderer* m_renderer = nullptr;
};

class TextureAtlas{
public:
	TextureAtlas(TextureRegion texture, int width, int height): width(width), height(height), texture(texture) {};

	void addSequence(const std::string& name, const SDL_Point& offset, int nX, int nY);
	void extendSequence(const std::string& name, const SDL_Point& offset, int nX, int nY);

	std::vector<TextureRegion>& getSequence(std::string name) { return sequences[name]; }

	int getWidth() { return width; }
	int getHeight() { return height; }

private:
	std::unordered_map<std::string, std::vector<TextureRegion>> sequences;

	int width, height;
	TextureRegion texture;
};


#endif /* TEXTUREREGION_H_ */
