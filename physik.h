#pragma once

// standard
#include <stdio.h>
#include <string.h>

// local
#include "maths.h"

// stack handles
typedef enum {
	PERMUTATION_TYPE_VOID,
	PERMUTATION_TYPE_ELEMENTAL,
	PERMUTATION_TYPE_COMPOUND,
	PERMUTATION_TYPE_SOLUTION
} permutationType;

typedef enum {
	PHYSICAL_STATE_SOLID,
	PHYSICAL_STATE_LIQUID,
	PHYSICAL_STATE_GAS,
	PHYSICAL_STATE_PLASMA
} physicalState;

typedef enum {
	ELEMENT_HYDROGEN,
	ELEMENT_HELIUM,
	ELEMENT_LITHIUM,
	ELEMENT_BERYLLIUM,
	ELEMENT_BORON,
	ELEMENT_CARBON,
	ELEMENT_NITROGEN,
	ELEMENT_OXYGEN,
	ELEMENT_FLUORINE,
	ELEMENT_NEON
} element;

typedef struct {
	permutationType permutation;
	physicalState state;
	element element;
} blok;

// prototypes

