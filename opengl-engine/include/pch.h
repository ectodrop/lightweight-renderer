#ifndef PCH_H
#define PCH_H
#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <cmath>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <stack>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <stdlib.h>
#include <windows.h>
#include <Commdlg.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#endif // !PCH_H