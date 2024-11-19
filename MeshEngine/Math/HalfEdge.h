#pragma once

#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

const int64_t invalid = -1;

struct HalfEdgeHandle
{
    int64_t index = invalid;

    bool operator==(const HalfEdgeHandle& b) const { return index == b.index; };
    bool operator!=(const HalfEdgeHandle& b) const { return index != b.index; };
    bool operator<(const HalfEdgeHandle& b) const { return index < b.index; };
    bool operator>(const HalfEdgeHandle& b) const { return index > b.index; };
    operator int() const { return index; };
};

struct HalfEdgeVertexHandle
{
    int64_t index = invalid;

    bool operator==(const HalfEdgeVertexHandle& b) const { return index == b.index; };
    bool operator!=(const HalfEdgeVertexHandle& b) const { return index != b.index; };
    bool operator<(const HalfEdgeVertexHandle& b) const { return index < b.index; };
    bool operator>(const HalfEdgeVertexHandle& b) const { return index > b.index; };
    operator int() const { return index; };
};

struct HalfEdgeFaceHandle
{
    int64_t index = invalid;

    bool operator==(const HalfEdgeFaceHandle& b) const { return index == b.index; };
    bool operator!=(const HalfEdgeFaceHandle& b) const { return index != b.index; };
    bool operator<(const HalfEdgeFaceHandle& b) const { return index < b.index; };
    bool operator>(const HalfEdgeFaceHandle& b) const { return index > b.index; };
    operator int() const { return index; };
};

struct HalfEdge
{
    //The face it belongs to, is invalid (== -1) if a boundary half-edge
    HalfEdgeFaceHandle fh;
    //The vertex it points to is always valid
    HalfEdgeVertexHandle dst;
    //The twin half-edge is always VALID. Even for boundary half-edges!
    HalfEdgeHandle twin;
    //The next HalfEdge in the CCW order is always valid
    HalfEdgeHandle next;
    //The previous HalfEdge in the CCW order can be stored for the optimization purposes. For the triangle meshes prev = next->next->next
    HalfEdgeHandle prev;
};

template<typename T>
struct HalfEdgeVertex
{
    //An outgoing HalfEdge from this vertex. It is == -1 if the vertex is isolated
    HalfEdgeHandle heh;
    T data{};
};

struct HalfEdgeFace
{
    //One of the HalfEdges belonging to the Face, always valid
    HalfEdgeHandle heh;
};

template<typename T>
class HalfEdgeTable
{
public:
    //Adds vertex
    HalfEdgeVertexHandle addVertex(T data)
    {
        HalfEdgeVertexHandle vh{ static_cast<int64_t>(m_vertices.size()) };
        m_vertices.push_back(HalfEdgeVertex<T>{ {invalid}, data });

        return vh;
    }

    //Adds triangulated face
    HalfEdgeFaceHandle addFace(HalfEdgeVertexHandle vh0, HalfEdgeVertexHandle vh1, HalfEdgeVertexHandle vh2)
    {
        if (vh0.index <= invalid || vh1.index <= invalid || vh2.index <= invalid)
            throw std::exception("Invalid VertexHandle");

        deleteFakeTwins();

        int64_t size = static_cast<int64_t>(m_halfEdges.size());
        HalfEdgeFaceHandle fh{ static_cast<int64_t>(m_faces.size()) };
        HalfEdgeHandle twin{ invalid };
        HalfEdgeHandle heh0{ size };
        HalfEdgeHandle heh1{ size + 1 };
        HalfEdgeHandle heh2{ size + 2 };

        deref(vh0).heh = heh0;
        deref(vh1).heh = heh1;
        deref(vh2).heh = heh2;

        m_halfEdges.push_back(HalfEdge{ fh, vh1, twin, heh1, heh2 });
        m_halfEdges.push_back(HalfEdge{ fh, vh2, twin, heh2, heh0 });
        m_halfEdges.push_back(HalfEdge{ fh, vh0, twin, heh0, heh1 });
        m_faces.push_back({ heh0 });

        return fh;
    }

    //Adds quad face
    HalfEdgeFaceHandle addFace(HalfEdgeVertexHandle vh0, HalfEdgeVertexHandle vh1, HalfEdgeVertexHandle vh2, HalfEdgeVertexHandle vh3)
    {
        if (vh0.index <= invalid || vh1.index <= invalid || vh2.index <= invalid || vh3.index <= invalid)
            throw std::exception("Invalid VertexHandle");

        deleteFakeTwins();

        int64_t size = static_cast<int64_t>(m_halfEdges.size());
        HalfEdgeFaceHandle fh{ static_cast<int64_t>(m_faces.size()) };
        HalfEdgeHandle twin{ invalid };
        HalfEdgeHandle heh0{ size };
        HalfEdgeHandle heh1{ size + 1 };
        HalfEdgeHandle heh2{ size + 2 };
        HalfEdgeHandle heh3{ size + 3 };

        deref(vh0).heh = heh0;
        deref(vh1).heh = heh1;
        deref(vh2).heh = heh2;
        deref(vh3).heh = heh3;

        m_halfEdges.push_back(HalfEdge{ fh, vh1, twin, heh1, heh3 });
        m_halfEdges.push_back(HalfEdge{ fh, vh2, twin, heh2, heh0 });
        m_halfEdges.push_back(HalfEdge{ fh, vh3, twin, heh3, heh1 });
        m_halfEdges.push_back(HalfEdge{ fh, vh0, twin, heh0, heh2 });
        m_faces.push_back({ heh0 });

        return fh;
    }

    //Builds twins for half-edges. This function must be called in the end
    void connectTwins()
    {
        deleteFakeTwins();

        std::map<std::pair<HalfEdgeVertexHandle, HalfEdgeVertexHandle>, HalfEdgeHandle> collection;
        std::vector<HalfEdgeHandle> notwins;

        // Prepare data
        for (auto& he : m_halfEdges)
        {
            if (he.twin.index == invalid)
                collection.emplace(std::pair<HalfEdgeVertexHandle, HalfEdgeVertexHandle>(he.dst, deref(he.prev).dst), handle(he));
        }

        // Connecting twins
        for (auto& he : m_halfEdges)
        {
            auto twin = collection.find(std::pair<HalfEdgeVertexHandle, HalfEdgeVertexHandle>(deref(he.prev).dst, he.dst));

            if (twin != collection.end())
            {
                he.twin = twin->second;
                deref(twin->second).twin = handle(he);
            }
            else
                notwins.push_back(handle(he));
        }

        createFakeTwins(notwins);
    }

    //Deletes face and re-link all vertices, half-edges, faces. 
    void deleteFace(HalfEdgeFaceHandle fh)
    {
        if (fh.index >= static_cast<int64_t>(m_faces.size()) || fh.index <= invalid)
            throw std::exception("Invalid FaceHandle");

        HalfEdgeHandle start_heh = deref(fh).heh;
        HalfEdgeHandle next_heh = start_heh;

        std::vector<HalfEdgeVertexHandle> temp_vertices;
        std::vector<HalfEdgeHandle> temp_halfEdges;

        std::vector<HalfEdgeVertexHandle> marked_vertices;
        std::vector<HalfEdgeHandle> marked_halfEdges;

        std::vector<HalfEdgeHandle> mapping_halfEdges(m_halfEdges.size());
        std::vector<HalfEdgeVertexHandle> mapping_vertices(m_vertices.size());

        // For all halfEdge heh of face fh do
        do
        {
            deref(next_heh).fh.index = invalid;
            temp_vertices.push_back(destVertex(next_heh));

            if (deref(twin(next_heh)).fh.index == invalid)
                temp_halfEdges.push_back(next_heh);

            next_heh = next(next_heh);
        } while (next_heh != start_heh);

        // Fix (heh)
        for (auto& he : temp_halfEdges)
        {
            HalfEdgeHandle& h0 = he;
            HalfEdgeVertexHandle& v0 = deref(h0).dst;
            HalfEdgeHandle& next0 = deref(h0).next;
            HalfEdgeHandle& prev0 = deref(h0).prev;

            HalfEdgeHandle& h1 = deref(he).twin;
            HalfEdgeVertexHandle& v1 = deref(h1).dst;
            HalfEdgeHandle& next1 = deref(h1).next;
            HalfEdgeHandle& prev1 = deref(h1).prev;

            deref(prev0).next = next1;
            deref(prev1).next = next0;

            deref(next1).prev = prev0;
            deref(next0).prev = prev1;

            // h0 mark as deleted
            // h1 mark as deleted
            marked_halfEdges.push_back(h0);
            marked_halfEdges.push_back(h1);

            if (deref(v0).heh == h1)
            {
                if (next0 == h1)
                {
                    // v0 mark as deleted
                    marked_vertices.push_back(v0);

                    v0.index = invalid;
                }
                else
                    deref(v0).heh = next0;
            }

            if (deref(v1).heh == h0)
            {
                if (next1 == h0)
                {
                    // v1 mark as deleted
                    marked_vertices.push_back(v1);

                    v1.index = invalid;
                }
                else
                    deref(v1).heh = next1;
            }
        }

        // Adjust outgoing halfEdge(VertexHandle vh)
        for (auto& vh : temp_vertices)
        {
            HalfEdgeHandle start_heh = deref(vh).heh;
            HalfEdgeHandle next_heh = start_heh;

            do
            {
                next_heh = next(twin(next_heh));
            } while ((deref(next_heh).fh.index == invalid) && (deref(twin(next_heh)).fh.index == invalid) && (next_heh != start_heh));

            deref(vh).heh = next_heh;
        }

        // Store mapping for halfEdges
        for (int index = 0; index < mapping_halfEdges.size(); ++index)
            mapping_halfEdges[index] = (HalfEdgeHandle{ index });

        // Store mapping for vertices
        for (int index = 0; index < mapping_vertices.size(); ++index)
            mapping_vertices[index] = (HalfEdgeVertexHandle{ index });

        // Remove marked faces
        m_faces.erase(m_faces.begin() + fh.index);

        // Remove marked halfEdges
        for (auto& heh : marked_halfEdges)
        {
            m_halfEdges.erase(m_halfEdges.begin() + mapping_halfEdges[heh.index].index);

            // Update  mapping for halfEdges
            for (int64_t index = heh.index; index < mapping_halfEdges.size(); ++index)
                --mapping_halfEdges[index].index;
        }

        // Remove marked vertices
        for (auto& vh : marked_vertices)
        {
            m_vertices.erase(m_vertices.begin() + mapping_vertices[vh.index].index);

            // Update mapping for vertices
            for (int64_t index = vh.index; index < mapping_vertices.size(); ++index)
                --mapping_vertices[index].index;
        }

        // Remapping
        for (auto& halfedge : m_halfEdges)
        {
            if (halfedge.fh > fh)
                --halfedge.fh.index;

            halfedge.dst = mapping_vertices[halfedge.dst.index];
            halfedge.next = mapping_halfEdges[halfedge.next.index];
            halfedge.prev = mapping_halfEdges[halfedge.prev.index];
            halfedge.twin = mapping_halfEdges[halfedge.twin.index];
        }
        for (auto& vertex : m_vertices)
            vertex.heh = mapping_halfEdges[vertex.heh.index];
        for (auto& face : m_faces)
            face.heh = mapping_halfEdges[face.heh.index];
    }

    //For a given half-edge returns previous linked half-edge
    HalfEdgeHandle prev(HalfEdgeHandle heh) const
    {
        return deref(heh).prev;
    }

    //For a given half-edge returns next linked half-edge
    HalfEdgeHandle next(HalfEdgeHandle heh) const
    {
        return deref(heh).next;
    }

    //For a given half-edge returns twin half-edge
    HalfEdgeHandle twin(HalfEdgeHandle heh) const
    {
        return deref(heh).twin;
    }

    //For a given half-edge returns end vertex
    HalfEdgeVertexHandle destVertex(HalfEdgeHandle heh) const
    {
        return deref(heh).dst;
    }

    //For a given half-edge returns start vertex
    HalfEdgeVertexHandle sourceVertex(HalfEdgeHandle heh) const
    {
        return destVertex(twin(heh));
    }

    //For a given half-edge handle returns half-edge
    HalfEdge& deref(HalfEdgeHandle heh)
    {
        return m_halfEdges[heh.index];
    }

    const HalfEdge& deref(HalfEdgeHandle heh) const
    {
        return m_halfEdges[heh.index];
    }

    //For a given half-edge returns half-edge handle
    HalfEdgeHandle handle(const HalfEdge& he) const
    {
        return { static_cast<int64_t>(&he - &m_halfEdges[0]) };
    }

    HalfEdgeVertex<T>& deref(HalfEdgeVertexHandle vh)
    {
        return m_vertices[vh.index];
    }

    const HalfEdgeVertex<T>& deref(HalfEdgeVertexHandle vh) const
    {
        return m_vertices[vh.index];
    }

    HalfEdgeVertexHandle handle(const HalfEdgeVertex<T>& v) const
    {
        return { static_cast<int>(&v - &m_vertices[0]) };
    }

    HalfEdgeFace& deref(HalfEdgeFaceHandle fh)
    {
        return m_faces[fh.index];
    }

    const HalfEdgeFace& deref(HalfEdgeFaceHandle fh) const
    {
        return m_faces[fh.index];
    }

    HalfEdgeFaceHandle handle(const HalfEdgeFace& f) const
    {
        return { static_cast<int>(&f - &m_faces[0]) };
    }

    //For a given vertex handle set/get point
    const T& getPoint(HalfEdgeVertexHandle handle) const
    {
        return m_vertices[handle.index].data;
    }

    void setPoint(HalfEdgeVertexHandle handle, T point)
    {
        m_vertices[handle.index].data = point;
    }

    //For a given half-edge handle set/get start point (vertex)
    const T& getStartPoint(HalfEdgeHandle handle) const
    {
        return m_vertices[sourceVertex(handle).index].data;
    }

    void setStartPoint(HalfEdgeHandle handle, T point)
    {
        m_vertices[sourceVertex(handle).index].data = point;
    }

    //For a given half-edge handle set/get end point (vertex)
    const T& getEndPoint(HalfEdgeHandle handle) const
    {
        return m_vertices[destVertex(handle).index].data;
    }

    void setEndPoint(HalfEdgeHandle handle, T point)
    {
        m_vertices[destVertex(handle).index].data = point;
    }

    const std::vector<HalfEdgeVertex<T>>& getVertices() const
    {
        return m_vertices;
    }

    const std::vector<HalfEdgeFace>& getFaces() const
    {
        return m_faces;
    }

    const std::vector<HalfEdge>& getHalfEdges() const
    {
        return m_halfEdges;
    }

    std::vector<HalfEdgeVertex<T>>& getVertices()
    {
        return m_vertices;
    }

    std::vector<HalfEdgeFace>& getFaces()
    {
        return m_faces;
    }

    std::vector<HalfEdge>& getHalfEdges()
    {
        return m_halfEdges;
    }

private:
    void createFakeTwins(std::vector<HalfEdgeHandle>& halfEdges)
    {
        m_fakeTwins = m_halfEdges.size();

        // Building fake twins
        for (auto& boundary : halfEdges)
        {
            if (deref(boundary).twin.index != invalid)
                continue;

            int64_t checksPrimary = invalid;
            int64_t checksSecondary = invalid;
            int64_t added = m_halfEdges.size();
            HalfEdgeFaceHandle fh_empty{};
            HalfEdgeHandle start_heh = boundary;
            HalfEdgeHandle previus = prev(boundary);

            // Add twin to boundary
            deref(boundary).twin.index = added;
            m_halfEdges.push_back({ fh_empty, destVertex(previus), boundary, added + 1, added - 1 });

            // Visiting all the half-edges directed to a specified Vertex
            // Ideally, the first condition should always be met, but anyway i introduced a spare
            while ((destVertex(previus) != destVertex(start_heh)) && (checksPrimary < m_fakeTwins))
            {
                added = m_halfEdges.size();

                // Find while prev not boundary
                while ((twin(previus).index != invalid) && (checksSecondary < m_fakeTwins))
                {
                    boundary = twin(previus);
                    previus = prev(boundary);

                    ++checksSecondary;
                }
                checksSecondary = invalid;

                deref(previus).twin.index = added;
                m_halfEdges.push_back({ fh_empty, destVertex(prev(previus)), previus, added + 1, added - 1 });

                boundary = previus;
                previus = prev(boundary);

                ++checksPrimary;
            }
            checksPrimary = invalid;

            // Connect start with end
            deref(HalfEdgeHandle{ added }).next = twin(start_heh);
            deref(twin(start_heh)).prev.index = added;
        }
    };

    void deleteFakeTwins()
    {
        // there is no need to do anything
        if (m_fakeTwins != invalid)
        {
            for (size_t i = m_fakeTwins; i < m_halfEdges.size(); ++m_fakeTwins)
                deref(m_halfEdges[i].twin).twin = { invalid };

            m_halfEdges.erase(m_halfEdges.begin() + static_cast<int64_t>(m_fakeTwins), m_halfEdges.end());
            m_fakeTwins = invalid;
        }
    }

    std::vector<HalfEdgeVertex<T>> m_vertices;
    std::vector<HalfEdge> m_halfEdges;
    std::vector<HalfEdgeFace> m_faces;

    int64_t m_halfEdge = invalid;
    int64_t m_fakeTwins = invalid;
};