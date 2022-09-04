#pragma once
#include "renderer.h"
#include <accel/bvh.h>

#define MAX_SCENE_OBJECTS 1024
#define NUM_ATTRIBUTES 5

class RasterRenderer : public Renderer {
	using Renderer::Renderer;
    std::vector<std::vector<Triangle>> _mesh_to_triangles_map = std::vector<std::vector<Triangle>>(MAX_SCENE_OBJECTS);
    // vertex buffer object, vertex array object, element buffer object, frame buffer object
	unsigned int _vbo[NUM_ATTRIBUTES], _vao, _ebo[MAX_SCENE_OBJECTS], _fbo, _rbo;
    unsigned int _fbo_tex;

    unsigned int _grid_vao, _grid_vbo;
    unsigned int _box_vao, _box_vbo;
    Shader* _axis_shader;
    
    std::vector<glm::vec3> _bb_vertices = std::vector<glm::vec3>(1);
    std::vector<glm::vec3> _grid_vertices = {
        { -10000.0f, 0.0f, 0.0f },
        { 10000.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, -10000.0f },
        { 0.0f, 0.0f, 10000.0f },
        { 0.0f, -10000.0f, 0.0f },
        { 0.0f, 10000.0f, 0.0f },
        { 0.0f, 10000.0f, 0.0f },
        { 0.0f, 10000.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
    };
public:
    RasterRenderer(Scene* scene) {
        _Scene = scene;
        CompileShaders();
        Init();
    }
    /// <summary>
    /// returns the index of the mesh that the ray intersects with, -1 if none
    /// </summary>
    /// <param name="ray"></param>
    /// <returns></returns>
    int IntersectScene(Ray ray) {
        _grid_vertices.pop_back();
        _grid_vertices.pop_back();
        _grid_vertices.push_back(ray.origin);
        _grid_vertices.push_back(ray.origin + ray.dir * 1000.0f);
        InitGrid();
        if (_bvh == NULL) {
            _bvh = new BVH(_Triangles, _Vertices, _Scene);
            _bvh->BuildBVH();
        }
        int result = _bvh->BVHIntersect(ray);
        if (result < 0) return -1;
        Bound b = _bvh->GetNodes()[result]->bounds;
        int tri = _bvh->GetIndices()[_bvh->GetNodes()[result]->start_prim];
        return _Scene->_Triangle_Mesh_Map[tri];
    }
	void Init() override {
        InitFrameBuffer();
        InitDebugBoxes();
        InitGrid();

        glGenVertexArrays(1, &_vao);
        // after binding a VAO, all subsequent calls to:
        // glEnable/DisableVertexAttribArray, glVertexAttribPointer
        // the Vetex buffer objects that are associated with the Vertex Attributes are also saved
        glBindVertexArray(_vao);
        // stores the id of the Vertex Buffer Object that is created
        glGenBuffers(NUM_ATTRIBUTES, _vbo);
        unsigned int
            vertex_vbo = _vbo[0],
            normal_vbo = _vbo[1],
            tangent_vbo = _vbo[2],
            bitangent_vbo = _vbo[3],
            texc_vbo = _vbo[4];
        // assigns the VBO to a variable tracked by the library
        // store the vertex data in the variable tracked by the library
        // use GL_STATIC_DRAW becaus we will use the data a lot and it won't change
        if (_Vertices.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_Vertices[0]) * _Vertices.size(), &_Vertices[0], GL_STATIC_DRAW);
        }
        
        if (_Normals.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_Normals[0]) * _Normals.size(), &_Normals[0], GL_STATIC_DRAW);
        }
        
        if (_Tangents.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, tangent_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_Tangents[0]) * _Tangents.size(), &_Tangents[0], GL_STATIC_DRAW);
        }

        if (_Bitangents.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, bitangent_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_Bitangents[0]) * _Bitangents.size(), &_Bitangents[0], GL_STATIC_DRAW);
        }

        if (_Texcoords.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, texc_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_Texcoords[0]) * _Texcoords.size(), &_Texcoords[0], GL_STATIC_DRAW);
        }

        // tells opengl how to interpret the vertex attributes with respect to the currently assigned Array Buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, tangent_vbo);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, bitangent_vbo);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, texc_vbo);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(4);


        glGenBuffers(MAX_SCENE_OBJECTS, _ebo);

        if (_Triangles.size() > 0) {
            for (int i = 0; i < _Triangles.size(); i++) {
                int mesh_id = _Scene->_Triangle_Mesh_Map[i];
                _mesh_to_triangles_map[mesh_id].push_back(_Triangles[i]);
            }
            for (int i = 0; i < MAX_SCENE_OBJECTS; i++) {
                auto triangles = _mesh_to_triangles_map[i];
                if (triangles.size() > 0) {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo[i]);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles[0]) * triangles.size(), &triangles[0], GL_STATIC_DRAW);
                }
            }
        }

	}
    void RenderScene() override {
        glm::mat4 view, clip, model;
        auto cam = _Scene->GetCamera();

        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw basis axis        
        _axis_shader->use();
        view = cam->ViewMatrix();
        clip = cam->ClipMatrix();
        _axis_shader->setMat4("view", GL_FALSE, glm::value_ptr(view));
        _axis_shader->setMat4("clip", GL_FALSE, glm::value_ptr(clip));
        glBindVertexArray(_grid_vao);

        glDrawArrays(GL_LINES, 0, _grid_vertices.size());
            
        if (_bb_vertices.size() > 0) {
            glBindVertexArray(_box_vao);
            glDrawArrays(GL_LINES, 0, _bb_vertices.size());
        }

        // draw actual meshes
        _Shader->use();
        // clears the color buffer with whatever the clearColour is set to
        // good practice to clear before every render cycle
        glBindVertexArray(_vao);
        //_Shader->setMat4("model", GL_FALSE, glm::value_ptr(transform));
        for (int mesh = 0; mesh < MAX_SCENE_OBJECTS; mesh++) {
            auto triangles = _mesh_to_triangles_map[mesh];
            if (triangles.size() > 0 && _Scene->_Meshes[mesh]->_Visible) {
                model = _Scene->_Meshes[mesh]->_Model_Matrix;
                _Shader->setMat4("view", GL_FALSE, glm::value_ptr(view));
                _Shader->setMat4("clip", GL_FALSE, glm::value_ptr(clip));
                _Shader->setMat4("model", GL_FALSE, glm::value_ptr(model));
                SetTextures(_Scene->_Meshes[mesh], _Shader);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo[mesh]);
                glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);
            }
        }
        //glDrawArrays(GL_TRIANGLES, 0, _Vertices.size());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void CompileShaders() override {
        _Shader = new Shader("shaders/raster.vert", "shaders/raster.frag");

        _axis_shader = new Shader("shaders/axis.vert", "shaders/axis.frag");

    }

    void RedrawBVHBoxes(std::vector<int> flagged_nodes) {
        _bb_vertices.clear();
        for (int i = 0; i < flagged_nodes.size(); i++) {
            if (flagged_nodes[i]) {
                auto node = _bvh->GetNodes()[i];
                auto v = DebugBVHBoundingBox(node->bounds);
                _bb_vertices.insert(_bb_vertices.end(), v.begin(), v.end());
            }
        }
        if (_bb_vertices.size() > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, _box_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_bb_vertices[0]) * _bb_vertices.size(), &_bb_vertices[0], GL_STATIC_DRAW);
        }

    }

    GLuint GetScreenTexture() override {
        return _fbo_tex;
    }
private:
    std::vector<glm::vec3> DebugBVHBoundingBox(Bound bound) {
        glm::vec3 extent = bound.extent();
        glm::vec3 p = glm::vec3(0.001f);
        glm::vec3 ex_x = glm::vec3(extent.x + p.x * 2, 0, 0);
        glm::vec3 ex_y = glm::vec3(0, extent.y + p.y * 2, 0);
        glm::vec3 ex_z = glm::vec3(0, 0, extent.z + p.z * 2);
        glm::vec3 mi = bound.bmin - p;
        glm::vec3 ma = bound.bmax + p;
        std::vector<glm::vec3> vertices = {
            // pillars
            { mi }, { mi + ex_y },
            { ma }, { ma - ex_y },
            { mi + ex_x }, { mi + ex_x + ex_y },
            { mi + ex_z }, { mi + ex_z + ex_y },

            // min corner
            { mi }, { mi + ex_x },
            { mi }, { mi + ex_z },
            { mi + ex_y }, { mi + ex_y + ex_x },
            { mi + ex_y }, { mi + ex_y + ex_z },

            // max corner
            { ma }, { ma - ex_x },
            { ma }, { ma - ex_z },
            { ma - ex_y }, { ma - ex_y - ex_x},
            { ma - ex_y }, { ma - ex_y - ex_z},
        };
        return vertices;
    }
    void InitFrameBuffer() {
        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glGenTextures(1, &_fbo_tex);
        glBindTexture(GL_TEXTURE_2D, _fbo_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, INITIAL_WIDTH, INITIAL_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbo_tex, 0);
        GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
        glGenRenderbuffers(1, &_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, INITIAL_WIDTH, INITIAL_HEIGHT);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void InitGrid() {

        glGenVertexArrays(1, &_grid_vao);
        // after binding a VAO, all subsequent calls to:
        // glEnable/DisableVertexAttribArray, glVertexAttribPointer
        // the Vetex buffer objects that are associated with the Vertex Attributes are also saved
        glBindVertexArray(_grid_vao);
        // stores the id of the Vertex Buffer Object that is created
        glGenBuffers(1, &_grid_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, _grid_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_grid_vertices[0]) * _grid_vertices.size(), &_grid_vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void InitDebugBoxes() {
        glGenVertexArrays(1, &_box_vao);
        // after binding a VAO, all subsequent calls to:
        // glEnable/DisableVertexAttribArray, glVertexAttribPointer
        // the Vetex buffer objects that are associated with the Vertex Attributes are also saved
        glBindVertexArray(_box_vao);
        // stores the id of the Vertex Buffer Object that is created
        glGenBuffers(1, &_box_vbo);
        
        glBindBuffer(GL_ARRAY_BUFFER, _box_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_bb_vertices[0]) * _bb_vertices.size(), &_bb_vertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    void SetTextures(Mesh3D* mesh, Shader* shader) {
        auto mat = mesh->_Mesh_Material;
        if (mat.Diffuse_Index >= 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _Textures[mat.Diffuse_Index]);
            shader->setInt("diffuseMap", 0);
        }
        if (mat.Normal_Index >= 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _Textures[mat.Normal_Index]);
            shader->setInt("normalMap", 1);
        }
        shader->setVec3("diffuseBase", mat.Diffuse);
    }

    
    GLuint GetFrameBufferObject() {
        return _fbo;
    }
};