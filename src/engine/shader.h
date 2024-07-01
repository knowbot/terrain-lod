#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/// _shader class from https://learnopengl.com
/// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
/// modified to store the shader on memory, and permit editing and recompilation at runtime


class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr, const char* tess_ctrl_path = nullptr, const char* tess_eval_path = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertex_code;
        std::string fragment_code;
        std::string geometry_code;
        std::string tess_ctrl_code;
        std::string tess_eval_code;
        std::ifstream v_shader_file;
        std::ifstream f_shader_file;
        std::ifstream g_shader_file;
        std::ifstream tc_shader_file;
        std::ifstream te_shader_file;
        // ensure ifstream objects can throw exceptions:
        v_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        f_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        g_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        tc_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        te_shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            v_shader_file.open(vertex_path);
            f_shader_file.open(fragment_path);
            std::stringstream v_shader_stream, f_shader_stream;
            // read file's buffer contents into streams
            v_shader_stream << v_shader_file.rdbuf();
            f_shader_stream << f_shader_file.rdbuf();
            // close file handlers
            v_shader_file.close();
            f_shader_file.close();
            // convert stream into string
            vertex_code = v_shader_stream.str();
            fragment_code = f_shader_stream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometry_path != nullptr)
            {
                g_shader_file.open(geometry_path);
                std::stringstream g_shader_stream;
                g_shader_stream << g_shader_file.rdbuf();
                g_shader_file.close();
                geometry_code = g_shader_stream.str();
            }

            // if tessellation paths are present, load those as well
            if (tess_ctrl_path != nullptr)
            {
                tc_shader_file.open(tess_ctrl_path);
                std::stringstream tc_shader_stream;
                tc_shader_stream << tc_shader_file.rdbuf();
                tc_shader_file.close();
                tess_ctrl_code = tc_shader_stream.str();
            }

            if (tess_eval_path != nullptr)
            {
                te_shader_file.open(tess_eval_path);
                std::stringstream te_shader_stream;
                te_shader_stream << te_shader_file.rdbuf();
                te_shader_file.close();
                tess_eval_code = te_shader_stream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* v_shader_code = vertex_code.c_str();
        const char * f_shader_code = fragment_code.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &v_shader_code, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment _shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &f_shader_code, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometry_path != nullptr)
        {
            const char * g_shader_code = geometry_code.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &g_shader_code, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }

        unsigned int tess_ctrl;
        // also do that for tessellation shaders
        if (tess_ctrl_path != nullptr)
        {
            const char* tc_shader_code = tess_ctrl_code.c_str();
            tess_ctrl = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tess_ctrl, 1, &tc_shader_code, NULL);
            glCompileShader(tess_ctrl);
            checkCompileErrors(tess_ctrl, "TESS_CONTROL");
        }

        unsigned int tess_eval;
        if (tess_eval_path != nullptr)
        {
            const char* te_shader_code = tess_eval_code.c_str();
            tess_eval = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(tess_eval, 1, &te_shader_code, NULL);
            glCompileShader(tess_eval);
            checkCompileErrors(tess_eval, "TESS_EVALUATION");
        }

        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometry_path != nullptr)
            glAttachShader(ID, geometry);
        if (tess_ctrl_path != nullptr)
            glAttachShader(ID, tess_ctrl);
        if (tess_eval_path != nullptr)
            glAttachShader(ID, tess_eval);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometry_path != nullptr)
            glDeleteShader(geometry);
        if (tess_ctrl_path != nullptr)
            glDeleteShader(tess_ctrl);
        if (tess_eval_path != nullptr)
            glDeleteShader(tess_eval);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif