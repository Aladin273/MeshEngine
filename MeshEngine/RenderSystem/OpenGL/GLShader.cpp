#include "GLShader.h"

#include <fstream>
#include <sstream>

#include <glad/gl.h>

#include "MeshEngine/Misc/Logger.h"

GLShader::GLShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
    struct CachedShader { std::string vertexPath; std::string fragmentPath; std::string geometryPath; GLShader* shader;};

    static std::vector<CachedShader> s_cachedShaders;

    for (const CachedShader& cachedShader : s_cachedShaders)
    {
        if (vertexPath == cachedShader.vertexPath && fragmentPath == cachedShader.fragmentPath && geometryPath == cachedShader.geometryPath && cachedShader.shader)
        {
            ID = cachedShader.shader->ID;

            m_vertexId = cachedShader.shader->m_vertexId;
            m_fragmentId = cachedShader.shader->m_fragmentId;
            m_geometryId = cachedShader.shader->m_geometryId;

            return;
        }
    }

    create(vertexPath, fragmentPath, geometryPath);
    s_cachedShaders.push_back({ vertexPath, fragmentPath, geometryPath, this });
}

void GLShader::bind()
{
    glUseProgram(ID);
}

void GLShader::unbind()
{
    glUseProgram(0);
}

void GLShader::attachVertex()
{
    glAttachShader(ID, m_geometryId);
    glLinkProgram(ID);
}

void GLShader::detachVertex()
{
    glDetachShader(ID, GL_VERTEX_SHADER);
    glLinkProgram(ID);
}

void GLShader::attachFragment()
{
    glAttachShader(ID, m_geometryId);
    glLinkProgram(ID);
}

void GLShader::detachFragment()
{
    glDetachShader(ID, GL_FRAGMENT_SHADER);
    glLinkProgram(ID);
}

void GLShader::attachGeometry()
{
    glAttachShader(ID, m_geometryId);
    glLinkProgram(ID);
}

void GLShader::detachGeometry()
{
    glDetachShader(ID, GL_GEOMETRY_SHADER);
    glLinkProgram(ID);
}

void GLShader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void GLShader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void GLShader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void GLShader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void GLShader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void GLShader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void GLShader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void GLShader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void GLShader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void GLShader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::create(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath /*= ""*/)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // If geometry shader path is provided, also load it
        if (!geometryPath.empty())
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        MeshEngine::Logger::error("SHADER::FILE_NOT_SUCCESFULLY_READ");
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment, geometry;

    // Compile vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    m_vertexId = vertex;
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    check(vertex, "VERTEX");

    // Compile fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    m_fragmentId = fragment;
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    check(fragment, "FRAGMENT");

    // Create shader program and attach vertex and fragment shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    // Compile and attach geometry shader if provided
    if (!geometryPath.empty())
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        m_geometryId = geometry;
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        check(geometry, "GEOMETRY");
        glAttachShader(ID, geometry);
    }

    // Link shader program
    glLinkProgram(ID);
    if (check(ID, "PROGRAM"))
    {
        MeshEngine::Logger::info("GLShader created successfully: {} {} {}", vertexPath, fragmentPath, geometryPath);
    }
    else
    {
        MeshEngine::Logger::error("GLShader creation failed: {} {} {}", vertexPath, fragmentPath, geometryPath);
    }

    // Delete shaders as they're linked into the program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (!geometryPath.empty())
    {
        glDeleteShader(geometry);
    }
}

bool GLShader::check(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    
    if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            MeshEngine::Logger::error("PROGRAM_LINKING_ERROR: {:}\n{:}", type, infoLog);

            return false;
        }
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            MeshEngine::Logger::error("SHADER_COMPILATION_ERROR: {:}\n{:}", type, infoLog);

            return false;
        }
    }

    return true;
}

