#pragma once
#include "TechniqueStep.h"
#include "RenderChannels.h"

namespace GFX::Pipeline
{
	class Technique : public Probe::IProbeable
	{
		bool active;
		std::string name;
		uint64_t channels;
		std::vector<TechniqueStep> steps;

	public:
		inline Technique(const std::string& name, uint64_t channels, bool active = true) noexcept : active(active), name(name), channels(channels) {}
		inline Technique(std::string&& name, uint64_t channels, bool active = true) noexcept : active(active), name(std::move(name)), channels(channels) {}
		Technique(Technique&&) = default;
		Technique& operator=(Technique&&) = default;
		~Technique() = default;

		constexpr const std::string& GetName() const noexcept { return name; }
		constexpr bool IsActive() const noexcept { return active; }
		constexpr bool& Active() noexcept { return active; }
		constexpr void Activate() noexcept { active = true; }
		constexpr void Deactivate() noexcept { active = false; }
		inline void AddStep(TechniqueStep&& step) noexcept { steps.emplace_back(std::forward<TechniqueStep&&>(step)); }

		void SetParentReference(Graphics& gfx, const GfxObject& parent);
		void Submit(JobData& data, uint64_t channelFilter) noexcept;
		bool Accept(Graphics& gfx, Probe::BaseProbe& probe) noexcept override;
	};
}