#pragma once
#include "constants.h"
#include "pch.h"
#include "./components/menu_bar.cpp"

class UIContainer {
	ImGuiIO* _io;
	GLFWwindow* _window;
	std::map<std::string, std::function<void(std::any)>> _ui_callbacks;
	std::map<std::string, std::any> _ui_variables;
	std::vector<UIComponent *> _components;
	UIComponent* _menubar;
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);
	}
	void InitComponents() {
		_menubar = new MenuBarComponent(_ui_callbacks, _ui_variables);
		// _components.push_back();
	}
public:
	UIContainer() {
		InitComponents();
		WindowSetup(_window);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		_io = &ImGui::GetIO(); (void)*_io;
		_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(_window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}
	void UISetVar(std::string var_name, std::any a) {
		_ui_variables[var_name] = a;
	}
	void UISetCallback(std::string callback_name, std::function<void(std::any)> callback) {
		_ui_callbacks[callback_name] = callback;
	}
	void PreRender() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	}
	void Render() {
		PreRender();
		_menubar->Render();
		ImGui::ShowDemoWindow();
		for (auto& component : _components) {
			component->Render();
		}
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// needed if Viewports are enabled
		if (_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
	void WindowSetup(GLFWwindow* &window) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "OpenGL", NULL, NULL);
		if (window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(-1);
		}

		// switches the current context so all changes are applied to "window"
		// we could call this again with a different window
		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(-1);
		}
		glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);

		// sets up a callback function whenever the size of the window is changed
		// callback is called once when it is first bound
		glfwSetFramebufferSizeCallback(window, UIContainer::framebuffer_size_callback);
		//glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetCursorPosCallback(*window, mouse_callback);
		//glfwSetScrollCallback(*window, scroll_callback);

	}
	GLFWwindow* GetWindow() {
		return _window;
	}
	
};