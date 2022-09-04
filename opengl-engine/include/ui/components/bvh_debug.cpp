#pragma once
#include "pch.h"
#include <ui/ui_component.h>
#include <rendering/scene.h>
#include <accel/bvh.h>

class BVHDebugComponent : public UIComponent {
	using UIComponent::UIComponent;
public:
	void Render() override {
		ImGui::Begin("BVH");
		auto bvh = GetUIVar<BVH*>("main_bvh");
		if (bvh != nullptr) {
			if (_checked_nodes.size() < bvh->GetNodes().size()) {
				_checked_nodes.resize(bvh->GetNodes().size(), false);
			}
			DrawTree(bvh, 0);
		}

		ImGui::End();
	}
private:
	std::vector<int> _checked_nodes = std::vector<int>();

	void PropagateCheck(BVH* bvh, int node_id, bool val) {
		if (node_id < 0) return;
		auto node = bvh->GetNodes()[node_id];
		if (node->right >= 0) 
			_checked_nodes[node->right] = val;
		if (node->left >= 0)
			_checked_nodes[node->left] = val;

		PropagateCheck(bvh, node->right, val);
		PropagateCheck(bvh, node->left, val);
	}

	void DrawTree(BVH* bvh, int node_id) {
		if (node_id < 0) return;
		auto node = bvh->GetNodes()[node_id];
		ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_Framed;
		if (node->left == -1 && node->right == -1)
			tree_flags = ImGuiTreeNodeFlags_Leaf;
		std::string id = "##" + std::to_string(node_id);
		ImGui::Checkbox(id.c_str(), (bool*) (_checked_nodes.data() + node_id)); ImGui::SameLine();
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
			PropagateCheck(bvh, node_id, !_checked_nodes[node_id]);
			InvokeUICallback("redraw-debug-bvh-boxes", _checked_nodes);
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			_checked_nodes[node_id] = !_checked_nodes[node_id];
			InvokeUICallback("redraw-debug-bvh-boxes", _checked_nodes);
		}
		bool is_open = ImGui::TreeNodeEx((void*)(intptr_t)node_id, tree_flags, "Node %d", node_id);
		if (is_open) {
			DrawTree(bvh, node->left);
			DrawTree(bvh, node->right);
			ImGui::TreePop();
		}
	}
};