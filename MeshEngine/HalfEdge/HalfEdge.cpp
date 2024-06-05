#include "HalfEdge.h"

/*

namespace heds
{
    /////////////////////// Actions

    template<typename T>
    VertexHandle HalfEdgeTable<T>::addVertex(T data) // std::find, if exists, return handle of existing?
    {
        VertexHandle vh{ static_cast<int64_t>(m_vertices.size()) };
        m_vertices.push_back(Vertex<T>{ {invalid}, data });

        return vh;
    }

    template<typename T>
    FaceHandle HalfEdgeTable<T>::addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2)
    {
        if (vh0.index <= invalid || vh1.index <= invalid || vh2.index <= invalid)
            throw std::exception("Invalid VertexHandle");

        deleteFakeTwins();

        int64_t size = static_cast<int64_t>(m_halfEdges.size());
        FaceHandle fh{ static_cast<int64_t>(m_faces.size()) };
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

    template<typename T>
    FaceHandle HalfEdgeTable<T>::addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2, VertexHandle vh3)
    {
        if (vh0.index <= invalid || vh1.index <= invalid || vh2.index <= invalid || vh3.index <= invalid)
            throw std::exception("Invalid VertexHandle");

        deleteFakeTwins();

        int64_t size = static_cast<int64_t>(m_halfEdges.size());
        FaceHandle fh{ static_cast<int64_t>(m_faces.size()) };
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

    template<typename T>
    void HalfEdgeTable<T>::connectTwins()
    {
        deleteFakeTwins();

        std::map<std::pair<VertexHandle, VertexHandle>, HalfEdgeHandle> collection;
        std::vector<HalfEdgeHandle> notwins;

        // Prepare data
        for (auto& he : m_halfEdges)
        {
            if (he.twin.index == invalid)
                collection.emplace(std::pair<VertexHandle, VertexHandle>(he.dst, deref(he.prev).dst), handle(he));
        }

        // Connecting twins
        for (auto& he : m_halfEdges)
        {
            auto twin = collection.find(std::pair<VertexHandle, VertexHandle>(deref(he.prev).dst, he.dst));

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

    template<typename T>
    void HalfEdgeTable<T>::deleteFace(FaceHandle fh)
    {
        if (fh.index >= static_cast<int64_t>(m_faces.size()) || fh.index <= invalid)
            throw std::exception("Invalid FaceHandle");

        HalfEdgeHandle start_heh = deref(fh).heh;
        HalfEdgeHandle next_heh = start_heh;

        std::vector<VertexHandle> temp_vertices;
        std::vector<HalfEdgeHandle> temp_halfEdges;

        std::vector<VertexHandle> marked_vertices;
        std::vector<HalfEdgeHandle> marked_halfEdges;

        std::vector<HalfEdgeHandle> mapping_halfEdges(m_halfEdges.size());
        std::vector<VertexHandle> mapping_vertices(m_vertices.size());

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
            VertexHandle& v0 = deref(h0).dst;
            HalfEdgeHandle& next0 = deref(h0).next;
            HalfEdgeHandle& prev0 = deref(h0).prev;

            HalfEdgeHandle& h1 = deref(he).twin;
            VertexHandle& v1 = deref(h1).dst;
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
            mapping_vertices[index] = (VertexHandle{ index });

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

    template<typename T>
    void HalfEdgeTable<T>::createFakeTwins(std::vector<HalfEdgeHandle>& halfEdges)
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
            FaceHandle fh_empty{};
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
    }

    template<typename T>
    void HalfEdgeTable<T>::deleteFakeTwins()
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

    /////////////////////// Helpers

    template<typename T>
    HalfEdgeHandle HalfEdgeTable<T>::prev(HalfEdgeHandle heh) const
    {
        return deref(heh).prev;
    }

    template<typename T>
    HalfEdgeHandle HalfEdgeTable<T>::next(HalfEdgeHandle heh) const
    {
        return deref(heh).next;
    }

    template<typename T>
    HalfEdgeHandle HalfEdgeTable<T>::twin(HalfEdgeHandle heh) const
    {
        return deref(heh).twin;
    }

    template<typename T>
    VertexHandle HalfEdgeTable<T>::destVertex(HalfEdgeHandle heh) const
    {
        return deref(heh).dst;
    }

    template<typename T>
    VertexHandle HalfEdgeTable<T>::sourceVertex(HalfEdgeHandle heh) const
    {
        return destVertex(twin(heh));
    }

    template<typename T>
    HalfEdge& HalfEdgeTable<T>::deref(HalfEdgeHandle heh)
    {
        return m_halfEdges[heh.index];
    }

    template<typename T>
    const HalfEdge& HalfEdgeTable<T>::deref(HalfEdgeHandle heh) const
    {
        return m_halfEdges[heh.index];
    }

    template<typename T>
    HalfEdgeHandle HalfEdgeTable<T>::handle(const HalfEdge& he) const
    {
        return { static_cast<int64_t>(&he - &m_halfEdges[0]) };
    }

    template<typename T>
    Vertex<T>& HalfEdgeTable<T>::deref(VertexHandle vh)
    {
        return m_vertices[vh.index];
    }

    template<typename T>
    const Vertex<T>& HalfEdgeTable<T>::deref(VertexHandle vh) const
    {
        return m_vertices[vh.index];
    }

    template<typename T>
    VertexHandle HalfEdgeTable<T>::handle(const Vertex<T>& v) const
    {
        return { static_cast<int>(&v - &m_vertices[0]) };
    }

    template<typename T>
    Face& HalfEdgeTable<T>::deref(FaceHandle fh)
    {
        return m_faces[fh.index];
    }

    template<typename T>
    const Face& HalfEdgeTable<T>::deref(FaceHandle fh) const
    {
        return m_faces[fh.index];
    }

    template<typename T>
    FaceHandle HalfEdgeTable<T>::handle(const Face& f) const
    {
        return { static_cast<int>(&f - &m_faces[0]) };
    }

    /////////////////////// Data implementation 

    template<typename T>
    const T& HalfEdgeTable<T>::getPoint(VertexHandle handle) const
    {
        return m_vertices[handle.index].data;
    }

    template<typename T>
    void HalfEdgeTable<T>::setPoint(VertexHandle handle, T point)
    {
        m_vertices[handle.index].data = point;
    }

    template<typename T>
    const T& HalfEdgeTable<T>::getStartPoint(HalfEdgeHandle handle) const
    {
        return m_vertices[sourceVertex(handle).index].data;
    }

    template<typename T>
    void HalfEdgeTable<T>::setStartPoint(HalfEdgeHandle handle, T point)
    {
        m_vertices[sourceVertex(handle).index].data = point;
    }

    template<typename T>
    const T& HalfEdgeTable<T>::getEndPoint(HalfEdgeHandle handle) const
    {
        return m_vertices[destVertex(handle).index].data;
    }

    template<typename T>
    void HalfEdgeTable<T>::setEndPoint(HalfEdgeHandle handle, T point)
    {
        m_vertices[destVertex(handle).index].data = point;
    }

    /////////////////////// Containers getters

    template<typename T>
    const std::vector<Vertex<T>>& HalfEdgeTable<T>::getVertices() const
    {
        return m_vertices;
    }

    template<typename T>
    const std::vector<Face>& HalfEdgeTable<T>::getFaces() const
    {
        return m_faces;
    }

    template<typename T>
    const std::vector<HalfEdge>& HalfEdgeTable<T>::getHalfEdges() const
    {
        return m_halfEdges;
    }

*/
