#pragma once
#include "Drawable.h"
class World : public Drawable
{
public:
	World();
	void draw();

private:
	void genGround();
	Mesh* ground;
	

};

