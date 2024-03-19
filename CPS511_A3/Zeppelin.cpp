#include "Zeppelin.h"
#include "utils.h"
#include <math.h>
#include "Propellor.h"
#include <stdlib.h>
#include <stdio.h>
#include  <iostream>
#include <format>
#include <cstdio>
#include "MatrixManager.h"

#define INITIAL_YAW -90

using mm = MatrixManager;


GLfloat robotBody_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat robotBody_mat_specular[] = { 0.45f,0.55f,0.45f,1.0f };
GLfloat robotBody_mat_diffuse[] = { 0.1f,0.35f,0.1f,1.0f };
GLfloat robotBody_mat_shininess[] = { 32.0F };

GLfloat robotArm_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat robotArm_mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat robotArm_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat robotArm_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gun_mat_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat gun_mat_shininess[] = { 100.0F };

GLfloat robotLowerBody_mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat robotLowerBody_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat robotLowerBody_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat robotLowerBody_mat_shininess[] = { 76.8F };

// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


Zeppelin::Zeppelin(float x, float y, float z, float xs, float ys, float zs) : Drawable(4)  {
	this->x = x;
	this->y = y;
	this->z = z;
    this->xStretch = xs;
    this->yStretch = ys;
    this->zStretch = zs;
	this->yawAngle = 0; // start off facing left
	this->pitchAngle = 0;
	this->rollAngle = 0;
	this->hspeed = 0.0;
	this->vspeed = 2;
	this->t = 0;
	this->engineAngle = 0;
	this->hasHealth = true;
	this->health = 100;

	float missileAngle = yawAngle;
	
	this->propAngle = 0;
	
	this->props[0] = new Propellor(PROP_ORIENTATION_LEFT, 0, 0, 1);
	this->props[1] = new Propellor(PROP_ORIENTATION_RIGHT, 0, 0, 1);
	this->yawSpeed = 80;
	this->moveDirection = 0;
	bool fired = false;
	bool hit = false;


	this->body = &meshes[0];
	this->rudder = &meshes[1];
	this->car = &meshes[2];
	missileLoc = glm::vec3(-1, -1, 0);

	body->displacementTune = 4;
	body->mapType = UV_MAP_CYLINDRICAL;
	rudder->mapType = UV_ORTHO;
	car->mapType = UV_MAP_CYLINDRICAL;

	props[0]->meshes[0].meshFilename = (char *)"meshes/zep_prop.obj";
	props[0]->meshes[1].meshFilename = (char*)"meshes/zep_engine_rod.obj";
	props[0]->meshes[2].meshFilename = (char*)"meshes/zep_engine_case.obj";

	props[1]->meshes[0].meshFilename = (char*)"meshes/zep_prop.obj";
	props[1]->meshes[1].meshFilename = (char*)"meshes/zep_engine_rod.obj";
	props[1]->meshes[2].meshFilename = (char*)"meshes/zep_engine_case.obj";

	props[0]->meshes[0].vertFileName = (char*)"a3.vert";
	props[0]->meshes[0].fragFileName = (char*)"a3.frag";
	props[0]->sameShaderForAll();

	props[1]->meshes[0].vertFileName = (char*)"a3.vert";
	props[1]->meshes[0].fragFileName = (char*)"a3.frag";
	props[1]->sameShaderForAll();

	props[0]->meshes[0].texture.textureFolder = (char*)"textures/propellor_blade";
	props[0]->meshes[1].texture.textureFolder = (char*)"textures/engine_housing_rotor";
	props[0]->meshes[2].texture.textureFolder = (char*)"textures/engine_housing";

	props[1]->meshes[0].texture.textureFolder = (char*)"textures/propellor_blade";
	props[1]->meshes[1].texture.textureFolder = (char*)"textures/engine_housing_rotor";
	props[1]->meshes[2].texture.textureFolder = (char*)"textures/engine_housing";

	props[0]->assignShaders();
	props[0]->loadMeshes();

	props[1]->assignShaders();
	props[1]->loadMeshes();


}

void Zeppelin::fire() {
	if (!fired) {
		float fdvx = -glm::cos(glm::radians(yawAngle));
		float fdvz = glm::sin(glm::radians(yawAngle));
		missleFDV = glm::normalize(glm::vec3(fdvx, 0, fdvz));
		fired = true;
		missileAngle = yawAngle;
	}

}

void Zeppelin::draw(long t) {

}

void Zeppelin::fall(long t) {

}

void Zeppelin::draw(long t, unsigned short moveDirection) {
	if (hit) {
		health -= 34;
		hit = false;
	}
	if (health < 0) {
		props[0]->hit = true;
		props[1]->hit = true;
	}
	this->t = t;
	this->moveDirection = moveDirection;
	if (fired) {
		missileLoc += 12 * ((float)t/1000000) * missleFDV;
		if (missileLoc.x < -20 || missileLoc.x > 20 || missileLoc.z < -20 || missileLoc.z > 20) {
			fired = false;
		}
		mm::push();
		mm::translate(missileLoc.x, missileLoc.y, missileLoc.z);
		mm::rotate(missileAngle, 0, 1, 0);
		mm::rotate(90, 0, 0, 1);;
		mm::scale(0.1, 0.25, 0.1);
		drawMesh(&this->meshes[3], this);
		mm::pop();
	}
	//std::cout << "movedir: " << moveDirection << "\n";
	mm::push();
	
	this->move();
	//mm::rotate(this->yawAngle - INITIAL_YAW, 0, 1, 0);
	this->drawBody();
	this->props[0]->draw(float(t)/1000000, this->moveDirection);
	this->props[1]->draw(float(t) / 1000000, this->moveDirection);
	this->drawRudders();
	this->drawElevators();
	this->drawCar();
//	drawMesh(&props[0]->meshes[0], props[0]);
	
	mm::pop();

	
}

void Zeppelin::drawCar() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);
	mm::push();
	mm::translate(0, -1, 0);
	if (health < 0) {
	mm::rotate(45, 0, 0, 1);
	}
	mm::scale(0.15, 0.09, 0.15);
	drawMesh(&this->meshes[2], this);
	mm::pop();
}

void Zeppelin::drawBody() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotLowerBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotLowerBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotLowerBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotLowerBody_mat_shininess);

	
	mm::push();
	mm::translate(-1, -1, 0);
	mm::rotate(90, 0, 0, 1);;
	mm::scale(0.1, 0.25, 0.1);
	drawMesh(&this->meshes[3], this);
	mm::pop();
	

	mm::push();
	if (health < 0) {
		mm::rotate(66, 0, 0, 1);
	}
	mm::rotate(90, 0, 0, 1);
	mm::scale(this->xStretch, this->yStretch, this->zStretch);
	Drawable::drawMesh(&this->meshes[0], this);
	mm::pop();
}



void Zeppelin::drawRudders() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);
	mm::push();
	
	mm::translate(2, -0.6, 0.0);
	if (health < 0) {
		mm::translate(1, -0.5, -0.6);
		mm::rotate(78, 1, 0, 0);
	}
	mm::scale(0.3, 0.1, 0.1);
	drawMesh(this->rudder, this);
	mm::pop();

	mm::push();
	mm::translate(2, 0.6, 0.0);
	if (health < 0) {
		mm::translate(-3, 1, -0.2);
		mm::rotate(25, 1, 0, 0);
	}
	mm::scale(0.3, 0.1, 0.1);
	drawMesh(this->rudder, this);
	mm::pop();
}

void Zeppelin::drawElevators() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	mm::push();
	mm::translate(2, 0, 0.6);
	if (health < 0) {
		mm::translate(-2, -1, 0.3);
		mm::rotate(49, 1, 0, 0);
	}
	mm::rotate(90, 1, 0, 0);
	mm::scale(0.3, 0.1, 0.1);
	drawMesh(this->rudder, this);
	mm::pop();

	mm::push();
	mm::translate(2, 0, -0.6);
	if (health < 0) {
		mm::translate(1, 2, 1);
		mm::rotate(37, 1, 0, 0);
	}
	mm::rotate(90, 1, 0, 0);
	mm::scale(0.3, 0.1, 0.1);
	drawMesh(this->rudder, this);
	mm::pop();
}

void Zeppelin::yaw() {
	float t2 = (float)this->t / 1000000;
	// can't turn left and right at same time
	
	if ((this->moveDirection & (ZEP_LEFT | ZEP_RIGHT)) == (ZEP_LEFT | ZEP_RIGHT)) {
		return;
		std::cout << "none\n";
	}
	else if ((this->moveDirection & ZEP_LEFT) == ZEP_LEFT) {
		this->yawAngle += this->yawSpeed * t2;
		while (this->yawAngle > 360) {
			this->yawAngle -= 360;
		}
		while (yawAngle < 0) {
			this->yawAngle += 360;
		}
		assert(this->yawAngle >= 0);
		assert(this->yawAngle <= 360);
		std::cout << "left\n";
		if (this->hspeed == 0) {
			this->props[0]->SetEngineSpeed(-60);
			this->props[1]->SetEngineSpeed(60);
		}
		else if (this->hspeed != 0) {
			this->props[0]->SetEngineSpeed(60);
			this->props[1]->SetEngineSpeed(120);
		}
	}
	else if ((this->moveDirection & ZEP_RIGHT) == ZEP_RIGHT) {
		this->yawAngle -= this->yawSpeed * t2;
		while (yawAngle < 0) {
			this->yawAngle += 360;
		}
		while (this->yawAngle > 360) {
			this->yawAngle -= 360;
		}
		assert(this->yawAngle >= 0);
		assert(this->yawAngle <= 360);
		std::cout << "right\n";
		if (this->hspeed == 0) {
			this->props[0]->SetEngineSpeed(60);
			this->props[1]->SetEngineSpeed(-60);
		}
		else if (this->hspeed != 0) {
			this->props[0]->SetEngineSpeed(120);
			this->props[1]->SetEngineSpeed(60);
		}
	}
	
}

void Zeppelin::liftLand() {
	float t2 = (float)this->t / 1000000;
	if ((this->moveDirection & (ZEP_UP | ZEP_DOWN)) == (ZEP_UP | ZEP_DOWN)) {
		
		return;
	}
	else if ((this->moveDirection & ZEP_UP) == ZEP_UP) {
		this->props[0]->SetEngineSpeed(60);
		this->props[1]->SetEngineSpeed(60);
		this->y += this->vspeed * t2;
	}
	else if ((this->moveDirection & ZEP_DOWN) == ZEP_DOWN) {
		this->y -= this->vspeed * t2;
		this->props[0]->SetEngineSpeed(-60);
		this->props[1]->SetEngineSpeed(-60);
	}
}

void Zeppelin::move() {
	if (health < 0) {
		if (this->y < 1) {
			mm::translate(this->x, this->y, this->z);
			return;
		}
		this->y -= this->vspeed * ((float)this->t / 1000000);
		mm::translate(this->x, this->y, this->z);
		return;
	}
	float seconds = (float)this->t / 1000000; // micro seconds to seconds
	bool p1 = this->props[0]->startLiftLandProcess(this->moveDirection);
	bool p2 = this->props[1]->startLiftLandProcess(this->moveDirection);
	if (p1 && p2) {
		this->yaw();
		this->liftLand();
	}
	
	float yawAngleRad = this->yawAngle * (3.14159265358979 / 180.0);
	float fdvx = -cos(yawAngleRad);
	float fdvz = sin(yawAngleRad);
	//printf("x-angle: %f, yangle: %f", fdvx, fdvz);
	
	mm::translate(this->x + fdvx * seconds * hspeed, this->y,  this->z + fdvz * seconds * hspeed);
	mm::rotate(this->yawAngle, 0, 1, 0);
	if (!fired) {
		missileLoc = mm::getCTM() * glm::vec4(-1, -1, 0, 1);
	}
	this->x += fdvx * seconds * hspeed;
	this->z += fdvz * seconds * hspeed;
}

void Zeppelin::start(unsigned long moveDirection) {

	this->hspeed = 2;
	this->props[0]->SetEngineSpeed(60);
	this->props[1]->SetEngineSpeed(60);
}

void Zeppelin::stop() {
	this->hspeed = 0;
	//this->vspeed = 0;
	this->props[0]->SetEngineSpeed(0);
	this->props[1]->SetEngineSpeed(0);
}

void Zeppelin::setupProps() {

}


