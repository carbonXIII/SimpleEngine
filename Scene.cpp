/*
 * Scene.cpp
 *
 *  Created on: Aug 6, 2017
 *      Author: Jordan Richards
 */

#include "Scene.h"

#include <cmath>

using namespace std;

void CameraComponent::snap(Vector target){
	parent->move(getCameraLocation() - target);
}

void CameraComponent::setTarget(Object* target, Vector offset){
	this->target = target;
	this->location = offset;
}

void CameraComponent::setTarget(Vector location){
	this->target = nullptr;
	this->location = location;
}

Vector CameraComponent::getCameraLocation(){
	Vector center(res.x, res.y);
	center /= parent->getLocalTransform().scale * 2;

	return -(parent->getLocalTransform().loc - center);
}

void CameraComponent::update(double dt){
	Vector targetLoc(location);
	if(target != nullptr){
		targetLoc += (target->getWorldTransform().loc - parent->getWorldTransform().loc);
	}

	parent->move(Transform((getCameraLocation() - targetLoc)*(dt*max(10.,abs(target->findComponent<PhysicsComponent>()->getVelocity()))/parent->getLocalTransform().scale)));
}

void Scene::init(){
	m_physicsEngine = addComponent<PhysicsEngine>(scale, gravAccel);

	Object::init();
}


