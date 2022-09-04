#pragma once
#include "pch.h"


class UIComponent {
protected:
	typedef std::map<std::string, std::function<void(std::optional<std::any>)>> CallbackMap;
	typedef std::map<std::string, std::optional<std::any>> VariableMap;
	CallbackMap &_Callbacks;
	VariableMap &_Variables;
	template <typename T>
	T GetUIVar(std::string name) {
		if (_Variables[name].has_value()) {
			return std::any_cast<T>(_Variables[name].value());
		}
		// return default T value
		return {};
	}
	void SetUIVar(std::string name, std::optional<std::any> val) {
		_Variables[name] = val;
	}
	void InvokeUICallback(std::string name, std::optional<std::any> arg) {
		_Callbacks[name](arg);
	}
public:
	UIComponent(CallbackMap &ui_callbacks, VariableMap &ui_variables): _Callbacks(ui_callbacks), _Variables(ui_variables) {}
	virtual void Render() = 0;
};

