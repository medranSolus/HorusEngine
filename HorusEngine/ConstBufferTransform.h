#pragma once
#include "ConstBufferVertex.h"
#include "CBuffers.h"
#include "GfxObject.h"

namespace GFX::Resource
{
	class ConstBufferTransform : public IBindable
	{
		static std::unique_ptr<ConstBufferVertex<Data::CBuffer::Transform>> vertexBuffer;
		const GfxObject& parent;

	protected:
		Data::CBuffer::Transform GetBufferData(Graphics& gfx) noexcept;
		virtual void UpdateBind(Graphics& gfx, const Data::CBuffer::Transform& buffer) noexcept;

	public:
		ConstBufferTransform(Graphics& gfx, const GfxObject& parent, UINT slot = 0U);
		ConstBufferTransform(const ConstBufferTransform&) = delete;
		ConstBufferTransform& operator=(const ConstBufferTransform&) = delete;
		virtual ~ConstBufferTransform() = default;

		inline void Bind(Graphics& gfx) noexcept override { UpdateBind(gfx, GetBufferData(gfx)); }
		inline std::string GetRID() const noexcept override { return "?"; }
	};
}