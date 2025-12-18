#include <string>

class Normal
{
   private:
    std::string name;
    float mean;
    float stddev;

   public:
    Normal( std::string name, float one, float two ) : name( name ), mean( one ), stddev( two ) {}

    float& getOne() { return mean; };
    float& getTwo() { return stddev; };
    std::string& getName() { return name; };

    void setOne( float one ) { this->mean = one; }
    void setTwo( float two ) { this->stddev = two; }
};

class Threepoint
{
   private:
    std::string name;
    float min;
    float mode;
    float max;

   public:
    Threepoint( std::string name, float min, float mode, float max )
        : name( name ), min( min ), mode( mode ), max( max ) {}
    float& getMin() { return min; };
    float& getMode() { return mode; };
    float& getMax() { return max; };
    std::string& getName() { return name; };

    void setMin( float min ) { this->min = min; }
    void setMode( float mode ) { this->mode = mode; }
    void setMax( float max ) { this->max = max; }
};
