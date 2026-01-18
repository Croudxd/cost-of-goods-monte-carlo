#include "gui.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <mutex>
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
        std::lock_guard<std::mutex> lock1( objects_mutex );
        std::lock_guard<std::mutex> lock2( threeobjects_mutex );
        std::lock_guard<std::mutex> lock3( simsize_mutex );
        local_objects = objects;
        local_threeobjects = threeobjects;
        local_simsize = simsize;
    }

    std::vector<std::vector<float>> normal_distribution_vector;
    std::vector<std::vector<float>> triangle_distribution_vector;

    for ( int x = 0; x < objects.size(); x++ )
    {
        normal_distribution_vector.push_back( gen_normal_distribution_vector( local_simsize, local_objects[x] ) );
    }

    for ( int x = 0; x < threeobjects.size(); x++ )
    {
        triangle_distribution_vector.push_back( gen_triangle_distribution_vector( local_simsize, local_threeobjects[x] ) );
    }

    std::vector<int> step_three_batch = s3_nm_batches(
        triangle_distribution_vector[10], normal_distribution_vector[7] );
    std::vector<int> step_two_batch = s_num_batches(
        triangle_distribution_vector[10], normal_distribution_vector[4],
        normal_distribution_vector[6] );
    std::vector<int> step_one_batch = s_num_batches(
        triangle_distribution_vector[10], normal_distribution_vector[3],
        normal_distribution_vector[5] );

    std::vector<float> s3 =
        s_time( triangle_distribution_vector[7], step_three_batch,
                triangle_distribution_vector[8] );
    std::vector<float> s2 =
        s_time( triangle_distribution_vector[5], step_two_batch,
                triangle_distribution_vector[6] );
    std::vector<float> s1 =
        s_time( triangle_distribution_vector[3], step_one_batch,
                triangle_distribution_vector[4] );

    std::vector<float> tot_time =
        Tot_Time( s3, s2, s1, triangle_distribution_vector[9] );
    std::vector<float> rm =
        RM( normal_distribution_vector[2], normal_distribution_vector[0],
            triangle_distribution_vector[0], normal_distribution_vector[1],
            triangle_distribution_vector[1] );

    std::vector<float> lno = LnO( tot_time, triangle_distribution_vector[2],
                                  triangle_distribution_vector[10] );
    std::vector<float> results;
    for ( int x = 0; x < rm.size(); x++ )
    {
        float result = rm[x] + lno[x];
        results.push_back( result );
    }

    std::lock_guard<std::mutex> lock( max_result_mutex );
    max_result = *std::max_element( results.begin(), results.end() );

    std::lock_guard<std::mutex> lnopercent_lock( lnopercent_result_mutex );
    lnopercent_result = mean( Percentage_LnO( rm, lno ) );

    std::lock_guard<std::mutex> min_result_lock( min_result_mutex );
    min_result = *std::min_element( results.begin(), results.end() );

    std::lock_guard<std::mutex> mean_result_lock( mean_result_mutex );
    mean_result = mean( results );
    // 10th and 90th
    std::lock_guard<std::mutex> tenthpercentile_lock( tenthpercentile_result_mutex );
    tenthpercentile_result = percentile( results, 0.10f );

    std::lock_guard<std::mutex> nintypercentile_lock( nintypercentile_result_mutex );
    nintypercentile_result = percentile( results, 0.90f );
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
