#include "ENBHelperPlus.h"

void ENBHelperPlus::InstallHooks()
{
	HMODULE enbhelper = REL::Module::IsVR() ? LoadLibraryA(".\\Data\\SKSE\\Plugins\\enbhelpervr.dll") : LoadLibraryA(".\\Data\\SKSE\\Plugins\\enbhelperse.dll");

	if (!enbhelper) {
		logger::warn("ENB Helper not present, which must be installed for ENB Helper Plus to function.");
		return;
	}

	JumpProcAddress(GetProcAddress(enbhelper, "GetWeatherTransition"),	std::bit_cast<int64_t>(&ENBHelperPlus::GetWeatherTransition));
	JumpProcAddress(GetProcAddress(enbhelper, "GetCurrentWeather"),		std::bit_cast<int64_t>(&ENBHelperPlus::GetCurrentWeather));
	JumpProcAddress(GetProcAddress(enbhelper, "GetOutgoingWeather"),	std::bit_cast<int64_t>(&ENBHelperPlus::GetOutgoingWeather));

	REL::Relocation<std::uintptr_t> GetCurrentRoomLightingTemplate_hook_first{ REL::RelocationID(25703, 26250, 0x1403C5400), REL::Relocate(0x134,  0x139)};
	REL::Relocation<std::uintptr_t> GetPreviousRoomLightingTemplate_hook{ REL::RelocationID(25703, 26250, 0x1403C5400), REL::Relocate(0x57, 0x57) };
	REL::Relocation<std::uintptr_t> GetCurrentRoomLightingTemplate_hook_second{ REL::RelocationID(25703, 26250, 0x1403C5400), REL::Relocate(0x84, 0x84) };

	auto& trampoline = SKSE::GetTrampoline();

	// standard
	trampoline.write_call<5>(GetCurrentRoomLightingTemplate_hook_first.address(), GetCurrentRoomLightingTemplate);

	// transitions
	_GetPreviousRoomLightingTemplate	= trampoline.write_call<5>(GetPreviousRoomLightingTemplate_hook.address(), GetPreviousRoomLightingTemplate);
	_GetCurrentRoomLightingTemplate		= trampoline.write_call<5>(GetCurrentRoomLightingTemplate_hook_second.address(), GetCurrentRoomLightingTemplate);
}

RE::BGSLightingTemplate* ENBHelperPlus::GetPreviousRoomLightingTemplate(RE::Sky* sky)
{
	previousRoomLightingTemplate = _GetPreviousRoomLightingTemplate(sky);
	return previousRoomLightingTemplate;
}

RE::BGSLightingTemplate* ENBHelperPlus::GetCurrentRoomLightingTemplate(RE::Sky* sky)
{
	currentRoomLightingTemplate = _GetCurrentRoomLightingTemplate(sky);
	return currentRoomLightingTemplate;
}

bool ENBHelperPlus::ValidInterior(RE::PlayerCharacter* player)
{
	return (player && player->parentCell && player->parentCell->IsInteriorCell() && !player->parentCell->UsesSkyLighting() && player->parentCell->GetRuntimeData().lightingTemplate);
}

bool ENBHelperPlus::GetWeatherTransition(float& t)
{
	const auto sky = RE::Sky::GetSingleton();
	const auto player = RE::PlayerCharacter::GetSingleton();

	if (sky) {
		if (ValidInterior(player)) {
			if (sky->lightingTransition <= 0.00f || sky->lightingTransition >= 1.00f) {
				t = 1.00f;
			} else {
				t = sky->lightingTransition;
			}
		} else {
			t = sky->currentWeatherPct;
		}
		return true;
	}

	return false;
}

bool ENBHelperPlus::GetCurrentWeather(DWORD& id)
{
	const auto sky = RE::Sky::GetSingleton();
	const auto player = RE::PlayerCharacter::GetSingleton();

	if (sky) {
		if (ValidInterior(player)) {
			if (currentRoomLightingTemplate) {
				id = currentRoomLightingTemplate->GetFormID();
			} else {
				id = player->parentCell->GetRuntimeData().lightingTemplate->GetFormID();
			}
			return true;
		} else if (sky->currentWeather) {
			id = sky->currentWeather->GetFormID();
			return true;
		}
	}

	return false;
}

bool ENBHelperPlus::GetOutgoingWeather(DWORD& id)
{
	const auto sky = RE::Sky::GetSingleton();
	const auto player = RE::PlayerCharacter::GetSingleton();

	if (sky) {
		if (ValidInterior(player)) {
			if (previousRoomLightingTemplate) {
				id = previousRoomLightingTemplate->GetFormID();
			} else {
				id = 0;
			}
			return true;
		} else if (sky->lastWeather) {
			id = sky->lastWeather->GetFormID();
			return true;
		}
	}

	return false;
}
