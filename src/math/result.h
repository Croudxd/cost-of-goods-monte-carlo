#pragma once
int s3_nm_batches(float api_volume, float batch_size_step_three );

int s_num_batches(float api_volume, float ufstep, float batch_size_step );

float s_time( float cycle_time_step, int s_nm_batchs, float restart_time_step );

float Tot_Time( float s3_time, float s2_time, float s1_time, float setup_cleaning );

float RM( float rm_solvntrgnt_costs, float rm_zts, float rm_zts_kg,float rm_zts_UF, float rm_zts_UF_kg );

float LnO( float tot_time, float lno_rate, float api_volume );

float Percentage_LnO( float rm, float lno );

// float API_COGs_model( float rm, float lno );
