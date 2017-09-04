/*
 * PhysicsEngine.cpp
 *
 *  Created on: Aug 5, 2017
 *      Author: Jordan Richards
 */

#include "PhysicsEngine.h"

#include <iostream>
#include <math.h>

using namespace std;

void PhysicsComponent::init(){
	if(engine == nullptr)engine = parent->findComponentInHeirarchy<PhysicsEngine>();
	//TODO: deal with error handling (when engine is still nullptr)

	engine->registerComponent(this);
}

void PhysicsComponent::destroy(Object*){
	engine->removeComponent(this);
}

void PhysicsComponent::update(double dt){
	b2Vec2 pos = body->GetPosition();
	float rot = body->GetAngle() * 180. / M_PI;

	double scale = engine->getScale();
	Vector newLoc(pos.x/scale, pos.y/scale);
	newLoc += engine->getLocation();

	parent->teleportToWorld(Transform(newLoc, rot, parent->getWorldTransform().scale));
}

FixtureInfo* PhysicsComponent::allocateFixture(bool sensor, uint32_t* it){
	m_fixtures.emplace_back(parent, fixture_it, sensor);
	if(it != nullptr)*it = fixture_it;
	auto temp = fixture_it++;
	return m_fixtures.data() + temp;
}

uint32_t PhysicsComponent::addFixture(const Collider& collider, bool sensor){
	auto box = collider.toB2Poly(engine->getScale());

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.isSensor = sensor;
	fixture.density = density;
	fixture.friction = friction;

	uint32_t ret;
	fixture.userData = (void*)allocateFixture(sensor, &ret);
	body->CreateFixture(&fixture);

	return ret;
}

void PhysicsComponent::setBeginHandler(uint32_t fixtureID, function<void(FixtureInfo*)> callback){
	m_fixtures[fixtureID].begin = callback;
}

void PhysicsComponent::setEndHandler(uint32_t fixtureID, function<void(FixtureInfo*)> callback){
	m_fixtures[fixtureID].end = callback;
}

void PhysicsComponent::addForce(Vector force, bool centered, Vector point){
	if(centered)body->ApplyForceToCenter(b2Vec2(force.x,force.y),true);
	else body->ApplyForce(b2Vec2(force.x,force.y),b2Vec2(point.x,point.y),true);
}

void PhysicsComponent::addImpulse(Vector force, bool centered, Vector point){
	if(centered)body->ApplyLinearImpulseToCenter(b2Vec2(force.x,force.y),true);
	else body->ApplyLinearImpulse(b2Vec2(force.x,force.y),b2Vec2(point.x,point.y),true);
}

Vector PhysicsComponent::getVelocity(){
	b2Vec2 vel = body->GetLinearVelocity();
	return Vector(vel.x,vel.y);
}

float PhysicsComponent::getMass(){
	return body->GetMass();
}

void PhysicsContactListener::BeginContact(b2Contact* contact){
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	FixtureInfo* aInfo = (fixtureA->GetUserData() != nullptr) ? static_cast<FixtureInfo*>(fixtureA->GetUserData()) : nullptr;
	FixtureInfo* bInfo = (fixtureB->GetUserData() != nullptr) ? static_cast<FixtureInfo*>(fixtureB->GetUserData()) : nullptr;

	if(aInfo->begin)aInfo->begin(bInfo);
	if(bInfo->begin)bInfo->begin(aInfo);
}

void PhysicsContactListener::EndContact(b2Contact* contact){
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	FixtureInfo* aInfo = (fixtureA->GetUserData() != nullptr) ? static_cast<FixtureInfo*>(fixtureA->GetUserData()) : nullptr;
	FixtureInfo* bInfo = (fixtureB->GetUserData() != nullptr) ? static_cast<FixtureInfo*>(fixtureB->GetUserData()) : nullptr;

	if(aInfo->end)aInfo->end(bInfo);
	if(bInfo->end)bInfo->end(aInfo);
}

void PhysicsEngine::init(){
	world = new b2World(gravity);

	world->SetContactListener(&contactListener);
}

void PhysicsEngine::destroy(Object*){
	delete world;
}

void PhysicsEngine::registerComponent(PhysicsComponent* component){
	b2BodyDef bodyDef;

	bodyDef.type = component->fixed ? b2_staticBody : b2_dynamicBody;
	bodyDef.fixedRotation = component->fixedRot;
	Transform t = component->parent->getWorldTransform() - parent->getWorldTransform();
	Vector pos = t.loc * scale;
	bodyDef.position.Set(pos.x, pos.y);
	bodyDef.angle = t.rot;
	bodyDef.userData = (void*)component->getParent();

	component->body = world->CreateBody(&bodyDef);

	component->addFixture(component->collider, false);
}

void PhysicsEngine::update(double dt){
	world->Step(dt, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	removeDead();
}

void PhysicsEngine::removeComponent(PhysicsComponent* component){
	toDestroy.push_back(component->body);
}

PhysicsEngine::~PhysicsEngine(){
	delete world;
}

void PhysicsEngine::removeDead(){
	for(b2Body* body : toDestroy){
		world->DestroyBody(body);
	}toDestroy.clear();
}
