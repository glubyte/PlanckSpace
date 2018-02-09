#include "perception.h"

void initProj()
{
	// extract from config file
	projshit.fov = 45.0f;
	projshit.vd = 100.0f;

	projshit.np = 0.1f;

	genProj();

	projshit.view.c[0] = 0.0f;
	projshit.view.c[1] = -0.6f;
	projshit.view.c[2] = -10.0f;
}
void genProj()
{
	float c0;

	projshit.ar = (float)sdlshit.winWidth / (float)sdlshit.winHeight;

	c0 = (float)tan(projshit.fov * (PI / 180.0f) / 2.0f);
	projshit.proj.c1 = 1.0f / c0;
	projshit.proj.c2 = projshit.ar / c0;
	projshit.proj.c3 = -projshit.vd / (projshit.vd - projshit.np);
	projshit.proj.c4 = -projshit.vd * projshit.np / (projshit.vd - projshit.np);
}