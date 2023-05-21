#pragma once

#include <vector>
#include <map>

#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/constants.hpp"
#include "ThirdParty/glm/gtx/transform.hpp"

namespace heds
{
    const int64_t invalid = -1;

    struct HalfEdgeHandle
    {
        int64_t index = invalid;

        bool operator==(const HalfEdgeHandle& b) const { return index == b.index; };
        bool operator!=(const HalfEdgeHandle& b) const { return index != b.index; };
        bool operator<(const HalfEdgeHandle& b) const { return index < b.index; };
        bool operator>(const HalfEdgeHandle& b) const { return index > b.index; };
    };

    struct VertexHandle
    {
        int64_t index = invalid;

        bool operator==(const VertexHandle& b) const { return index == b.index; };
        bool operator!=(const VertexHandle& b) const { return index != b.index; };
        bool operator<(const VertexHandle& b) const { return index < b.index; };
        bool operator>(const VertexHandle& b) const { return index > b.index; };
    };

    struct FaceHandle
    {
        int64_t index = invalid;

        bool operator==(const FaceHandle& b) const { return index == b.index; };
        bool operator!=(const FaceHandle& b) const { return index != b.index; };
        bool operator<(const FaceHandle& b) const { return index < b.index; };
        bool operator>(const FaceHandle& b) const { return index > b.index; };
    };

    struct HalfEdge
    {
        //The face it belongs to, is invalid (== -1) if a boundary half-edge
        FaceHandle fh;
        //The vertex it points to is always valid
        VertexHandle dst;
        //The twin half-edge is always VALID. Even for boundary half-edges!
        HalfEdgeHandle twin;
        //The next HalfEdge in the CCW order is always valid
        HalfEdgeHandle next;
        //The previous HalfEdge in the CCW order can be stored for the optimization purposes. For the triangle meshes prev = next->next->next
        HalfEdgeHandle prev;
    };

    struct Face
    {
        //One of the HalfEdges belonging to the Face, always valid
        HalfEdgeHandle heh;
    };

    struct Vertex
    {
        //An outgoing HalfEdge from this vertex. It is == -1 if the vertex is isolated
        HalfEdgeHandle heh;
        glm::vec3 data;
    };

    class HalfEdgeTable
    {
    public:
        //Adds vertex
        VertexHandle addVertex(glm::vec3 position);
        //Adds triangulated face
        FaceHandle addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2);
        //Adds quad face
        FaceHandle addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2, VertexHandle vh3);
        //Builds twins for half-edges. This function must be called in the end
        void connectTwins();
        //Deletes face and re-link all vertices, half-edges, faces. 
        void deleteFace(FaceHandle fh);

        //For a given half-edge returns previous linked half-edge
        HalfEdgeHandle prev(HalfEdgeHandle heh) const;
        //For a given half-edge returns next linked half-edge
        HalfEdgeHandle next(HalfEdgeHandle heh) const;
        //For a given half-edge returns twin half-edge
        HalfEdgeHandle twin(HalfEdgeHandle heh) const;
        //For a given half-edge returns end vertex
        VertexHandle destVertex(HalfEdgeHandle heh) const;
        //For a given half-edge returns start vertex
        VertexHandle sourceVertex(HalfEdgeHandle heh) const;

        //For a given half-edge handle returns half-edge
        HalfEdge& deref(HalfEdgeHandle heh);
        const HalfEdge& deref(HalfEdgeHandle heh) const;
        //For a given half-edge returns half-edge handle
        HalfEdgeHandle handle(const HalfEdge& he) const;

        Vertex& deref(VertexHandle vh);
        const Vertex& deref(VertexHandle vh) const;
        VertexHandle handle(const Vertex& v) const;

        Face& deref(FaceHandle fh);
        const Face& deref(FaceHandle fh) const;
        FaceHandle handle(const Face& f) const;

        //For a given vertex handle set/get point
        const glm::vec3& getPoint(VertexHandle handle) const;
        void setPoint(VertexHandle handle, glm::vec3 point);

        //For a given half-edge handle set/get start point (vertex)
        const glm::vec3& getStartPoint(HalfEdgeHandle handle) const;
        void setStartPoint(HalfEdgeHandle handle, glm::vec3 point);

        //For a given half-edge handle set/get end point (vertex)
        const glm::vec3& getEndPoint(HalfEdgeHandle handle) const;
        void setEndPoint(HalfEdgeHandle handle, glm::vec3 point);

        const std::vector<Vertex>& getVertices() const;
        const std::vector<Face>& getFaces() const;
        const std::vector<HalfEdge>& getHalfEdges() const;

        friend HalfEdgeTable createCube(glm::vec3 center, float length);
        friend HalfEdgeTable createCone(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
        friend HalfEdgeTable createCylinder(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
        friend HalfEdgeTable createTorus(glm::vec3 dir, float minorRadius, float majorRadius, uint32_t majorSegments);
        friend HalfEdgeTable createArrow(glm::vec3 dir, float R1, float h1, float R2, float h2, uint32_t numSubdivisions);
        friend HalfEdgeTable createPlane(glm::vec3 dir, float width, float heigth, uint32_t numSubdivisions);

    private:
        //Deletes fake twins
        void deleteFakeTwins();

        std::vector<Vertex> m_vertices;
        std::vector<HalfEdge> m_halfEdges;
        std::vector<Face> m_faces;

        int64_t m_halfEdge = invalid;
        int64_t m_fakeTwins = invalid;
    };

    HalfEdgeTable createCube(glm::vec3 center, float length);
    HalfEdgeTable createCone(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
    HalfEdgeTable createCylinder(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
    HalfEdgeTable createTorus(glm::vec3 dir, float minorRadius, float majorRadius, uint32_t majorSegments);
    HalfEdgeTable createArrow(glm::vec3 dir, float R1, float h1, float R2, float h2, uint32_t numSubdivisions);
    HalfEdgeTable createPlane(glm::vec3 dir, float width, float heigth, uint32_t numSubdivisions);
}
