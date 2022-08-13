#pragma once
#include "pch.h"
#include "constants.h"
#include "loader.h"
#include "input_source.h"
#include "quad.h"
#include <ui/ui_master.h>
#include <rendering/shader.h>
#include <rendering/camera.h>
#include <rendering/renderer.h>
#include <rendering/raster_renderer.cpp>
#include <rendering/mesh3d.h>
#include <rendering/scene.h>

class OpenGLEngine {

	float _delta_time = 0.0f, _last_frame = 0.0f;
	UIContainer* _ui_container;
	Scene* _scene;
	Renderer* _renderer;
	Loader* _loader;
	InputSource* _input_source;
	Quad* quad;

	void GetComputeWorkSize() {
		int work_grp_cnt[3];

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

		printf("max global (total) work group counts x:%i y:%i z:%i\n",
			work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);


		int work_grp_size[3];

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

		printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
			work_grp_size[0], work_grp_size[1], work_grp_size[2]);
		int work_grp_inv;
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
		printf("max local work group invocations %i\n", work_grp_inv);
	}
public:
	OpenGLEngine() {
		Init();
	}
	~OpenGLEngine() {
		OnDestroy();
	}
	void Init() {
		_scene = new Scene();
		_loader = new Loader();
		_ui_container = new UIContainer();
		_renderer = new RasterRenderer(_scene);
		_input_source = new InputSource(_ui_container->GetWindow());


		_ui_container->UISetVar("main-scene", _scene);
		_ui_container->InitComponents();
		_input_source->AddKeyListener([&](int key, int scancode, int action, int mod) {
			_scene->GetCamera()->HandleKeyInput(key, scancode, action, mod);
		});

		GetComputeWorkSize();
		SetUICallbacks();
		GLuint tex_output;
		//glGenTextures(1, &tex_output);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, tex_output);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, INITIAL_WIDTH, INITIAL_HEIGHT, 0, GL_RGBA, GL_FLOAT,
		//	NULL);
		//glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		//------------------ Main loop -----------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		// loops until the window is closed by the user
		// pressing the close window button sets this variable to true
		
		
		quad = new Quad();
	}

	void SetUICallbacks() {
		_ui_container->UISetCallback<glm::vec2>("on-scene-drag", [&](glm::vec2 drag_delta) {
			_scene->GetCamera()->HandleMouseDrag(drag_delta);
		});
		_ui_container->UISetCallback<float>("on-scene-mouse-wheel", [&](float drag_delta) {
			_scene->GetCamera()->HandleMouseScroll(drag_delta);
		});

		_ui_container->UISetCallback<std::string>("loader-open-file", [&](std::string path) {
			_loader->LoadScene(path, _renderer, _scene);
		});
		
		_ui_container->UISetCallback<glm::vec2>("resize-window", [&](glm::vec2 dim) {
			_renderer->ResizeTexture(dim.x, dim.y);
		});
	}

	void OnDestroy() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glFinish(); // hangs until all previous opengl calls are finished (to get accurate times)
		// just freeing any resources that were allocated by GLFW
		glfwTerminate();
	}

	void Update() {
		// compute time between frames
		_delta_time = glfwGetTime() - _last_frame;
		_last_frame = glfwGetTime();
		// check for keystrokes
		//processInput(window);
		//glfwSetMouseButtonCallback(window, mouse_button_callback);
		// RENDERING COMMANDS

		// set var every frame because it can change
		_ui_container->UISetVar("frame_buffer_tex", _renderer->GetScreenTexture());
		

		//ray_shader->useCompute(INITIAL_WIDTH, INITIAL_HEIGHT, 1);
		_renderer->RenderScene();
		//rayShader.setMat3("camera_rotation", false, camera.rotateMatrix());
		//rayShader.setVec3("camera_position", camera.pos);
		//rayShader.setFloat("u_time", glfwGetTime());

		_ui_container->Render();

		glfwSwapInterval(0);
		// gets rid of flickering using double buffers
		glfwSwapBuffers(_ui_container->GetWindow());
		// checks if any events have been fired, and then calls any callbacks we assigned
		// i.e. keystrokes, controller input
		glfwPollEvents();
	}

	bool ShouldClose() {
		return glfwWindowShouldClose(_ui_container->GetWindow());
	}
};
