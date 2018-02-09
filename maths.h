#pragma once
#define PI 3.14159265358979323846

// standard
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
// local

// stack handles
typedef enum {
	TOKEN_TYPE_VOID,
	TOKEN_TYPE_OPERAND,
	TOKEN_TYPE_BINARY_OPERATOR,
	TOKEN_TYPE_UNARY_OPERATOR,
	TOKEN_TYPE_EQUALITY,
	TOKEN_TYPE_OPEN_PARENTHESIS,
	TOKEN_TYPE_CLOSE_PARENTHESIS,
	TOKEN_TYPE_INEQUALITY
} tokenType;

typedef enum {
	OPERATOR_VOID,
	OPERATOR_ADD,
	OPERATOR_MINUS,
	OPERATOR_MULTIPLY,
	OPERATOR_DIVIDE,
	OPERATOR_EXP,
	OPERATOR_SQRT,
	OPERATOR_SIN,
	OPERATOR_COS,
	OPERATOR_TAN,
	OPERATOR_DOT,
	OPERATOR_CROSS,
	OPERATOR_INTEGRAL,
	OPERATOR_DERIVATIVE,
	OPERATOR_FACTORIAL,
	OPERATOR_SUM
} operator;

typedef enum {
	OPERAND_VOID,
	OPERAND_INTEGER,
	OPERAND_DECIMAL,
	OPERAND_VARIABLE,
	OPERAND_VECTOR,
	OPERAND_MATRIX,
	OPERAND_COMPLEX,
} operand;

typedef enum {
	NUMBER_INTEGER,
	NUMBER_DECIMAL,
	NUMBER_FRACTION,
	NUMBER_COMPLEX,
} numberType;

typedef struct {
	numberType type;
	unsigned char data[1024]; // maximum number size is 1024 bits
} number;

typedef struct {
	tokenType type;
	operator operator;
	operand operand;
	int integer;
	float decimal;
	char variable;
} token;

typedef enum {
	NODE_TYPE_VOID,
	NODE_TYPE_ROOT,
	NODE_TYPE_INTERNAL,
	NODE_TYPE_LEAF,
} nodeType;
unsigned int nodeCursor;

typedef struct node {
	nodeType type;
	token token;
	struct node* left;
	struct node* right;
} node;
node tree[500];

typedef struct {
	float i, j;
} vec2;

typedef struct {
	float c[3];
} vec3;

typedef struct {
	float c1, c2, c3, c4;
} vec4;

typedef struct {
	float a[16];
} mat4x4;

// prototypes
/* constructors and destructors */
void cleanToken(token* token);
void cleanTokens(token* tokens, unsigned int numTokens);
void cleanNode(node* node);
void cleanNodes(node* nodes, unsigned int numNodes);

/* numerical computation */
// arithmetic
void add(number a, number b, number* result);
int addGay(int a, int b);
int multiply(int a, int b);
int power(int a, int b);
// linear algebra
float dot(vec3 a, vec3 b);
vec3 cross(vec3 a, vec3 b);
float length(vec3 a);
vec3 normalize(vec3 a);
// coordinate functions
void rotateZY(float* mat3x3, float angle);
void rotateZ(vec3* pos, float angle);

/* symbolic computation */
// modules
void simplify(token* tokens, char* expression);
// lexical analysis
void mathLexicon(token* tokens, char* expression);
unsigned char formatCheck(token* tokens);
// tree generation/handling
void genNode(node* node, token token);
void genTree(node* root, token* tokens);
void genBranch(node* node, token* tokens);
void printNode(node* node);
void printTree(node* tree);
// tree manipulation
void treeArithmetic(node* root, node* branch);
void evaluateNode(node* root, node* node);