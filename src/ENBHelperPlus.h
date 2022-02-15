#pragma once

class ENBHelperPlus
{
public:
	static void InstallHooks();

private:
	ENBHelperPlus() = default;

	static bool ValidInterior(RE::PlayerCharacter* player);

	static bool GetWeatherTransition(float& t);

	static bool GetCurrentWeather(DWORD& id);

	static bool GetOutgoingWeather(DWORD& id);

	static RE::BGSLightingTemplate* GetPreviousRoomLightingTemplate(RE::Sky* sky);
	static RE::BGSLightingTemplate* GetCurrentRoomLightingTemplate(RE::Sky* sky);

	static inline REL::Relocation<decltype(GetCurrentRoomLightingTemplate)>  _GetCurrentRoomLightingTemplate;
	static inline REL::Relocation<decltype(GetPreviousRoomLightingTemplate)> _GetPreviousRoomLightingTemplate;

	static inline RE::BGSLightingTemplate* currentRoomLightingTemplate;
	static inline RE::BGSLightingTemplate* previousRoomLightingTemplate;
};

