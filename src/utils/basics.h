#pragma once

#include <math.h>
#include <stdlib.h> // size_t
#include <stdbool.h> // booleans
#include <assert.h>

// Typed NULL pointer
#define nullptr ((void *)NULL)

// Swap variable values
#define swap(a, b, type) do { \
    type *_a = (type *)&a;    \
    type *_b = (type *)&b;    \
    type temp = a;            \
    *_a = b;                  \
    *_b = temp;               \
} while (0)
