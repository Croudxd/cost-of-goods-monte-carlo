#pragma once
#include <random>

#include "model/normal.h"

float gen_normal_distribution( int simsize, Normal& n, std::mt19937& gen  );
float gen_triangle_distribution ( int simsize, Threepoint& n, std::mt19937& gen );
