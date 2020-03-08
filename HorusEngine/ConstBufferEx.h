#pragma once
#include "Codex.h"
#include "DynamicCBuffer.h"

namespace GFX::Resource
{
	class ConstBufferEx : public IBindable
	{
	protected:
		UINT slot;
		std::string name;
		const Data::CBuffer::DCBLayoutElement& rootLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	public:
		ConstBufferEx(Graphics& gfx, const std::string& tag, const Data::CBuffer::DCBLayoutElement& root,
			UINT slot = 0U, const Data::CBuffer::DynamicCBuffer* buffer = nullptr);
		ConstBufferEx(const ConstBufferEx&) = delete;
		ConstBufferEx& operator=(const ConstBufferEx&) = delete;
		virtual ~ConstBufferEx() = default;

		void Update(Graphics& gfx, const Data::CBuffer::DynamicCBuffer& buffer);
	};
}