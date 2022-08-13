#pragma once
#include "pch.h"

class InputSource {
	inline static std::vector<std::function<void(int, int, int, int)>> _key_listeners;
public:
	InputSource(GLFWwindow* window) {
		_key_listeners = std::vector<std::function<void(int, int, int, int)>>();
		glfwSetKeyCallback(window, InputSource::OnKeyPress);
	}
	static void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
		for (auto& callback : _key_listeners) {
			callback(key, scancode, action, mods);
		}
	}
	void AddKeyListener(std::function<void(int, int, int, int)> func) {
		_key_listeners.push_back(func);
	}
};