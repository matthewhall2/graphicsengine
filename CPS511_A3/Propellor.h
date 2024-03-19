#pragma once
#include "utils.h"
#include "World.h"

typedef enum {
	NONE=0,
	ZEP_LEFT = 1,
	ZEP_RIGHT = 2,
	ZEP_UP = 4,
	ZEP_DOWN = 8
} ZEP_DIRECTION;

typedef enum {
	PROP_ORIENTATION_LEFT = 0,
	PROP_ORIENTATION_RIGHT
} Orientation;

class Propellor : public Drawable
{
public:
	Propellor();
	Propellor(Orientation orientation, float fbPos, float udPos, float depth);
	void draw(float t, unsigned short d);
	void SetEngineSpeed(float s);
	void SetEngineBlockRotorSpeed(float s);
	bool startLiftLandProcess(unsigned short a);
	bool hit;

private:
	float engineAngle, propAngle;
	float frontBackPos, upDownPos;
	float depth;
	float engineSpeed, engineBlockSpeed;
	
	void drawEngineCasing();
	void drawBlades();

	float t;
	
	Orientation orientation;
	GLUquadric* propRotor[2];

	Mesh* blade;
	Mesh* rotor;
	Mesh* casing;

};

