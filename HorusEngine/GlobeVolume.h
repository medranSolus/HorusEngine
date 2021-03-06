#pragma once
#include "IVolume.h"

namespace GFX::Light::Volume
{
	class GlobeVolume : public IVolume
	{
	protected:
		void UpdateTransform(float volume, const Data::CBuffer::DynamicCBuffer& lightBuffer) noexcept override;

	public:
		GlobeVolume(Graphics& gfx, unsigned int density);
		virtual ~GlobeVolume() = default;
	};
}