#pragma once
#include "ShadowMapPass.h"

namespace GFX::Pipeline::RenderPass
{
	class SpotLightingPass : public Base::QueuePass
	{
		ShadowMapPass shadowMapPass;
		Camera::ICamera* mainCamera = nullptr;
		GfxResPtr<GFX::Resource::ConstBufferExPixelCache> shadowBuffer;

		static inline Data::CBuffer::DCBLayout MakeLayout() noexcept;

	public:
		SpotLightingPass(Graphics& gfx, const std::string& name);
		virtual ~SpotLightingPass() = default;

		constexpr void BindCamera(Camera::ICamera& camera) noexcept { mainCamera = &camera; shadowMapPass.BindCamera(camera); }
		inline std::vector<Base::BasePass*> GetInnerPasses() override { return { &shadowMapPass }; }

		void Reset() noexcept override;
		Base::BasePass& GetInnerPass(std::deque<std::string> nameChain) override;
		void Execute(Graphics& gfx) override;
	};
}