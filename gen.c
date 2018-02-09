#include "gen.h"

/*
void genSphere(int radius) 
{
	FILE* sphere = fopen("sphere", "w");
	vec3 v;
	int rs = radius * radius;

	for (v.i = -radius; v.i < radius + 1; v.i++)
	{
		for (v.j = -radius; v.j < radius + 1; v.j++)
		{
			for (v.k = -radius; v.k < radius + 1; v.k++)
			{
				if (dot(v, v) < rs)
				{
					fprintf(sphere, "%i ", 1);
					continue;
				}
				fprintf(sphere, "%i ", 0);
			}
			fprintf(sphere, "\n");
		}
		fprintf(sphere, "\n");
	}
	fclose(sphere);
}
*/
void testFile(unsigned long int size)
{
	FILE* test = fopen("test", "w");
	unsigned long int i;
	for (i = 0; i < size; i++)
	{
		fprintf(test, "%i ", 1);
	}
	fclose(test);
}