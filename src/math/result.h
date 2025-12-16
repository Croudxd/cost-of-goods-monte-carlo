#include <vector>

std::vector<int> s3_nm_batches(std::vector<float> api_volume,
                               std::vector<float> batch_size_step_three);

std::vector<int> s_num_batches(std::vector<float> api_volume,
                               std::vector<float> ufstep,
                               std::vector<float> batch_size_step);

std::vector<float> s_time(std::vector<float> cycle_time_step,
                          std::vector<int> s_nm_batchs,
                          std::vector<float> restart_time_step);

std::vector<float> Tot_Time(std::vector<float> s3_time,
                            std::vector<float> s2_time,
                            std::vector<float> s1_time,
                            std::vector<float> setup_cleaning);

std::vector<float> RM(std::vector<float> rm_solvntrgnt_costs,
                      std::vector<float> rm_zts, std::vector<float> rm_zts_kg,
                      std::vector<float> rm_zts_UF,
                      std::vector<float> rm_zts_UF_kg);
std::vector<float> LnO(std::vector<float> tot_time, std::vector<float> lno_rate,
                       std::vector<float> api_volume);
std::vector<float> Percentage_LnO(std::vector<float> rm,
                                  std::vector<float> lno);

std::vector<float> API_COGs_model(std::vector<float> rm,
                                  std::vector<float> lno);
