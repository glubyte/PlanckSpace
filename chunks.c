#include "chunks.h"

void compressObject(objectFile* object)
{
	/* 
	a simple statistical redundancy compressor. scans for adjacent permutations along each dimension and compresses into a single permutation with a flag 
	denoting the number of permutations 

	i.e. if 5 solid elemental copper blocks are found adjacent along a single dimension, in the world file, this will appear as 5x29s rather than 29s29s29s29s29s

	following, if 2 compressed solid elemental copper blocks of size 5 are found adjacent along another dimension, this will appear as 2x5x29s 
	where n is the largest size of the chunks. likewise extending into R3
	*/
	struct {
		unsigned int element;
		char state;
	} blockBuffer[2];

	unsigned int i = 0, j = 0;
	unsigned int blockSize = 1;

	// acquire file size
	fseek(object->raw, 0, SEEK_END);
	object->rawBytes = (unsigned int)ftell(object->raw);
	rewind(object->raw);

	printf("Initial file size: %i bytes\n",object->rawBytes);

	// initialize block buffer
	fscanf(object->raw, "%i", &blockBuffer[0].element);
	fscanf(object->raw, "%1c", &blockBuffer[0].state);
	fscanf(object->raw, "%i", &blockBuffer[1].element);
	fscanf(object->raw, "%1c", &blockBuffer[1].state);
	
	
}