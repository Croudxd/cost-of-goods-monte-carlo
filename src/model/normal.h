#include <string>

class Normal {
private:
  std::string name;
  float mean;
  float stddev;
  float minusthreedev;
  float plusthreedev;

public:
  Normal(std::string name, float one, float two, float three, float four)
      : name(name), mean(one), stddev(two), minusthreedev(three),
        plusthreedev(four) {}

  float &getOne() { return mean; };
  float &getTwo() { return stddev; };
  float &getThree() { return minusthreedev; };
  float &getFour() { return plusthreedev; };
  std::string &getName() { return name; };

  void setOne(float one) { this->mean = one; }
  void setTwo(float two) { this->stddev = two; }
  void setThree(float three) { this->minusthreedev = three; }
  void setFour(float four) { this->plusthreedev = four; }
};

class Threepoint {
private:
  std::string name;
  float min;
  float mode;
  float max;

public:
  Threepoint(std::string name, float min, float mode, float max)
      : name(name), min(min), mode(mode), max(max) {}
  float &getMin() { return min; };
  float &getMode() { return mode; };
  float &getMax() { return max; };
  std::string &getName() { return name; };

  void setMin(float min) { this->min = min; }
  void setMode(float mode) { this->mode = mode; }
  void setMax(float max) { this->max = max; }
};
