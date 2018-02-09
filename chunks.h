#pragma once

// standard
#include <stdio.h>
#include <ctype.h>
// local

// stack handles
typedef struct {
	// header
	unsigned int blocks;
	unsigned int dimx, dimy;
	// data
	FILE* raw;
	unsigned int rawBytes;
	FILE* compressed;
	unsigned int compressedBytes;
} objectFile;

typedef struct chunk {
	// weighted averages of all sorts of physical properties for use in the descriptor and compressor
	struct chunk** octant; // pointers to the 8 octants which are also chunks
} body;

// prototypes
void compressObject(objectFile* object); // ultimately compress chunk since any given planet will be created and loaded chunk-wise