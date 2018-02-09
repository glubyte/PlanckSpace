#include "maths.h"

void cleanToken(token* token)
{
	token->type = TOKEN_TYPE_VOID;
	token->operand = OPERAND_VOID;
	token->operator = OPERATOR_VOID;
	token->integer = 0;
}
void cleanTokens(token* tokens, unsigned int numTokens)
{
	for (unsigned int i = 0; i < numTokens; i++)
	{
		cleanToken(&tokens[i]);
	}
}
void cleanNode(node* node)
{
	node->type = NODE_TYPE_VOID;
	node->left = NULL;
	node->right = NULL;
	cleanToken(&node->token);
}
void cleanNodes(node* nodes, unsigned int numNodes)
{
	for (unsigned int i = 0; i < numNodes; i++)
	{
		cleanNode(&nodes[i]);
	}
}
void add(number a, number b, number* result)
{
	
}
int addGay(int a, int b)
{
	return a + b;
}
int multiply(int a, int b)
{
	return a * b;
}
int power(int a, int b) 
{
	while (b > 1)
	{
		a *= a;
		b--;
	}
	return a;
}
/*
float dot(vec3 a, vec3 b) 
{
	return a.i * b.i + a.j * b.j + a.k * b.k;
}
vec3 cross(vec3 a, vec3 b)
{
	vec3 result;

	result.i = a.j * b.k - a.k * b.j;
	result.j = a.k * b.i - a.i * b.k;
	result.k = a.i * b.j - a.j * b.i;

	return result;
}
float length(vec3 a) 
{
	return sqrt(dot(a, a));
}
vec3 normalize(vec3 a) 
{
	vec3 result;
	float l = length(a);
	
	result.i = a.i / l;
	result.j = a.j / l;
	result.k = a.k / l;

	return result;
}
*/
void rotateZY(float* mat3x3, float angle)
{
	float c = cos(angle);
	float s = sin(angle);

	mat3x3[0] = c * c;
	mat3x3[1] = -s * c;
	mat3x3[2] = s;
	mat3x3[3] = s;
	mat3x3[4] = c;
	mat3x3[5] = 0;
	mat3x3[6] = -c * s;
	mat3x3[7] = s * s;
	mat3x3[8] = c;
}
void rotateZ(vec3* pos, float angle)
{
	vec3 result;
	float rad = (PI / 180.f) * angle;
	// x' = cos(theta) * x - sin(theta) * y
	// y' = sin(theta) * x + cos(theta) * y
	result.c[0] = cos(rad) * pos->c[0] - sin(rad) * pos->c[1];
	result.c[1] = sin(rad) * pos->c[0] + cos(rad) * pos->c[1];
	pos->c[0] = result.c[0];
	pos->c[1] = result.c[1];
}

void simplify(token* tokens, char* expression)
{
	// generate tokens
	mathLexicon(tokens, expression);
	// check format
	if (formatCheck(tokens)) 
	{
		printf("Expression rejected.\n");
		return;
	}
	// generate tree
	genBranch(tree, &tokens[1]); // tokens[1] is the first token; tokens[0] is always a void token to prevent seg faults
	// print initial tree
	//printTree(tree);
	//printf("\n");
	// solve tree
	treeArithmetic(tree, tree);
	// print solution
	printf("Simplified to:\n");
	printTree(tree);
	printf("\n");
}

void mathLexicon(token* tokens, char* expression)
{
	// The math lexicon analyzes a given expression from human input and returns the tokens to be utilized in all symbolic computations.
	unsigned int tokenCount = 1; // start at 1 and leave 0 void
	unsigned int i = 0, j = 0;

	while (expression[i])
	{
		if (isgraph(expression[i]))
		{
			if (isdigit(expression[i]))
			{
				char intBuffer[256];
				// prepare for integer of indefinite size
				tokens[tokenCount].type = TOKEN_TYPE_OPERAND;
				tokens[tokenCount].operand = OPERAND_INTEGER;

				while (isdigit(expression[i]))
				{
					intBuffer[j] = expression[i];
					i++;
					j++;
				}
				intBuffer[j] = '\0';
				tokens[tokenCount].integer = atoi(intBuffer);

				j = 0;
				tokenCount++;
				continue;
			}
			if (isalpha(expression[i]))
			{
				// check for specific strings which can be operators and long variable names
				// variables with names longer than one character should be enclosed within parenthesis
				// check if previous token is a variable. if so, generate a multiply token followed by the new variable token

				// for now, default to single variables
				tokens[tokenCount].type = TOKEN_TYPE_OPERAND;
				tokens[tokenCount].operand = OPERAND_VARIABLE;
				tokens[tokenCount].variable = expression[i];

				i++;
				tokenCount++;
				continue;
			}
			switch (expression[i])
			{
				default:
				{
					printf("Could not understand '%c'.\n", expression[i]);
					break;
				}
				case '=':
				{
					tokens[tokenCount].type = TOKEN_TYPE_EQUALITY;

					tokenCount++;
					break;
				}
				case ')':
				{
					tokens[tokenCount].type = TOKEN_TYPE_CLOSE_PARENTHESIS;

					tokenCount++;
					break;
				}
				case '(':
				{
					tokens[tokenCount].type = TOKEN_TYPE_OPEN_PARENTHESIS;

					tokenCount++;
					break;
				}
				case '+':
				{
					tokens[tokenCount].type = TOKEN_TYPE_BINARY_OPERATOR;
					tokens[tokenCount].operator = OPERATOR_ADD;

					tokenCount++;
					break;
				}
				case '-':
				{
					// check preceding token. if operand or close parenthesis, generate addition token. if operator or void, leave as is
					if (tokens[tokenCount - 1].type == TOKEN_TYPE_OPERAND || tokens[tokenCount - 1].type == TOKEN_TYPE_CLOSE_PARENTHESIS) // or equality
					{
						tokens[tokenCount].type = TOKEN_TYPE_BINARY_OPERATOR;
						tokens[tokenCount].operator = OPERATOR_ADD;
						tokenCount++;

						tokens[tokenCount].type = TOKEN_TYPE_UNARY_OPERATOR;
						tokens[tokenCount].operator = OPERATOR_MINUS;
						tokenCount++;
						break;
					}

					tokens[tokenCount].type = TOKEN_TYPE_UNARY_OPERATOR;
					tokens[tokenCount].operator = OPERATOR_MINUS;
					tokenCount++;
					break;
				}
				case '*':
				{
					tokens[tokenCount].type = TOKEN_TYPE_BINARY_OPERATOR;
					tokens[tokenCount].operator = OPERATOR_MULTIPLY;

					tokenCount++;
					break;
				}
				case '/':
				{
					tokens[tokenCount].type = TOKEN_TYPE_BINARY_OPERATOR;
					tokens[tokenCount].operator = OPERATOR_DIVIDE;

					tokenCount++;
					break;
				}
				case '^':
				{
					tokens[tokenCount].type = TOKEN_TYPE_BINARY_OPERATOR;
					tokens[tokenCount].operator = OPERATOR_EXP;

					tokenCount++;
					break;
				}
				case '!':
				{
					tokens[tokenCount].type = TOKEN_TYPE_UNARY_OPERATOR;
					tokens[tokenCount].operator = OPERATOR_FACTORIAL;

					tokenCount++;
					break;
				}
			}
		}
		i++;
	}
}
unsigned char formatCheck(token* tokens)
{
	// Analyzes lexicon output and evaluates mathematical soundness of expression
	unsigned int i = 1;
	int p = 0;

	// Verify parenthesis syntax
	i = 1;
	while (tokens[i].type)
	{
		if (p < 0)
		{
			printf("Invalid parenthesis syntax.\n");
			return 1;
		}
		switch (tokens[i].type)
		{
			case TOKEN_TYPE_OPEN_PARENTHESIS:
			{
				if (tokens[i + 1].type == TOKEN_TYPE_CLOSE_PARENTHESIS)
				{
					printf("Invalid parenthesis syntax.\n");
					return 1;
				}
				p++;
				break;
			}
			case TOKEN_TYPE_CLOSE_PARENTHESIS:
			{
				p--;
				break;
			}
		}
		i++;
	}
	if (p != 0)
	{
		printf("Invalid parenthesis syntax.\n");
		return 1;
	}

	// Verify operator/operand symmetry
	i = 1;
	while (tokens[i].type)
	{
		switch (tokens[i].type)
		{
			case TOKEN_TYPE_BINARY_OPERATOR:
			{
				switch (tokens[i - 1].type)
				{
					case TOKEN_TYPE_BINARY_OPERATOR:
					case TOKEN_TYPE_OPEN_PARENTHESIS:
					case TOKEN_TYPE_VOID:
					{
						printf("Invalid operator syntax.\n");
						return 1;
					}
				}
				switch (tokens[i + 1].type)
				{
					case TOKEN_TYPE_BINARY_OPERATOR:
					case TOKEN_TYPE_CLOSE_PARENTHESIS:
					case TOKEN_TYPE_VOID:
					{
						printf("Invalid operator syntax.\n");
						return 1;
					}
				}
				break;
			}
			case TOKEN_TYPE_OPERAND:
			{
				switch (tokens[i - 1].type)
				{
					case TOKEN_TYPE_CLOSE_PARENTHESIS:
					case TOKEN_TYPE_OPERAND:
					{
						printf("Invalid operand syntax.\n");
						return 1;
					}
				}
				switch (tokens[i + 1].type)
				{
					case TOKEN_TYPE_OPEN_PARENTHESIS:
					case TOKEN_TYPE_OPERAND:
					{
						printf("Invalid operand syntax.\n");
						return 1;
					}
				}
				break;
			}
			case TOKEN_TYPE_UNARY_OPERATOR:
			{
				break;
			}
		}
		i++;
	}
	
	return 0;
}
void genNode(node* node, token token)
{
	// to illustrate, given a chunk of the node array {node, void, void}; the branch pointers will point to the next empty slots of the array via nodeCursor
	node->token = token;
	node->left = &tree[nodeCursor];
	nodeCursor++;
	node->right = &tree[nodeCursor];
	nodeCursor++;
}
void genTree(node* root, token* tokens)
{
	// the equality is the only mathematical symbol that cannot exist more than once per expression tree
	// thus, it would be redundant to include a search for it in every recursive tree call. this is remedied by 2 distinct tree generation functions; genTree and genBranch
	unsigned int i = 0;

	// locate equality
	while (tokens[i].type)
	{
		if (tokens[i].type == TOKEN_TYPE_EQUALITY)
		{
			root->type = NODE_TYPE_ROOT;
			genNode(root, tokens[i]);
			cleanToken(&tokens[i]);
			genBranch(root->left, tokens);
			genBranch(root->right, &tokens[i + 1]);
			return;
		}
		i++;
	}
}
void genBranch(node* node, token* tokens)
{
	unsigned int i = 0, p = 0, pi = 0, pf = 0;

	// look for addition, ignore anything in parenthesis
	while (tokens[i].type)
	{
		if (tokens[i].type == TOKEN_TYPE_OPEN_PARENTHESIS)
		{
			p++;
		}
		else if (tokens[i].type == TOKEN_TYPE_CLOSE_PARENTHESIS)
		{
			p--;
		}
		if (tokens[i].operator == OPERATOR_ADD && p == 0)
		{
			node->type = NODE_TYPE_INTERNAL;
			genNode(node, tokens[i]);
			cleanToken(&tokens[i]);
			genBranch(node->left, tokens);
			genBranch(node->right, &tokens[i + 1]);
			return;
		}
		i++;
	}
	// look for multiplication and division, ignore anything in parenthesis
	i = 0;
	p = 0;
	while (tokens[i].type)
	{
		if (tokens[i].type == TOKEN_TYPE_OPEN_PARENTHESIS)
		{
			p++;
		}
		else if (tokens[i].type == TOKEN_TYPE_CLOSE_PARENTHESIS)
		{
			p--;
		}
		if ((tokens[i].operator == OPERATOR_MULTIPLY || tokens[i].operator == OPERATOR_DIVIDE) && p == 0)
		{
			node->type = NODE_TYPE_INTERNAL;
			genNode(node, tokens[i]);
			cleanToken(&tokens[i]);
			genBranch(node->left, tokens);
			genBranch(node->right, &tokens[i + 1]);
			return;
		}
		i++;
	}
	// look for unary operators
	i = 0;
	p = 0;
	while (tokens[i].type)
	{
		if (tokens[i].type == TOKEN_TYPE_OPEN_PARENTHESIS)
		{
			p++;
		}
		else if (tokens[i].type == TOKEN_TYPE_CLOSE_PARENTHESIS)
		{
			p--;
		}
		if (tokens[i].type == TOKEN_TYPE_UNARY_OPERATOR && p == 0)
		{
			switch (tokens[i].operator)
			{
			case OPERATOR_MINUS:
			{
				node->type = NODE_TYPE_INTERNAL;
				genNode(node, tokens[i]);
				cleanToken(&tokens[i]);
				node->left->type = NODE_TYPE_VOID;
				genBranch(node->right, &tokens[i + 1]);
				return;
			}
			}
		}
		i++;
	}
	// look for other operators

	// look for parenthesis
	i = 0;
	p = 0;
	while (tokens[i].type)
	{
		switch (tokens[i].type)
		{
			case TOKEN_TYPE_OPEN_PARENTHESIS:
			{
				if (p == 0)
				{
					pi = i;
				}
				p++;
				break;
			}
			case TOKEN_TYPE_CLOSE_PARENTHESIS:
			{
				p--;
				if (p == 0)
				{
					pf = i;
				}
				break;
			}
		}
		i++;
	}
	if (pi != pf)
	{
		cleanToken(&tokens[pi]);
		cleanToken(&tokens[pf]);
		genBranch(node, &tokens[pi + 1]);
		return;
	}
	// look for operands
	i = 0;
	while (tokens[i].type)
	{
		if (tokens[i].type == TOKEN_TYPE_OPERAND)
		{
			node->type = NODE_TYPE_LEAF;
			genNode(node, tokens[i]);
			cleanToken(&tokens[i]);
			node->left->type = NODE_TYPE_VOID;
			node->right->type = NODE_TYPE_VOID;
			return;
		}
		i++;
	}
}
void printNode(node* node)
{
	switch (node->token.type)
	{
		case TOKEN_TYPE_EQUALITY:
		{
			printf("=");
			break;
		}
		case TOKEN_TYPE_BINARY_OPERATOR:
		case TOKEN_TYPE_UNARY_OPERATOR:
		{
			switch (node->token.operator)
			{
				case OPERATOR_ADD:
				{
					if (node->right->token.operator != OPERATOR_MINUS && node->right->left->token.operator != OPERATOR_MINUS && node->right->token.integer >= 0 && node->right->left->token.integer >= 0)
					{
						printf("+");
					}
					break;
				}
				case OPERATOR_MINUS:
				{
					printf("-");
					break;
				}
				case OPERATOR_MULTIPLY:
				{
					printf("*");
					break;
				}
				case OPERATOR_DIVIDE:
				{
					printf("/");
					break;
				}
				case OPERATOR_EXP:
				{
					printf("^");
					break;
				}
			}
			break;
		}
		case TOKEN_TYPE_OPERAND:
		{
			switch (node->token.operand)
			{
				case OPERAND_INTEGER:
				{
					printf("%i", node->token.integer);
					break;
				}
			}
			break;
		}
	}
}
void printTree(node* tree)
{
	unsigned char pflag = 0;

	if (tree->left->type)
	{
		if ((tree->token.operator != tree->left->token.operator) && tree->left->type != NODE_TYPE_LEAF)
		{
			printf("(");
			pflag = 1;
		}
		printTree(tree->left);
		if (pflag == 1)
		{
			printf(")");
			pflag = 0;
		}
	}
	printNode(tree);
	if (tree->right->type)
	{
		if ((tree->token.operator != tree->right->token.operator) && tree->right->type != NODE_TYPE_LEAF)
		{
			if (tree->token.operator != OPERATOR_ADD && tree->left->token.operator != OPERATOR_MINUS)
			{
				printf("(");
				pflag = 1;
			}
		}
		printTree(tree->right);
		if (pflag == 1)
		{
			printf(")");
			pflag = 0;
		}
	}
}
void treeArithmetic(node* root, node* branch)
{
	if (branch->left->type != NODE_TYPE_LEAF && branch->left->type != NODE_TYPE_VOID)
	{
		treeArithmetic(root, branch->left);
	}
	if (branch->right->type != NODE_TYPE_LEAF && branch->right->type != NODE_TYPE_VOID)
	{
		treeArithmetic(root, branch->right);
	}
	evaluateNode(root, branch);
}
void evaluateNode(node* root, node* node)
{
	switch (node->token.operator)
	{
		case OPERATOR_ADD:
		{
			// at the moment, assumed to operate on integer
			node->type = NODE_TYPE_LEAF;
			node->token.type = TOKEN_TYPE_OPERAND;
			node->token.operand = OPERAND_INTEGER;
			node->token.operator = OPERATOR_VOID;
			node->token.integer = node->left->token.integer + node->right->token.integer;
			cleanNode(node->left);
			cleanNode(node->right);
			printf("= ");
			printTree(root);
			printf("\n");
			break;
		}
		case OPERATOR_MINUS:
		{
			node->type = NODE_TYPE_LEAF;
			node->token.type = TOKEN_TYPE_OPERAND;
			node->token.operator = OPERATOR_VOID;
			switch (node->right->token.operand)
			{
				case OPERAND_INTEGER:
				{
					node->token.operand = OPERAND_INTEGER;
					node->token.integer = -node->right->token.integer;
					break;
				}
			}
			cleanNode(node->right);
			break;
		}
		case OPERATOR_MULTIPLY:
		{
			// at the moment, assumed to operate on integer
			node->type = NODE_TYPE_LEAF;
			node->token.type = TOKEN_TYPE_OPERAND;
			node->token.operand = OPERAND_INTEGER;
			node->token.operator = OPERATOR_VOID;
			node->token.integer = node->left->token.integer * node->right->token.integer;
			cleanNode(node->left);
			cleanNode(node->right);
			printf("= ");
			printTree(root);
			printf("\n");
			break;
		}
	}
}
