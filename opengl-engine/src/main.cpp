#include "pch.h"

#include "engine.h"

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	INITIAL_WIDTH,
	INITIAL_HEIGHT,
	45.0f);

Ray mouse_ray;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

double prevMouseX = INITIAL_WIDTH / 2.0;
double prevMouseY = INITIAL_HEIGHT / 2.0;
bool mouse_dragging = false;
glm::vec2 mouse_drag_start;
bool firstMouseMove = true;
// callback to change the viewport size if the window is resized
// window is unused parameter
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	camera.width = width;
	camera.height = height;
}
void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	
	if (firstMouseMove) {
		prevMouseX = xPos;
		prevMouseY = yPos;
		firstMouseMove = false;
	}
	prevMouseX = xPos;
	prevMouseY = yPos;
	if (mouse_dragging) {
		float dx = xPos - mouse_drag_start.x;
		float dy = yPos - mouse_drag_start.y;
		//std:: cout << dx << " " << dy <<  std::endl;
		float sensitivity = 0.1f;
		camera.rotate(dy * -1.0f * sensitivity, dx * sensitivity);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mod) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			float x = (2 * prevMouseX - camera.width) / camera.width;
			float y = (camera.height - 2 * prevMouseY) / camera.height;
			mouse_ray.dir = glm::vec3(x, y, -1);
			mouse_ray.origin = camera.pos;
			mouse_dragging = true;
			mouse_drag_start = glm::vec2(prevMouseX, prevMouseY);
		}
		if (action == GLFW_RELEASE) {
			mouse_dragging = false;
			camera.lockRotation();
		}
	}
}
// normal mousewheels only provide y offset
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.fov += yoffset * -1;
	if (camera.fov < 1.0f) {
		camera.fov = 1.0f;
	}
	if (camera.fov > 45.0f) {
		camera.fov = 45.0f;
	}
}
void processInput(GLFWwindow* window) {
	float movementSpeed = 0.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		movementSpeed *= 5.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		// simply sets a variable called "windowShoudlClose" tracked by the library
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.translate(glm::normalize(camera.front * glm::vec3(1,0,1)) *movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.translate(glm::normalize(camera.front * glm::vec3(1,0,1)) * -1.0f * movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.translate(camera.right * movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.translate(camera.right * -1.0f * movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.translate(camera.up * movementSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.translate(camera.up * -1.0f * movementSpeed);
	}

	// std::cout << glm::to_string(camera.cameraPos) << std::endl;
}


unsigned int loadCubemap(std::vector<std::string> faces) {
	stbi_set_flip_vertically_on_load(false);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	for (int i = 0; i < 6; i++) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cout << "Failed to load texture: " << stbi_failure_reason() << std::endl;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

std::vector<float> genTorus(float inner, float outer, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
	int slices = 40;
	int sliceVertices = 40;
	float thickness = outer - inner;
	float center = outer - (thickness / 2);
	for (int slice = 0; slice < slices; slice++) {
		float yaw = slice * ((M_PI * 2.0f) / (float)slices);
		glm::vec3 offset(center * cos(yaw), 0.0f, center * sin(yaw));
		for (int vertex = 0; vertex < sliceVertices; vertex++) {
			float pitch = vertex * ((M_PI * 2.0f) / (float)sliceVertices);
			glm::vec3 circle = glm::vec3(
				thickness * cos(yaw) * cos(pitch),
				thickness * sin(pitch),
				thickness * sin(yaw) * cos(pitch)
			);
			// std::cout << (int)circle.x << "," << (int)circle.y << "," << (int)circle.z << std::endl;
			circle += offset;
			// insert vertex position
			vertices.insert(vertices.end(), { circle.x, circle.y, circle.z });
			circle -= offset;
			// insert vertex normal
			vertices.insert(vertices.end(), { circle.x, circle.y, circle.z });
		}
	}
	int vertexSize = 6;
	int rightOffset = sliceVertices;
	int numVertices = slices * sliceVertices;
	for (int i = 0; i < numVertices; i++) {
		int offset = 1;
		if ((i + 1) % sliceVertices == 0) offset += -1 * sliceVertices;
		indices.push_back(i);
		indices.push_back((i + rightOffset) % numVertices);
		indices.push_back((i + rightOffset + offset) % numVertices);

		indices.push_back(i);
		indices.push_back((i + offset) % numVertices);
		indices.push_back((i + rightOffset + offset) % numVertices);
		//std::cout << i << "," << (i+rightOffset) % numVertices<< "," << (i+rightOffset+offset) % numVertices<<std::endl;
		//std::cout << i << "," << (i+offset)  % numVertices<< "," << (i+rightOffset+offset) % numVertices<<std::endl;
	}
	//std::cout << vertices.size() << std::endl;
	return vertices;
}

void getComputeWorkSize() {
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

void WindowSetup(GLFWwindow** window) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	*window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	// switches the current context so all changes are applied to "window"
	// we could call this again with a different window
	glfwMakeContextCurrent(*window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}
	glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);

	// sets up a callback function whenever the size of the window is changed
	// callback is called once when it is first bound
	glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);
	//glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPosCallback(*window, mouse_callback);
	//glfwSetScrollCallback(*window, scroll_callback);

}
int main()
{
	OpenGLEngine* engine = new OpenGLEngine();
	while (!engine->ShouldClose())
	{
		engine->Update();
	}
	
	return 0;
}
