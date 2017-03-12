#pragma once

#include <vector>
#include "2DVector.h"
#include "Types.h"
#include "vertBufs.h"

/** Base particle class. */
class CParticle {
public:
	C2DVector velocity; ///<Velocity of particle.
	C2DVector pos; ///<Position.
	float life; ///<Remaining life in milliseconds
	int size; ///<Edge-size of drawn square.
	rgba colour; ///<Colour of drawn square;
	float inverseMass;

	void integrate(const double& dT);
};


/** Base particle emitter clase .*/
class CEmitter {
public:
	CEmitter() { live = false; vertBuf = NULL;colourBuf=NULL;
		drawCount = 0;};
	~CEmitter();
	void setParticles(int count);
	void update(const double& dT);
	void drawtoBuffer() ;

	C2DVector pos; ///<Position.
	bool live; ///<True if particle effect is on.
	int drawCount; ///<Keeps track of how many particles are in the draw buffer.
//private:
	void initParticles();

	int particleCount; ///<Number of particles
	std::vector<CParticle> particles; ///<The particles owned by this emitter.

	vBuf::T2DVertTri* vertBuf; ///<Pointer to the vertex buffer used for drawing.
	vBuf::TColTri* colourBuf; ///<Pointer to the colour buffer used for drawing.
};

const C2DVector gravity(0,-300.0f);
const float damping = 0.1f;