#ifndef ENGINE_H
#define ENGINE_H
#include "pch.h"
#include "constants.h"
#include <ui/ui_master.h>
#include <shader.h>
#include <camera.h>
#include <texture.h>
#include <mesh3d.h>
#include <scene.h>
#include <quad.h>
class OpenGLEngine {
	float delta_time = 0.0f, last_frame = 0.0f;
	UIContainer* ui_container;
	Shader* ray_shader;
	Quad* quad;
	Scene* scene;
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
	OpenGLEngine(){
		Init();
	}
	~OpenGLEngine() {
		OnDestroy();
	}
	void Init() {
		ui_container = new UIContainer();


		GetComputeWorkSize();

		GLuint tex_output;
		glGenTextures(1, &tex_output);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_output);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, INITIAL_WIDTH, INITIAL_HEIGHT, 0, GL_RGBA, GL_FLOAT,
			NULL);
		glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		ui_container->UISetVar("frame_buffer_tex", tex_output);
		ray_shader = new Shader("shaders\\ray.comp");
		//------------------ Main loop -----------------------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		// loops until the window is closed by the user
		// pressing the close window button sets this variable to true
		quad = new Quad();
		scene = new Scene();
		quad->Init();
		ui_container->UISetCallback("loader-open-file", [&](std::any a) {
			scene->LoadScene(std::any_cast<std::string>(a));
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
		delta_time = glfwGetTime() - last_frame;
		last_frame = glfwGetTime();
		// check for keystrokes
		//processInput(window);
		//glfwSetMouseButtonCallback(window, mouse_button_callback);
		// RENDERING COMMANDS
		// clears the color buffer with whatever the clearColour is set to
		// good practice to clear before every render cycle
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		ray_shader->useCompute(INITIAL_WIDTH, INITIAL_HEIGHT, 1);

		//rayShader.setMat3("camera_rotation", false, camera.rotateMatrix());
		//rayShader.setVec3("camera_position", camera.pos);
		//rayShader.setFloat("u_time", glfwGetTime());

		quad->RenderFullScreen();

		ui_container->Render();
		glfwSwapInterval(0);
		// gets rid of flickering using double buffers
		glfwSwapBuffers(ui_container->GetWindow());
		// checks if any events have been fired, and then calls any callbacks we assigned
		// i.e. keystrokes, controller input
		glfwPollEvents();
	}

	bool ShouldClose() {
		return glfwWindowShouldClose(ui_container->GetWindow());
	}
};

#endif