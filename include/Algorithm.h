#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <stddef.h>

#include "Types.h"

ssize_t binary_search_exact(std::vector<Placement>*, Vector2D*, int, int);
ssize_t binary_search_region(std::vector<Placement>*, Vector2D*, Vector2D*, int, int);

#endif
