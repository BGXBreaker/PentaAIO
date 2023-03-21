#pragma once
#include "../plugin_sdk/plugin_sdk.hpp"

namespace Database
{
	inline std::string DBVersion = "0.4.2";
	void InitiateSlot(TreeTab* tab, game_object_script entity, spellslot slot, std::string name, std::string spellName, bool defaultValue, int mode);
	std::string getDisplayName(game_object_script target);
	int getCastingImportance(game_object_script target);
	void InitializeCancelMenu(TreeTab* tab);
	void InitializeBuffMenu(TreeTab* tab);
	bool canOnAllyBuff(uint32_t buffHash);
	bool canCancel(game_object_script target);
	bool canGuaranteeHit(game_object_script target, float range = FLT_MAX, float speed = FLT_MAX, float delay = 0, float width = 0);
}