#include <random>

#include "model/normal.h"

float gen_normal_distribution( int simsize, Normal& n, std::mt19937& gen)
{
    std::normal_distribution<> d( n.getOne(), n.getTwo() );
    float result = d( gen );
    return result;
}

float gen_triangle_distribution( int simsize, Threepoint& n, std::mt19937& gen )
{
        std::uniform_real_distribution<> dis( 0.0, 1.0 );
        float U = dis( gen );
        float F = ( n.getMode() - n.getMin() ) / ( n.getMax() - n.getMin() );
        if ( U < F )
        {
            float r = n.getMin() + std::sqrt( U * ( n.getMax() - n.getMin() ) * ( n.getMode() - n.getMin() ) );
            return r;

        }
        else
        {
            float r = n.getMax() - std::sqrt( ( 1 - U ) * ( n.getMax() - n.getMin() ) * ( n.getMax() - n.getMode() ) );
            return r;
        }

    
}
