#include "gui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "math/random.h"
#include "math/result.h"
#include <algorithm>
#include <vector>
#include <chrono>
#include <mutex>
#include <numeric>
#include <thread>
std::vector<Normal> objects;
std::mutex objects_mutex;
std::vector<Threepoint> threeobjects;
std::mutex threeobjects_mutex;
static int simsize = 0;
std::mutex simsize_mutex;

//Results
std::mutex max_result_mutex;
static float max_result = 0.0f;

std::mutex mean_result_mutex;
static float mean_result = 0.0f;

std::mutex tenthpercentile_result_mutex;
static float tenthpercentile_result = 0.0f;

std::mutex nintypercentile_result_mutex;
static float nintypercentile_result = 0.0f;

std::mutex min_result_mutex;
static float min_result = 0.0f;

std::mutex lnopercent_result_mutex;
static float lnopercent_result = 0.0f;



float mean(std::vector<float> vec)
{
 if(vec.empty()){
        return 0;
    }
    auto count = static_cast<float>(vec.size());
    return std::reduce(vec.begin(), vec.end()) / count;
}
float percentile(std::vector<float> data, float p)
{
    if (data.empty())
        return 0.0f;

    std::sort(data.begin(), data.end());

    float idx = p * (data.size() - 1);
    size_t lo = static_cast<size_t>(idx);
    size_t hi = lo + 1;

    if (hi >= data.size())
        return data[lo];

    // Linear interpolation (recommended)
    float weight = idx - lo;
    return data[lo] * (1.0f - weight) + data[hi] * weight;
}

void calculate()
{
    std::lock_guard< std::mutex> lock1(objects_mutex);
    std::lock_guard< std::mutex> lock2(threeobjects_mutex);
    std::lock_guard< std::mutex> lock3(simsize_mutex);
    std::vector<std::vector<float>> normal_distribution_vector;
    std::vector<std::vector<float>> triangle_distribution_vector;
    for (int x = 0; x < objects.size(); x++) {
      normal_distribution_vector.push_back(gen_normal_distribution_vector(simsize, objects[x]));
    }

    for (int x = 0; x < threeobjects.size(); x++) {
      triangle_distribution_vector.push_back(gen_triangle_distribution_vector(simsize, threeobjects[x]));
    }

    std::vector<int> step_three_batch = s3_nm_batches(
        triangle_distribution_vector[10], normal_distribution_vector[7]);
    std::vector<int> step_two_batch = s_num_batches(
        triangle_distribution_vector[10], normal_distribution_vector[4],
        normal_distribution_vector[6]);
    std::vector<int> step_one_batch = s_num_batches(
        triangle_distribution_vector[10], normal_distribution_vector[3],
        normal_distribution_vector[5]);

    std::vector<float> s3 =
        s_time(triangle_distribution_vector[7], step_three_batch,
               triangle_distribution_vector[8]);
    std::vector<float> s2 =
        s_time(triangle_distribution_vector[5], step_two_batch,
               triangle_distribution_vector[6]);
    std::vector<float> s1 =
        s_time(triangle_distribution_vector[3], step_one_batch,
               triangle_distribution_vector[4]);

    std::vector<float> tot_time =
        Tot_Time(s3, s2, s1, triangle_distribution_vector[9]);
    std::vector<float> rm =
        RM(normal_distribution_vector[2], normal_distribution_vector[0],
           triangle_distribution_vector[0], normal_distribution_vector[1],
           triangle_distribution_vector[1]);

    std::vector<float> lno = LnO(tot_time, triangle_distribution_vector[2],
                                 triangle_distribution_vector[10]);
    std::vector<float> results;
    for (int x = 0; x < rm.size(); x++) {
      float result = rm[x] + lno[x];
      results.push_back(result);
    }
    std::lock_guard< std::mutex> lock(max_result_mutex);
    max_result = *std::max_element(results.begin(), results.end());

    std::lock_guard<std::mutex> lnopercent_lock(lnopercent_result_mutex);
    lnopercent_result  = mean(Percentage_LnO(rm, lno));

    std::lock_guard<std::mutex> min_result_lock(min_result_mutex);
    min_result = *std::min_element(results.begin(), results.end());

    std::lock_guard<std::mutex> mean_result_lock(mean_result_mutex);
    mean_result = mean(results);
    //10th and 90th
    std::lock_guard<std::mutex> tenthpercentile_lock(tenthpercentile_result_mutex);
    tenthpercentile_result = percentile(results, 0.10f);

    std::lock_guard<std::mutex> nintypercentile_lock(nintypercentile_result_mutex);
    nintypercentile_result = percentile(results, 0.90f);



}

void gui() {
  float x;
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Cost of goods");

  if (objects.empty()) {
    std::vector<std::string> names = {
        "RM1",     "RM2",    "RM_SolvntRgnt_Costs",
        "UF_Step1",         "UF_Step2",        "Batch_Size_Step1",
        "Batch_Size_Step2", "Batch_Size_Step3"};
    // Initialize Normal objects with zeros
    std::vector<std::string> threevarname = {
        "RM1_CostKg",   "RM1_CostKg",   "LnO_Rate",
        "Cycle_Time_Step1",   "Restart_Time_Step1", "Cycle_Time_Step2",
        "Restart_Time_Step2", "Cycle_Time_Step3",   "Restart_Time_Step3",
        "Setup_Cleaning",     "api_volume"};

    for (const auto &name : names) {
      std::lock_guard< std::mutex> lock1(objects_mutex);
      objects.emplace_back(name, 1.0f, 1.0f);
    }
    for (const auto &name : threevarname) {
      std::lock_guard< std::mutex> lock1(threeobjects_mutex);
      threeobjects.emplace_back(name, 1.0f, 1.0f, 1.0f);
    }
  }
  for (size_t i = 0; i < objects.size(); i++) {
    std::lock_guard< std::mutex> lock1(objects_mutex);
    Normal &n = objects[i];

    ImGui::PushID(static_cast<int>(i));
    ImGui::Text("%s", n.getName().c_str());
    ImGui::InputFloat(("##one" + std::to_string(i)).c_str(), &n.getOne());
    ImGui::InputFloat(("##two" + std::to_string(i)).c_str(), &n.getTwo());
    ImGui::PopID();
  }
  for (size_t i = 0; i < threeobjects.size(); i++) {
    std::lock_guard< std::mutex> lock1(threeobjects_mutex);
    Threepoint &n = threeobjects[i];
    ImGui::PushID(static_cast<int>(i));
    ImGui::Text("%s", n.getName().c_str());
    ImGui::InputFloat(("##min" + std::to_string(i)).c_str(), &n.getMin());
    ImGui::InputFloat(("##mode" + std::to_string(i)).c_str(), &n.getMode());
    ImGui::InputFloat(("##max" + std::to_string(i)).c_str(), &n.getMax());
    ImGui::PopID();
  }

  {
    std::lock_guard< std::mutex> lock1(simsize_mutex);
    ImGui::InputInt("Simulation Count", &simsize);
  }
  if (ImGui::Button("Calculate")) { 
    std::lock_guard< std::mutex> lock1(simsize_mutex);
      if(simsize <= 0){
          
      }
      else
      {
        std::thread worker([] {
            calculate();
            // send result back to GUI thread
        });
        worker.detach();
      }
       
  }
  float local_max;
  float local_min;
  float local_mean;
  float local_ninty;
  float local_tenth;
  {
    std::lock_guard<std::mutex> maxlock(max_result_mutex);
    local_max = max_result;
    std::lock_guard<std::mutex> minlock(min_result_mutex);
    local_min = min_result;
    std::lock_guard<std::mutex> meanlock(mean_result_mutex);
    local_mean = mean_result;
    std::lock_guard<std::mutex> nintylock(nintypercentile_result_mutex);
    local_ninty = nintypercentile_result;
    std::lock_guard<std::mutex> tenthlock(tenthpercentile_result_mutex);
    local_tenth = tenthpercentile_result;
  }
  ImGui::Text("Max: %f", local_max);
  ImGui::Text("Min: %f", local_min);
  ImGui::Text("Mean: %f", local_mean);
  ImGui::Text("10th percentile: %f", local_tenth);
  ImGui::Text("90th percentile: %f", local_ninty);
  ImGui::End();
  ImGui::Render();
}
