#include "GLRenderSystem.h"

GLRenderSystem::~GLRenderSystem()
{

}

void GLRenderSystem::init()
{
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glLineWidth(m_lineSize);
    glPointSize(m_pointSize);
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

void GLRenderSystem::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

void GLRenderSystem::renderTriangles(const std::vector<Vertex>& vertices)
{
    glBegin(GL_TRIANGLES);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(vertex.color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderTriangles(const std::vector<Vertex>& vertices, glm::vec4 color)
{
    glBegin(GL_TRIANGLES);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderPolygons(const std::vector<Vertex>& vertices)
{
    glBegin(GL_TRIANGLE_STRIP);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(vertex.color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderPolygons(const std::vector<Vertex>& vertices, glm::vec4 color)
{
    glBegin(GL_TRIANGLE_STRIP);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderQuads(const std::vector<Vertex>& vertices)
{
    glBegin(GL_QUADS);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(vertex.color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderQuads(const std::vector<Vertex>& vertices, glm::vec4 color)
{
    glBegin(GL_QUADS);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();
}

void GLRenderSystem::renderPoints(const std::vector<Vertex>& vertices, float size)
{
    if (size >= 0.0)
        glPointSize(size);

    glBegin(GL_POINTS);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(vertex.color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();

    glPointSize(m_pointSize);
}

void GLRenderSystem::renderPoints(const std::vector<Vertex>& vertices, glm::vec4 color, float size)
{
    if (size >= 0.0)
        glPointSize(size);

    glBegin(GL_POINTS);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();

    glPointSize(m_pointSize);
}

void GLRenderSystem::renderLines(const std::vector<Vertex>& vertices, float size)
{
    if (size >= 0.0)
        glLineWidth(size);

    glBegin(GL_LINES);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(vertex.color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();

    glLineWidth(m_lineSize);
}

void GLRenderSystem::renderLines(const std::vector<Vertex>& vertices, glm::vec4 color, float size)
{
    if (size >= 0.0)
        glLineWidth(size);

    glBegin(GL_LINES);
    for (auto& vertex : vertices)
    {
        glColor4fv(glm::value_ptr(color));
        glNormal3fv(glm::value_ptr(vertex.normal));
        glVertex3fv(glm::value_ptr(vertex.position));
    }
    glEnd();

    glLineWidth(m_lineSize);
}

void GLRenderSystem::setupMaterial(const Material& material, Face face)
{
    int faceIndex = static_cast<int>(face);

    glMaterialfv(faceIndex, GL_AMBIENT, glm::value_ptr(material.ambient));
    glMaterialfv(faceIndex, GL_DIFFUSE, glm::value_ptr(material.diffuse));
    glMaterialfv(faceIndex, GL_SPECULAR, glm::value_ptr(material.specular));
    glMaterialfv(faceIndex, GL_EMISSION, glm::value_ptr(material.emission));
    glMaterialf(faceIndex, GL_SHININESS, material.shininess);
}

void GLRenderSystem::setupLightDir(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is)
{
    if (index > 7 || index < 0)
        return;

    glLightfv(GL_LIGHT0 + index, GL_POSITION, glm::value_ptr(glm::vec4(position, 0.0f)));
    glLightfv(GL_LIGHT0 + index, GL_AMBIENT, glm::value_ptr(Ia));
    glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, glm::value_ptr(Id));
    glLightfv(GL_LIGHT0 + index, GL_SPECULAR, glm::value_ptr(Is));
}

void GLRenderSystem::setupLightSpot(uint32_t index, glm::vec3 position, glm::vec3 direction, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float cutoff, float exponent)
{
    if (index > 7 || index < 0)
        return;

    glLightfv(GL_LIGHT0 + index, GL_POSITION, glm::value_ptr(glm::vec4(position, 1.0f)));
    glLightfv(GL_LIGHT0 + index, GL_SPOT_DIRECTION, glm::value_ptr(-glm::normalize(direction)));
    glLightf(GL_LIGHT0 + index, GL_SPOT_CUTOFF, cutoff);
    glLightf(GL_LIGHT0 + index, GL_SPOT_EXPONENT, exponent);
    glLightfv(GL_LIGHT0 + index, GL_AMBIENT, glm::value_ptr(Ia));
    glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, glm::value_ptr(Id));
    glLightfv(GL_LIGHT0 + index, GL_SPECULAR, glm::value_ptr(Is));
}

void GLRenderSystem::setupLightPoint(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float constant, float linear, float quadratic)
{
    if (index > 7 || index < 0)
        return;

    glLightfv(GL_LIGHT0 + index, GL_POSITION, glm::value_ptr(glm::vec4(position, 1.0f)));
    glLightfv(GL_LIGHT0 + index, GL_AMBIENT, glm::value_ptr(Ia));
    glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, glm::value_ptr(Id));
    glLightfv(GL_LIGHT0 + index, GL_SPECULAR, glm::value_ptr(Is));
    glLightf(GL_LIGHT0 + index, GL_CONSTANT_ATTENUATION, constant);
    glLightf(GL_LIGHT0 + index, GL_LINEAR_ATTENUATION, linear);
    glLightf(GL_LIGHT0 + index, GL_QUADRATIC_ATTENUATION, quadratic);
}

void GLRenderSystem::setLight(uint32_t index, bool enable)
{
    if (index > 7 || index < 0)
        return;

    if (enable)
        glEnable(GL_LIGHT0 + index);
    else
        glDisable(GL_LIGHT0 + index);
}

void GLRenderSystem::setLighting(bool enable)
{
    if (enable)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
}

void GLRenderSystem::setColorMaterial(bool enable)
{
    if (enable)
        glEnable(GL_COLOR_MATERIAL);
    else
        glDisable(GL_COLOR_MATERIAL);
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

bool GLRenderSystem::getLight(uint32_t index) const
{
    GLboolean state;
    glGetBooleanv(GL_LIGHT0 + index, &state);

    return state;
}

bool GLRenderSystem::getLighting() const
{
    GLboolean state;
    glGetBooleanv(GL_LIGHTING, &state);

    return state;
}

bool GLRenderSystem::getColorMaterial() const
{
    GLboolean state;
    glGetBooleanv(GL_COLOR_MATERIAL, &state);

    return state;
}

bool GLRenderSystem::getDepth() const
{
    GLboolean state;
    glGetBooleanv(GL_DEPTH_TEST, &state);
    return state;
}

bool GLRenderSystem::getSmooth() const
{
    GLboolean state;
    glGetBooleanv(GL_SMOOTH, &state);
    return state;
}

void GLRenderSystem::setPointSize(float size)
{
    if (size >= 0.0)
        m_pointSize = size;
}
void GLRenderSystem::setLineSize(float size)
{
    if (size >= 0.0)
        m_lineSize = size;
}

float GLRenderSystem::getPointSize() const
{
    return m_pointSize;
}
float GLRenderSystem::getLineSize() const
{
    return m_lineSize;
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
    }
}

const glm::mat4& GLRenderSystem::getProjMatrix() const
{
    return m_proj_matrix;
}
