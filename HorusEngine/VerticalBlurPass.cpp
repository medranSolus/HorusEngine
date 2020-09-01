#include "VerticalBlurPass.h"
#include "RenderPassesBase.h"
#include "PipelineResources.h"
#include "GfxResources.h"

namespace GFX::Pipeline::RenderPass
{
	VerticalBlurPass::VerticalBlurPass(Graphics& gfx, const std::string& name) : FullscreenPass(gfx, name)
	{
		AddBindableSink<Resource::RenderTargetShaderInput>("halfTarget");
		AddBindableSink<GFX::Resource::ConstBufferExPixelCache>("kernel");

		RegisterSink(Base::SinkDirectBindable<GFX::Resource::ConstBufferExPixelCache>::Make("direction", direction));
		RegisterSink(Base::SinkDirectBuffer<Resource::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(Base::SinkDirectBuffer<Resource::DepthStencil>::Make("depthStencil", depthStencil));

		RegisterSource(Base::SourceDirectBuffer<Resource::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(Base::SourceDirectBuffer<Resource::DepthStencil>::Make("depthStencil", depthStencil));

		AddBind(GFX::Resource::PixelShader::Get(gfx, "BlurPS.cso"));
		AddBind(GFX::Resource::Blender::Get(gfx, true));
		AddBind(GFX::Resource::DepthStencilState::Get(gfx, GFX::Resource::DepthStencilState::StencilMode::Mask));
		AddBind(GFX::Resource::Sampler::Get(gfx, GFX::Resource::Sampler::Type::Linear, true));
	}

	void VerticalBlurPass::Execute(Graphics& gfx) noexcept(!IS_DEBUG)
	{
		direction->GetBuffer()["vertical"] = true;
		direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}