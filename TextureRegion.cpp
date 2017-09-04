/*
 * TextureRegion.cpp
 *
 *  Created on: Aug 8, 2017
 *      Author: Jordan Richards
 */

#include "TextureRegion.h"
#include "Stage.h"
#include <limits.h>

using namespace std;

TextureRegion::TextureRegion(SDL_Texture* texture, const SDL_Rect& rect, const SDL_Point& origin){
	parent = texture;
	region = rect;
	this->origin = origin;
}

TextureRegion::TextureRegion(SDL_Texture* texture, int x, int y, int width, int height, int originX, int originY){
	parent = texture;
	region = {x,y,width,height};
	this->origin = {originX, originY};
}

TextureRegion::TextureRegion(SDL_Texture* texture){
	parent = texture;

	uint32_t format;
	int access, w, h;
	SDL_QueryTexture(texture, &format, &access, &w, &h);
	region = {0,0,w,h};
	origin = {0,0};
}

TextureRegion::TextureRegion(const TextureRegion& other){
	parent = other.parent;
	region = other.region;
	origin = other.origin;
}

void TextureRegion::renderCopy(SDL_Renderer* rend, Transform t){
	SDL_Rect dest {t.loc.x,t.loc.y,region.w*t.scale,region.h*t.scale};

	if(t.rot == 0){
		SDL_RenderCopy(rend, this->parent, &region, &dest);
	}else{
		SDL_RenderCopyEx(rend, this->parent, &region, &dest, t.rot, &origin, SDL_FLIP_NONE);
	}
}

TextureManager::TextureManager(Object* parent): Component(parent){
	Stage* stage = nullptr;
	Object* curParent = parent;
	while(curParent != nullptr){
		stage = dynamic_cast<Stage*>(curParent);
		if(stage != nullptr)break;
		curParent = curParent->getParent();
	}

	if(stage != nullptr)m_renderer = stage->getRenderer();
}

TextureRegion TextureManager::load(const string& file, const SDL_Rect& rect, const SDL_Point& origin){
	auto it = textures.find(file);
	if(it == textures.end()){
		textures[file] = SDL_CreateTextureFromSurface(m_renderer, IMG_Load(file.c_str()));
	}

	if(rect.w == 0 && rect.h == 0)return TextureRegion(textures[file]);
	return TextureRegion(textures[file],rect,origin);
}

TextureRegion TextureManager::create(const string& name, const SDL_Point& size){
	textures[name] = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, size.x, size.y);

	return TextureRegion(textures[name]);
}

vector<TextureRegion> TextureRegion::split(int width, int height, int nX, int nY, const SDL_Point& offset){
	vector<TextureRegion> ret;

	int iX = 0; int iY = 0;
	if(nX == -1)nX = INT_MAX;
	if(nY == -1)nY = INT_MAX;

	SDL_Rect r = {0,0,width,height};
	while(iY < nY){
		r.y = offset.y + iY*height;
		if(r.y + r.h > region.h)break;
		iX = 0;
		while(iX < nX){
			r.x = offset.x + iX*width;
			if(r.x + r.w > region.w)break;

			ret.emplace_back(parent, r);

			iX++;
		}iY++;
	}

	return ret;
}

void TextureAtlas::addSequence(const string& name, const SDL_Point& offset, int nX, int nY){
	auto it = sequences.find(name);
	if(it != sequences.end()){
		it->second.clear();
	}

	sequences[name] = texture.split(width, height, nX, nY, offset);
}

void TextureAtlas::extendSequence(const string& name, const SDL_Point& offset, int nX, int nY){
	auto it = sequences.find(name);
	std::vector<TextureRegion> n = texture.split(width, height, nX, nY, offset);

	if(it == sequences.end()){
		sequences[name] = n;
	}
	else sequences[name].insert(sequences[name].end(), n.begin(), n.end());

	sequences[name] = texture.split(width, height, nX, nY, offset);
}
