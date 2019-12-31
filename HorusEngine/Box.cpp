#include "Box.h"
#include "Primitives.h"
#include "GfxResources.h"
#include "Math.h"

namespace GFX::Object
{
	Box::Box(Graphics& gfx, BasicType::ColorFloat material, float x0, float y0, float z0, float rotationR) : pos(x0, y0, z0), r(rotationR)
	{
		std::mt19937_64 engine(std::random_device{}());
		rotationScale.x = rand(-M_PI, M_PI, engine);
		rotationScale.y = rand(-M_PI, M_PI, engine);
		rotationScale.z = rand(-M_PI, M_PI, engine);
		posScale.x = rand(-M_PI, M_PI, engine);
		posScale.y = rand(-M_PI, M_PI, engine);
		posScale.z = rand(-M_PI, M_PI, engine);
		
		if (!IsStaticInit())
		{
			auto list = Primitive::Cube::Make();
			AddStaticBind(std::make_unique<Resource::VertexBuffer>(gfx, list.vertices));
			AddStaticIndexBuffer(std::make_unique<Resource::IndexBuffer>(gfx, list.indices));
			
			auto vertexShader = std::make_unique<Resource::VertexShader>(gfx, L"PhongVS.cso");
			auto bytecodeVS = vertexShader->GetBytecode();
			AddStaticBind(std::move(vertexShader));
			AddStaticBind(std::make_unique<Resource::PixelShader>(gfx, L"PhongPS.cso"));

			AddStaticBind(std::make_unique<Resource::InputLayout>(gfx, list.vertices.GetLayout().GetDXLayout(), bytecodeVS));

			AddStaticBind(std::make_unique<Resource::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)); // Mesh: D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ
		}
		AddBind(std::make_unique<Resource::ConstantTransformBuffer>(gfx, *this));

		Resource::ObjectConstantBuffer buffer;
		buffer.materialColor = material;
		buffer.specularIntensity = 0.6f;
		buffer.specularPower = 60.0f;
		AddBind(std::make_unique<Resource::ConstantPixelBuffer<Resource::ObjectConstantBuffer>>(gfx, buffer, 1U));
	}

	void Box::Update(float dX, float dY, float dZ, float angleDZ, float angleDX, float angleDY) noexcept
	{
		DirectX::XMStoreFloat3(&angle,
			DirectX::XMVectorModAngles(DirectX::XMVectorMultiplyAdd(DirectX::XMLoadFloat3(&rotationScale),
				DirectX::XMVectorSet(angleDX, angleDY, angleDZ, 0.0f), DirectX::XMLoadFloat3(&angle))));
		DirectX::XMStoreFloat3(&pos,
			DirectX::XMVectorModAngles(DirectX::XMVectorMultiplyAdd(DirectX::XMLoadFloat3(&posScale),
				DirectX::XMVectorSet(dX, dY, dZ, 0.0f), DirectX::XMLoadFloat3(&pos))));
	}

	DirectX::XMMATRIX Box::GetTransformMatrix() const noexcept
	{
		return DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&angle)) * // Rotation around center
			DirectX::XMMatrixTranslation(r, 0.0f, 0.0f) *										// Move to side of rotation sphere
			DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&pos));		// Rotate around sphere
	}
}