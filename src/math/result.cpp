#include "math/result.h"

#include <cmath>
#include <utility>
#include <vector>


int s3_nm_batches(float api_volume, float batch_size_step_three )
{
    float result = round( api_volume / batch_size_step_three );
    return result;
}

int s_num_batches( float api_volume, float ufstep, float batch_size_step )
{
    float result = round( api_volume * ufstep / batch_size_step );
    return result;
}

float s_time( float cycle_time_step,int nm_batchs,float restart_time_step )
{
    float result = cycle_time_step + ( nm_batchs - 1.0 ) * restart_time_step;
    return result;
}

float Tot_Time( float s3_time, float s2_time, float s1_time, float setup_cleaning )
{
    float result = s3_time + s2_time + s1_time + setup_cleaning;
    return result;
}
float RM( float rm_solvntrgnt_costs, float rm_zts, float rm_zts_kg, float rm_zts_UF, float rm_zts_UF_kg )
{
    float result = rm_solvntrgnt_costs + rm_zts * rm_zts_kg + rm_zts_UF * rm_zts_UF_kg;
    return result;
}
float LnO( float tot_time, float lno_rate, float api_volume )
{
    float result = tot_time * lno_rate / api_volume;
    return result;
}

float Percentage_LnO( float rm, float lno )
{
    float result = rm / lno;
    return result;
}

std::vector<float> API_COGs_model( std::vector<float> rm,
                                   std::vector<float> lno )
{
    std::vector<float> results;
    results.reserve( 10000000 );
    for ( int x = 0; x < rm.size(); x++ )
    {
        float result = rm[x] + lno[x];
        results.push_back( std::move( result ) );
    }
    return results;
}
