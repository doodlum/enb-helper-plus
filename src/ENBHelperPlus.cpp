
#include "ENBHelperPlus.h"

void ENBHelperPlus::InstallHooks()
{
	HMODULE enbhelper = LoadLibrary(TEXT("enbhelperse.dll"));

	auto& trampoline = SKSE::GetTrampoline();

	_GetWeatherTransition	= trampoline.write_call<5>(reinterpret_cast<std::uintptr_t>(GetProcAddress(enbhelper, "GetWeatherTransition")), GetWeatherTransition);
	_GetCurrentWeather		= trampoline.write_call<5>(reinterpret_cast<std::uintptr_t>(GetProcAddress(enbhelper, "GetCurrentWeather")), GetCurrentWeather);
	_GetOutgoingWeather		= trampoline.write_call<5>(reinterpret_cast<std::uintptr_t>(GetProcAddress(enbhelper, "GetOutgoingWeather")), GetOutgoingWeather);
}

bool ENBHelperPlus::CheckValidInterior(RE::PlayerCharacter* player)
{
	return (player && player->parentCell && player->parentCell->IsInteriorCell() && player->parentCell->lightingTemplate);
}

bool ENBHelperPlus::GetWeatherTransition(float& t)
{
	const auto sky = RE::Sky::GetSingleton();
	const auto player = RE::PlayerCharacter::GetSingleton();

	if (sky) {
		if (CheckValidInterior(player)) {
			t = sky->lightingTransition == 0.00f ? 1.00f : sky->lightingTransition;
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
		if (CheckValidInterior(player)) {
			if (sky->currentRoom) {
				const auto room = sky->currentRoom.get();
				if (room) {
					const auto roomRef = room->extraList.GetByType(RE::ExtraDataType::kRoomRefData);
					if (roomRef) {
						const auto roomRefData = static_cast<RE::ExtraRoomRefData*>(roomRef)->data;
						if (roomRefData && roomRefData->lightingTemplate) {
							id = roomRefData->lightingTemplate->GetFormID();
							return true;
						}
					}
				}
			}
			id = player->parentCell->lightingTemplate->GetFormID();
			return true;
		} else if (sky->currentWeather) {
			id = sky->currentWeather->GetFormID();
			return true;
		}
	}

	return false;
	;
}

bool ENBHelperPlus::GetOutgoingWeather(DWORD& id)
{
	const auto sky = RE::Sky::GetSingleton();
	const auto player = RE::PlayerCharacter::GetSingleton();

	if (sky) {
		if (CheckValidInterior(player)) {
			if (sky->previousRoom) {
				const auto room = sky->previousRoom.get();
				if (room) {
					const auto roomRef = room->extraList.GetByType(RE::ExtraDataType::kRoomRefData);
					if (roomRef) {
						const auto roomRefData = static_cast<RE::ExtraRoomRefData*>(roomRef)->data;
						if (roomRefData && roomRefData->lightingTemplate) {
							id = roomRefData->lightingTemplate->GetFormID();
							return true;
						}
					}
				}
			}
			id = 0;
			return true;
		} else if (sky->lastWeather) {
			id = sky->lastWeather->GetFormID();
			return true;
		}
	}

	return false;
}
