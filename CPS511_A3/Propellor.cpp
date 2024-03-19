#include "Propellor.h"
#include "Zeppelin.h"
#include "MatrixManager.h"

using mm = MatrixManager;


#define ENGINE_BLOCK_MOVE_SPEED -90


Propellor::Propellor(Orientation orientation, float fbPos, float udPos, float depth) : Drawable(3) {
	this->orientation = orientation;
	this->frontBackPos = fbPos;
	this->upDownPos = udPos;
	this->engineAngle = 0;
	this->propAngle = 0;
	this->depth = depth;
	this->engineSpeed = 0;
	this->engineBlockSpeed = 0;
	this->t = 0;
	bool hit = false;

	//propRotor[0] = gluNewQuadric();
//	propRotor[1] = gluNewQuadric();

	this->blade = &meshes[0];
	this->rotor = &meshes[1];
	this->casing = &meshes[2];

	rotor->mapType = UV_MAP_CYLINDRICAL;
	blade->mapType = UV_MAP_CYLINDRICAL;
	casing->mapType = UV_MAP_CYLINDRICAL;

}

Propellor::Propellor() : Drawable(1) {

}

void Propellor::draw(float t, unsigned short d) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);
	//this->startLiftLandProcess(d);
	this->engineAngle += t * this->engineBlockSpeed;
	this->propAngle += t * this->engineSpeed;
	this->t = t;

	mm::push();
	mm::translate(0, 0, (this->orientation == PROP_ORIENTATION_LEFT) * depth + (this->orientation == PROP_ORIENTATION_RIGHT) * -depth);
	mm::rotate(engineAngle, 0, 0, 1);
	if (this->orientation == PROP_ORIENTATION_RIGHT) {
		mm::rotate(180, 1, 0, 0);
	}
    //mm::rotate(180, 1 * (this->orientation == PROP_ORIENTATION_RIGHT), 0, 0);
	this->drawEngineCasing();

	mm::push();
	mm::rotate(90, 1, 0, 0);
	if (hit) {
		mm::translate(1, -0.5, -0.6);
		mm::rotate(78, 1, 0, 0);
		mm::scale(0.1, 0.1, 0.1);
	}
	else {
		mm::rotate(90, 1, 0, 0);
		mm::scale(0.1, 0.1, 0.1);
	}
	drawMesh(rotor, this); // engine rotator
	mm::pop();
	mm::pop();
}

void Propellor::drawEngineCasing() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	mm::push();
	mm::translate(0, 0, 0.5);
	this->drawBlades();

	mm::push();
	mm::translate(-0.3, 0, 0);
	if (hit) {
		mm::translate(-0.7, -0.5, -0.6);
		mm::rotate(10, 1, 1, 0);
	}
	mm::scale(0.05, 0.05, 0.05);
	mm::rotate(90, 0, 0, 1);
	drawMesh(rotor, this);
	mm::pop();

	mm::push();
	//mm::translate(0, 0, 0);
	if (hit) {
		mm::translate(1, -0.5, -0.6);
		mm::rotate(78, 1, 0, 0);
	}
	mm::scale(0.1, 0.1, 0.1);
	mm::rotate(90, 0, 0, 1);
	drawMesh(casing, this);
	mm::pop();

	mm::pop();
}

void Propellor::drawBlades() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);
	mm::push();
	mm::translate(-0.4, 0, 0);
	mm::rotate(this->propAngle, 1, 0, 0);

	mm::push();
	mm::rotate(60, 1, 0, 0);
	if (hit) {
		mm::translate(0, 0.4, -0.1);
		mm::rotate(78, 1, 1, 0);
	}
	mm::translate(0, 0, 0.15);
	mm::rotate(-45, 0, 0, 1);
	mm::rotate(90, 1, 0, 0);
	mm::scale(0.05, 0.1, 0.25);
	drawMesh(blade, this);
	mm::pop();

	mm::push();
	mm::rotate(-60, 1, 0, 0);
	if (hit) {
		mm::translate(-0.1, -0.3, -0.2);
		mm::rotate(69, 1, 0, 1);
	}
	mm::translate(0, 0, 0.15);
	mm::rotate(-45, 0, 0, 1);
	mm::rotate(90, 1, 0, 0);
	mm::scale(0.05, 0.1, 0.25);
	drawMesh(blade, this);
	mm::pop();

	mm::push();
	mm::rotate(180, 1, 0, 0);
	if (hit) {
		mm::translate(0.2, -0.1, 0.6);
		mm::rotate(78, 1, 0, 0);
	}
	mm::translate(0, 0, 0.15);
	mm::rotate(-45, 0, 0, 1);
	mm::rotate(90, 1, 0, 0);
	mm::scale(0.05, 0.1, 0.25);
	drawMesh(blade, this);
	mm::pop();

	mm::pop();
}

void Propellor::SetEngineSpeed(float t) {
	this->engineSpeed = t;
}

void Propellor::SetEngineBlockRotorSpeed(float t) {
	this->engineBlockSpeed = t;
}

bool Propellor::startLiftLandProcess(unsigned short d) {
	if (d == 0) {
		if (this->engineAngle < -1) {
			this->engineBlockSpeed = -(ENGINE_BLOCK_MOVE_SPEED);
			return false;
		}
		else if (this->engineAngle > 1) {
			this->engineBlockSpeed = ENGINE_BLOCK_MOVE_SPEED;
			return false;
		}
		else {
			this->engineBlockSpeed = 0;
			this->engineAngle = 0;
			return true;
		}
		//this->engineBlockSpeed = 0;
		//return true;
	}
	if ((d & (ZEP_LEFT | ZEP_RIGHT)) == (ZEP_LEFT | ZEP_RIGHT)) {
		return true;
	}
	if ((d & (ZEP_UP | ZEP_DOWN)) == (ZEP_UP | ZEP_DOWN)) {
		return true;
	}

	if (((d & (ZEP_LEFT | ZEP_UP)) == (ZEP_LEFT | ZEP_UP)) || ((d & (ZEP_RIGHT | ZEP_UP)) == (ZEP_RIGHT | ZEP_UP))) {
		if (this->engineAngle > -44) {
			engineBlockSpeed = ENGINE_BLOCK_MOVE_SPEED;
			return false;
		}
		else if (this->engineAngle < -46) {
			engineBlockSpeed = -(ENGINE_BLOCK_MOVE_SPEED);
			return false;
		}
		else {
			this->engineAngle = -45;
			this->engineBlockSpeed = 0;
			return true;
		}
	}

	if (((d & (ZEP_LEFT | ZEP_DOWN)) == (ZEP_LEFT | ZEP_DOWN)) || ((d & (ZEP_RIGHT | ZEP_DOWN)) == (ZEP_RIGHT | ZEP_DOWN))) {
		if (this->engineAngle < 44) {
			engineBlockSpeed = -(ENGINE_BLOCK_MOVE_SPEED);
			return false;
		}
		else if (this->engineAngle > 46) {
			engineBlockSpeed = ENGINE_BLOCK_MOVE_SPEED;
			return false;
		}
		else {
			this->engineAngle = 45;
			this->engineBlockSpeed = 0;
			return true;
		}
	}

	if (((d & ZEP_UP) == (ZEP_UP)) || ((d & ZEP_DOWN) == (ZEP_DOWN))) {
		if (this->engineAngle > -90) {
			engineBlockSpeed = ENGINE_BLOCK_MOVE_SPEED;
			return false;
		}
		else {
			this->engineAngle = -90;
			this->engineBlockSpeed = 0;
			return true;
		}
	}  

	if (((d & ZEP_LEFT) == (ZEP_LEFT)) || ((d & ZEP_RIGHT) == (ZEP_RIGHT))) {
		if (this->engineAngle < -1) {
			engineBlockSpeed = -(ENGINE_BLOCK_MOVE_SPEED);
			return false;
		}
		else if (this->engineAngle > 1) {
			engineBlockSpeed = ENGINE_BLOCK_MOVE_SPEED;
			return false;
		}
		else {
			this->engineAngle = 0;
			this->engineBlockSpeed = 0;
			return true;
		}
	}

	


}