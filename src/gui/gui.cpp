#include "gui.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <mutex>
#include <random>
#include <numeric>
#include <thread>
#include <vector>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "math/random.h"
#include "math/result.h"

// Vectors
std::vector<Normal> objects;
std::mutex objects_mutex;
std::vector<Threepoint> threeobjects;
std::mutex threeobjects_mutex;
static int simsize = 0;
std::mutex simsize_mutex;

// Results
std::vector<float> results;
//
//
//

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

// Helpers

float mean( std::vector<float> vec )
{
    if ( vec.empty() )
    {
        return 0;
    }
    auto count = static_cast<float>( vec.size() );
    return std::reduce( vec.begin(), vec.end() ) / count;
}

float percentile( std::vector<float> data, float p )
{
    if ( data.empty() )
        return 0.0f;

    std::sort( data.begin(), data.end() );

    float idx = p * ( data.size() - 1 );
    size_t lo = static_cast<size_t>( idx );
    size_t hi = lo + 1;

    if ( hi >= data.size() )
        return data[lo];

    float weight = idx - lo;
    return data[lo] * ( 1.0f - weight ) + data[hi] * weight;
}

// Main monte carlo
void calculate()
{
    std::vector<Normal> local_objects;
    std::vector<Threepoint> local_threeobjects;
    int local_simsize;
    {
        std::lock_guard<std::mutex> lock1(objects_mutex);
        std::lock_guard<std::mutex> lock2(threeobjects_mutex);
        std::lock_guard<std::mutex> lock3(simsize_mutex);
        local_objects = objects;
        local_threeobjects = threeobjects;
        local_simsize = simsize;
    }

    std::vector<float> final_cogs_list;
    final_cogs_list.reserve(local_simsize); // Allocate once!

    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < local_simsize; i++) 
    {
        float api_vol = local_threeobjects[10].generate(gen);
        float rm1_qty = local_objects[0].generate(gen);
        float rm1_price = local_threeobjects[0].generate(gen);



        int step_three_batch = calc_s3_nm_batches(api_vol, local_objects[7].generate(gen));
        int step_two_batch   = calc_s_num_batches(api_vol, local_objects[4].generate(gen), local_objects[6].generate(gen));
        
        float s3_t = calc_s_time(local_threeobjects[7].generate(gen), step_three_batch, local_threeobjects[8].generate(gen));
        float s2_t = calc_s_time(local_threeobjects[5].generate(gen), step_two_batch, local_threeobjects[6].generate(gen));
        float s1_t = calc_s_time(local_threeobjects[3].generate(gen), 0 /*batch logic*/, local_threeobjects[4].generate(gen));

        float tot_time = s3_t + s2_t + s1_t + local_threeobjects[9].generate(gen);
        
        float rm_cost = local_objects[2].generate(gen) + (rm1_qty * rm1_price) + ...;
        float lno_cost = (tot_time * local_threeobjects[2].generate(gen)) / api_vol;

        final_cogs_list.push_back(rm_cost + lno_cost);
    }
    update_dashboard_results(final_cogs_list);
}

void results()
{
    float local_max, local_min, local_mean, local_ninty, local_tenth;
    {
        std::lock_guard<std::mutex> lock1( max_result_mutex );
        local_max = max_result;
        std::lock_guard<std::mutex> lock2( min_result_mutex );
        local_min = min_result;
        std::lock_guard<std::mutex> lock3( mean_result_mutex );
        local_mean = mean_result;
        std::lock_guard<std::mutex> lock4( nintypercentile_result_mutex );
        local_ninty = nintypercentile_result;
        std::lock_guard<std::mutex> lock5( tenthpercentile_result_mutex );
        local_tenth = tenthpercentile_result;
    }

    ImGui::TextDisabled( "MONTE CARLO SIMULATION STATISTICS" );
    ImGui::Dummy( ImVec2( 0, 5 ) );
    if ( ImGui::BeginTable( "ResultsTable", 2, ImGuiTableFlags_NoSavedSettings ) )
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text( "Expected Mean:" );
        ImGui::TableNextColumn();
        ImGui::TextColored( ImVec4( 0.2f, 0.8f, 1.0f, 1.0f ), "$ %.2f", local_mean );  // Light Blue

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text( "Confidence (10th - 90th):" );
        ImGui::TableNextColumn();
        ImGui::Text( "%.2f - %.2f", local_tenth, local_ninty );

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text( "Range (Min - Max):" );
        ImGui::TableNextColumn();
        ImGui::Text( "%.2f - %.2f", local_min, local_max );

        ImGui::EndTable();
    }
}
void gui( GLFWwindow* window )
{
    if ( objects.empty() )
    {
        // For the Normal objects
        std::vector<std::string> names = {
            "Raw Material 1 (Qty)",
            "Raw Material 2 (Qty)",
            "Solvents/Reagents Cost",
            "Yield Efficiency (S1)",
            "Yield Efficiency (S2)",
            "Batch Size: Step 1",
            "Batch Size: Step 2",
            "Batch Size: Step 3" };

        // For the Three-point objects
        std::vector<std::string> threevarname = {
            "RM1 Cost ($/kg)",
            "RM2 Cost ($/kg)",
            "Labor & Overhead Rate",
            "Cycle Time: Step 1 (hr)",
            "Restart Time: Step 1",
            "Cycle Time: Step 2 (hr)",
            "Restart Time: Step 2",
            "Cycle Time: Step 3 (hr)",
            "Restart Time: Step 3",
            "Setup & Cleaning",
            "Total API Volume" };

        std::lock_guard<std::mutex> lock1( objects_mutex );
        for ( const auto& name : names )
            objects.emplace_back( name, 1.0f, 1.0f );

        std::lock_guard<std::mutex> lock2( threeobjects_mutex );
        for ( const auto& name : threevarname )
            threeobjects.emplace_back( name, 1.0f, 1.0f, 1.0f );
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
    ImGui::SetNextWindowSize( io.DisplaySize );

    ImGui::Begin( "Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );

    if ( ImGui::BeginTable( "MainLayout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV ) )
    {
        ImGui::TableSetupColumn( "Inputs", ImGuiTableColumnFlags_WidthStretch, 0.65f );
        ImGui::TableSetupColumn( "Dashboard", ImGuiTableColumnFlags_WidthStretch, 0.35f );
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::BeginChild( "InputRegion", ImVec2( 0, 0 ), false, ImGuiWindowFlags_AlwaysVerticalScrollbar );

        ImGui::TextColored( ImVec4( 0.4f, 0.7f, 1.0f, 1.0f ), "NORMAL DISTRIBUTIONS" );
        ImGui::Separator();
        if ( ImGui::BeginTable( "NormalFields", 3, ImGuiTableFlags_RowBg ) )
        {
            ImGui::TableSetupColumn( "Variable", ImGuiTableColumnFlags_WidthFixed, 180.0f );
            ImGui::TableSetupColumn( "Mean" );
            ImGui::TableSetupColumn( "StdDev" );
            ImGui::TableHeadersRow();

            std::lock_guard<std::mutex> lock( objects_mutex );
            for ( size_t i = 0; i < objects.size(); i++ )
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "%s", objects[i].getName().c_str() );
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth( -1 );
                ImGui::InputFloat( ( "##u" + std::to_string( i ) ).c_str(), &objects[i].getOne(), 0, 0, "%.2f" );
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth( -1 );
                ImGui::InputFloat( ( "##s" + std::to_string( i ) ).c_str(), &objects[i].getTwo(), 0, 0, "%.2f" );
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::TextColored( ImVec4( 0.4f, 0.7f, 1.0f, 1.0f ), "THREE-POINT DISTRIBUTIONS" );
        ImGui::Separator();
        if ( ImGui::BeginTable( "ThreeFields", 4, ImGuiTableFlags_RowBg ) )
        {
            ImGui::TableSetupColumn( "Variable", ImGuiTableColumnFlags_WidthFixed, 180.0f );
            ImGui::TableSetupColumn( "Min" );
            ImGui::TableSetupColumn( "Mode" );
            ImGui::TableSetupColumn( "Max" );
            ImGui::TableHeadersRow();

            std::lock_guard<std::mutex> lock( threeobjects_mutex );
            for ( size_t i = 0; i < threeobjects.size(); i++ )
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text( "%s", threeobjects[i].getName().c_str() );
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth( -1 );
                ImGui::InputFloat( ( "##min" + std::to_string( i ) ).c_str(), &threeobjects[i].getMin(), 0, 0, "%.2f" );
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth( -1 );
                ImGui::InputFloat( ( "##mod" + std::to_string( i ) ).c_str(), &threeobjects[i].getMode(), 0, 0, "%.2f" );
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth( -1 );
                ImGui::InputFloat( ( "##max" + std::to_string( i ) ).c_str(), &threeobjects[i].getMax(), 0, 0, "%.2f" );
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImVec4( 0.10f, 0.10f, 0.12f, 1.00f ) );
        if ( ImGui::BeginChild( "ResultDashboard", ImVec2( 0, 0 ), true, ImGuiChildFlags_AlwaysUseWindowPadding ) )
        {
            ImGui::Text( "CONTROL PANEL" );
            ImGui::Separator();

            ImGui::Text( "Sim Count:" );
            ImGui::SameLine();
            ImGui::SetNextItemWidth( -1 );
            ImGui::InputInt( "##SimCount", &simsize );

            if ( ImGui::Button( "RUN MONTE CARLO", ImVec2( -1, 50 ) ) )
            {
                if ( simsize > 0 )
                {
                    std::thread( []()
                                 { calculate(); } )
                        .detach();
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            results();
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::EndTable();
    }

    ImGui::End();
    ImGui::Render();
}
