/*
 * structures.h
 *
 *  Created on: Jul 29, 2017
 *      Author: Jordan Richards
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <SDL2/SDL.h>
#include <vector>

struct Pixel{
	uint8_t a,r,g,b;

	constexpr Pixel(int r, int g, int b, int a = 255): a(a), r(r), g(g), b(b) {}

	Pixel(const Pixel& other){
		this->b = other.b;
		this->g = other.g;
		this->r = other.r;
		this->a = other.a;
	}
};

struct Vector{
	Vector(): x(0), y(0) {}
	Vector(double x, double y): x(x), y(y) {}
	Vector(const Vector& other): x(other.x), y(other.y) {}

	double x;
	double y;

	Vector& operator+= (const Vector& other){
		x += other.x;
		y += other.y;

		return (*this);
	}

	Vector& operator-= (const Vector& other){
		x -= other.x;
		y -= other.y;

		return (*this);
	}

	Vector operator+ (const Vector& other){
		return Vector(*this)+=other;
	}

	Vector operator- (){
		return Vector(*this)*=-1;
	}

	Vector operator- (const Vector& other){
		return Vector(*this)-=other;
	}

	Vector& operator *= (double scale){
		x *= scale;
		y *= scale;

		return (*this);
	}

	Vector& operator /= (double scale){
		x /= scale;
		y /= scale;

		return (*this);
	}

	Vector operator* (double scale){
		return Vector(*this)*=scale;
	}

	Vector operator/ (double scale){
		return Vector(*this)/=scale;
	}
};

extern double abs(Vector v);

struct Transform{
	Vector loc;
	double rot, scale;

	Transform(): loc(), rot(0), scale(1) {};
	Transform(double x, double y, double rot = 0, double scale = 1): loc(x,y), rot(rot), scale(scale) {}
	Transform(Vector loc, double rot = 0, double scale = 1): loc(loc), rot(rot), scale(scale) {}
	Transform(const Transform& other): loc(other.loc), rot(other.rot), scale(other.scale) {};

	Transform operator+ (const Transform& r){
		return (Transform(*this) += r);
	}

	Transform& operator+= (const Transform& r){
		loc += r.loc;
		rot += r.rot;
		scale *= r.scale;

		return (*this);
	}

	Transform operator- (const Transform& r){
		return (Transform(*this) -= r);
	}

	Transform& operator-= (const Transform& r){
		loc -= r.loc;
		rot -= r.rot;
		scale /= r.scale;

		return (*this);
	}
};

class Object;
class Component{
public:
	Component(Object* parent = nullptr): parent(parent) {};

	virtual void init() = 0;
	virtual void update(double delta_time) = 0;
	virtual void destroy(Object* guardian = nullptr) = 0;

	Object* getParent(){
		return parent;
	}

	virtual ~Component() {}

protected:
	Object* parent;
};

class Object : public Component{
public:
	Object(Transform transform = Transform(), Object* parent = nullptr): Component(), transform(transform) {
		if(parent != nullptr)parent->attachChild(this);
	};

	virtual void init();
	virtual void update(double delta_time);
	virtual void destroy(Object* guardian = nullptr);
	virtual void draw(SDL_Renderer* rend);

	Transform getLocalTransform();
	Transform getWorldTransform();
	void teleportToLocal(Transform transform);
	void teleportToWorld(Transform transform);
	void move(Transform delta);

	void attachChild(Object* obj);
	void removeChild(Object* obj);

	template <typename C, class... Args>
	typename std::enable_if<std::is_base_of<Component,C>::value, Component*>::type addComponent(Args&&... args){
		Component* c = static_cast<Component*>(new C(this,args...));
		components.push_back(c);
		return c;
	}

	template <typename C>
	C* findComponent(){
		for(Component* comp : components){
			C* ret = dynamic_cast<C*>(comp);
			if(ret != nullptr)return ret;
		}return nullptr;
	}

	template <typename C>
	C* findComponentInHeirarchy(){
		Object* curObject = this;
		while(curObject != nullptr){
			C* result = curObject->findComponent<C>();
			if(result)return result;

			curObject = curObject->getParent();
		}return nullptr;
	}

	virtual ~Object();

	inline Transform getParentWorldTransform();

private:
	void updateWorldTransform();

	std::vector<Component*> components;
	std::vector<Object*> children;//TODO: replace with hash table

	Transform transform;

	Transform worldTransform;
	bool needUpdateWorldTransform = true;
};

#endif /* STRUCTURES_H_ */
