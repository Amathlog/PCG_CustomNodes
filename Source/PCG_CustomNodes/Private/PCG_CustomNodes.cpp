#include "PCG_CustomNodes.h"
#include "Modules/ModuleManager.h"

class FPCG_CustomNodesModule final : public IModuleInterface
{
public:
	//~ IModuleInterface implementation
	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}
	//~ End IModuleInterface implementation
};

IMPLEMENT_MODULE(FPCG_CustomNodesModule, PCG_CustomNodes);

PCG_CUSTOMNODES_API DEFINE_LOG_CATEGORY(LogPCG_CustomNodes);
