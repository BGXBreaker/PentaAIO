#include "../plugin_sdk/plugin_sdk.hpp"
#include "galio.h"
#include "utils.h"

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
		TreeEntry* e_mode = nullptr;
		TreeEntry* allow_tower_dive = nullptr;
		TreeEntry* use_everfrost = nullptr;
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
	}



	void on_update();
	void on_draw();
	void on_after_attack_orbwalker(game_object_script target);
	void on_process_spell_cast(game_object_script sender, spell_instance_script spell);

	void q_logic();
	void w_logic();
	void e_logic();

	float last_w_time;
	float w_charg_range;

	float r_radius[] = { 4000.0f, 4750.0f, 5500.0f };

	void load()
	{
		q = plugin_sdk->register_spell(spellslot::q, 825);
		q->set_skillshot(0.25f, 150.0f, 1400.0f, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_circle);
		w = plugin_sdk->register_spell(spellslot::w, 0);
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
				combo::use_e = combo->add_checkbox(myhero->get_model() + ".combo.e", "Use E", true);
				combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
				auto e_mode = combo->add_tab(myhero->get_model() + ".combo.e", "E mode");
				{
					combo::e_mode = combo->add_combobox(myhero->get_model() + ".combo.e.mode", "E Mode", { {"EW", nullptr},{"WE", nullptr } }, 0);
				}
				combo::allow_tower_dive = combo->add_hotkey(myhero->get_model() + ".combo.allow_tower_dive", "Allow Tower Dive", TreeHotkeyMode::Toggle, 'A', true);
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

				draw_settings::draw_range_w = draw_settings->add_checkbox(myhero->get_model() + ".drawingw", "Draw w range", true);
				draw_settings::draw_range_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
				draw_settings::w_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.w.color", "w Color", color_pink, true);

				draw_settings::draw_range_e = draw_settings->add_checkbox(myhero->get_model() + ".drawinge", "Draw e range", true);
				draw_settings::draw_range_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
				draw_settings::e_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.e.color", "e Color", color_yellow, true);

				draw_settings::rMinimapRange = draw_settings->add_checkbox(myhero->get_model() + ".drawingr", "Draw Mini map R range", true);
				draw_settings::rMinimapRange->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
				draw_settings::r_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.r.color", "R Mini map Color", color_blue, true);


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
			if (enemy->is_valid() && !enemy->is_dead() && enemy->get_distance(myhero->get_position()) <= 450)
			{
				if (myhero->has_buff(buff_hash("GalioW")))
				{
					if (myhero->get_position().count_enemies_in_range(w_charg_range) >= 1 && myhero->count_enemies_in_range(425) == 0)
					{
						myhero->update_charged_spell(w->get_slot(), myhero->get_position(), true);
					}
				}
				else
				{
					if (myhero->get_position().count_enemies_in_range(w->range()) >= 1)
						if (w->start_charging())
						{
							return;
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
			if (e->is_ready() && e->get_damage(target) + myhero->get_auto_attack_damage(target) > target->get_health())
			{
				if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
				{
					e->cast(target);
				}
			}

			if (myhero->get_distance(target) > myhero->get_attack_range())
			{
				if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
				{
					if (e->is_ready() && combo::use_e->get_bool() && combo::e_mode->get_int() == 0 && w->is_ready() && combo::use_w->get_bool())
					{
						e->cast(target);
					}
				}
			}
			if (e->is_ready() && !w->is_ready())
			{
				if (e->get_damage(target) + myhero->get_auto_attack_damage(target) > target->get_health())
				{
					if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
					{
						e->cast(target);
					}
				}
			}
			if (e->is_ready() && combo::use_e->get_bool() && combo::e_mode->get_int() == 1)
			{
				if (!myhero->is_under_enemy_turret() || combo::allow_tower_dive->get_bool())
				{
					if (target->has_buff(buff_hash("GalioWslow")))
					{
						(e->cast(target));
					}

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
	}

	void on_after_attack_orbwalker(game_object_script target)
	{
		//aqwae
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
	void on_draw()
	{

		if (myhero->is_dead())
		{
			return;
		}
		if (q->is_ready() && draw_settings::draw_range_q->get_bool())
			draw_manager->add_circle(myhero->get_position(), q->range(), draw_settings::q_color->get_color());

		if (w->is_ready() && draw_settings::draw_range_w->get_bool())
			draw_manager->add_circle(myhero->get_position(), w->range(), draw_settings::w_color->get_color());

		if (e->is_ready() && draw_settings::draw_range_e->get_bool())
			draw_manager->add_circle(myhero->get_position(), e->range(), draw_settings::e_color->get_color());

		if (r->is_ready() && draw_settings::rMinimapRange->get_bool())
			draw_manager->draw_circle_on_minimap(myhero->get_position(), r->range(), draw_settings::r_color->get_color());

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
			w_charg_range = 250 + (t * 150) < 470 ? 250 + (t * 150) : 470;
		}
		else
		{
			w_charg_range = 0;
		}
		if (myhero->get_position().count_enemies_in_range(w_charg_range) > 0)
		{
			draw_manager->add_circle(myhero->get_position(), w_charg_range, MAKE_COLOR(0, 255, 0, 255), 1.f);
		}
		if (myhero->get_position().count_enemies_in_range(w_charg_range) == 0)
		{
			draw_manager->add_circle(myhero->get_position(), w_charg_range, MAKE_COLOR(255, 0, 0, 255), 1.f);
		}



		if (draw_settings::draw_damage_settings::draw_damage->get_bool())
		{
			for (auto& enemy : entitylist->get_enemy_heroes())
			{
				if (enemy->is_valid() && !enemy->is_dead() && enemy->is_hpbar_recently_rendered())
				{
					float damage = 0.0f;


					if (q->is_ready() && draw_settings::draw_damage_settings::q_damage->get_bool())
						damage += q->get_damage(enemy);

					if (e->is_ready() && draw_settings::draw_damage_settings::e_damage->get_bool())
						damage += e->get_damage(enemy);

					for (int i = 0; i < draw_settings::draw_damage_settings::aa_damage->get_int(); i++)
						damage += myhero->get_auto_attack_damage(enemy);

					if (damage != 0.0f)
						utils::draw_dmg_rl(enemy, damage, 0x8000ff00);
				}
			}
		}

	}
}
