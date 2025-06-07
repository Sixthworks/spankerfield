#include "gui.h"

void GUI::RenderMainWindow() {
    // Style anpassungen
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Modernere Farben und Rundungen
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.33f, 0.37f, 1.00f);
    
    // Hauptfenster
    ImGui::Begin("Spankerfield", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Tabs für bessere Organisation
    if (ImGui::BeginTabBar("MainTabs")) {
        
        // Combat Tab
        if (ImGui::BeginTabItem("Combat")) {
            ImGui::BeginGroup();
            // ...existing combat related features...
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        
        // Visual Tab
        if (ImGui::BeginTabItem("Visuals")) {
            ImGui::BeginGroup();
            // ...existing visual features... 
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        
        // Misc Tab
        if (ImGui::BeginTabItem("Misc")) {
            ImGui::BeginGroup();
            // ...existing misc features...
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        
        // Settings Tab
        if (ImGui::BeginTabItem("Settings")) {
            ImGui::BeginGroup();
            // ...existing settings...
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}
