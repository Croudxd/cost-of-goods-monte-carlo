#pragma once
#include "model/normal.h"
#include <vector>
std::vector<float> gen_normal_distribution_vector(int simsize, Normal &n);
std::vector<float> gen_triangle_distribution_vector(int simsize, Threepoint &n);
std::vector<float> gen_discrete_distribution_vector(int simsize, Threepoint &n);
