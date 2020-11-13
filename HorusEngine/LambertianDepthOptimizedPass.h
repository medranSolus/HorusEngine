#pragma once
#include "QueuePass.h"
#include "NullPixelShader.h"
#include "VertexShader.h"
#include "DepthStencilState.h"

namespace GFX::Pipeline::RenderPass
{
	class LambertianDepthOptimizedPass : public Base::QueuePass
	{
		Camera::ICamera* mainCamera = nullptr;
		GfxResPtr<GFX::Resource::NullPixelShader> depthOnlyPS;
		GfxResPtr<GFX::Resource::VertexShader> depthOnlyVS;
		GfxResPtr<GFX::Resource::DepthStencilState> depthOnlyStencilState;
		GfxResPtr<GFX::Resource::DepthStencilState> lambertianStencilState;

	public:
		LambertianDepthOptimizedPass(Graphics& gfx, const std::string& name);
		virtual ~LambertianDepthOptimizedPass() = default;

		constexpr void BindCamera(Camera::ICamera& camera) noexcept { mainCamera = &camera; }

		void Execute(Graphics& gfx) override;
	};
}