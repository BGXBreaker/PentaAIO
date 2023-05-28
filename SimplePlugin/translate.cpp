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
				{ translation_hash("PentaAIO : Galio"), L"五連殺合集:加里歐" },
				{ translation_hash("Anivia"), L"冰鳥" },
				{ translation_hash("PentaAIO : Anivia"), L"五連殺合集:冰鳥" },
				{ translation_hash("Wait R slow for Q"), L"R緩速再Q" },
				{ translation_hash("Use W when leaving out of R range"), L"用W如果目標嘗試離開R" },
				{ translation_hash("Only E on frosted"), L"只對冰凍目標用E" },
				{ translation_hash("Min. % Mana For Use R"), L"使用R的最少百分比魔力" },
				{ translation_hash("W Interrupt Spell"), L"W打斷技能" },
				{ translation_hash("Use W On"), L"W用在" },
				{ translation_hash("Lasthit Settings"), L"最後一擊設置" },
				{ translation_hash("Use E if unable to AA"), L"如果不能普功才用E" },
				{ translation_hash("Auto Q"), L"自動Q" },
				{ translation_hash("Q Anti-Gapcloser"), L"Q 反突進" },
				{ translation_hash("W Anti-Gapcloser"), L"W 反突進" },
				{ translation_hash("W pull"), L"W目標後面" },
				{ translation_hash("W push"), L"W目標前面" },
				{ translation_hash("Q KS"), L"Q 偷頭" },
				{ translation_hash("E KS"), L"E 偷頭" },
				{ translation_hash("Q Color"), L"Q 顏色" },
				{ translation_hash("W Color"), L"W 顏色" },
				{ translation_hash("E Color"), L"E 顏色" },
				{ translation_hash("R Color"), L"R 顏色" },
				{ translation_hash("Draw Damage"), L"繪製傷害" },
				{ translation_hash("Draw Damage color"), L"繪製傷害顏色" },
				{ translation_hash("Draw Q Damage"), L"繪製Q傷害" },
				{ translation_hash("Draw E Damage"), L"繪製E傷害" },
				{ translation_hash("Draw R Damage"), L"繪製R傷害" },
				{ translation_hash("Colors"), L"顏色" },
				{ translation_hash("W interrupt"), L"W打斷" },
				{ translation_hash("Enabled"), L"已開啟" },
				{ translation_hash("0 = disable"), L"0 = 關閉" },
				{ translation_hash("Spells to Cancel:"), L"要取消的技能:" },
				{ translation_hash("Use WQ combo"), L"用WQ連招" },
				{ translation_hash("Semi Manual"), L"半手動" },
				{ translation_hash("when enemy is on your face"), L"當敵人在你臉上時" },
				{ translation_hash("eg:YiQ , ZedR, etc"), L"劍聖Q,劫R等等" },
				{ translation_hash("Don't close r if target have unkillable buff"), L"如果目標無敵時不要關r" },
				{ translation_hash("eg:KayleR,TaricR,etc"), L"天使大,寶石大等等" }
			}
		);
	}
};

