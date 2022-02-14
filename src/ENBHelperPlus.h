#pragma once

class ENBHelperPlus
{
public:
	static void InstallHooks();

private:
	ENBHelperPlus() = default;

	static bool CheckValidInterior(RE::PlayerCharacter* player);

	static bool GetWeatherTransition(float& t);

	static bool GetCurrentWeather(DWORD& id);

	static bool GetOutgoingWeather(DWORD& id);

	static inline REL::Relocation<decltype(GetWeatherTransition)> _GetWeatherTransition;

	static inline REL::Relocation<decltype(GetCurrentWeather)> _GetCurrentWeather;

	static inline REL::Relocation<decltype(GetOutgoingWeather)> _GetOutgoingWeather;

};
