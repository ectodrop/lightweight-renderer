#pragma once
#include "pch.h"
#include <ui/ui_component.h>
#include <rendering/scene.h>
class SceneTreeComponent : public UIComponent {
public:
	SceneTreeComponent(CallbackMap& callbacks, VariableMap& variables) : UIComponent(callbacks, variables) {
		_scene = GetUIVar<Scene*>("main_scene");
	}

	void Render() override {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(250, 0));

		ImGui::Begin("Scene Tree");
		if (_scene->_Meshes.size() > 0) DrawTree();
		else ImGui::Text("Nothing to see here :)");
		ImGui::End();
		ImGui::Begin("Mesh Inspector");
		// std::cout << std::any_cast<int>(_Variables["selected_mesh"]) << std::endl;
		if (GetSelectedNode() >= 0) DrawInspector();
		ImGui::End();

		ImGui::PopStyleVar();
	}
private:
	Scene* _scene;
	int GetSelectedNode() {
		return GetUIVar<int>("selected_mesh");
	}
	void DrawTree(int parent_id = 0) {
		ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (_scene->_Scene_Tree[parent_id].size() == 0)
			tree_flags = ImGuiTreeNodeFlags_Leaf;
		if (GetSelectedNode() == parent_id) {
			tree_flags |= ImGuiTreeNodeFlags_Selected;
		}
		bool is_open = ImGui::TreeNodeEx((void*)(intptr_t)parent_id, tree_flags, _scene->_Meshes[parent_id]->_Mesh_Name.c_str());

		if (ImGui::IsItemClicked()) {
			SetUIVar("selected_mesh", parent_id);
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
		auto mesh = _scene->_Meshes[GetSelectedNode()];
		ImGui::Checkbox("Visible", &mesh->_Visible);

		if (ImGui::CollapsingHeader("Transform")) {
			ImGui::PushItemWidth(50);
			ImGui::Text("Position");
			ImGui::DragFloat("x##pos", &mesh->_Position.x, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
			ImGui::SameLine();
			ImGui::DragFloat("y##pos", &mesh->_Position.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
			ImGui::SameLine();
			ImGui::DragFloat("z##pos", &mesh->_Position.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);

			ImGui::Text("Euler angles");
			ImGui::DragFloat("x##ang", &mesh->_Euler_Angles.x, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
			ImGui::SameLine();
			ImGui::DragFloat("y##ang", &mesh->_Euler_Angles.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
			ImGui::SameLine();
			ImGui::DragFloat("z##ang", &mesh->_Euler_Angles.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);

			ImGui::Text("Scale");
			ImGui::DragFloat("s", &mesh->_Scale.x, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
			ImGui::SameLine();
			ImGui::DragFloat("s", &mesh->_Scale.y, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);
			ImGui::SameLine();
			ImGui::DragFloat("s", &mesh->_Scale.z, 0.05f, -FLT_MAX, +FLT_MAX, "%.3f", flags);

			ImGui::PopItemWidth();
		}

		if (ImGui::CollapsingHeader("Material")) {
			auto mesh = _scene->_Meshes[GetSelectedNode()];
			auto diff = mesh->_Mesh_Material.Diffuse;
			auto spec = mesh->_Mesh_Material.Specular;
			auto emm = mesh->_Mesh_Material.Emissive;

			float diffuse[3] = { diff.r, diff.g, diff.b };
			ImGui::ColorEdit3("Diffuse", diffuse, ImGuiColorEditFlags_Float);
			mesh->_Mesh_Material.Diffuse.r = diffuse[0];
			mesh->_Mesh_Material.Diffuse.g = diffuse[1];
			mesh->_Mesh_Material.Diffuse.b = diffuse[2];

			float specular[3] = { spec.r, spec.g, spec.b };

			ImGui::ColorEdit3("Specular", specular, ImGuiColorEditFlags_Float);
			mesh->_Mesh_Material.Specular.r = specular[0];
			mesh->_Mesh_Material.Specular.g = specular[1];
			mesh->_Mesh_Material.Specular.b = specular[2];

			float emissive[3] = { emm.r, emm.g, emm.b };

			ImGui::ColorEdit3("Emissive", emissive, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR);
			mesh->_Mesh_Material.Emissive.r = emissive[0];
			mesh->_Mesh_Material.Emissive.g = emissive[1];
			mesh->_Mesh_Material.Emissive.b = emissive[2];
		}

		_scene->_Meshes[0]->ResetModelMatrix();
		_scene->UpdateMeshMatrices(0);
	}
};