#include "GLRenderSystem.h"

GLRenderSystem::~GLRenderSystem()
{

}

void GLRenderSystem::init()
{
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    glLineWidth(m_lineWidth);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
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
    glClearColor(r, g, b, a);
    glClearDepth(d);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderSystem::setViewport(double x, double y, double width, double height)
{
    glViewport(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height));
}

void GLRenderSystem::renderTriangles(const std::vector<Vertex>& vertices)
{
    glBegin(GL_TRIANGLES);
    for (auto& vertex : vertices)
    {
        glColor3fv(glm::value_ptr(vertex.color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderTriangles(const std::vector<Vertex>& vertices, glm::vec3 color)
{
    glBegin(GL_TRIANGLES);
    for (auto& vertex : vertices)
    {
        glColor3fv(glm::value_ptr(color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderLines(const std::vector<Vertex>& vertices)
{
    glBegin(GL_LINES);
    for (auto& vertex : vertices)
    {
        glColor3fv(glm::value_ptr(vertex.color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderLines(const std::vector<Vertex>& vertices, glm::vec3 color)
{
    glBegin(GL_LINES);
    for (auto& vertex : vertices)
    {
        glColor3fv(glm::value_ptr(color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::setupMaterial(const Material& material, Face face)
{
    int current = static_cast<int>(face);

    glMaterialfv(current, GL_AMBIENT, glm::value_ptr(material.ambient));
    glMaterialfv(current, GL_DIFFUSE, glm::value_ptr(material.diffuse));
    glMaterialfv(current, GL_SPECULAR, glm::value_ptr(material.specular));
    glMaterialfv(current, GL_EMISSION, glm::value_ptr(material.emission));
    glMaterialf(current, GL_SHININESS, material.shininess);
}

void GLRenderSystem::setupLightDir(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is)
{
    if (index > m_maxIndex)
        return;

    glLightfv(GL_LIGHT0 + index, GL_POSITION, glm::value_ptr(glm::vec4(position, 0.0f)));
    glLightfv(GL_LIGHT0 + index, GL_AMBIENT, glm::value_ptr(Ia));
    glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, glm::value_ptr(Id));
    glLightfv(GL_LIGHT0 + index, GL_SPECULAR, glm::value_ptr(Is));
}

void GLRenderSystem::setupLightPoint(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float k_const, float k_linear, float k_quadratic)
{
    if (index > m_maxIndex)
        return;

    glLightfv(GL_LIGHT0 + index, GL_POSITION, glm::value_ptr(glm::vec4(position, 1.0f)));
    glLightfv(GL_LIGHT0 + index, GL_AMBIENT, glm::value_ptr(Ia));
    glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, glm::value_ptr(Id));
    glLightfv(GL_LIGHT0 + index, GL_SPECULAR, glm::value_ptr(Is));
    glLightf(GL_LIGHT0 + index, GL_CONSTANT_ATTENUATION, k_const);
    glLightf(GL_LIGHT0 + index, GL_LINEAR_ATTENUATION, k_linear);
    glLightf(GL_LIGHT0 + index, GL_QUADRATIC_ATTENUATION, k_quadratic);
}

void GLRenderSystem::turnLight(uint32_t index, bool enable)
{
    if (index > m_maxIndex)
        return;
    else if (enable)
        glEnable(GL_LIGHT0 + index);
    else
        glDisable(GL_LIGHT0 + index);
}

void GLRenderSystem::setDepth(bool enable)
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void GLRenderSystem::setSmooth(bool enable)
{
    if (enable)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);
}

void GLRenderSystem::setLighting(bool enable)
{
    if (enable)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
}

void GLRenderSystem::setLightTwoSide(bool enable)
{
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, enable);
}

void GLRenderSystem::setColorMaterial(bool enable)
{
    if (enable)
        glEnable(GL_COLOR_MATERIAL);
    else
        glDisable(GL_COLOR_MATERIAL);
}

bool GLRenderSystem::getLighting()
{
    GLboolean state;
    glGetBooleanv(GL_LIGHTING, &state);

    return state;
}

bool GLRenderSystem::getLightTwoSide()
{
    GLboolean state;
    glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE, &state);

    return state;
}

bool GLRenderSystem::getDepth()
{
    GLboolean state;
    glGetBooleanv(GL_DEPTH_TEST, &state);
    return state;
}

bool GLRenderSystem::getSmooth()
{
    GLboolean state;
    glGetBooleanv(GL_SMOOTH, &state);
    return state;
}

bool GLRenderSystem::getColorMaterial()
{
    GLboolean state;
    glGetBooleanv(GL_COLOR_MATERIAL, &state);

    return state;
}

bool GLRenderSystem::turnedLight(uint32_t index)
{
    GLboolean state;
    glGetBooleanv(GL_LIGHT0 + index, &state);

    return state;
}

void GLRenderSystem::setWorldMatrix(const glm::mat4& matrix)
{
    if (m_wordl_matrix != matrix)
    {
        m_wordl_matrix = matrix;

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(m_view_matrix * m_wordl_matrix));
    }
}

const glm::mat4& GLRenderSystem::getWorldMatrix() const
{
    return m_wordl_matrix;
}

void GLRenderSystem::setViewMatrix(const glm::mat4& matrix)
{
    if (m_view_matrix != matrix)
    {
        m_view_matrix = matrix;

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(m_view_matrix * m_wordl_matrix));
    }
}

const glm::mat4& GLRenderSystem::getViewMatrix() const
{
    return m_view_matrix;
}

void GLRenderSystem::setProjMatrix(const glm::mat4& matrix)
{
    if (m_proj_matrix != matrix)
    {
        m_proj_matrix = matrix;
    
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(m_proj_matrix));
        glMatrixMode(GL_MODELVIEW);
    }
}

const glm::mat4& GLRenderSystem::getProjMatrix() const
{
    return m_proj_matrix;
}
