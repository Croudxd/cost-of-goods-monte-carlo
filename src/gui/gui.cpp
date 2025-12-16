#include "gui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "math/random.h"
#include "math/result.h"
#include <algorithm>
#include <vector>
std::vector<Normal> objects;
std::vector<Threepoint> threeobjects;
static float max_result = 0.0f;
static float mean = 0.0f;
static float min = 0.0f;
static float tenthpercentile = 0.0f;
static float nintypercentile = 0.0f;
static float laopercentage = 0.0f;

void gui() {
  float x;
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Cost of goods");

  if (objects.empty()) {
    std::vector<std::string> names = {
        "RM_ZTS323_UF",     "RM_ZTS760_UF",    "RM_SolvntRgnt_Costs",
        "UF_Step1",         "UF_Step2",        "Batch_Size_Step1",
        "Batch_Size_Step2", "Batch_Size_Step3"};
    // Initialize Normal objects with zeros
    std::vector<std::string> threevarname = {
        "RM_ZTS323_CostKg",   "RM_ZTS760_CostKg",   "LnO_Rate",
        "Cycle_Time_Step1",   "Restart_Time_Step1", "Cycle_Time_Step2",
        "Restart_Time_Step2", "Cycle_Time_Step3",   "Restart_Time_Step3",
        "Setup_Cleaning",     "api_volume"};

    for (const auto &name : names) {
      objects.emplace_back(name, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    for (const auto &name : threevarname) {
      threeobjects.emplace_back(name, 1.0f, 1.0f, 1.0f);
    }
  }
  for (size_t i = 0; i < objects.size(); i++) {
    Normal &n = objects[i];

    ImGui::PushID(static_cast<int>(i));
    ImGui::Text("%s", n.getName().c_str());
    ImGui::InputFloat(("##one" + std::to_string(i)).c_str(), &n.getOne());
    ImGui::InputFloat(("##two" + std::to_string(i)).c_str(), &n.getTwo());
    ImGui::InputFloat(("##three" + std::to_string(i)).c_str(), &n.getThree());
    ImGui::InputFloat(("##four" + std::to_string(i)).c_str(), &n.getFour());
    ImGui::PopID();
  }
  for (size_t i = 0; i < threeobjects.size(); i++) {
    Threepoint &n = threeobjects[i];
    ImGui::PushID(static_cast<int>(i));
    ImGui::Text("%s", n.getName().c_str());
    ImGui::InputFloat(("##min" + std::to_string(i)).c_str(), &n.getMin());
    ImGui::InputFloat(("##mode" + std::to_string(i)).c_str(), &n.getMode());
    ImGui::InputFloat(("##max" + std::to_string(i)).c_str(), &n.getMax());
    ImGui::PopID();
  }

  static int a = 0;
  ImGui::InputInt("Simulation Count", &a);
  if (ImGui::Button("Calculate")) {
    if (objects[0].getOne() == 0.0) {
      ImGui::Text("Dont leave any variables as 0.");
    }
    std::vector<std::vector<float>> normal_distribution_vector;
    std::vector<std::vector<float>> triangle_distribution_vector;
    for (int x = 0; x < objects.size(); x++) {
      normal_distribution_vector.push_back(
          gen_normal_distribution_vector(a, objects[x]));
    }
    for (int x = 0; x < threeobjects.size(); x++) {
      triangle_distribution_vector.push_back(
          gen_triangle_distribution_vector(a, threeobjects[x]));
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

    ///
    ///
    ///
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
    max_result = *std::max_element(results.begin(), results.end());
    Percentage_LnO(rm, lno);
  }
  ImGui::Text("Max: %f", max_result);
  ImGui::End();
  ImGui::Render();
}
