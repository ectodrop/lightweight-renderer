#pragma once
#include "pch.h"
#include "constants.h"
#include "loader.h"
#include "input_source.h"
#include "quad.h"
#include <yaml-cpp/yaml.h>
#include <ui/ui_master.h>
#include <rendering/shader.h>
#include <rendering/camera.h>
#include <rendering/renderer.h>
#include <rendering/raster_renderer.cpp>
#include <rendering/pathtrace_renderer.cpp>
#include <rendering/mesh3d.h>
#include <rendering/scene.h>
static void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "[High Severity]: " << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "[Medium Severity]: " << message << std::endl;
		break;
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "[Low Severity]: " << message << std::endl;
		break;
	default:
		break;
	}
}

YAML::Emitter& operator << (YAML::Emitter& out, glm::vec3 vec) {
	out << YAML::Flow;
	out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
	return out;
}


class OpenGLEngine {

	float _delta_time = 0.0f, _last_frame = 0.0f;
	UIContainer* _ui_container;
	Scene* _scene;
	RasterRenderer* _raster_renderer;
	PathtraceRenderer* _path_tracer_renderer;
	Loader* _loader;
	InputSource* _input_source;

	bool _accumulate_samples = false;

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
		
		glDebugMessageCallback(OpenGLLogMessage, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		
		_raster_renderer = new RasterRenderer(_scene);
		_path_tracer_renderer = new PathtraceRenderer(_scene);
		_input_source = new InputSource(_ui_container->GetWindow());

		_ui_container->UISetVar("scene_window_size", glm::vec2(INITIAL_WIDTH, INITIAL_HEIGHT));

		_ui_container->UISetVar("main_scene", _scene);
		_ui_container->UISetVar("main_bvh", std::nullopt);
		_ui_container->UISetVar("pathtrace_screen_tex", _path_tracer_renderer->GetScreenTexture());
		_ui_container->UISetVar("selected_mesh", -1);


		// should call after all ui vars are initialized
		_ui_container->InitComponents();
		_input_source->AddKeyListener([&](int key, int scancode, int action, int mod) {
			_scene->GetCamera()->HandleKeyInput(key, scancode, action, mod);
		});

		GetComputeWorkSize();
		SetUICallbacks();

		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);
	}

	void SetUICallbacks() {
		_ui_container->UISetCallback<glm::vec2>("on-scene-left-drag", [&](glm::vec2 drag_delta) {
			_scene->GetCamera()->RotateByDrag(drag_delta);
		});

		_ui_container->UISetCallback<glm::vec2>("on-scene-right-drag", [&](glm::vec2 drag_delta) {
			_scene->GetCamera()->OffsetCenterByDrag(drag_delta);
		});

		_ui_container->UISetCallback<float>("on-scene-mouse-wheel", [&](float drag_delta) {
			_scene->GetCamera()->HandleMouseScroll(drag_delta);
		});

		_ui_container->UISetCallback<std::string>("loader-open-file", [&](std::string path) {
			_loader->LoadScene(path, _raster_renderer, _path_tracer_renderer, _scene);
		});
		
		_ui_container->UISetCallback<glm::vec2>("resize-window", [&](glm::vec2 dim) {
			_scene->GetCamera()->width = dim.x;
			_scene->GetCamera()->height = dim.y;
			_ui_container->UISetVar("scene_window_size", dim);
		});

		_ui_container->UISetCallback<glm::vec2>("on-scene-dblclick", [&](glm::vec2 screen_coords) {
			auto t1 = std::chrono::high_resolution_clock::now();
			int mesh = _raster_renderer->IntersectScene(_scene->GetCamera()->GetWorldSpaceRay(screen_coords));
			auto t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> ms_double = t2 - t1;
			std::string name = "None";
			if (mesh >= 0) {
				name = _scene->_Meshes[mesh]->_Mesh_Name;
				 _ui_container->UISetVar("selected_mesh", mesh);
				 _ui_container->UISetVar("main_bvh", _raster_renderer->GetBVH());
			}
			std::cout << name << ": " << ms_double.count() << "ms" << std::endl;
		});

		_ui_container->UISetCallback<bool>("render-traced-scene", [&](bool is_rendering) {
			_accumulate_samples = is_rendering;
			if (is_rendering) {
				_path_tracer_renderer->BuildScene();
			}
			_ui_container->UISetVar("pathtrace_screen_tex", _path_tracer_renderer->GetScreenTexture());
		});

		_ui_container->UISetCallback<int>("recompile-shaders", [&](int _) {
			_raster_renderer->CompileShaders();
			_path_tracer_renderer->CompileShaders();
		});

		_ui_container->UISetCallback<std::vector<int>>("redraw-debug-bvh-boxes", [&](std::vector<int> flagged_nodes) {
			_raster_renderer->RedrawBVHBoxes(flagged_nodes);
		});

		_ui_container->UISetCallback<std::string>("export-scene", [&](std::string file_name) {
			YAML::Emitter out;
			out << YAML::BeginMap;
			out << YAML::Key << "name" << YAML::Value << "name";

			// camera
			auto cam = _scene->GetCamera();
			out << YAML::Key << "camera";
			out << YAML::Value;
			{
				out << YAML::BeginMap;
				out << YAML::Key << "pos" << YAML::Value << cam->pos;
				out << YAML::Key << "pitch" << YAML::Value << cam->pitch;
				out << YAML::Key << "yaw" << YAML::Value << cam->yaw;
				out << YAML::Key << "center" << YAML::Value << cam->center;
				out << YAML::EndMap;
			}
			out << YAML::Key << "meshes";
			out << YAML::Value;
			{
				out << YAML::BeginSeq;
				for (auto mesh : _scene->_Meshes) {
					out << YAML::BeginMap;
					out << YAML::Key << "visible" << YAML::Value << mesh->_Visible;
					out << YAML::Key << "name" << YAML::Value << mesh->_Mesh_Name;
					out << YAML::Key << "position" << YAML::Value << mesh->_Position;
					out << YAML::Key << "euler_angles" << YAML::Value << mesh->_Euler_Angles;
					out << YAML::Key << "scale" << YAML::Value << mesh->_Scale;
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
			}
			out << YAML::EndMap;

			std::ofstream scene_file(file_name);
			scene_file << out.c_str();
			scene_file.close();
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
		_ui_container->UISetVar("frame_buffer_tex", _raster_renderer->GetScreenTexture());
		_ui_container->UISetVar("delta_time", _delta_time * 1000);

		//ray_shader->useCompute(INITIAL_WIDTH, INITIAL_HEIGHT, 1);
		_raster_renderer->RenderScene();
		//rayShader.setMat3("camera_rotation", false, camera.rotateMatrix());
		//rayShader.setVec3("camera_position", camera.pos);
		//rayShader.setFloat("u_time", glfwGetTime());
		if (_accumulate_samples){
			_path_tracer_renderer->RenderScene();
		}
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
