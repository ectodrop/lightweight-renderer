#pragma once
#include "pch.h"
#include <ui/ui_component.h>
#include <rendering/scene.h>
class RenderResultComponent : public UIComponent {
	using UIComponent::UIComponent;
public:
	void Render() override {
		ImVec2 dim = ImVec2(GetUIVar<glm::vec2>("scene_window_size").x, GetUIVar<glm::vec2>("scene_window_size").y);
		ImGui::SetNextWindowSize(dim);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Render");

		ImVec2 vmin = ImGui::GetWindowContentRegionMin();
		ImVec2 vmax = ImGui::GetWindowContentRegionMax();
		ImVec2 wsize = ImVec2(vmax.x - vmin.x, vmax.y - vmin.y);

		ImGui::Image((ImTextureID)GetUIVar<unsigned int>("pathtrace_screen_tex"), wsize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::PopStyleVar();
		ImGui::End();
	}
};