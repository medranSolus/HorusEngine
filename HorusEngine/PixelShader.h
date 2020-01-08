#pragma once
#include "IBindable.h"

namespace GFX::Resource
{
	class PixelShader : public IBindable
	{
	protected:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	public:
		PixelShader(Graphics & gfx, const std::wstring & path);
		PixelShader(const PixelShader&) = delete;
		PixelShader & operator=(const PixelShader&) = delete;
		~PixelShader() = default;

		inline void Bind(Graphics & gfx) noexcept override { GetContext(gfx)->PSSetShader(pixelShader.Get(), nullptr, 0U); }
	};
}
