#include "Gui.h"

static int counter = 0;

void Gui::basicMenu()
{
    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    if (ImGui::Button("Button"))
        counter++;
    ImGui::SameLine();


    ImGui::Text("counter = %d", counter);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}



void Gui::Theme()
{

}