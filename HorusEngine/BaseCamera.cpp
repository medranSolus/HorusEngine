#include "BaseCamera.h"
#include "Math.h"

namespace Camera
{
	inline GFX::Data::CBuffer::DCBLayout BaseCamera::MakeLayout() noexcept
	{
		static GFX::Data::CBuffer::DCBLayout layout;
		static bool initNeeded = true;
		if (initNeeded)
		{
			layout.Add(DCBElementType::Matrix, "inverseViewProjection");
			layout.Add(DCBElementType::Float3, "cameraPos");
			initNeeded = false;
		}
		return layout;
	}

	DirectX::FXMMATRIX BaseCamera::UpdateProjection() const noexcept
	{
		DirectX::XMMATRIX matrix = DirectX::XMMatrixPerspectiveFovLH(projection.fov, projection.screenRatio, projection.nearClip, projection.farClip);
		DirectX::XMStoreFloat4x4(&projectionMatrix, matrix);
		return matrix;
	}

	void BaseCamera::UpdateBuffer() noexcept
	{
		DirectX::XMStoreFloat4x4(&cameraBuffer->GetBuffer()["inverseViewProjection"],
			DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, GetView() * GetProjection())));
	}

	BaseCamera::BaseCamera(GFX::Graphics& gfx, GFX::Pipeline::RenderGraph& graph, const std::string& name,
		float fov, float nearClip, float farClip, const DirectX::XMFLOAT3& position) noexcept
		: ICamera(name), projection({ fov, gfx.GetRatio(), nearClip, farClip })
	{
		cameraBuffer = GFX::Resource::ConstBufferExPixelCache::Get(gfx, typeid(BaseCamera).name() + name, MakeLayout(), 2U);
		cameraBuffer->GetBuffer()["cameraPos"] = position;
		indicator = std::make_shared<GFX::Shape::CameraIndicator>(gfx, graph, position, name, DirectX::XMFLOAT3(0.5f, 0.5f, 1.0f));
		frustrum = std::make_shared<GFX::Shape::CameraFrustrum>(gfx, graph, position, name, DirectX::XMFLOAT3(1.0f, 0.5f, 0.5f), projection);
	}

	DirectX::FXMMATRIX BaseCamera::GetProjection() const noexcept
	{
		if (projectionUpdate)
		{
			projectionUpdate = false;
			return UpdateProjection();
		}
		else
			return DirectX::XMLoadFloat4x4(&projectionMatrix);
	}

	DirectX::FXMMATRIX BaseCamera::GetView() const noexcept
	{
		if (viewUpdate)
		{
			viewUpdate = false;
			return UpdateView();
		}
		else
			return DirectX::XMLoadFloat4x4(&viewMatrix);
	}

	void BaseCamera::Roll(float delta) noexcept
	{
		DirectX::XMStoreFloat3(&up,
			DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&up),
				DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, delta)));
		indicator->UpdateAngle({ 0.0f, 0.0f, delta });
		frustrum->UpdateAngle({ 0.0f, 0.0f, delta });
		viewUpdate = true;
	}

	void BaseCamera::BindCamera(GFX::Graphics& gfx) const noexcept
	{
		gfx.SetView(GetView());
		gfx.SetProjection(GetProjection());
	}

	void BaseCamera::Accept(GFX::Graphics& gfx, GFX::Probe::BaseProbe& probe) noexcept
	{
		ImGui::Checkbox("Enable Indicator", &enableIndicator);
		if (enableIndicator)
		{
			ImGui::SameLine();
			ImGui::Checkbox("Enable Frustrum", &enableFrustrum);
		}
		projectionUpdate |= probe.VisitCamera(projection);
		if (projectionUpdate)
			frustrum->SetParams(gfx, projection);
		//indicator->Accept(gfx, probe);
	}

	void BaseCamera::Submit(uint64_t channelFilter) noexcept
	{
		if (enableIndicator)
		{
			const DirectX::XMFLOAT3& position = cameraBuffer->GetBufferConst()["cameraPos"];
			indicator->SetPos(position);
			indicator->Submit(channelFilter);
			if (enableFrustrum)
			{
				frustrum->SetPos(position);
				frustrum->Submit(channelFilter);
			}
		}
	}
}