/*
 * PhysicsEngine.h
 *
 *  Created on: Aug 5, 2017
 *      Author: Jordan Richards
 */

#ifndef PHYSICSENGINE_H_
#define PHYSICSENGINE_H_

#include <vector>
#include <functional>
#include <Box2D/Box2D.h>

#include "structures.h"

struct Collider{
	Collider(float width, float height, float x = 0, float y = 0): x(x), y(y), w(width), h(height) {};
	Collider(const Collider& c): x(c.x), y(c.y), w(c.w), h(c.h) {};

	b2PolygonShape toB2Poly(double scale) const{
		b2PolygonShape s;

		float hx = w * scale / 2;
		float hy = h * scale / 2;

		s.SetAsBox(hx,hy,b2Vec2(x*scale+hx,y*scale+hy),0);

		return s;
	}

	float x, y, w, h;
};

struct FixtureInfo{
	Object* parent;
	uint32_t fixtID;
	bool isSensor;

	std::function<void(FixtureInfo*)> begin;
	std::function<void(FixtureInfo*)> end;

	explicit FixtureInfo(Object* parent, uint32_t fixtureID, bool sensor): parent(parent), fixtID(fixtureID), isSensor(sensor), begin(), end() {};
};

class PhysicsEngine;
class PhysicsComponent: public Component{
friend class PhysicsEngine;
public:
	PhysicsComponent(Object* parent, const Collider& collider, float density, float friction, bool fixed = false, bool fixRotation = false, PhysicsEngine* engine = nullptr): Component(parent), engine(engine), collider(collider), fixedRot(fixRotation), fixed(fixed), density(density), friction(friction) {}

	virtual void init();
	virtual void destroy(Object*);
	virtual void update(double dt);

	uint32_t addFixture(const Collider& collider, bool sensor = false);

	void setBeginHandler(uint32_t fixtureID, std::function<void(FixtureInfo*)> callback);
	void setEndHandler(uint32_t fixtureID, std::function<void(FixtureInfo*)> callback);

	FixtureInfo getFixtureInfo(uint32_t fixtureID){
		return m_fixtures[fixtureID];
	}

	Collider getCollider(){
		return collider;
	}

	void addForce(Vector force, bool centered = true, Vector point = Vector());
	void addImpulse(Vector impulse, bool centered = true, Vector point = Vector());

	Vector getVelocity();
	float getMass();

private:
	b2Body* body = nullptr;
	PhysicsEngine* engine;
	Collider collider;

	bool fixedRot;
	bool fixed;
	float density;
	float friction;

	uint32_t fixture_it = 0;
	std::vector<FixtureInfo> m_fixtures;

	FixtureInfo* allocateFixture(bool sensor, uint32_t* it = nullptr);
};

class PhysicsContactListener: public b2ContactListener{
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
};

class PhysicsEngine: public Component{
public:
	PhysicsEngine(Object* parent, double scale, float gravAccel = 9.8f): Component(parent), gravity(0.f,gravAccel), scale(scale) {}

	void init();
	void destroy(Object*);

	void registerComponent(PhysicsComponent* component);
	void removeComponent(PhysicsComponent* component);

	void update(double dt);

	double getScale(){
		return scale;
	}

	Vector getLocation(){
		return parent->getWorldTransform().loc;
	}

	virtual ~PhysicsEngine();

private:
	b2Vec2 gravity;
	PhysicsContactListener contactListener;

	b2World* world = nullptr;
	std::vector<b2Body*> toDestroy;

	void removeDead();

	const int VELOCITY_ITERATIONS = 8;
	const int POSITION_ITERATIONS = 3;

	double scale;
};

#endif /* PHYSICSENGINE_H_ */
