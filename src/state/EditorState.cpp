#include "EditorState.h"

#include "imgui.h"

void EditorState::update(float dt)
{

}

void EditorState::render(float dt)
{
    ImGui::Begin("Hello, world!");
    ImGui::Button("Look at this pretty button");
    ImGui::End();
}
