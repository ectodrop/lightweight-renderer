#pragma once
#include "pch.h"
#include <ui/ui_component.h>
#include "constants.h"

class ModelViewComponent: public UIComponent{
	using UIComponent::UIComponent;
	void Render() override {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Scene");

		ImVec2 vmin = ImGui::GetWindowContentRegionMin();
		ImVec2 vmax = ImGui::GetWindowContentRegionMax();
		ImVec2 wsize = ImVec2(vmax.x - vmin.x, vmax.y - vmin.y);
		vmin.x += ImGui::GetWindowPos().x;
		vmin.y += ImGui::GetWindowPos().y;
		vmax.x += ImGui::GetWindowPos().x;
		vmax.y += ImGui::GetWindowPos().y;
		ImVec2 absolute_mouse_pos = ImGui::GetMousePos();
		glm::vec2 relative_mouse_pos(absolute_mouse_pos.x - vmin.x, absolute_mouse_pos.y - vmin.y);
		ImGui::Image((ImTextureID)GetUIVar<unsigned int>("frame_buffer_tex"), wsize, ImVec2(0, 1), ImVec2(1, 0));

		InvokeUICallback("resize-window", glm::vec2(wsize.x, wsize.y));
		if (ImGui::IsWindowFocused()) {
			auto left_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			auto right_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			InvokeUICallback("on-scene-left-drag", glm::vec2(left_delta.x, left_delta.y));
			InvokeUICallback("on-scene-right-drag", glm::vec2(right_delta.x, right_delta.y));
		}
		float mouse_dir = ImGui::GetIO().MouseWheel;
		if (ImGui::IsWindowHovered()) {
			if (mouse_dir) {
				InvokeUICallback("on-scene-mouse-wheel", mouse_dir);
			}

			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				glm::vec2 window_size(wsize.x, wsize.y);
				relative_mouse_pos = (relative_mouse_pos * 2.0f - window_size) / window_size;
				relative_mouse_pos.y *= -1.0f;
				// std::cout << relative_mouse_pos.x << "," << relative_mouse_pos.y << std::endl;

				InvokeUICallback("on-scene-dblclick", relative_mouse_pos);
			}
		}


		ImGui::End();
		ImGui::PopStyleVar();
	}
};