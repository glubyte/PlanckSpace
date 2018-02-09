// stop SDL from fucking with shit
#define SDL_MAIN_HANDLED

// standard
#include <stdio.h>
// local
#include "vk.h"
#include "sdl.h"
#include "menu.h"
#include "perception.h"
#include "gen.h"
#include "fps.h"
#include "chunks.h"
#include "init.h"
#include "maths.h"

int main(void)
{
	/* REGISTER ALLOCATION */
	unsigned char flag = 1;
	int mod;
	uint32_t ticks = 0;
	char expression[200];

	/* INITIALIZATION */
	// nothing yet

	/* MIND LOOP */
	printf("Welcome, organism.\n\n");
	while (flag)
	{
		printf("Select a module.\n");
		printf("1. Mathematics Processor\n");
		printf("2. Perceiver\n");
		printf("0. *Terminate Mind\n");
		while (scanf("%i", &mod) == 0)
		{
			printf("Unable to understand.\n");
			flushInput();
		}
		flushInput();

		switch (mod)
		{
			default:
			{
				printf("Invalid module.\n");
				break;
			}
			case 0:
			{
				flag = 0;
				break;
			}
			case 1:
			{
				token tokens[500];
				printf("Mathematics processor intialized.\nTo return to module selection, type exit.\n");
				while (1)
				{
					// acquire expression
					printf("Enter an arithmetic expression for evaluation:\n");
					cleanTokens(tokens, 500);
					cleanNodes(tree, 500);
					nodeCursor = 1;
					fgets(expression, sizeof(expression), stdin);
					if (strcmp("exit\n", expression) == 0)
					{
						break;
					}
					simplify(tokens, expression);
				}
				break;
			}
			case 2:
			{
				if (init())
					break;
				mainMenu();
				break;
			}
		}
	}
	printf("Mind terminated.\n");
	return 0;
}