#include "TechniqueStep.h"

namespace GFX::Pipeline
{
	void Job::Execute(Graphics& gfx)
	{
		data->Bind(gfx);
		step->Bind(gfx);
		gfx.DrawIndexed(data->GetIndexCount());
	}
}