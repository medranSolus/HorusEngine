#pragma once
#include "RenderPass.h"
#include <array>

namespace GFX::Pipeline
{
	class RenderCommander
	{
		std::array<RenderPass, 3> passes;

	public:
		RenderCommander() = default;
		RenderCommander(const RenderCommander&) = default;
		RenderCommander& operator=(const RenderCommander&) = default;
		~RenderCommander() = default;

		inline void Add(Job&& job, size_t targetPass) noexcept { passes.at(targetPass).Add(std::forward<Job>(job)); }

		void Render(Graphics& gfx) noexcept(!IS_DEBUG);
		void Reset() noexcept;
	};
}