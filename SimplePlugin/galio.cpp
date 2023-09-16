#include "../plugin_sdk/plugin_sdk.hpp"
#include "galio.h"
#include "utils.h"
#include "translate.h"
namespace galio
{
	script_spell* q = nullptr;
	script_spell* w = nullptr;
	script_spell* e = nullptr;
	script_spell* r = nullptr;
	script_spell* wCharge = nullptr;

	TreeTab* main_tab = nullptr;

	namespace draw_settings
	{
		TreeEntry* draw_range_q = nullptr;
		TreeEntry* q_color = nullptr;
		TreeEntry* draw_range_w = nullptr;
		TreeEntry* w_color = nullptr;
		TreeEntry* draw_range_e = nullptr;
		TreeEntry* e_color = nullptr;
		TreeEntry* rMinimapRange = nullptr;
		TreeEntry* r_color = nullptr;
		TreeEntry* Draw_Damage = nullptr;
		TreeEntry* Draw_Damage_color = nullptr;
		TreeEntry* Draw_DMG_rl = nullptr;
		TreeEntry* draw_transparency = nullptr;
		TreeEntry* draw_text = nullptr;
		TreeEntry* x_pos = nullptr;
		TreeEntry* y_pos = nullptr;

		namespace draw_damage_settings
		{
			TreeEntry* draw_damage = nullptr;
			TreeEntry* q_damage = nullptr;
			TreeEntry* e_damage = nullptr;
			TreeEntry* aa_damage = nullptr;
		}
	}
	namespace combo
	{
		TreeEntry* use_q = nullptr;
		TreeEntry* use_w = nullptr;
		TreeEntry* use_e = nullptr;
		TreeEntry* use_r = nullptr;
		TreeEntry* e_mode = nullptr;
		TreeEntry* allow_tower_dive = nullptr;
		TreeEntry* use_everfrost = nullptr;
		TreeEntry* reamin_cd = nullptr;
		TreeEntry* ally_hp = nullptr;
		TreeEntry* enemy_distance = nullptr;
		TreeEntry* semi_r = nullptr;
		std::map<std::uint32_t, TreeEntry*> use_r_on;
	}
	namespace harass
	{
		TreeEntry* use_q = nullptr;
	}
	namespace laneclear
	{
		TreeEntry* spell_farm = nullptr;
		TreeEntry* use_q = nullptr;
		TreeEntry* q_use_if_hit_minions = nullptr;
	}
	namespace jungleclear
	{
		TreeEntry* use_q = nullptr;
		TreeEntry* use_e = nullptr;
	}

	namespace fleemode
	{
		TreeEntry* use_e = nullptr;
	}
	namespace hitchance
	{
		TreeEntry* q_hitchance = nullptr;
		TreeEntry* e_hitchance = nullptr;
	}



	void on_update();
	void on_draw();
	void draw_dmg_rl();
	void on_after_attack_orbwalker(game_object_script target);
	void on_process_spell_cast(game_object_script sender, spell_instance_script spell);
	float calculate_q_damage(game_object_script enemy);
	float calculate_q_wind_damage(game_object_script enemy);
	float calculate_e_damage(game_object_script enemy);
	float dmg_can_do();

	std::vector<float> q_damages = { 70,105,140,175,210 };
	float q_coef = 0.75;

	std::vector<float> e_damages = { 90,130,170,210,250 };
	float e_coef = 0.9;

	void q_logic();
	void w_logic();
	void e_logic();
	void r_logic();
	float last_w_time;
	float last_e_time;
	float w_charg_range;

	float r_radius[] = { 4000.0f, 4750.0f, 5500.0f };

	void load()
	{
		translate::load();
		q = plugin_sdk->register_spell(spellslot::q, 825);
		q->set_skillshot(0.25f, 150.0f, 1400.0f, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_circle);
		w = plugin_sdk->register_spell(spellslot::w, 325);
		w->set_charged(250.0f, 450.0f, 2.0f);
		w->set_skillshot(0.0f, 450.0f, FLT_MAX, { }, skillshot_type::skillshot_circle);
		e = plugin_sdk->register_spell(spellslot::e, 650);
		e->set_skillshot(0.4f, 160.0f, 2300.0f, { collisionable_objects::minions,collisionable_objects::heroes,collisionable_objects::walls }, skillshot_type::skillshot_line);
		r = plugin_sdk->register_spell(spellslot::r, 4000);


		q->set_spell_lock(false);
		w->set_spell_lock(false);
		e->set_spell_lock(false);
		r->set_spell_lock(false);

		main_tab = menu->create_tab("galio", "Galio");
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
		{
			main_tab->add_separator(myhero->get_model() + ".aio", "PentaAIO : " + myhero->get_model());
			auto combo = main_tab->add_tab(myhero->get_model() + ".combo", "Combo Settings");
			{
				combo::use_q = combo->add_checkbox(myhero->get_model() + ".combo.q", "Use Q", true);
				combo::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
				combo::use_w = combo->add_checkbox(myhero->get_model() + ".combo.w", "Use W", true);
				combo::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
				auto w_setting = combo->add_tab(myhero->get_model() + ".combo.w", "W setting");
				{
					combo::reamin_cd = combo->add_slider(myhero->get_model() + ".cd", " Don't use E if W remaing cooldown >= X", 2, 1, 18);
				}
				combo::use_e = combo->add_checkbox(myhero->get_model() + ".combo.e", "Use E", true);
				combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
				auto e_mode1 = combo->add_tab(myhero->get_model() + ".combo.e", "E mode");
				{
					combo::e_mode = combo->add_combobox(myhero->get_model() + ".combo.e.mode", "E Mode", { {"EW", nullptr},{"WE", nullptr } }, 0);
				}
				combo::allow_tower_dive = combo->add_hotkey(myhero->get_model() + ".combo.allow_tower_dive", "Allow Tower Dive", TreeHotkeyMode::Toggle, 'A', true);
				combo::use_r = combo->add_checkbox(myhero->get_model() + ".combo.r", "Use R", true);
				combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
				auto r_config = combo->add_tab(myhero->get_model() + ".combo.r.config", "R settings");
				{
					combo::semi_r = r_config->add_hotkey(myhero->get_model() + ".semi.r", "Semi R", TreeHotkeyMode::Hold, 'R', false);
					combo::ally_hp = r_config->add_slider(myhero->get_model() + ".hp", " Ally HP.%", 30, 1, 100);
					combo::enemy_distance = r_config->add_slider(myhero->get_model() + ".pos", " Distance ally and enemy", 400, 1, 610);
					auto use_r_on_tab = r_config->add_tab(myhero->get_model() + ".combo.use_r_on", "Use R On");
					{
						for (auto&& ally : entitylist->get_ally_heroes())
						{
							if (ally->is_me()) continue;
							combo::use_r_on[ally->get_network_id()] = use_r_on_tab->add_checkbox(std::to_string(ally->get_network_id()), ally->get_model(), true, false);
							combo::use_r_on[ally->get_network_id()]->set_texture(ally->get_square_icon_portrait());
						}
					}
				}
			}
			auto harass = main_tab->add_tab(myhero->get_model() + ".harass", "Harass Settings");
			{
				harass::use_q = harass->add_checkbox(myhero->get_model() + ".harass.q", "Use Q", true);
				harass::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
			}
			auto laneclear = main_tab->add_tab(myhero->get_model() + ".laneclear", "Lane Clear Settings");
			{
				laneclear::spell_farm = laneclear->add_hotkey(myhero->get_model() + ".laneclear.enabled", "Toggle Spell Farm", TreeHotkeyMode::Toggle, 0x04, true);
				laneclear::use_q = laneclear->add_checkbox(myhero->get_model() + ".laneclear.q", "Use Q", true);
				laneclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
				auto q_config = laneclear->add_tab(myhero->get_model() + ".laneclear.q.config", "Q Config");
				{
					laneclear::q_use_if_hit_minions = q_config->add_slider(myhero->get_model() + ".laneclear.q.use_can_hit_minions", "if Q hit more than", 3, 1, 6);
				}
			}
			auto jungleclear = main_tab->add_tab(myhero->get_model() + ".jungleclear", "Jungle Clear Settings");
			{
				jungleclear::use_q = jungleclear->add_checkbox(myhero->get_model() + ".jungleclear.q", "Use Q", true);
				jungleclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
				jungleclear::use_e = jungleclear->add_checkbox(myhero->get_model() + ".jungleclear.e", "Use E", true);
				jungleclear::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
			}
			auto fleemode = main_tab->add_tab(myhero->get_model() + ".fleemode", "Flee Mode");
			{
				fleemode::use_e = fleemode->add_checkbox(myhero->get_model() + ".flee.e", "Use E", true);
				fleemode::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
			}
			auto hitchance = main_tab->add_tab(myhero->get_model() + ".hitchance", "Hitchance Settings");
			{
				hitchance::q_hitchance = hitchance->add_combobox(myhero->get_model() + ".hitchance.q", "Hitchance Q", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
				hitchance::e_hitchance = hitchance->add_combobox(myhero->get_model() + ".hitchance.e", "Hitchance E", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
			}
			auto draw_settings = main_tab->add_tab(myhero->get_model() + ".drawings", "Drawings Settings");
			{
				float color_red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
				float color_pink[] = { 1.0f, 0.5f, 0.5f, 1.0f };
				float color_yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
				float color_blue[] = { 0.0f, 0.4f, 1.0f, 1.0f };
				float color_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };

				draw_settings::draw_range_q = draw_settings->add_checkbox(myhero->get_model() + ".drawingQ", "Draw Q range", true);
				draw_settings::draw_range_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
				draw_settings::q_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.q.color", "Q Color", color_red, true);

				draw_settings::draw_range_w = draw_settings->add_checkbox(myhero->get_model() + ".drawingw", "Draw W range", true);
				draw_settings::draw_range_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
				draw_settings::w_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.w.color", "W Color", color_pink, true);

				draw_settings::draw_range_e = draw_settings->add_checkbox(myhero->get_model() + ".drawinge", "Draw E range", true);
				draw_settings::draw_range_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
				draw_settings::e_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.e.color", "E Color", color_yellow, true);

				draw_settings::rMinimapRange = draw_settings->add_checkbox(myhero->get_model() + ".drawingr", "Draw Mini map R range", true);
				draw_settings::rMinimapRange->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
				draw_settings::r_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.r.color", "R Mini map Color", color_blue, true);

				draw_settings::draw_text = draw_settings->add_checkbox(myhero->get_model() + ".text", "Draw R text", false);
				draw_settings::x_pos = draw_settings->add_slider(myhero->get_model() + ".xpos", "Text X position", 500, 1, 1000);
				draw_settings::y_pos = draw_settings->add_slider(myhero->get_model() + ".ypos", "Text Y position", 500, 1, 1000);


				auto draw_damage = draw_settings->add_tab(myhero->get_model() + ".draw.damage", "Draw Damage");
				{
					draw_settings::draw_damage_settings::draw_damage = draw_damage->add_checkbox(myhero->get_model() + ".draw.damage.enabled", "Draw Combo Damage", true);
					draw_settings::draw_transparency = draw_settings->add_colorpick("color", "Draw Damage color", color_green, true);
					draw_settings::draw_damage_settings::q_damage = draw_damage->add_checkbox(myhero->get_model() + ".draw.damage.q", "Draw Q Damage", true);
					draw_settings::draw_damage_settings::q_damage->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
					draw_settings::draw_damage_settings::e_damage = draw_damage->add_checkbox(myhero->get_model() + ".draw.damage.e", "Draw E Damage", true);
					draw_settings::draw_damage_settings::e_damage->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
					draw_settings::draw_damage_settings::aa_damage = draw_damage->add_slider(myhero->get_model() + ".draw.damage.aa", "Draw x AA Damage", 2, 0, 8);
				}
			}


			event_handler<events::on_update>::add_callback(on_update);
			event_handler<events::on_env_draw>::add_callback(on_draw);
			event_handler<events::on_after_attack_orbwalker>::add_callback(on_after_attack_orbwalker);
			event_handler<events::on_process_spell_cast>::add_callback(on_process_spell_cast);
			event_handler<events::on_draw>::add_callback(draw_dmg_rl);

			utils::on_load();

		}
	}
	void unload()
	{
		plugin_sdk->remove_spell(q);
		plugin_sdk->remove_spell(w);
		plugin_sdk->remove_spell(e);
		plugin_sdk->remove_spell(r);

		menu->delete_tab(main_tab);

		event_handler<events::on_update>::remove_handler(on_update);
		event_handler<events::on_env_draw>::remove_handler(on_draw);
		event_handler<events::on_process_spell_cast>::remove_handler(on_process_spell_cast);
		event_handler<events::on_after_attack_orbwalker>::remove_handler(on_after_attack_orbwalker);
		event_handler<events::on_draw >::remove_handler(draw_dmg_rl);

	}
	void on_update()
	{
		if (myhero->is_dead())
		{
			return;
		}
		if (r->is_ready())
		{
			r->set_range(r_radius[r->level() - 1]);
		}
		if (r->is_ready() && combo::semi_r->get_bool())
		{
			r_logic();
		}
		if (orbwalker->combo_mode())
		{
			if (q->is_ready() && combo::use_q->get_bool())
			{
				q_logic();
			}
			if (w->is_ready() && combo::use_w->get_bool())
			{
				w_logic();
			}
			if (e->is_ready() && combo::use_e->get_bool())
			{
				e_logic();
			}
		}
		if (orbwalker->harass())
		{

			if (!myhero->is_under_enemy_turret())
			{
				if (q->is_ready() && harass::use_q->get_bool())
				{
					q_logic();
				}
			}
		}
		if (orbwalker->flee_mode())
		{
			if (e->is_ready() && fleemode::use_e->get_bool() && myhero->get_position().count_enemies_in_range(450) == 0)
			{
				if (e->cast(hud->get_hud_input_logic()->get_game_cursor_position()))
				{
					return;
				}
			}
		}
		if (orbwalker->lane_clear_mode() && laneclear::spell_farm->get_bool())
		{
			auto lane_minions = entitylist->get_enemy_minions();

			auto monsters = entitylist->get_jugnle_mobs_minions();

			lane_minions.erase(std::remove_if(lane_minions.begin(), lane_minions.end(), [](game_object_script x)
				{
					return !x->is_valid_target(q->range());/* || q->get_damage(x) < x->get_health() */
				}), lane_minions.end());
			monsters.erase(std::remove_if(monsters.begin(), monsters.end(), [](game_object_script x)
				{
					return !x->is_valid_target(q->range());
				}), monsters.end());

			/*std::sort(lane_minions.begin(), lane_minions.end(), [](game_object_script a, game_object_script b)
			{
					return a->get_position().distance(myhero->get_position()) < b->get_position().distance(myhero->get_position());
			});*/

			std::sort(monsters.begin(), monsters.end(), [](game_object_script a, game_object_script b)
				{
					return a->get_max_health() > b->get_max_health();
				});

			if (!lane_minions.empty()) // >= laneclear::q_use_if_hit_minions->get_int()
			{
				if (q->is_ready()) //&& laneclear::q_use_if_hit_minions->get_int()
				{
					q->cast_on_best_farm_position(laneclear::q_use_if_hit_minions->get_int());
				}

			}

			if (!monsters.empty())
			{
				if (q->is_ready() && jungleclear::use_q->get_bool() && ((jungleclear::use_e->get_bool() && !e->is_ready()) || !jungleclear::use_e->get_bool()))
				{
					q->cast(monsters.front()->get_position());
				}

				if (e->is_ready() && jungleclear::use_e->get_bool())
				{
					e->cast(monsters.front()->get_position());
				}
			}
		}
	}
#pragma region q_logic 
	void q_logic()
	{
		auto target = target_selector->get_target(q->range(), damage_type::magical);
		{
			if (target != nullptr)
			{

				if (q->cast(target, utils::get_hitchance(hitchance::q_hitchance)))
				{
					return;
				}

			}

		}
	}
#pragma endregion

#pragma region w_logic
	void w_logic()
	{
		auto target = target_selector->get_target(w->range(), damage_type::magical);
		for (auto&& enemy : entitylist->get_enemy_heroes())
		{
			if (enemy->is_valid() && !enemy->is_dead() && enemy->get_distance(myhero->get_position()) <= w->range())
			{
				if (myhero->has_buff(buff_hash("GalioW")))
				{
					if (myhero->get_position().count_enemies_in_range(w_charg_range) >= 1 && myhero->count_enemies_in_range(w->range() - 20) == 0)
					{
						myhero->update_charged_spell(w->get_slot(), myhero->get_position(), true);
					}
				}
				else
				{
					if (myhero->get_position().count_enemies_in_range(w->range()) >= 1)
					{
						if (w->start_charging())
						{
							return;
						}
					}
				}

			}

		}
	}
#pragma endregion

#pragma region e_logic
	void e_logic()
	{
		auto target = target_selector->get_target(e->range(), damage_type::magical);

		if (target != nullptr)
		{
			if (dmg_can_do() > target->get_real_health())
			{
				if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
				{
					auto pos = e->get_prediction(target).get_cast_position();
					if (!evade->is_dangerous(pos))
					{
						e->cast(pos);
						//myhero->print_chat(1, "killable e pos");
					}
					else
					{
						e->cast(target, utils::get_hitchance(hitchance::e_hitchance));
						//myhero->print_chat(1, "killable e2");
					}
				}

			}

			if (myhero->get_distance(target) > myhero->get_attack_range())
			{
				if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
				{
					if (e->is_ready() && combo::use_e->get_bool() && combo::e_mode->get_int() == 0 && w->is_ready() && combo::use_w->get_bool())
					{
						e->cast(target, utils::get_hitchance(hitchance::e_hitchance));
						//myhero->print_chat(1, "EW");
					}
				}
			}
			if ((e->is_ready() && !w->is_ready() && w->cooldown_time() >= combo::reamin_cd->get_int()) || (!combo::use_w->get_bool()))
			{
				if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
				{
					e->cast(target, utils::get_hitchance(hitchance::e_hitchance));
					//myhero->print_chat(1, "combo E");
				}
			}
			if (e->is_ready() && combo::use_e->get_bool() && combo::e_mode->get_int() == 1 && combo::use_w->get_bool())
			{
				if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
				{
					if (target->has_buff(buff_hash("GalioWslow")))
					{
						e->cast(target, utils::get_hitchance(hitchance::e_hitchance));
						//myhero->print_chat(1, "WE");
					}

				}
			}
		}
	}
	void r_logic()
	{
		if (combo::use_r->get_bool() && combo::semi_r->get_bool())
		{
			if (!combo::semi_r->get_bool()) return;
			for (auto&& ally : entitylist->get_ally_heroes())
			{
				if (ally == nullptr || ally->is_dead() || !ally->is_targetable()) continue;
				if (ally->get_distance(myhero) > r->range()) continue;
				if (ally->is_me()) continue;

				if (utils::has_unkillable_buff(ally)) continue;
				if (!utils::enabled_in_map(combo::use_r_on, ally)) continue;
				if (ally->get_health_percent() > combo::ally_hp->get_int()) continue;
				if (ally->get_position().count_enemies_in_range(combo::enemy_distance->get_int()) == 0) continue;
				if (ally->get_position().count_enemies_in_range(combo::enemy_distance->get_int()) >= 1 && myhero->get_position().count_allys_in_range(r->range()) >= 1)
				{
					r->cast(ally);
					return;
				}
			}
		}
	}

	void on_process_spell_cast(game_object_script sender, spell_instance_script spell)
	{
		auto spell_hash = spell->get_spell_data()->get_name_hash();
		if (sender->is_me() && spell_hash == spell_hash("GalioW"))
		{
			last_w_time = gametime->get_time();
			w_charg_range = 175;
			//myhero->print_chat(1, "Test w cast");
		}
		if (sender->is_me() && spell_hash == spell_hash("GalioE"))
		{
			last_e_time = gametime->get_time();
		}
	}

	void on_after_attack_orbwalker(game_object_script target)
	{
		if (q->is_ready())
		{
			if (target->is_ai_hero() && (orbwalker->combo_mode() && combo::use_q->get_bool() && myhero->is_in_auto_attack_range(target) && myhero->get_buff(buff_hash("galiopassive"))))
			{
				if (q->cast(target, utils::get_hitchance(hitchance::q_hitchance)))
				{
					return;
				}
			}
		}
	}
	float calculate_q_damage(game_object_script enemy)
	{
		float q_raw_damage = q_damages[q->level() - 1] + (myhero->get_total_ability_power() * q_coef);
		float q_calculated_damage = damagelib->calculate_damage_on_unit(myhero, enemy, damage_type::magical, q_raw_damage);

		return q_calculated_damage;
	}
	float calculate_q_wind_damage(game_object_script enemy)
	{
		const float baseDamage = 0.1f;
		const float apRatio = 0.04f;
		const float maxHealth = enemy->get_max_health();
		const float apBonus = myhero->get_total_ability_power() / 100.0f * apRatio;
		const float totalDamage = maxHealth * (baseDamage + apBonus);

		float q_wind_calculated_damage = damagelib->calculate_damage_on_unit(myhero, enemy, damage_type::magical, totalDamage);

		return q_wind_calculated_damage;
	}
	float calculate_e_damage(game_object_script enemy)
	{
		float e_raw_damage = e_damages[e->level() - 1] + (myhero->get_total_ability_power() * e_coef);
		float e_calculated_damage = damagelib->calculate_damage_on_unit(myhero, enemy, damage_type::magical, e_raw_damage);

		return e_calculated_damage;
	}
	void draw_dmg_rl()
	{
		if (draw_settings::draw_damage_settings::draw_damage->get_bool())
		{
			for (auto& enemy : entitylist->get_enemy_heroes())
			{
				if (enemy->is_valid() && !enemy->is_dead() && enemy->is_hpbar_recently_rendered())
				{
					float damage = 0.0f;

					if (q->is_ready() && draw_settings::draw_damage_settings::q_damage->get_bool())
					{
						damage += (calculate_q_damage(enemy) + calculate_q_wind_damage(enemy));
					}

					if (e->is_ready() && draw_settings::draw_damage_settings::e_damage->get_bool())
						damage += calculate_e_damage(enemy);

					for (int i = 0; i < draw_settings::draw_damage_settings::aa_damage->get_int(); i++)
						damage += myhero->get_auto_attack_damage(enemy);

					if (damage != 0.0f)
						utils::draw_dmg_rl(enemy, damage, draw_settings::draw_transparency->get_color());
				}
			}
		}
		if (r->is_ready() && draw_settings::rMinimapRange->get_bool())
			draw_manager->draw_circle_on_minimap(myhero->get_position(), r->range(), draw_settings::r_color->get_color());
	}
	float dmg_can_do()
	{
		float total_dmg = 0.0f;
		for (auto& enemy : entitylist->get_enemy_heroes())
		{
			if (q->is_ready())
			{
				total_dmg += (calculate_q_damage(enemy) + calculate_q_wind_damage(enemy));
			}
			if (e->is_ready())
			{
				total_dmg += (calculate_e_damage(enemy));
			}
			if (orbwalker->can_attack())
			{
				total_dmg += myhero->get_auto_attack_damage(enemy);
			}
		}
		return total_dmg;
	}
	void on_draw()
	{

		if (myhero->is_dead())
		{
			return;
		}
		if (q->is_ready() && draw_settings::draw_range_q->get_bool())
		{
			draw_manager->add_circle_with_glow(myhero->get_position(), draw_settings::q_color->get_color(), q->range(), 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));
		}

		if (w->is_ready() && draw_settings::draw_range_w->get_bool())
			draw_manager->add_circle_with_glow(myhero->get_position(), draw_settings::w_color->get_color(), w->range(), 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));

		if (e->is_ready() && draw_settings::draw_range_e->get_bool())
			draw_manager->add_circle_with_glow(myhero->get_position(), draw_settings::e_color->get_color(), e->range(), 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));


		//draw_manager->draw_circle_on_minimap(myhero->get_position(), r_radius[r->level() - 1], r->range(), draw_settings::r_color->get_color());
		if (laneclear::spell_farm->get_bool())
		{
			auto pos = myhero->get_position() + vector(75, -40);
			renderer->world_to_screen(pos, pos);
			draw_manager->add_text_on_screen((pos), MAKE_COLOR(0, 255, 0, 255), 17, "Farm: on");
		}
		else
		{
			auto pos = myhero->get_position() + vector(75, -40);
			renderer->world_to_screen(pos, pos);
			draw_manager->add_text_on_screen((pos), MAKE_COLOR(255, 0, 0, 255), 17, "Farm: off");
		}
		if (combo::allow_tower_dive->get_bool())
		{
			auto pos = myhero->get_position() + vector(75, -80);
			renderer->world_to_screen(pos, pos);
			draw_manager->add_text_on_screen((pos), MAKE_COLOR(0, 255, 0, 255), 17, "Dive on");
		}
		else
		{
			auto pos = myhero->get_position() + vector(75, -80);
			renderer->world_to_screen(pos, pos);
			draw_manager->add_text_on_screen((pos), MAKE_COLOR(255, 0, 0, 255), 17, "Dive off");
		}

		if (myhero->has_buff(buff_hash("GalioW")))
		{
			auto t = fmaxf(0, gametime->get_time() - last_w_time - 0.0f); //0.0
			w_charg_range = 250 + (t * 150) < 470 ? 250 + (t * 150) : 450;
		}
		else
		{
			w_charg_range = 0;
		}
		if (myhero->get_position().count_enemies_in_range(w_charg_range) > 0)
		{
			draw_manager->add_circle_with_glow(myhero->get_position(), MAKE_COLOR(0, 255, 0, 255), w_charg_range, 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));
		}
		if (myhero->get_position().count_enemies_in_range(w_charg_range) == 0)
		{
			draw_manager->add_circle_with_glow(myhero->get_position(), MAKE_COLOR(0, 255, 0, 255), w_charg_range, 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));
		}
		if (combo::use_r->get_bool() && r->is_ready())
		{
			for (auto&& ally : entitylist->get_ally_heroes())
			{
				if (ally->is_me()) continue;
				if (ally->is_valid() && !ally->is_dead() && utils::enabled_in_map(combo::use_r_on, ally) && ally->get_health_percent() <= combo::ally_hp->get_int() && !utils::has_unkillable_buff(ally) && myhero->get_position().count_enemies_in_range(combo::enemy_distance->get_int()) >= 1 && ally->get_position().count_allys_in_range(r->range()) >= 1)
				{
					auto pos = myhero->get_position() + vector(draw_settings::x_pos->get_int(), draw_settings::y_pos->get_int());
					renderer->world_to_screen(pos, pos);
					draw_manager->add_text_on_screen((pos), MAKE_COLOR(0, 255, 0, 255), 30, "Press R to safe the world!!!");
				}
			}
		}
		if (draw_settings::draw_text->get_bool())
		{
			auto pos = myhero->get_position() + vector(draw_settings::x_pos->get_int(), draw_settings::y_pos->get_int());
			renderer->world_to_screen(pos, pos);
			draw_manager->add_text_on_screen((pos), MAKE_COLOR(0, 255, 0, 255), 30, "Press R to safe the world!!!");
		}
	}
}
