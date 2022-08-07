#pragma once
#include "pch.h"
class UIComponent {
protected:
	std::map<std::string, std::function<void(std::any)>> &ui_callbacks;
	std::map<std::string, std::any> &ui_variables;
public:
	UIComponent(std::map<std::string, std::function<void(std::any)>> &callbacks,
		std::map<std::string, std::any> &vars) : ui_callbacks(callbacks), ui_variables(vars) {}
	virtual void Render() = 0;
};

