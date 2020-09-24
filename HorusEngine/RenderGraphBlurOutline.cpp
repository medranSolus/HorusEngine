#include "RenderGraphBlurOutline.h"
#include "RenderPasses.h"
#include "GfxResources.h"
#include "Math.h"

namespace GFX::Pipeline
{
	void RenderGraphBlurOutline::SetKernel() noexcept(!IS_DEBUG)
	{
		assert(radius <= MAX_RADIUS);
		auto& buffer = kernel->GetBuffer();
		buffer["radius"] = radius;
		float sum = 0.0f;
		for (int i = 0; i <= radius; ++i)
		{
			const float g = Math::Gauss(static_cast<float>(i), sigma);
			sum += g;
			buffer["coefficients"][i] = g;
		}
		for (int i = 0; i <= radius; ++i)
			buffer["coefficients"][i] = static_cast<float>(buffer["coefficients"][i]) / sum;
	}

	RenderGraphBlurOutline::RenderGraphBlurOutline(Graphics& gfx, int radius, float sigma)
		: RenderGraph(gfx), radius(radius), sigma(sigma)
	{
		depthOnly = std::make_shared<Resource::DepthStencilShaderInput>(gfx, 8U, Resource::DepthStencil::Usage::DepthOnly);
		AddGlobalSource(RenderPass::Base::SourceDirectBuffer<Resource::DepthStencilShaderInput>::Make("depthOnly", depthOnly));

		geometryBuffer = Resource::RenderTargetEx::Get(gfx, 4U,
			{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT });
		AddGlobalSource(RenderPass::Base::SourceDirectBuffer<Resource::RenderTargetEx>::Make("geometryBuffer", geometryBuffer));

		// Setup blur cbuffers
		Data::CBuffer::DCBLayout kernelLayout;
		kernelLayout.Add(DCBElementType::Integer, "radius");
		kernelLayout.Add(DCBElementType::Array, "coefficients");
		kernelLayout["coefficients"].InitArray(DCBElementType::Float, MAX_RADIUS + 1);
		Data::CBuffer::DynamicCBuffer kernelBuffer(std::move(kernelLayout));
		kernel = std::make_shared<GFX::Resource::ConstBufferExPixelCache>(gfx, "$kernelBuffer", kernelBuffer, 0U);
		SetKernel();
		AddGlobalSource(RenderPass::Base::SourceDirectBindable<GFX::Resource::ConstBufferExPixelCache>::Make("kernel", kernel));

		Data::CBuffer::DCBLayout directionLayout;
		directionLayout.Add(DCBElementType::Bool, "vertical");
		Data::CBuffer::DynamicCBuffer directionBuffer(std::move(directionLayout));
		blurDirection = std::make_shared<GFX::Resource::ConstBufferExPixelCache>(gfx, "$blurDirection", directionBuffer, 1U);
		AddGlobalSource(RenderPass::Base::SourceDirectBindable<GFX::Resource::ConstBufferExPixelCache>::Make("blurDirection", blurDirection));

		skyboxTexture = GFX::Resource::TextureCube::Get(gfx, "Skybox\\Space", ".png");
		AddGlobalSource(RenderPass::Base::SourceDirectBindable<GFX::Resource::TextureCube>::Make("skyboxTexture", skyboxTexture));

		// Setup all passes
		{
			auto pass = std::make_unique<RenderPass::ClearBufferPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::ClearBufferPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::ClearBufferPass>("clearDO");
			pass->SetSinkLinkage("buffer", "$.depthOnly");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::ClearBufferPass>("clearGBuff");
			pass->SetSinkLinkage("buffer", "$.geometryBuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::DepthOnlyPass>(gfx, "depthOnly");
			pass->SetSinkLinkage("depthStencil", "clearDO.buffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::LambertianDepthOptimizedPass>(gfx, "lambertianDepthOptimized");
			pass->SetSinkLinkage("geometryBuffer", "clearGBuff.buffer");
			pass->SetSinkLinkage("depthStencil", "depthOnly.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::LambertianClassicPass>(gfx, "lambertianClassic");
			pass->SetSinkLinkage("geometryBuffer", "lambertianDepthOptimized.geometryBuffer");
			pass->SetSinkLinkage("depthStencil", "lambertianDepthOptimized.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::LightingPass>(gfx, "lighting");
			pass->SetSinkLinkage("geometryBuffer", "lambertianClassic.geometryBuffer");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depth", "lambertianClassic.depth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::SkyboxPass>(gfx, "skybox");
			pass->SetSinkLinkage("skyboxTexture", "$.skyboxTexture");
			pass->SetSinkLinkage("renderTarget", "lighting.renderTarget");
			pass->SetSinkLinkage("depthStencil", "lambertianClassic.depthStencil");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::OutlineGenerationPass>(gfx, "outlineGeneration");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		const unsigned int blurTargetWidth = gfx.GetWidth() / 2;
		const unsigned int blurTargetHeight = gfx.GetHeight() / 2;
		AppendPass(std::make_unique<RenderPass::OutlineDrawBlurPass>(gfx, "outlineDrawBlur", blurTargetWidth, blurTargetHeight));
		{
			auto pass = std::make_unique<RenderPass::HorizontalBlurPass>(gfx, "horizontalBlur", blurTargetWidth, blurTargetHeight);
			pass->SetSinkLinkage("blurTarget", "outlineDrawBlur.blurTarget");
			pass->SetSinkLinkage("kernel", "$.kernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::VerticalBlurPass>(gfx, "verticalBlur");
			pass->SetSinkLinkage("halfTarget", "horizontalBlur.halfTarget");
			pass->SetSinkLinkage("renderTarget", "skybox.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineGeneration.depthStencil");
			pass->SetSinkLinkage("kernel", "$.kernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<RenderPass::WireframePass>(gfx, "wireframe");
			pass->SetSinkLinkage("renderTarget", "verticalBlur.renderTarget");
			pass->SetSinkLinkage("depthStencil", "verticalBlur.depthStencil");
			AppendPass(std::move(pass));
		}
		SetSinkSource("backbuffer", "wireframe.renderTarget");
		Finalize();
	}

	void RenderGraphBlurOutline::BindMainCamera(Camera::ICamera& camera)
	{
		dynamic_cast<RenderPass::DepthOnlyPass&>(FindPass("depthOnly")).BindCamera(camera);
		dynamic_cast<RenderPass::LambertianDepthOptimizedPass&>(FindPass("lambertianDepthOptimized")).BindCamera(camera);
		dynamic_cast<RenderPass::LambertianClassicPass&>(FindPass("lambertianClassic")).BindCamera(camera);
		dynamic_cast<RenderPass::LightingPass&>(FindPass("lighting")).BindCamera(camera);
		dynamic_cast<RenderPass::SkyboxPass&>(FindPass("skybox")).BindCamera(camera);
	}

	void RenderGraphBlurOutline::SetKernel(int radius, float sigma) noexcept(!IS_DEBUG)
	{
		this->sigma = sigma;
		this->radius = radius;
		SetKernel();
	}

	void RenderGraphBlurOutline::ShowWindow() noexcept
	{
		ImGui::Text("Blur Control");
		if (ImGui::SliderInt("Radius", &radius, 1, MAX_RADIUS) || ImGui::SliderFloat("Sigma", &sigma, 0.1f, 20.0f, "%.1f"))
			SetKernel();
	}
}