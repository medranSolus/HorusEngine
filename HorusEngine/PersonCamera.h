#pragma once
#include "BaseCamera.h"

namespace Camera
{
	class PersonCamera : public BaseCamera
	{
		DirectX::XMFLOAT3 eyeDirection = { 0.0f, 0.0f, 1.0f };

		DirectX::XMMATRIX UpdateView() const noexcept override;

	public:
		PersonCamera(GFX::Graphics& gfx, GFX::Pipeline::RenderGraph& graph, const CameraParams& params) noexcept;
		PersonCamera(const PersonCamera&) = default;
		PersonCamera& operator=(const PersonCamera&) = default;
		virtual ~PersonCamera() = default;

		void MoveX(float dX) noexcept override;
		inline void MoveY(float dY) noexcept override { static_cast<DirectX::XMFLOAT3&>(cameraBuffer->GetBuffer()["cameraPos"]).y += dY; viewUpdate = true; }

		void Rotate(float angleDX, float angleDY) noexcept override;
	};
}