#pragma once
#include "pch.h"
#include <ui/ui_component.h>
#include <rendering/scene.h>
class SceneTreeComponent : public UIComponent {
	int _selected_node = -1;
	Scene* _scene;
	void DrawTree(int parent_id = 0) {
		ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (_scene->_Scene_Tree[parent_id].size() == 0)
			tree_flags = ImGuiTreeNodeFlags_Leaf;
		if (_selected_node == parent_id) {
			tree_flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool is_open = ImGui::TreeNodeEx((void*)(intptr_t)parent_id, tree_flags, _scene->_Meshes[parent_id]->_Mesh_Name.c_str());

		if (ImGui::IsItemClicked()) {
			_selected_node = parent_id;
		}

		if (is_open) {
			for (int child_id : _scene->_Scene_Tree[parent_id]) {
				DrawTree(child_id);
			}
			ImGui::TreePop();
		}
	}

	void DrawInspector() {
		ImGuiSliderFlags flags = ImGuiSliderFlags_None;

		ImGui::PushItemWidth(50);
		auto mesh = _scene->_Meshes[_selected_node];
		ImGui::Text("Position");
		ImGui::DragFloat("x##1", &mesh->_Position.x, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
		ImGui::SameLine();
		ImGui::DragFloat("y##1", &mesh->_Position.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
		ImGui::SameLine();
		ImGui::DragFloat("z##1", &mesh->_Position.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);

		ImGui::Text("Euler angles");
		ImGui::DragFloat("x##2", &mesh->_Euler_Angles.x, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
		ImGui::SameLine();
		ImGui::DragFloat("y##2", &mesh->_Euler_Angles.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
		ImGui::SameLine();
		ImGui::DragFloat("z##2", &mesh->_Euler_Angles.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);

		ImGui::Text("Scale");
		ImGui::DragFloat("x##3", &mesh->_Scale.x, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
		ImGui::SameLine();
		ImGui::DragFloat("y##3", &mesh->_Scale.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
		ImGui::SameLine();
		ImGui::DragFloat("z##3", &mesh->_Scale.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);

		ImGui::PopItemWidth();

		_scene->_Meshes[0]->ResetModelMatrix();
		_scene->UpdateMeshMatrices(0);
	}
public:
	SceneTreeComponent(CallbackMap callbacks, VariableMap variables): UIComponent(callbacks, variables) {
		_scene = GetUIVar<Scene*>("main-scene");
	}

	void Render() override {
		ImGui::Begin("Scene Tree");
		if (_scene->_Meshes.size() > 0) DrawTree();
		else ImGui::Text("Nothing to see here :)");
		ImGui::End();
		ImGui::Begin("Mesh Inspector");
		if (_selected_node >= 0) DrawInspector();
		ImGui::End();
	}
};