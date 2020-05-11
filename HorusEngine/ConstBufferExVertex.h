#pragma once
#include "ConstBufferEx.h"

namespace GFX::Resource
{
	class ConstBufferExVertex : public ConstBufferEx
	{
		using ConstBufferEx::ConstBufferEx;
		using ConstBufferEx::GetContext;
		using ConstBufferEx::constantBuffer;
		using ConstBufferEx::rootLayout;
		using ConstBufferEx::name;
		using ConstBufferEx::slot;

	public:
		ConstBufferExVertex(const ConstBufferExVertex&) = delete;
		ConstBufferExVertex& operator=(const ConstBufferExVertex&) = delete;
		virtual ~ConstBufferExVertex() = default;

		static std::shared_ptr<ConstBufferExVertex> Get(Graphics& gfx, const std::string& tag, const Data::CBuffer::DCBLayoutElement& root,
			UINT slot = 0U, const Data::CBuffer::DynamicCBuffer* buffer = nullptr);

		static std::string GenerateRID(const std::string& tag, const Data::CBuffer::DCBLayoutElement& root,
			UINT slot = 0U, const Data::CBuffer::DynamicCBuffer* buffer = nullptr) noexcept;

		inline void Bind(Graphics& gfx) noexcept override { GetContext(gfx)->VSSetConstantBuffers(slot, 1U, constantBuffer.GetAddressOf()); }
		inline std::string GetRID() const noexcept override { return GenerateRID(name, rootLayout, slot, nullptr); }
	};

	template<>
	struct is_resolvable_by_codex<ConstBufferExVertex>
	{
		static constexpr bool generate{ true };
	};
}