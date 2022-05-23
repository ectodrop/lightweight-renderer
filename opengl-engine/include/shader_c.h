#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <fstream>
#include <iostream>
#include <sstream>
class Shader {
    public:
        unsigned int ID;
        Shader(const char* computeShaderPath) {
            std::string content = loadFile(computeShaderPath);
            const char* source = content.c_str();

        }
        Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
            // >>>> COMPILE SHADER <<<<<
            // to get the compile status of the shaders
            int success;
            char infoLog[512];
            std::string vcontent = loadFile(vertexShaderPath);
            std::string fcontent = loadFile(fragmentShaderPath);
            const char* vertexSource = vcontent.c_str();
            const char* fragmentSource = fcontent.c_str();
      
            unsigned int vertexShader;
            // store the id of the created vertex shader
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            // assign the vertex shader code to the shader we just created
            glShaderSource(vertexShader, 1, &vertexSource, NULL);
            glCompileShader(vertexShader);
            // check the compile status
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "Error Compiling Vertex Shader: " << infoLog << std::endl;
                exit(-1);
            }

            // same thing as vertex shader except with fragment shader
            unsigned int fragmentShader;
            fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
            glCompileShader(fragmentShader);
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                std::cout << "Error Compiling Fragment Shader: " << infoLog << std::endl;
                exit(-1);
            }

            // now link the the shaders we just compiled into 1 program
            // this runs whenever we render
            unsigned int shaderProgram;
            shaderProgram = glCreateProgram();
            // the inputs and outputs of the shaders should correspond to one another
            // otherwise you get a linking error
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
            glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "Error Linking Shaders: " << infoLog << std::endl;
            }
            // glUseProgram(shaderProgram);
            // delete the vertex and fragment shaders, not needed after linking
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            ID = shaderProgram;
        }
        void use() {
            glUseProgram(ID);
        }
        void setFloat(const char *name, float val) {
            glUniform1f(glGetUniformLocation(ID, name), val);
        }
        void setInt(const char *name, int val) {
            glUniform1i(glGetUniformLocation(ID, name), val);
        }
        void setBool(const char *name, bool val) {
            glUniform1i(glGetUniformLocation(ID, name), (int)val);
        }
        void setVec4(const char *name, float x, float y, float z, float w) {
            glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
        }
        void setVec3(const char *name, float x, float y, float z) {
            glUniform3f(glGetUniformLocation(ID, name), x, y, z);
        }
        void setVec2(const char *name, float x, float y) {
            glUniform2f(glGetUniformLocation(ID, name), x, y);
        }
        void setMat4(const char *name, GLboolean transpose, float *value) {
            glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, transpose, value);
        }
        void free() {
            glDeleteShader(ID);
        }
    private:
        std::string loadFile(const char* path) {
            std::ifstream file;
            std::string sourceString;
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                file.open(path);
                // dump contents into buffer
                std::stringstream stream;
                stream << file.rdbuf();
                file.close();
                return stream.str();
            }
            catch (const std::ifstream::failure& e) {
                std::cout << "Error while reading shader file: " << e.what() << std::endl;
                exit(-1);
            }
            catch (const std::exception& e) {
                std::cout << "Unknown Error: " << e.what() << std::endl;
                exit(-1);
            }
            return "";
        }
};
#endif