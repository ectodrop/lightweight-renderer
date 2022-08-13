#pragma once
#include "pch.h"
#include <ui/ui_component.h>
#include "constants.h"

class ModelViewComponent: public UIComponent{
	using UIComponent::UIComponent;
	void Render() override {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Scene");
		if (ImGui::IsWindowFocused()) {
			auto delta = ImGui::GetMouseDragDelta();
			InvokeUICallback("on-scene-drag", glm::vec2(delta.x, delta.y));
		}
		float mouse_dir = ImGui::GetIO().MouseWheel;
		if (mouse_dir && ImGui::IsWindowHovered()) {
			InvokeUICallback("on-scene-mouse-wheel", mouse_dir);
		}

		ImVec2 vmin = ImGui::GetWindowContentRegionMin();
		ImVec2 vmax = ImGui::GetWindowContentRegionMax();
		ImVec2 wsize = ImVec2(vmax.x - vmin.x, vmax.y - vmin.y);
		ImGui::Image((ImTextureID)GetUIVar<unsigned int>("frame_buffer_tex"), wsize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();
	}
};