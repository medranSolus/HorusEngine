#include "SSAOPass.h"
#include "RenderPassesBase.h"
#include "PipelineResources.h"
#include "GfxResources.h"
#include "Math.h"

namespace GFX::Pipeline::RenderPass
{
	inline Data::CBuffer::DCBLayout SSAOPass::MakeLayout() noexcept
	{
		static Data::CBuffer::DCBLayout layout;
		static bool initNeeded = true;
		if (initNeeded)
		{
			layout.Add(DCBElementType::Array, "kernel");
			layout["kernel"].InitArray(DCBElementType::Float3, SSAO_KERNEL_SIZE);
			layout.Add(DCBElementType::Float, "bias");
			layout.Add(DCBElementType::Float2, "tileDimensions");
			layout.Add(DCBElementType::Float, "sampleRadius");
			layout.Add(DCBElementType::Float, "ssaoPower");
			layout.Add(DCBElementType::UInteger, "kernelSize");
			initNeeded = false;
		}
		return layout;
	}

	SSAOPass::SSAOPass(Graphics& gfx, const std::string& name)
		: BindingPass(name), FullscreenPass(gfx, name)
	{
		renderTarget = GfxResPtr<Resource::RenderTargetShaderInput>(gfx, 11U, DXGI_FORMAT_R32_FLOAT);
		ssaoScratchBuffer = GfxResPtr<Resource::RenderTargetShaderInput>(gfx, 11U, DXGI_FORMAT_R32_FLOAT);

		kernelBuffer = GFX::Resource::ConstBufferExPixelCache::Get(gfx, typeid(SSAOPass).name(), MakeLayout(), 13U);
		kernelBuffer->GetBuffer()["bias"] = bias;
		kernelBuffer->GetBuffer()["tileDimensions"] = DirectX::XMFLOAT2(4.0f * (gfx.GetWidth() / SSAO_NOISE_SIZE), 8.0f * (gfx.GetHeight() / SSAO_NOISE_SIZE));
		kernelBuffer->GetBuffer()["sampleRadius"] = radius;
		kernelBuffer->GetBuffer()["ssaoPower"] = power;
		kernelBuffer->GetBuffer()["kernelSize"] = size;
		std::mt19937_64 engine(std::random_device{}());
		for (size_t i = 0; i < SSAO_KERNEL_SIZE; ++i)
		{
			const DirectX::XMVECTOR sample = DirectX::XMVectorSet(Math::RandNDC(engine),
				Math::RandNDC(engine), Math::Rand01(engine), 0.0f);

			float scale = static_cast<float>(i) / SSAO_KERNEL_SIZE;
			scale = Math::Lerp(0.1f, 1.0f, scale * scale);

			DirectX::XMStoreFloat3(&kernelBuffer->GetBuffer()["kernel"][i],
				DirectX::XMVectorMultiply(DirectX::XMVector3Normalize(sample), DirectX::XMVectorSet(scale, scale, scale, 0.0f)));
		}

		AddBindableSink<GFX::Resource::IBindable>("geometryBuffer");
		AddBindableSink<Resource::DepthStencilShaderInput>("depth");

		RegisterSource(Base::SourceDirectBindable<Resource::IRenderTarget>::Make("ssaoBuffer", renderTarget));
		RegisterSource(Base::SourceDirectBuffer<Resource::IRenderTarget>::Make("ssaoScratch", ssaoScratchBuffer));
		RegisterSource(Base::SourceDirectBindable<GFX::Resource::ConstBufferExPixelCache>::Make("ssaoKernel", kernelBuffer));

		AddBind(kernelBuffer);
		AddBind(GFX::Resource::PixelShader::Get(gfx, "AmbientOcclusionPS"));
		AddBind(GFX::Resource::Blender::Get(gfx, GFX::Resource::Blender::Type::None));

		Surface ssaoNoise(SSAO_NOISE_SIZE / 4, SSAO_NOISE_SIZE / 8, DXGI_FORMAT_R32G32_FLOAT);
		float* buffer = reinterpret_cast<float*>(ssaoNoise.GetBuffer());
		for (size_t i = 0; i < SSAO_NOISE_SIZE * 2; ++i)
			buffer[i] = Math::RandNDC(engine);
		AddBind(GFX::Resource::Texture::Get(gfx, ssaoNoise, "ssaoNoise", 12U));
	}

	void SSAOPass::Execute(Graphics& gfx)
	{
		assert(mainCamera);
		mainCamera->BindPS(gfx);
		FullscreenPass::Execute(gfx);
	}

	void SSAOPass::ShowWindow(Graphics& gfx)
	{
		if (ImGui::CollapsingHeader("SSAO"))
		{
			ImGui::Columns(2, "##ssao_options", false);
			ImGui::Text("Kernel size");
			ImGui::SetNextItemWidth(-1.0f);
			if (ImGui::InputInt("##kernel_size", reinterpret_cast<int*>(&size), 1))
			{
				if (size < 4)
					size = 4;
				else if (size > SSAO_KERNEL_SIZE)
					size = SSAO_KERNEL_SIZE;
				kernelBuffer->GetBuffer()["kernelSize"] = size;
			}
			ImGui::Text("Power");
			ImGui::SetNextItemWidth(-1.0f);
			if (ImGui::InputFloat("##ssao_power", &power, 0.01f, 0.0f, "%.2f"))
			{
				if (power < 0.0f)
					power = 0.0f;
				kernelBuffer->GetBuffer()["ssaoPower"] = power;
			}
			ImGui::NextColumn();
			ImGui::Text("Radius");
			ImGui::SetNextItemWidth(-1.0f);
			if (ImGui::InputFloat("##ssao_radius", &radius, 0.01f, 0.0f, "%.2f"))
			{
				if (radius < 0.01f)
					radius = 0.01f;
				kernelBuffer->GetBuffer()["sampleRadius"] = radius;
			}
			ImGui::Text("Bias");
			ImGui::SetNextItemWidth(-1.0f);
			if (ImGui::InputFloat("##ssao_bias", &bias, 0.001f, 0.0f, "%.3f"))
				kernelBuffer->GetBuffer()["bias"] = bias;
			ImGui::Columns(1);
		}
	}
}