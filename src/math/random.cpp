#include "model/normal.h"
#include <random>
#include <vector>

std::vector<float> gen_normal_distribution_vector(int simsize, Normal &n) {
  std::vector<float> random_numbers;

  for (int x = 0; x < simsize; x++) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(n.getOne(), n.getTwo());
    float result = d(gen);
    random_numbers.push_back(std::move(result));
  }

  return random_numbers;
}

std::vector<float> gen_triangle_distribution_vector(int simsize,
                                                    Threepoint &n) {
  std::vector<float> vec;
  vec.reserve(10000000);

  for (int x = 0; x < simsize; x++) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    float U = dis(gen);
    float F = (n.getMode() - n.getMin()) / (n.getMax() - n.getMin());
    if (U < F) {
      float r = n.getMin() + std::sqrt(U * (n.getMax() - n.getMin()) *
                                       (n.getMode() - n.getMin()));
      vec.push_back(std::move(r));
    } else {
      float r = n.getMax() - std::sqrt((1 - U) * (n.getMax() - n.getMin()) *
                                       (n.getMax() - n.getMode()));
      vec.push_back(std::move(r));
    }
  }
  return vec;
}

