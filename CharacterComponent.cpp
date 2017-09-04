/*
 * CharacterComponent.cpp
 *
 *  Created on: Aug 7, 2017
 *      Author: Jordan Richards
 */

#include "CharacterComponent.h"

void CharacterComponent::init(){
	if(controller == nullptr){
		controller = parent->findComponent<PhysicsComponent>();
	}

	Collider bottom_collider = controller->getCollider();

	bottom_collider.y = bottom_collider.h - 10;
	bottom_collider.h = 10;
	bottom_collider.x = 5;
	bottom_collider.w -= 10;

	bottom_sensor = controller->addFixture(bottom_collider, true);
	controller->setBeginHandler(bottom_sensor, [this](FixtureInfo* info) { if(info) grounded++; });
	controller->setEndHandler(bottom_sensor, [this](FixtureInfo* info) { if(info) grounded--; });
}

void CharacterComponent::update(double dt) {
	if(isJumping()){
		controller->addForce(Vector(0,-jumpAccel*controller->getMass()));
		jumpTime += dt;
		if(jumpTime > jumpAccelDuration)jumpTime = -1;
	}
}

bool CharacterComponent::isGrounded(){
	return grounded > 0;
}

bool CharacterComponent::isJumping(){
	return jumpTime >= 0;
}

void CharacterComponent::move(double normal){
	double d = normal*(isGrounded() ?  maxVelocity : maxAirVelocity) - controller->getVelocity().x;
	d *= (1 - airDampening * !isGrounded());
	controller->addForce(Vector(d*controller->getMass(),0));
}

void CharacterComponent::jump(){
	if(isGrounded() && !isJumping())jumpTime = 0;
}
