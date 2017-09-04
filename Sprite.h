/*
 * Sprite.h
 *
 *  Created on: Jul 29, 2017
 *      Author: Jordan Richards
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "structures.h"
#include "TextureRegion.h"
#include <SDL2/SDL.h>

class Sprite: public Object{
public:
	Sprite(const TextureRegion& texture, Transform transform = Transform(), Object* parent = nullptr): Object(transform, parent), texture(texture) {};
	Sprite(SDL_Texture* texture, Transform transform = Transform(), Object* parent = nullptr): Object(transform, parent), texture(texture) {};

	virtual void draw(SDL_Renderer* renderer);

protected:
	TextureRegion texture;
};

class AnimatedSprite: public Sprite{
public:
	AnimatedSprite(TextureRegion texture, int chunkWidth, int chunkHeight, int FPS, Transform transform = Transform(), Object* parent = nullptr): Sprite(TextureRegion(),transform, parent), atlas(texture, chunkWidth, chunkHeight), FPS(FPS) {};

	void addAnimation(const std::string& name, const SDL_Point& offset, int nX, int nY);
	void extendAnimation(const std::string& name, const SDL_Point& offset, int nX, int nY);

	void setAnimation(const std::string& name, int frame = 0);

	virtual void update(double dt);

private:
	TextureAtlas atlas;

	std::string name;
	double curFrame = 0;
	int FPS;
};

#endif /* SPRITE_H_ */
