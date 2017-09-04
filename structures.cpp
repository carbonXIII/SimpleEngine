/*
 * structures.cpp
 *
 *  Created on: Aug 4, 2017
 *      Author: Jordan Richards
 */
#include "structures.h"

double abs(Vector v){
	return sqrt(v.x*v.x + v.y*v.y);
}

void Object::init(){
	for(Component* comp : components)comp->init();
	for(Object* child : children)child->init();
}

void Object::update(double dt){
	for(Object* child : children)child->update(dt);
	for(Component* component : components)component->update(dt);
}

void Object::draw(SDL_Renderer* renderer){
	for(Object* child : children){
		child->draw(renderer);
	}
}

Transform Object::getLocalTransform(){
	return transform;
}

void Object::updateWorldTransform(){
	needUpdateWorldTransform = true;
	for(Object* child : children)child->updateWorldTransform();
}

Transform Object::getParentWorldTransform(){
	return parent == nullptr ? Transform() : parent->getWorldTransform();
}

Transform Object::getWorldTransform(){
	if(needUpdateWorldTransform){
		worldTransform = getParentWorldTransform() + transform;
		needUpdateWorldTransform = false;
	}
	return worldTransform;
}

void Object::teleportToWorld(Transform transform){
	this->transform = transform - getParentWorldTransform();
	updateWorldTransform();
}

void Object::teleportToLocal(Transform transform){
	this->transform = transform;
	updateWorldTransform();
}

void Object::move(Transform delta){
	this->transform += delta;
	updateWorldTransform();
}

void Object::attachChild(Object* obj){
	if(obj->parent != nullptr)obj->parent->removeChild(obj);
	children.push_back(obj);
	obj->parent = this;
}

void Object::removeChild(Object* obj){
	for(int i = 0; i < children.size(); i++){
		if(children[i] == obj){
			children.erase(children.begin() + i);
			break;
		}
	}
}

void Object::destroy(Object* guardian){
	if(guardian != nullptr){
		for(Object* child : children)guardian->attachChild(child);
	}else{
		for(Object* child : children){
			child->destroy();
		}
	}

	for(Component* comp : components)comp->destroy(guardian);
}

Object::~Object(){
	for(Component* comp : components)delete comp;
}
