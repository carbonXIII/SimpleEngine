/*
 * CharacterComponent.h
 *
 *  Created on: Aug 7, 2017
 *      Author: Jordan Richards
 */

#ifndef CHARACTERCOMPONENT_H_
#define CHARACTERCOMPONENT_H_

#include "PhysicsEngine.h"
#include "structures.h"

struct CharacterComponentParams{
	double jumpAccelDuration = .1;
	double jumpAccel = 60.;
	double airDampening = .5;
	double maxVelocity = 30.;
	double maxAirVelocity = 15.;

	CharacterComponentParams() {}
};

class CharacterComponent: public Component{
public:
	CharacterComponent(Object* parent, CharacterComponentParams params = CharacterComponentParams(), PhysicsComponent* physicsController = nullptr): Component(parent), controller(physicsController), jumpAccelDuration(params.jumpAccelDuration), jumpAccel(params.jumpAccel), airDampening(params.airDampening), maxVelocity(params.maxVelocity), maxAirVelocity(params.maxAirVelocity) {}

	virtual void init();
	virtual void destroy(Object*) {}
	virtual void update(double dt);

	bool isGrounded();
	bool isJumping();

	void move(double normal);
	void jump();

private:
	PhysicsComponent* controller;

	uint32_t bottom_sensor = -1;

	int grounded = 0;
	double jumpTime = -1;

	double jumpAccelDuration;
	double jumpAccel;
	double airDampening;
	double maxVelocity;
	double maxAirVelocity;
};

#endif /* CHARACTERCOMPONENT_H_ */
