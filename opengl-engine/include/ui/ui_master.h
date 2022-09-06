#pragma once
#include "constants.h"
#include "pch.h"
#include "./components/menu_bar.cpp"
#include "./components/model_view.cpp"
#include "./components/scene_tree.cpp"
#include "./components/render_result.cpp"
#include "./components/performance_dialog.cpp"
#include "./components/bvh_debug.cpp"

class UIContainer {
	ImGuiIO* _io;
	GLFWwindow* _window;
	inline static std::map<std::string, std::function<void(std::optional<std::any>)>> _ui_callbacks;
	inline static std::map<std::string, std::optional<std::any>> _ui_variables;
	std::vector<UIComponent *> _components;
	UIComponent* _menubar;

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		//glViewport(0, 0, width, height);
		//_ui_callbacks["resize-window"](glm::vec2(width, height));
	}
	void AddComponent(UIComponent* ui_component) {
		_components.push_back(ui_component);
	}
public:
	UIContainer() {
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
	void InitComponents() {
		_menubar = new MenuBarComponent(_ui_callbacks, _ui_variables);
		AddComponent(new ModelViewComponent(_ui_callbacks, _ui_variables));
		AddComponent(new RenderResultComponent(_ui_callbacks, _ui_variables));
		AddComponent(new SceneTreeComponent(_ui_callbacks, _ui_variables));
		AddComponent(new PerformanceDialogComponent(_ui_callbacks, _ui_variables));
		AddComponent(new BVHDebugComponent(_ui_callbacks, _ui_variables));
	}
	void UISetVar(std::string var_name, std::optional<std::any> a) {
		_ui_variables[var_name] = a;
	}
	template <typename T>
	void UISetCallback(std::string callback_name, std::function<void(T)> callback) {
		_ui_callbacks[callback_name] = [=](std::optional<std::any> a) {
			if (a.has_value()) {
				callback(std::any_cast<T>(a.value()));
			}
		};
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
		glfwSetFramebufferSizeCallback(window, UIContainer::FramebufferSizeCallback);
		//glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		//glfwSetCursorPosCallback(*window, mouse_callback);
		//glfwSetScrollCallback(*window, scroll_callback);

	}

	GLFWwindow* GetWindow() {
		return _window;
	}
	
	void GetWindowDimensions(int* out_width, int* out_height) {
		glfwGetWindowSize(_window, out_width, out_height);
	}

	void SetWindowDimensions(int width, int height) {
		glfwSetWindowSize(_window, width, height);
	}
};