/*
 * Sprite.cpp
 *
 *  Created on: Jul 29, 2017
 *      Author: Jordan Richards
 */

#include "Sprite.h"

using namespace std;

void Sprite::draw(SDL_Renderer* rend){
	texture.renderCopy(rend, getWorldTransform());

	Object::draw(rend);
}

void AnimatedSprite::update(double dt){
	texture = atlas.getSequence(name)[(int)curFrame];

	curFrame += (dt * FPS);
	curFrame = fmod(curFrame, atlas.getSequence(name).size());

	Sprite::update(dt);
}

void AnimatedSprite::addAnimation(const string& name, const SDL_Point& offset, int nX, int nY) {
	atlas.addSequence(name, {offset.x*atlas.getWidth(),offset.y*atlas.getHeight()}, nX, nY);
	if(this->name.empty()){
		this->name = name;
		curFrame = 0;
		texture = atlas.getSequence(name)[0];
	}
}

void AnimatedSprite::extendAnimation(const string& name, const SDL_Point& offset, int nX, int nY) {
	atlas.extendSequence(name, {offset.x*atlas.getWidth(),offset.y*atlas.getHeight()}, nX, nY);
}

void AnimatedSprite::setAnimation(const string& name, int frame){
	this->name = name;
	this->curFrame = frame % atlas.getSequence(name).size();
}
