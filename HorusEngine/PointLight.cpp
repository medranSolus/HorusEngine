#include "PointLight.h"
#include "GlobeVolume.h"
#include "SolidGlobe.h"
#include "SolidCone.h"
#include "TechniqueFactory.h"

namespace GFX::Light
{
	inline Data::CBuffer::DCBLayout PointLight::MakeLayout() noexcept
	{
		static Data::CBuffer::DCBLayout layout;
		static bool initNeeded = true;
		if (initNeeded)
		{
			layout.Add(DCBElementType::Color3, "lightColor");
			layout.Add(DCBElementType::Float, "lightIntensity");
			layout.Add(DCBElementType::Color3, "shadowColor");
			layout.Add(DCBElementType::Float, "atteuationLinear");
			layout.Add(DCBElementType::Float3, "lightPos");
			layout.Add(DCBElementType::Float, "attenuationQuad");
			initNeeded = false;
		}
		return layout;
	}

	PointLight::PointLight(Graphics& gfx, Pipeline::RenderGraph& graph, const std::string& name, float intensity,
		const Data::ColorFloat3& color, const DirectX::XMFLOAT3& position, size_t range, float radius)
		: range(range)
	{
		Data::CBuffer::DynamicCBuffer buffer(MakeLayout());
		buffer["lightIntensity"] = intensity;
		buffer["lightColor"] = color;
		buffer["lightPos"] = position;
		buffer["shadowColor"] = std::move(Data::ColorFloat3(0.005f, 0.005f, 0.005f));

		lightBuffer = Resource::ConstBufferExPixelCache::Get(gfx, typeid(PointLight).name() + name, std::move(buffer));
		mesh = std::make_unique<Shape::SolidGlobe>(gfx, graph, position, name, buffer["lightColor"], 4, 4, radius, radius, radius);
		volume = std::make_unique<Volume::GlobeVolume>(gfx, 3);

		SetAttenuation(range);
		volume->Update(lightBuffer->GetBufferConst());
		AddTechnique(gfx, Pipeline::TechniqueFactory::MakePointLighting(graph));
	}

	bool PointLight::Accept(Graphics& gfx, Probe::BaseProbe& probe) noexcept
	{
		static const size_t STEP = 1;
		ImGui::Columns(2, "##point_light", false);
		if (ImGui::InputScalar("Range", ImGuiDataType_U64, &range, &STEP))
			SetAttenuation(range);
		ImGui::NextColumn();
		if (ILight::Accept(gfx, probe))
		{
			lightBuffer->GetBuffer()["lightPos"] = mesh->GetPos();
			volume->Update(lightBuffer->GetBufferConst());
			return true;
		}
		return false;
	}
}