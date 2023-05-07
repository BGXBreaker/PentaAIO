#include "../plugin_sdk/plugin_sdk.hpp"
#include "translate.h"

namespace translate
{
	void load()
	{
		const auto& cn = translation->add_language("cn", "CN");

		cn->add_translation
		(
			{
				{ translation_hash("if Q hit more than"), L"如果Q命中多於X" },
				{ translation_hash("Allow Tower Dive"), L"允許塔殺" },
				{ translation_hash("E Mode"), L"E模式" },
				{ translation_hash("Galio"), L"加里歐" },
				{ translation_hash(" Don't use E if W remaing cooldown >= X"), L"不要用E,如果W CD>=X秒" },
				{ translation_hash("Semi R"), L"半手動R" },
				{ translation_hash(" Ally HP.%"), L"隊友血量%" },
				{ translation_hash(" Distance ally and enemy"), L"隊友和敵人之間的距離" },
				{ translation_hash("Draw Q range"), L"繪製Q範圍" },
				{ translation_hash("Draw W range"), L"繪製W範圍" },
				{ translation_hash("Draw E range"), L"繪製E範圍" },
				{ translation_hash("Draw Mini map R range"), L"在小地圖繪製R範圍" },
				{ translation_hash("Q Color"), L"Q 顏色" },
				{ translation_hash("W Color"), L"W 顏色" },
				{ translation_hash("E Color"), L"E 顏色" },
				{ translation_hash("R Color"), L"R 顏色" },
				{ translation_hash("Toggle Spell Farm"), L"技能清兵開關" },
				{ translation_hash("Draw Q Damage"), L"繪製Q傷害" },
				{ translation_hash("Draw E Damage"), L"繪製E傷害" },
				{ translation_hash("Draw x AA Damage"), L"繪製X普功傷害" },
				{ translation_hash("Draw Damage Color"), L"傷害顏色" },
				{ translation_hash("Draw Damage"), L"繪製傷害" },
				{ translation_hash("Draw Mini map R range"), L"繪製小地圖R" },
				{ translation_hash("Draw R text"), L"繪製R文本" },
				{ translation_hash("Text X position"), L"文本X位置" },
				{ translation_hash("Text Y position"), L"文本Y位置" },
				{ translation_hash("R Mini map Color"), L"小地圖R顏色" },
				{ translation_hash("PentaAIO : Galio"), L"五連殺合集:加里歐" }
			}
		);
	}
};

