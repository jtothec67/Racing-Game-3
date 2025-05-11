#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include <stdexcept>
#include <cstdlib>

namespace Renderer
{
    class Model
    {
    public:
        Model();
        Model(const std::string& _path);
        Model(const Model& _copy);
        Model& operator=(const Model& _assign);
        virtual ~Model();

        GLsizei vertex_count() const;
        GLuint vao_id();

        void Unload();

        float get_width() const;
        float get_height() const;
        float get_length() const;

        struct Vertex
        {
            //Vertex();
            glm::vec3 position = glm::vec3(0, 0, 0);
            glm::vec2 texcoord = glm::vec2(0, 0);
            glm::vec3 normal = glm::vec3(0, 0, 0);
        };

        struct Face
        {
            Vertex a;
            Vertex b;
            Vertex c;
        };

        const std::vector<Model::Face>& GetFaces() const;

        // Returns true if the model was loaded with material support.
        bool usesMaterials() const { return m_useMaterials; }

        // Structure for material-specific geometry.
        struct MaterialGroup
        {
            std::string materialName;
            std::vector<Face> faces;
            std::string texturePath; // Diffuse texture from the MTL file.
            GLuint vao = 0;
            GLuint vbo = 0;
        };

        // Returns the material groups (for multi-textured models).
        const std::vector<MaterialGroup>& GetMaterialGroups() const { return m_materialGroups; }

    private:
        // Legacy face data.
        std::vector<Face> m_faces;
        // Material groups when using multi-material mode.
        std::vector<MaterialGroup> m_materialGroups;

        GLuint m_vaoid = 0;
        GLuint m_vboid = 0;
        bool m_dirty = true;

        float m_width = 0.0f;
        float m_height = 0.0f;
        float m_length = 0.0f;

        // Flag indicating if the model uses materials.
        bool m_useMaterials = false;

        void split_string_whitespace(const std::string& _input, std::vector<std::string>& _output);
        void split_string(const std::string& _input, char _splitter, std::vector<std::string>& _output);
        void calculate_dimensions();

        // Helper to load a MTL file mapping material names to diffuse texture paths.
        void LoadMTL(const std::string& mtlFilePath, std::map<std::string, std::string>& materialToTextureMap);
    };

    inline Model::Model()
    {
    }

    inline Model::Model(const std::string& _path)
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> tcs;
        std::vector<glm::vec3> normals;
        std::string currentLine;

        std::string directory;
        size_t found = _path.find_last_of("/\\");
        if (found != std::string::npos)
            directory = _path.substr(0, found + 1);

        std::ifstream file(_path.c_str());
        if (!file.is_open())
        {
            std::cout << "Failed to open file: " << _path << std::endl;
            throw std::runtime_error("Failed to open model file");
        }

        std::map<std::string, std::string> materialToTexture;
        m_useMaterials = false;
        std::string currentMaterial = "default";
        std::map<std::string, size_t> materialGroupIndices;

        while (std::getline(file, currentLine))
        {
            if (currentLine.empty()) continue;
            std::vector<std::string> tokens;
            split_string_whitespace(currentLine, tokens);
            if (tokens.empty()) continue;

            if (tokens.at(0) == "v" && tokens.size() >= 4)
            {
                glm::vec3 p(atof(tokens.at(1).c_str()),
                    atof(tokens.at(2).c_str()),
                    atof(tokens.at(3).c_str()));
                positions.push_back(p);
            }
            else if (tokens.at(0) == "vt" && tokens.size() >= 3)
            {
                glm::vec2 tc(atof(tokens.at(1).c_str()),
                    1.0f - atof(tokens.at(2).c_str()));
                tcs.push_back(tc);
            }
            else if (tokens.at(0) == "vn" && tokens.size() >= 4)
            {
                glm::vec3 n(atof(tokens.at(1).c_str()),
                    atof(tokens.at(2).c_str()),
                    atof(tokens.at(3).c_str()));
                normals.push_back(n);
            }
            else if (tokens.at(0) == "mtllib" && tokens.size() >= 2)
            {
                std::string mtlFilename;
                for (size_t i = 1; i < tokens.size(); ++i)
                {
                    if (i > 1) mtlFilename += " ";
                    mtlFilename += tokens[i];
                }

                std::string mtlPath = directory + mtlFilename;
                LoadMTL(mtlPath, materialToTexture);
            }
            else if (tokens.at(0) == "usemtl" && tokens.size() >= 2)
            {
                m_useMaterials = true;
                currentMaterial = tokens.at(1);
                if (materialGroupIndices.find(currentMaterial) == materialGroupIndices.end())
                {
                    MaterialGroup group;
                    group.materialName = currentMaterial;
                    if (materialToTexture.find(currentMaterial) != materialToTexture.end())
                        group.texturePath = materialToTexture[currentMaterial];
                    materialGroupIndices[currentMaterial] = m_materialGroups.size();
                    m_materialGroups.push_back(group);
                }
            }
            else if (tokens.at(0) == "f" && tokens.size() >= 4)
            {
                Face f;
                std::vector<std::string> sub;
                split_string(tokens.at(1), '/', sub);
                if (sub.size() >= 1) f.a.position = positions.at(atoi(sub.at(0).c_str()) - 1);
                if (sub.size() >= 2 && !sub.at(1).empty()) {
                    int texIndex = atoi(sub.at(1).c_str()) - 1;
                    if (texIndex >= 0 && texIndex < tcs.size())
                        f.a.texcoord = tcs.at(texIndex);
                }
                if (sub.size() >= 3) f.a.normal = normals.at(atoi(sub.at(2).c_str()) - 1);

                for (size_t ti = 2; ti < tokens.size(); ti++)
                {
                    split_string(tokens.at(ti - 1), '/', sub);
                    if (sub.size() >= 1) f.b.position = positions.at(atoi(sub.at(0).c_str()) - 1);
                    if (sub.size() >= 2 && !sub.at(1).empty()) {
                        int texIndex = atoi(sub.at(1).c_str()) - 1;
                        if (texIndex >= 0 && texIndex < tcs.size())
                            f.b.texcoord = tcs.at(texIndex);
                    }
                    if (sub.size() >= 3) f.b.normal = normals.at(atoi(sub.at(2).c_str()) - 1);

                    split_string(tokens.at(ti), '/', sub);
                    if (sub.size() >= 1) f.c.position = positions.at(atoi(sub.at(0).c_str()) - 1);
                    if (sub.size() >= 2 && !sub.at(1).empty()) {
                        int texIndex = atoi(sub.at(1).c_str()) - 1;
                        if (texIndex >= 0 && texIndex < tcs.size())
                            f.c.texcoord = tcs.at(texIndex);
                    }
                    if (sub.size() >= 3) f.c.normal = normals.at(atoi(sub.at(2).c_str()) - 1);

                    if (m_useMaterials)
                    {
                        size_t index = materialGroupIndices[currentMaterial];
                        m_materialGroups[index].faces.emplace_back(f);
                        m_faces.emplace_back(f);
                    }
                    else
                    {
                        m_faces.push_back(f);
                    }
                }
            }
        }

        file.close();

        if (!m_useMaterials)
        {
            if (m_faces.empty())
                throw std::runtime_error("Model is empty");

            glGenBuffers(1, &m_vboid);
            if (!m_vboid)
                throw std::runtime_error("Failed to generate vertex buffer");

            glGenVertexArrays(1, &m_vaoid);
            if (!m_vaoid)
                throw std::runtime_error("Failed to generate vertex array");

            std::vector<GLfloat> data;
            for (size_t fi = 0; fi < m_faces.size(); ++fi)
            {
                data.push_back(m_faces[fi].a.position.x);
                data.push_back(m_faces[fi].a.position.y);
                data.push_back(m_faces[fi].a.position.z);
                data.push_back(m_faces[fi].a.texcoord.x);
                data.push_back(m_faces[fi].a.texcoord.y);
                data.push_back(m_faces[fi].a.normal.x);
                data.push_back(m_faces[fi].a.normal.y);
                data.push_back(m_faces[fi].a.normal.z);

                data.push_back(m_faces[fi].b.position.x);
                data.push_back(m_faces[fi].b.position.y);
                data.push_back(m_faces[fi].b.position.z);
                data.push_back(m_faces[fi].b.texcoord.x);
                data.push_back(m_faces[fi].b.texcoord.y);
                data.push_back(m_faces[fi].b.normal.x);
                data.push_back(m_faces[fi].b.normal.y);
                data.push_back(m_faces[fi].b.normal.z);

                data.push_back(m_faces[fi].c.position.x);
                data.push_back(m_faces[fi].c.position.y);
                data.push_back(m_faces[fi].c.position.z);
                data.push_back(m_faces[fi].c.texcoord.x);
                data.push_back(m_faces[fi].c.texcoord.y);
                data.push_back(m_faces[fi].c.normal.x);
                data.push_back(m_faces[fi].c.normal.y);
                data.push_back(m_faces[fi].c.normal.z);
            }

            glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data.at(0), GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glBindVertexArray(m_vaoid);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboid);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
            glEnableVertexAttribArray(2);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else
        {
            // New: Set up buffers for each material group.
            for (size_t gi = 0; gi < m_materialGroups.size(); ++gi)
            {
                MaterialGroup& group = m_materialGroups[gi];
                if (group.faces.empty())
                    continue;

                std::vector<GLfloat> data;
                for (size_t fi = 0; fi < group.faces.size(); ++fi)
                {
                    data.push_back(group.faces[fi].a.position.x);
                    data.push_back(group.faces[fi].a.position.y);
                    data.push_back(group.faces[fi].a.position.z);
                    data.push_back(group.faces[fi].a.texcoord.x);
                    data.push_back(group.faces[fi].a.texcoord.y);
                    data.push_back(group.faces[fi].a.normal.x);
                    data.push_back(group.faces[fi].a.normal.y);
                    data.push_back(group.faces[fi].a.normal.z);

                    data.push_back(group.faces[fi].b.position.x);
                    data.push_back(group.faces[fi].b.position.y);
                    data.push_back(group.faces[fi].b.position.z);
                    data.push_back(group.faces[fi].b.texcoord.x);
                    data.push_back(group.faces[fi].b.texcoord.y);
                    data.push_back(group.faces[fi].b.normal.x);
                    data.push_back(group.faces[fi].b.normal.y);
                    data.push_back(group.faces[fi].b.normal.z);

                    data.push_back(group.faces[fi].c.position.x);
                    data.push_back(group.faces[fi].c.position.y);
                    data.push_back(group.faces[fi].c.position.z);
                    data.push_back(group.faces[fi].c.texcoord.x);
                    data.push_back(group.faces[fi].c.texcoord.y);
                    data.push_back(group.faces[fi].c.normal.x);
                    data.push_back(group.faces[fi].c.normal.y);
                    data.push_back(group.faces[fi].c.normal.z);
                }

                glGenBuffers(1, &group.vbo);
                if (!group.vbo)
                    throw std::runtime_error("Failed to generate vertex buffer for material group");
                glGenVertexArrays(1, &group.vao);
                if (!group.vao)
                    throw std::runtime_error("Failed to generate vertex array for material group");

                glBindBuffer(GL_ARRAY_BUFFER, group.vbo);
                glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data.at(0), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(group.vao);
                glBindBuffer(GL_ARRAY_BUFFER, group.vbo);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }

            std::cout << _path << " uses: " << std::endl;
            for (auto& group : GetMaterialGroups())
            {
                std::cout << "  Material: " << group.materialName << std::endl;
            }
        }

        m_dirty = false;
        calculate_dimensions();
    }

    inline Model::~Model()
    {
        Unload();
    }

    inline Model::Model(const Model& _copy)
    {
        m_faces = _copy.m_faces;
        m_materialGroups = _copy.m_materialGroups;
    }

    inline Model& Model::operator=(const Model& _assign)
    {
        m_faces = _assign.m_faces;
        m_materialGroups = _assign.m_materialGroups;
        m_dirty = true;
        return *this;
    }

    inline void Model::split_string_whitespace(const std::string& _input, std::vector<std::string>& _output)
    {
        _output.clear();
        std::istringstream iss(_input);
        std::string token;
        while (iss >> token)
        {
            _output.push_back(token);
        }
    }

    inline void Model::split_string(const std::string& _input, char _splitter, std::vector<std::string>& _output)
    {
        _output.clear();
        std::stringstream ss(_input);
        std::string item;
        while (std::getline(ss, item, _splitter))
        {
            _output.push_back(item);
        }
    }

    inline void Model::LoadMTL(const std::string& mtlFilePath, std::map<std::string, std::string>& materialToTextureMap)
    {
        std::ifstream mtlFile(mtlFilePath.c_str());
        if (!mtlFile.is_open())
        {
            std::cout << "Failed to open MTL file: " << mtlFilePath << std::endl;
            return;
        }

        std::string line;
        std::string currentMat;
        while (std::getline(mtlFile, line))
        {
            std::vector<std::string> tokens;
            split_string_whitespace(line, tokens);
            if (tokens.empty())
                continue;
            if (tokens[0] == "newmtl" && tokens.size() >= 2)
            {
                currentMat = tokens[1];
            }
            else if (tokens[0] == "map_Kd" && tokens.size() >= 2)
            {
                materialToTextureMap[currentMat] = tokens[1];
            }
        }
        mtlFile.close();
    }

    inline GLuint Model::vao_id()
    {
        if (!m_useMaterials)
        {
            if (m_faces.empty())
                throw std::runtime_error("Model is empty");
            if (!m_vboid)
            {
                glGenBuffers(1, &m_vboid);
                if (!m_vboid)
                    throw std::runtime_error("Failed to generate vertex buffer");
            }
            if (!m_vaoid)
            {
                glGenVertexArrays(1, &m_vaoid);
                if (!m_vaoid)
                    throw std::runtime_error("Failed to generate vertex array");
            }
            if (m_dirty)
            {
                std::vector<GLfloat> data;
                for (size_t fi = 0; fi < m_faces.size(); ++fi)
                {
                    data.push_back(m_faces[fi].a.position.x);
                    data.push_back(m_faces[fi].a.position.y);
                    data.push_back(m_faces[fi].a.position.z);
                    data.push_back(m_faces[fi].a.texcoord.x);
                    data.push_back(m_faces[fi].a.texcoord.y);
                    data.push_back(m_faces[fi].a.normal.x);
                    data.push_back(m_faces[fi].a.normal.y);
                    data.push_back(m_faces[fi].a.normal.z);

                    data.push_back(m_faces[fi].b.position.x);
                    data.push_back(m_faces[fi].b.position.y);
                    data.push_back(m_faces[fi].b.position.z);
                    data.push_back(m_faces[fi].b.texcoord.x);
                    data.push_back(m_faces[fi].b.texcoord.y);
                    data.push_back(m_faces[fi].b.normal.x);
                    data.push_back(m_faces[fi].b.normal.y);
                    data.push_back(m_faces[fi].b.normal.z);

                    data.push_back(m_faces[fi].c.position.x);
                    data.push_back(m_faces[fi].c.position.y);
                    data.push_back(m_faces[fi].c.position.z);
                    data.push_back(m_faces[fi].c.texcoord.x);
                    data.push_back(m_faces[fi].c.texcoord.y);
                    data.push_back(m_faces[fi].c.normal.x);
                    data.push_back(m_faces[fi].c.normal.y);
                    data.push_back(m_faces[fi].c.normal.z);
                }

                glBindBuffer(GL_ARRAY_BUFFER, m_vboid);
                glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), &data.at(0), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(m_vaoid);
                glBindBuffer(GL_ARRAY_BUFFER, m_vboid);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                m_dirty = false;
            }
            return m_vaoid;
        }
        else
        {
            return 0;
        }
    }

    inline void Model::Unload()
    {
        if (!m_useMaterials)
        {
            if (m_vaoid)
            {
                glDeleteVertexArrays(1, &m_vaoid);
                m_vaoid = 0;
                m_dirty = true;
            }
            if (m_vboid)
            {
                glDeleteBuffers(1, &m_vboid);
                m_vboid = 0;
                m_dirty = true;
            }
        }
        else
        {
            for (auto& group : m_materialGroups)
            {
                if (group.vao)
                {
                    glDeleteVertexArrays(1, &group.vao);
                    group.vao = 0;
                }
                if (group.vbo)
                {
                    glDeleteBuffers(1, &group.vbo);
                    group.vbo = 0;
                }
            }
        }
    }

    inline GLsizei Model::vertex_count() const
    {
        if (!m_useMaterials)
            return static_cast<GLsizei>(m_faces.size() * 3);
        else
        {
            GLsizei count = 0;
            for (const auto& group : m_materialGroups)
                count += static_cast<GLsizei>(group.faces.size() * 3);
            return count;
        }
    }

    inline void Model::calculate_dimensions()
    {
        bool first = true;
        glm::vec3 min_pos, max_pos;
        auto processVertex = [&](const glm::vec3& pos)
            {
                if (first)
                {
                    min_pos = pos;
                    max_pos = pos;
                    first = false;
                }
                else
                {
                    min_pos = glm::min(min_pos, pos);
                    max_pos = glm::max(max_pos, pos);
                }
            };

        if (!m_useMaterials)
        {
            for (const auto& face : m_faces)
            {
                processVertex(face.a.position);
                processVertex(face.b.position);
                processVertex(face.c.position);
            }
        }
        else
        {
            for (const auto& group : m_materialGroups)
            {
                for (const auto& face : group.faces)
                {
                    processVertex(face.a.position);
                    processVertex(face.b.position);
                    processVertex(face.c.position);
                }
            }
        }

        m_width = max_pos.x - min_pos.x;
        m_height = max_pos.y - min_pos.y;
        m_length = max_pos.z - min_pos.z;
    }

    inline float Model::get_width() const { return m_width; }
    inline float Model::get_height() const { return m_height; }
    inline float Model::get_length() const { return m_length; }

    inline const std::vector<Model::Face>& Model::GetFaces() const
    {
        return m_faces;
    }
}
