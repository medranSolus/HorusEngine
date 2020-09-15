#pragma once
#include <stdint.h>

namespace GFX::Pipeline
{
	enum Channel : uint64_t
	{
		Main = 1,
		Shadow = 2,
		Light = 4
	};
}

typedef GFX::Pipeline::Channel RenderChannel;