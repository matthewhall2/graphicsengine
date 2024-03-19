#pragma once
#include "Propellor.h"
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Zeppelin : public Drawable
{
public:
	
	Zeppelin(float x, float y, float z, float xs, float ys, float zs);
	void draw(long t, unsigned short moveDirection);
	void yaw();
	void move();
	void pitch(float angle);
	void roll(float angle);
	void liftLand();

	void start(unsigned long md);
	void stop();
	void draw(long) override;

	

	Propellor *props[2];

	void setupProps();
	float x, y, z;
	float yawAngle, pitchAngle, rollAngle;

	glm::vec3 missileLoc;
	glm::vec3 missleFDV;
	bool hit;
	bool fired;
	float missileAngle;

	void fall(long t);

	void fire();


private:
	
	float xStretch, yStretch, zStretch;
	

	float hspeed, vspeed;
	long t;
	float engineAngle;
	float propAngle;
	float yawSpeed;
	unsigned short moveDirection;


	void drawBody();
	void drawRudders();
	void drawElevators();
	void drawPropellors();
	void drawPropellorShaft();
	void drawProperllorEngineCasing();
	void drawPropellorBlades();
	void drawCar();

	Mesh* body;
	Mesh* rudder;
	Mesh* car;

};

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
extern GLfloat robotBody_mat_ambient[] ;
extern GLfloat robotBody_mat_specular[] ;
extern GLfloat robotBody_mat_diffuse[] ;
extern GLfloat robotBody_mat_shininess[] ;


extern GLfloat robotArm_mat_ambient[] ;
extern GLfloat robotArm_mat_diffuse[] ;
extern GLfloat robotArm_mat_specular[] ;
extern GLfloat robotArm_mat_shininess[] ;

extern GLfloat gun_mat_ambient[] ;
extern GLfloat gun_mat_diffuse[] ;
extern GLfloat gun_mat_specular[] ;
extern GLfloat gun_mat_shininess[] ;

extern GLfloat robotLowerBody_mat_ambient[] ;
extern GLfloat robotLowerBody_mat_diffuse[] ;
extern GLfloat robotLowerBody_mat_specular[] ;
extern GLfloat robotLowerBody_mat_shininess[] ;


// Light properties
extern GLfloat light_position0[] ;
extern GLfloat light_position1[] ;
extern GLfloat light_diffuse[] ;
extern GLfloat light_specular[] ;
extern GLfloat light_ambient[] ;
