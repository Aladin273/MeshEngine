#include "MeshNode.h" 

MeshNode::MeshNode()
{
	m_mesh = std::make_unique<Mesh>(heds::HalfEdgeTable<Vertex>());
}

MeshNode::~MeshNode()
{

}

const BoundingBox& MeshNode::getBoundingBox() const
{
    return m_mesh->getBoundingBox();
}

void MeshNode::attachMesh(std::unique_ptr<Mesh> mesh)
{
	m_mesh = std::move(mesh);
}

Mesh* MeshNode::getMesh() const
{
	return m_mesh.get();
}
