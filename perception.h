#pragma once
/* 
The collection of all handles and prototypes utilized in perceiving the world.

The projection vector is relatively simple. It is a function of view distance and field of view and is constant throughout run-time until these variables are changed.
It is generated only once at run-time, which sharply increases processing speed via a stark reduction in the number of calculations per frame.

All associated calculations in transforming the local coordinates of objects to view coordinates are handled on the GPU.
*/

// standard
#include <stdio.h>
#include <math.h>
// local
#include "maths.h"
#include "sdl.h"

// stack handles
struct {
	vec4 proj;
	vec3 view;
	mat4x4 model;
	float fov; // field of view
	float vd; // view distance
	float np; // near plane
	float ar; // aspect ratio
} projshit;

// projection
void initProj();
void genProj();