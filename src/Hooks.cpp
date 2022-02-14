#include "Hooks.h"
#include "ENBHelperPlus.h"

namespace Hooks
{
	void Hooks::Install()
	{
		ENBHelperPlus::InstallHooks();
		logger::info("Installed all hooks");
	}
}
