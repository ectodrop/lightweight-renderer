#pragma once
#include "pch.h"
class UIComponent {
protected:
	typedef std::map<std::string, std::function<void(std::any)>> CallbackMap;
	typedef std::map<std::string, std::any> VariableMap;
	CallbackMap &_Callbacks;
	VariableMap &_Variables;
	template <typename T>
	T GetUIVar(std::string name) {
		return std::any_cast<T>(_Variables[name]);
	}
	void InvokeUICallback(std::string name, std::any arg) {
		return _Callbacks[name](arg);
	}
public:
	UIComponent(CallbackMap &ui_callbacks, VariableMap &ui_variables): _Callbacks(ui_callbacks), _Variables(ui_variables) {}
	virtual void Render() = 0;
};

