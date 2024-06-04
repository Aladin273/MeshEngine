#include "GLRenderSystem.h"

#include <glad/gl.h>
#include <stb/stb_image.h>

#include <spdlog/spdlog.h>

GLRenderSystem::~GLRenderSystem()
{

}

void GLRenderSystem::init()
{
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    unsigned int textureId;
    glGenTextures(1, &textureId);
    unsigned char data[] = { 255, 255, 255, 255 };
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    int maxFragTextures;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragTextures);

    for (size_t i = 0; i < maxFragTextures; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureId);
    }

    spdlog::info("GLRenderSystem inited successful");
}

void GLRenderSystem::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

void GLRenderSystem::setPolygonMode(bool fill)
{
    glPolygonMode(GL_FRONT_AND_BACK, fill ? GL_FILL : GL_LINE);
}

void GLRenderSystem::setPointSize(float size)
{
    glPointSize(size);
}

void GLRenderSystem::setLineSize(float size)
{
    glLineWidth(size);
}

void GLRenderSystem::clearDepth(float d)
{
    glClearDepth(d);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void GLRenderSystem::clearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderSystem::clearDisplay(float r, float g, float b, float a, float d)
{
    glClearDepth(d);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint32_t GLRenderSystem::bufferData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    if (vertices.empty() || indices.empty())
        return -1;

    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);

    m_dataMap[VAO] = std::make_tuple(VAO, VBO, EBO);
    return VAO;
}

void GLRenderSystem::unbufferData(uint32_t id)
{
    auto it = m_dataMap.find(id);

    if (it != m_dataMap.end())
    {
        unsigned int VAO = std::get<0>(it->second);
        unsigned int VBO = std::get<1>(it->second);
        unsigned int EBO = std::get<2>(it->second);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

        m_dataMap.erase(it);
    }
}

void GLRenderSystem::bufferSubData(uint32_t bufferId, uint32_t index, const Vertex& vertex)
{
    auto it = m_dataMap.find(bufferId);

    if (it != m_dataMap.end())
    {
        unsigned int VBO = std::get<1>(it->second);

        size_t offset = index * sizeof(Vertex);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(Vertex), &vertex);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

uint32_t GLRenderSystem::bufferTexture(const std::string& texturePath)
{
    uint32_t textureID = -1;

    int width, height, nrComponents;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrComponents, 0);
    
    if (data)
    {
        glGenTextures(1, &textureID);

        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);
    }

    return textureID;
}

void GLRenderSystem::unbufferTexture(uint32_t textureId)
{
    glDeleteTextures(1, &textureId);
}

void GLRenderSystem::bufferFrame(uint32_t& bufferId, uint32_t& renderId, uint32_t& textureId, uint32_t width, uint32_t height)
{
    // Create Framebuffer
    glGenFramebuffers(1, &bufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferId);

    // Create Texture
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    // Create Renderbuffer Object for depth and stencil attachment
    glGenRenderbuffers(1, &renderId);
    glBindRenderbuffer(GL_RENDERBUFFER, renderId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderId);

    // Check if framebuffer is complete
    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        //std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_frameMap[bufferId] = std::make_tuple(bufferId, renderId, textureId);
}

void GLRenderSystem::unbufferFrame(uint32_t bufferId)
{
    auto it = m_frameMap.find(bufferId);

    if (it != m_frameMap.end())
    {
        unsigned int frame = std::get<0>(it->second);
        unsigned int render = std::get<1>(it->second);
        unsigned int texture = std::get<2>(it->second);

        glDeleteFramebuffers(1, &frame);
        glDeleteRenderbuffers(1, &render);
        glDeleteTextures(1, &texture);

        m_frameMap.erase(it);
    }
}

void GLRenderSystem::bindBuffer(uint32_t bufferId)
{
    glBindVertexArray(bufferId);
}

void GLRenderSystem::unbindBuffer()
{
    glBindVertexArray(0);
}

void GLRenderSystem::bindTexture(uint32_t activeId, uint32_t textureId)
{
    glActiveTexture(GL_TEXTURE0 + activeId);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void GLRenderSystem::unbindTexture()
{
    glActiveTexture(GL_TEXTURE0);
}

void GLRenderSystem::bindFrame(uint32_t bufferId)
{
    glBindFramebuffer(GL_FRAMEBUFFER, bufferId);
}

void GLRenderSystem::unbindFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderSystem::renderTriangles()
{
    GLint count = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

void GLRenderSystem::renderQuads()
{
    GLint count = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

void GLRenderSystem::renderPoints()
{
    GLint count = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
    glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, 0);
}

void GLRenderSystem::renderLines()
{
    GLint count = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
    glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, 0);
}

void GLRenderSystem::renderTriangles(uint32_t index, uint32_t count)
{
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void*)(sizeof(unsigned int) * index));
}

void GLRenderSystem::renderQuads(uint32_t index, uint32_t count)
{
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (const void*)(sizeof(unsigned int) * index));
}

void GLRenderSystem::renderPoints(uint32_t index, uint32_t count)
{
    glDrawElements(GL_POINTS, count, GL_UNSIGNED_INT, (const void*)(sizeof(unsigned int) * index));
}

void GLRenderSystem::renderLines(uint32_t index, uint32_t count)
{
    glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, (const void*)(sizeof(unsigned int) * index));
}