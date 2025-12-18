#include "math/result.h"
#include <cmath>
#include <utility>

std::vector<int> s3_nm_batches(std::vector<float> api_volume,
                               std::vector<float> batch_size_step_three) {
  std::vector<int> results;
  results.reserve(10000000);
  for (int x = 0; x < api_volume.size(); x++) {
    float result = round(api_volume[x] / batch_size_step_three[x]);
    results.push_back(std::move(result));
  }
  return results;
}

std::vector<int> s_num_batches(std::vector<float> api_volume,
                               std::vector<float> ufstep,
                               std::vector<float> batch_size_step) {
  std::vector<int> results;
  results.reserve(10000000);
  for (int x = 0; x < api_volume.size(); x++) {
    float result = round(api_volume[x] * ufstep[x] / batch_size_step[x]);
    results.push_back(std::move(result));
  }
  return results;
}

std::vector<float> s_time(std::vector<float> cycle_time_step,
                          std::vector<int> nm_batchs,
                          std::vector<float> restart_time_step) {
  std::vector<float> results;
  results.reserve(10000000);
  for (int x = 0; x < cycle_time_step.size(); x++) {
    float result =
        cycle_time_step[x] + (nm_batchs[x] - 1.0) * restart_time_step[x];
    results.push_back(std::move(result));
  }
  return results;
}

std::vector<float> Tot_Time(std::vector<float> s3_time,
                            std::vector<float> s2_time,
                            std::vector<float> s1_time,
                            std::vector<float> setup_cleaning) {
  std::vector<float> results;
  results.reserve(10000000);
  for (int x = 0; x < s3_time.size(); x++) {
    float result = s3_time[x] + s2_time[x] + s1_time[x] + setup_cleaning[x];
    results.push_back(std::move(result));
  }
  return results;
}
std::vector<float> RM(std::vector<float> rm_solvntrgnt_costs,
                      std::vector<float> rm_zts, std::vector<float> rm_zts_kg,
                      std::vector<float> rm_zts_UF,
                      std::vector<float> rm_zts_UF_kg) {

  std::vector<float> results;
  results.reserve(10000000);
  for (int x = 0; x < rm_solvntrgnt_costs.size(); x++) {
    float result = rm_solvntrgnt_costs[x] + rm_zts[x] * rm_zts_kg[x] +
                   rm_zts_UF[x] * rm_zts_UF_kg[x];
    results.push_back(std::move(result));
  }
  return results;
}
std::vector<float> LnO(std::vector<float> tot_time, std::vector<float> lno_rate,
                       std::vector<float> api_volume) {

  std::vector<float> results;
  results.reserve(10000000);
  for (int x = 0; x < api_volume.size(); x++) {
    float result = tot_time[x] * lno_rate[x] / api_volume[x];
    results.push_back(std::move(result));
  }
  return results;
}

std::vector<float> Percentage_LnO(std::vector<float> rm,
                                  std::vector<float> lno) {

  std::vector<float> results;
  results.reserve(10000000);
  for (int x = 0; x < rm.size(); x++) {
    float result = rm[x] / lno[x];
    results.push_back(std::move(result));
  }
  return results;
}

std::vector<float> API_COGs_model(std::vector<float> rm,
                                  std::vector<float> lno) {

  std::vector<float> results;
  results.reserve(10000000);
  for (int x = 0; x < rm.size(); x++) {
    float result = rm[x] + lno[x];
    results.push_back(std::move(result));
  }
  return results;
}
