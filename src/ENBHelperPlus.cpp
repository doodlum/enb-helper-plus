#include "ENBHelperPlus.h"

#pragma pack(push, 1)
struct AbsoluteJump
{
	std::uint8_t jump{ 0xFF };       // 0
	std::uint8_t modRm{ 0x25 };      // 1
	std::int32_t relative32{ 0x0 };  // 2
	std::int64_t absolute64;         // 6
};
#pragma pack(pop)

void JumpProcAddress(FARPROC procAddress, int64_t address) 
{
	AbsoluteJump patch;
	patch.absolute64 = address;
	REL::safe_write(reinterpret_cast<std::uintptr_t>(procAddress), &patch, sizeof(AbsoluteJump));
}

void ENBHelperPlus::InstallHooks()
{

#ifdef SKYRIMVR
	HMODULE enbhelper = LoadLibrary(".\\Data\\SKSE\\Plugins\\enbhelpervr.dll");
#else
	HMODULE enbhelper = LoadLibrary(".\\Data\\SKSE\\Plugins\\enbhelperse.dll");
#endif

	if (enbhelper) {
		assert(GetProcAddress(enbhelper, "GetWeatherTransition"));
		assert(GetProcAddress(enbhelper, "GetCurrentWeather"));
		assert(GetProcAddress(enbhelper, "GetOutgoingWeather"));

		JumpProcAddress(GetProcAddress(enbhelper, "GetWeatherTransition"),	reinterpret_cast<int64_t>(&ENBHelperPlus::GetWeatherTransition));
		JumpProcAddress(GetProcAddress(enbhelper, "GetCurrentWeather"),		reinterpret_cast<int64_t>(&ENBHelperPlus::GetCurrentWeather));
		JumpProcAddress(GetProcAddress(enbhelper, "GetOutgoingWeather"),	reinterpret_cast<int64_t>(&ENBHelperPlus::GetOutgoingWeather));
	} else {
		logger::warn("ENB Helper not present, which must be installed for ENB Helper Plus to function.");
	}
}

bool ENBHelperPlus::CheckValidInterior(RE::PlayerCharacter* player)
{
	return (player && player->parentCell && player->parentCell->IsInteriorCell() && player->parentCell->lightingTemplate && !player->parentCell->UsesSkyLighting());
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
