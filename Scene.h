/*
 * Scene.h
 *
 *  Created on: Aug 6, 2017
 *      Author: Jordan Richards
 */

#ifndef SCENE_H_
#define SCENE_H_

#include "structures.h"
#include "PhysicsEngine.h"

#include <iostream>

class CameraComponent: public Component{
public:
	CameraComponent(Object* parent, const SDL_Point& resolution, bool smooth = true): Component(parent), res(resolution), smooth(smooth), location(res.x/2,res.y/2) {}

	virtual void init() {};
	virtual void destroy(Object* guardian) {};

	virtual void update(double dt);

	void snap(Vector target);
	void setTarget(Object* target, Vector offset = Vector());
	void setTarget(Vector location);

	void resize(const SDL_Point& resolution){
		this->res = resolution;

		Vector targetLoc(location);
		if(target != nullptr){
			targetLoc += (target->getWorldTransform().loc - parent->getWorldTransform().loc);
		}snap(targetLoc);
	}

	Vector getCameraLocation();

private:
	SDL_Point res;
	bool smooth;

	Object* target = nullptr;
	Vector location;
};

class Scene: public Object{
public:
	Scene(double scale, float gravAccel = 9.8f, Transform transform = Transform(), Object* parent = nullptr): Object(transform,parent), scale(scale), gravAccel(gravAccel) {};

	virtual void init();

	PhysicsEngine* getPhysicsEngine(){
		return static_cast<PhysicsEngine*>(m_physicsEngine);
	}

private:
	double scale;
	float gravAccel;
	Component* m_physicsEngine = nullptr;
};

#endif /* SCENE_H_ */
