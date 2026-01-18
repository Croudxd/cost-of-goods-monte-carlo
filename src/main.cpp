#include <GLFW/glfw3.h>

#include <iostream>

#include "assets/inter_font.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "gui/gui.h"
#include "imgui.h"

#ifdef _WIN32
#include <windows.h>

int main(); 
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    return main();
}
#endif

int main()
{
    if ( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    GLFWwindow* window =
        glfwCreateWindow( 800, 600, "Cost of goods monte carlo", nullptr, nullptr );
    if ( !window )
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent( window );
    glfwSwapInterval( 1 ); 

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImFont* font = io.Fonts->AddFontFromMemoryTTF(
        (void*)Inter_18pt_Regular_ttf,
        sizeof( Inter_18pt_Regular_ttf ),
        18.0f );

    if ( font == nullptr )
    {
        std::cerr << "Bombaclart";
        return -1;
    }
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.FramePadding = ImVec2( 10, 8 );
    style.ItemSpacing = ImVec2( 12, 10 );
    style.WindowBorderSize = 0.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4( 0.09f, 0.09f, 0.10f, 1.00f );
    colors[ImGuiCol_Header] = ImVec4( 0.20f, 0.25f, 0.29f, 0.55f );
    colors[ImGuiCol_HeaderHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.80f );

    colors[ImGuiCol_Button] = ImVec4( 0.11f, 0.41f, 0.71f, 1.00f );
    colors[ImGuiCol_ButtonHovered] = ImVec4( 0.18f, 0.53f, 0.88f, 1.00f );
    colors[ImGuiCol_ButtonActive] = ImVec4( 0.06f, 0.33f, 0.58f, 1.00f );

    colors[ImGuiCol_FrameBg] = ImVec4( 0.16f, 0.16f, 0.17f, 1.00f );
    colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.22f, 0.22f, 0.23f, 1.00f );
    colors[ImGuiCol_FrameBgActive] = ImVec4( 0.28f, 0.28f, 0.29f, 1.00f );

    colors[ImGuiCol_CheckMark] = ImVec4( 0.28f, 0.56f, 1.00f, 1.00f );
    colors[ImGuiCol_SliderGrab] = ImVec4( 0.28f, 0.56f, 1.00f, 1.00f );
    colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.37f, 0.61f, 1.00f, 1.00f );

    ImGui_ImplGlfw_InitForOpenGL( window, true );
    ImGui_ImplOpenGL3_Init( "#version 130" );

    while ( !glfwWindowShouldClose( window ) )
    {
        glfwPollEvents();
        gui( window );
        int display_w, display_h;
        glfwGetFramebufferSize( window, &display_w, &display_h );
        glViewport( 0, 0, display_w, display_h );
        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

        glfwSwapBuffers( window );
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow( window );
    glfwTerminate();

    return 0;
}
