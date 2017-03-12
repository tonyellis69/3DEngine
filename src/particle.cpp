#include "particle.h"

#include "stdlib.h" //For rand
#include <iostream> //for cerr
using namespace std;
/**  */
void CParticle::integrate(const double& dT) {

    // Update linear position.
    pos = pos + velocity  * (float)dT;


    // Update linear velocity from the acceleration (gravity).
    velocity = velocity + gravity * (float)dT;

	//impose drag 
	velocity  = velocity * (float)pow((double)damping, dT);

}

/** Set the number of particles. */
void CEmitter::setParticles(int count) {
	particleCount = count;
	particles.resize(count);
	if (vertBuf != NULL) {
		delete[] vertBuf;
		delete[] colourBuf;
	}
	vertBuf = new vBuf::T2DVertTri[particleCount];
	colourBuf = new vBuf::TColTri[particleCount];
	initParticles();
}

/** Initialise particles, ready for use. */
void CEmitter::initParticles() {
	for(size_t p=0;p<particles.size();p++) {
		particles[p].pos.set(0,0);
		particles[p].velocity.set(0,0);
	}
}





/** Update position of particles according to time passed.*/
void CEmitter::update(const double& dT) {
	
	for(int p=0;p<particleCount;p++) {
		//if (particles[p].life > 0 )
			particles[p].integrate(dT);
		particles[p].life -= (float)dT;
	}
}

/** Write drawing data for all live particles to a buffer. */
void CEmitter::drawtoBuffer() {
	drawCount = 0;
	C2DVector partPos; CParticle* particle; 
	vBuf::T2DVertTri* pVbuf = vertBuf ;
	vBuf::TColTri* pColourBuf = colourBuf;
	for(int p=0;p<particleCount;p++) {
		if (particles[p].life > 0 /*-1.5f*/ )
		{
			particle = &particles[p];
			partPos = pos + particle->pos ;
			vBuf::drawBLQuad(pVbuf++,partPos.x,partPos.y,particle->size,particle->size);
			vBuf::colourBLQuad(pColourBuf++,particle->colour);
			drawCount++;
		}
	}
	if (drawCount == 0)
		live = false;
}

CEmitter::~CEmitter() {
	if (vertBuf != NULL) {
		delete[] vertBuf;
		delete[] colourBuf;
	}
}
