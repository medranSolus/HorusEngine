#include "WireframePass.h"
#include "RenderPassesBase.h"
#include "GfxResources.h"

namespace GFX::Pipeline::RenderPass
{
	WireframePass::WireframePass(Graphics& gfx, const std::string& name)
		: BindingPass(name), QueuePass(name)
	{
		RegisterSink(Base::SinkDirectBuffer<Resource::IRenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(Base::SinkDirectBuffer<Resource::DepthStencil>::Make("depthStencil", depthStencil));

		RegisterSource(Base::SourceDirectBindable<Resource::IRenderTarget>::Make("renderTarget", renderTarget));

		AddBind(GFX::Resource::DepthStencilState::Get(gfx, GFX::Resource::DepthStencilState::StencilMode::Reverse));
		AddBind(GFX::Resource::Rasterizer::Get(gfx, D3D11_CULL_BACK));
		AddBind(GFX::Resource::Blender::Get(gfx, GFX::Resource::Blender::Type::None));
	}
}