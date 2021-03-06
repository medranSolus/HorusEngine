#include "ModelNode.h"

namespace GFX::Shape
{
	ModelNode::ModelNode(uint64_t id, const std::string& name, std::vector<std::shared_ptr<Mesh>>&& nodeMeshes,
		const DirectX::XMMATRIX& nodeTransform) noexcept
		: Object(name), id(id), meshes(std::move(nodeMeshes))
	{
		DirectX::XMStoreFloat4x4(&baseTransform, nodeTransform);
		currentTransform = std::make_shared<DirectX::XMFLOAT4X4>();
		for (auto& mesh : meshes)
			mesh->SetTransformMatrix(currentTransform);
	}

	void ModelNode::SetOutline() noexcept
	{
		for (const auto& mesh : meshes)
			mesh->SetOutline();
		for (const auto& child : children)
			child->SetOutline();
	}

	void ModelNode::DisableOutline() noexcept
	{
		for (const auto& mesh : meshes)
			mesh->DisableOutline();
		for (const auto& child : children)
			child->DisableOutline();
	}

	void ModelNode::Submit(uint64_t channelFilter, const DirectX::XMMATRIX& higherTransform) noexcept
	{
		const DirectX::XMMATRIX transformMatrix = DirectX::XMLoadFloat4x4(transform.get()) *
			DirectX::XMLoadFloat4x4(&baseTransform) * higherTransform;
		DirectX::XMStoreFloat4x4(currentTransform.get(), transformMatrix);
		for (const auto& mesh : meshes)
			mesh->Submit(channelFilter);
		for (const auto& child : children)
			child->Submit(channelFilter, transformMatrix);
	}

	bool ModelNode::Accept(Graphics& gfx, Probe::BaseProbe& probe) noexcept
	{
		bool change = false;
		for (const auto& mesh : meshes)
			change |= mesh->Accept(gfx, probe);
		return change;
	}

	bool ModelNode::Accept(Graphics& gfx, Probe::ModelProbe& probe) noexcept
	{
		bool change = false;
		if (probe.PushNode(*this))
		{
			for (auto& child : children)
				change |= child->Accept(gfx, probe);
			probe.PopNode();
		}
		return change;
	}

	void ModelNode::SetMesh(Graphics& gfx, bool meshOnly) noexcept
	{
		if (isMesh != meshOnly)
		{
			isMesh = meshOnly;
			if (isMesh)
				for (auto& nodeMesh : meshes)
					nodeMesh->SetTopologyMesh(gfx);
			else
				for (auto& nodeMesh : meshes)
					nodeMesh->SetTopologyPlain(gfx);
		}
		for (auto& node : children)
			node->SetMesh(gfx, meshOnly);
	}
}