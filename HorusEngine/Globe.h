#pragma once
#include "IShape.h"

namespace GFX::Shape
{
	class Globe : public IShape
	{
		DirectX::XMFLOAT3 sizes;

	public:
		Globe(Graphics& gfx, Pipeline::RenderGraph& graph, const DirectX::XMFLOAT3& position, const std::string& name, Data::ColorFloat4 color,
			unsigned int latitudeDensity, unsigned int longitudeDensity, float width = 1.0f, float height = 1.0f, float length = 1.0f);
		virtual ~Globe() = default;

		inline void SetTopologyMesh(Graphics& gfx) noexcept override { SetTopology(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ); }

		void UpdateTransformMatrix() noexcept override;
	};
}