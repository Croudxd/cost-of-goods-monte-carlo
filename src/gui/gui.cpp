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
#include <atomic>

std::atomic<bool> is_calculating{false}; 
// Vectors
std::vector<Normal> objects;
std::mutex objects_mutex;
std::vector<Threepoint> threeobjects;
std::mutex threeobjects_mutex;
static int simsize = 0;
std::mutex simsize_mutex;

// Results
std::mutex result_vec_mutex;
std::vector<float> result_vec;

std::mutex lnopercent_result_mutex;
static std::vector<float> lnopercent_result;

struct Result {
    float lnopercent, max, min, mean, ninty, tenth = 0.0;
};

Result final_result;
std::mutex Result_mutex;

// Helpers

float mean( std::vector<float> vec )
{
    if ( vec.empty() )
    {
        return 0.0;
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
    is_calculating = true;
    std::lock_guard<std::mutex> lock1( result_vec_mutex );
    std::lock_guard<std::mutex> lock2( lnopercent_result_mutex );
    std::vector<Normal> local_objects;
    std::vector<Threepoint> local_threeobjects;
    int local_simsize;

    {
        std::lock_guard<std::mutex> lock1( objects_mutex );
        std::lock_guard<std::mutex> lock2( threeobjects_mutex );
        std::lock_guard<std::mutex> lock3( simsize_mutex );
        local_objects = objects;
        local_threeobjects = threeobjects;
        local_simsize = simsize;
    }

    std::random_device rd;
    std::mt19937 gen( rd() );

    std::vector<float> normal_distribution_vector;
    std::vector<float> triangle_distribution_vector;

    for ( int x = 0; x < simsize; x ++)
    {

        for ( int x = 0; x < local_objects.size(); x++ )
        {
            normal_distribution_vector.push_back(gen_normal_distribution(local_simsize, local_objects[x], gen));
        }

        for ( int x = 0; x < threeobjects.size(); x++ )
        {
            triangle_distribution_vector.push_back(gen_triangle_distribution( local_simsize, local_threeobjects[x], gen));
        }

        int step_three_batch = s3_nm_batches( triangle_distribution_vector[10], normal_distribution_vector[7] );
        int step_two_batch = s_num_batches( triangle_distribution_vector[10], normal_distribution_vector[4], normal_distribution_vector[6] );
        int step_one_batch = s_num_batches( triangle_distribution_vector[10], normal_distribution_vector[3], normal_distribution_vector[5] );

        float s3 = s_time( triangle_distribution_vector[7], step_three_batch, triangle_distribution_vector[8] );
        float s2 = s_time( triangle_distribution_vector[5], step_two_batch, triangle_distribution_vector[6] );
        float s1 = s_time( triangle_distribution_vector[3], step_one_batch, triangle_distribution_vector[4] );

        float tot_time = Tot_Time( s3, s2, s1, triangle_distribution_vector[9] );
        float rm =RM( normal_distribution_vector[2], normal_distribution_vector[0],triangle_distribution_vector[0], normal_distribution_vector[1],triangle_distribution_vector[1] );
        float lno = LnO( tot_time, triangle_distribution_vector[2], triangle_distribution_vector[10] );

        float result = rm + lno;
        result_vec.push_back(result);
        lnopercent_result.push_back(Percentage_LnO ( rm , lno));
        triangle_distribution_vector.clear();
        normal_distribution_vector.clear();
    }


    float local_lnopercent, local_max, local_min, local_mean, local_ninty, local_tenth; 
    if (result_vec.empty())
    {
        local_lnopercent, local_max, local_min, local_mean, local_ninty, local_tenth = 0.0;
    }
    else {
        local_max = *std::max_element( result_vec.begin(),result_vec.end() );
        local_min = *std::min_element( result_vec.begin(), result_vec.end() );
        local_mean = mean( result_vec );
        local_tenth = percentile( result_vec, 0.10f );
        local_ninty = percentile( result_vec, 0.90f );
    }

    Result result = { local_lnopercent, local_max, local_min, local_mean, local_ninty, local_tenth};

    {

        std::lock_guard<std::mutex> lock1( Result_mutex );
        final_result = result;
    }

    is_calculating = false;
}


void results()
{
    if (is_calculating) 
    {
        ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(-1.0f, 0.0f), "Calculating...");
        return;  
    }

    float local_lnopercent, local_max, local_min, local_mean, local_ninty, local_tenth;
    {
        Result display_stats;
        {
            std::lock_guard<std::mutex> lock(Result_mutex);
            display_stats = final_result;
        }
        local_lnopercent = display_stats.lnopercent;
        local_max = display_stats.max;
        local_min = display_stats.min;
        local_mean = display_stats.mean;
        local_ninty = display_stats.ninty;
        local_tenth = display_stats.tenth;
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
