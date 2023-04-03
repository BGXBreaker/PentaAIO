#include "../plugin_sdk/plugin_sdk.hpp"
#include "milio.h"
#include "utils.h"
#include "SpellDB.h"

namespace milio
{
	script_spell* q = nullptr;
	script_spell* w = nullptr;
	script_spell* e = nullptr;
	script_spell* r = nullptr;

	TreeTab* main_tab = nullptr;

	namespace draw_settings
	{
		TreeEntry* draw_range_q = nullptr;
		TreeEntry* q_color = nullptr;
		TreeEntry* draw_range_w = nullptr;
		TreeEntry* w_color = nullptr;
		TreeEntry* draw_range_e = nullptr;
		TreeEntry* e_color = nullptr;
		TreeEntry* draw_range_r = nullptr;
		TreeEntry* r_color = nullptr;
	}
	namespace combo
	{
		TreeEntry* use_q = nullptr;
		TreeEntry* q_mode = nullptr;
		TreeEntry* use_w = nullptr;
		TreeEntry* out_range_w = nullptr;
		TreeEntry* w_if_ally_hp_below = nullptr;
		TreeEntry* use_e = nullptr;
		TreeEntry* e_myhero_hp_under = nullptr;
		TreeEntry* e_ally_hp_under = nullptr;
		TreeEntry* e_only_when_enemies_nearby = nullptr;
		TreeEntry* e_enemies_search_radius = nullptr;
		TreeEntry* e_calculate_incoming_damage = nullptr;
		TreeEntry* e_coming_damage_time = nullptr;
		std::map<std::uint32_t, TreeEntry*> e_use_on;
		TreeEntry* use_r = nullptr;
		TreeEntry* r_if_ally_nearby = nullptr;
		TreeEntry* r_if_ally_get_cc = nullptr;
		TreeEntry* r_if_ally_hp_below = nullptr;
		std::map<std::uint32_t, TreeEntry*> r_use_on;
	}
	namespace harass
	{
		TreeEntry* use_q = nullptr;
	}
	namespace misc
	{
		TreeEntry* use_q = nullptr;
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
	void on_gapcloser(game_object_script sender, antigapcloser::antigapcloser_args* args);

	void q_logic();
	void w_logic();
	void e_logic();
	void r_logic();

	float attack_range_buff[] = { 1.1f, 1.125f, 1.15f, 1.175f, 1.2f };
	bool can_use_e_on(game_object_script target);
	bool can_use_r_on(game_object_script target);
	hit_chance get_hitchance(TreeEntry* entry);

	void load()
	{
		q = plugin_sdk->register_spell(spellslot::q, 1000);
		q->set_skillshot(0.0f, 30.0f, 1200.0f, { }, skillshot_type::skillshot_line);
		w = plugin_sdk->register_spell(spellslot::w, 650);
		w->set_skillshot(0.25f, 500.0f, FLT_MAX, { }, skillshot_type::skillshot_circle);
		e = plugin_sdk->register_spell(spellslot::e, 650);
		r = plugin_sdk->register_spell(spellslot::r, 1400);

		q->set_spell_lock(false);
		w->set_spell_lock(false);
		e->set_spell_lock(false);
		r->set_spell_lock(false);

		myhero->print_chat(0x3, "<font color=\"#FFFFFF\">[<b><font color=\"#3F704D\">Milio | PentaAIO</font></b>]</font><font color=\"#3F704D\">:</font><font color=\"#90EE90\"> Loaded</font>");
		myhero->print_chat(0x3, "<font color=\"#3F704D\"><b>Suggested Prediction: </b><font color=\"#90EE90\">Aurora</font></font>");
		myhero->print_chat(0x3, "<font color=\"#3F704D\"><b> for settings 0=disable");
		myhero->print_chat(0x3, "<font color=\"#3F704D\"><b> any bugs or suggestion find me in DC :GameBreaker#3051 ");
		main_tab = menu->create_tab("milio", "Milio");
		main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
		{
			main_tab->add_separator(myhero->get_model() + ".aio", "PentaAIO : " + myhero->get_model());
			auto combo = main_tab->add_tab(myhero->get_model() + ".combo", "Combo Settings");
			{
				combo::use_q = combo->add_checkbox(myhero->get_model() + ".combo.q", "Use Q", true);
				combo::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
			}
			combo::use_w = combo->add_checkbox(myhero->get_model() + ".combo.w", "Use W", true);
			auto w_config = combo->add_tab(myhero->get_model() + ".combo.w.settings", "W settings");
			{
				combo::out_range_w = w_config->add_checkbox(myhero->get_model() + ".out.range.w", "Use W only out of aa range", true);
				combo::w_if_ally_hp_below = w_config->add_slider(myhero->get_model() + ".w.hp.below", "Use W when hp below (in %)", 45, 1, 100);
			}
			combo::use_e = combo->add_checkbox(myhero->get_model() + ".combo.e", "Use E", true);
			combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
			auto e_config = combo->add_tab(myhero->get_model() + ".combo.e.settings", "E Settings");
			{
				combo::e_myhero_hp_under = e_config->add_slider(myhero->get_model() + ".combo.e.myhero_hp_under", "Myhero HP is under (in %)", 50, 0, 100);
				combo::e_ally_hp_under = e_config->add_slider(myhero->get_model() + ".combo.e.ally_hp_under", "Ally HP is under (in %)", 75, 0, 100);
				combo::e_only_when_enemies_nearby = e_config->add_checkbox(myhero->get_model() + ".combo.e.only_when_enemies_nearby", "Only when enemies are nearby", true);
				combo::e_enemies_search_radius = e_config->add_slider(myhero->get_model() + ".combo.e.enemies_search_radius", "Enemies nearby search radius", 900, 300, 1600);
				combo::e_calculate_incoming_damage = e_config->add_checkbox(myhero->get_model() + ".combo.e.calculate_incoming_damage", "Calculate incoming damage", true);
				combo::e_coming_damage_time = e_config->add_slider(myhero->get_model() + ".combo.e.coming_damage_time", "Set coming damage time (in ms)", 1000, 0, 1000);

				auto use_e_on_tab = e_config->add_tab(myhero->get_model() + ".combo.e.use_on", "Use E on");
				{
					for (auto&& ally : entitylist->get_ally_heroes())
					{
						combo::e_use_on[ally->get_network_id()] = use_e_on_tab->add_checkbox(std::to_string(ally->get_network_id()), ally->get_model(), true, false);
						combo::e_use_on[ally->get_network_id()]->set_texture(ally->get_square_icon_portrait());
					}
				}
			}
			combo::use_r = combo->add_checkbox(myhero->get_model() + ".combo.r", "Use R", true);
			combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
			auto r_config = combo->add_tab(myhero->get_model() + ".combo.r.config", "R Settings");
			{
				combo::r_if_ally_nearby = r_config->add_slider(myhero->get_model() + ".r.ally.nearby", "Use r only if X ally nearby", 1, 0, 5);
				combo::r_if_ally_get_cc = r_config->add_slider(myhero->get_model() + ".r.ally.get.cc", "Use r only if X ally get CC", 1, 0, 5);
				combo::r_if_ally_hp_below = r_config->add_slider(myhero->get_model() + ".r.hp.below", "Use r when hp below (in %)", 30, 0, 100);

				auto use_r_on_tab = r_config->add_tab(myhero->get_model() + ".combo.r.use_on", "Use R On");
				{
					for (auto&& ally : entitylist->get_ally_heroes())
					{
						combo::r_use_on[ally->get_network_id()] = use_r_on_tab->add_checkbox(std::to_string(ally->get_network_id()), ally->get_model(), true, false);
						combo::r_use_on[ally->get_network_id()]->set_texture(ally->get_square_icon_portrait());
					}
				}
			}
		}
		auto harass = main_tab->add_tab(myhero->get_model() + ".harass", "Harass Settings");
		{
			harass::use_q = harass->add_checkbox(myhero->get_model() + ".harass.q", "Use Q", true);
			harass::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
		}
		auto misc = main_tab->add_tab(myhero->get_model() + ".antigapclose", "Anti Gapclose");
		{
			misc::use_q = misc->add_checkbox(myhero->get_model() + ".misc", "Use Q", true);
			misc::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
		}
		auto hitchance = main_tab->add_tab(myhero->get_model() + ".hitchance", "Hitchance Settings");
		{
			hitchance::q_hitchance = hitchance->add_combobox(myhero->get_model() + ".hitchance.q", "Hitchance Q", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
		}
		auto fleemode = main_tab->add_tab(myhero->get_model() + ".fleemode", "Flee Mode");
		{
			fleemode::use_e = fleemode->add_checkbox(myhero->get_model() + ".flee.e", "Use E", true);
			fleemode::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
		}
		auto draw_settings = main_tab->add_tab(myhero->get_model() + ".drawings", "Drawings Settings");
		{
			float color_red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
			float color_yellow[] = { 1.0f, 1.0f, 0.0f, 1.0f };
			float color_blue[] = { 0.0f, 0.4f, 1.0f, 1.0f };
			float color_dark_blue[] = { 0.4f, 0.3f, 1.0f, 1.0f };

			draw_settings::draw_range_q = draw_settings->add_checkbox(myhero->get_model() + ".drawingQ", "Draw Q range", true);
			draw_settings::draw_range_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
			draw_settings::q_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.q.color", "Q Color", color_red, true);

			draw_settings::draw_range_w = draw_settings->add_checkbox(myhero->get_model() + ".drawingw", "Draw W range", true);
			draw_settings::draw_range_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
			draw_settings::w_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.w.color", "W Color", color_blue, true);

			draw_settings::draw_range_e = draw_settings->add_checkbox(myhero->get_model() + ".drawinge", "Draw E range", true);
			draw_settings::draw_range_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
			draw_settings::e_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.e.color", "E Color", color_yellow, true);

			draw_settings::draw_range_r = draw_settings->add_checkbox(myhero->get_model() + ".drawingr", "Draw R range", true);
			draw_settings::draw_range_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
			draw_settings::r_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.r.color", "R Color", color_dark_blue, true);
		}
		antigapcloser::add_event_handler(on_gapcloser);

		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_env_draw>::add_callback(on_draw);
	}

	void unload()
	{
		plugin_sdk->remove_spell(q);
		plugin_sdk->remove_spell(w);
		plugin_sdk->remove_spell(e);
		plugin_sdk->remove_spell(r);

		menu->delete_tab(main_tab);

		antigapcloser::remove_event_handler(on_gapcloser);
		event_handler<events::on_update>::remove_handler(on_update);
		event_handler<events::on_env_draw>::remove_handler(on_draw);
	}
	void on_update()
	{
		if (myhero->is_dead())
		{
			return;
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
			if (r->is_ready() && combo::use_r->get_bool())
			{
				r_logic();
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
			if (e->is_ready() && fleemode::use_e->get_bool())
			{
				e->cast(myhero);
			}
		}
	}

#pragma region can_use_e_on
	bool can_use_e_on(game_object_script ally)
	{
		auto it = combo::e_use_on.find(ally->get_network_id());
		if (it == combo::e_use_on.end())
			return false;

		return it->second->get_bool();
	}
#pragma endregion

#pragma region can_use_r_on
	bool can_use_r_on(game_object_script ally)
	{
		auto it = combo::r_use_on.find(ally->get_network_id());
		if (it == combo::r_use_on.end())
			return false;

		return it->second->get_bool();
	}
#pragma endregion

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
		float attack_range_increase = attack_range_buff[w->level() - 1];

		if (combo::out_range_w->get_bool())
		{
			for (auto& enemy : entitylist->get_enemy_heroes())
			{
				if (enemy->is_valid() && !enemy->is_dead() && enemy->get_distance(myhero->get_position()) > myhero->get_attack_range() && (myhero->get_attack_range() * attack_range_increase >= enemy->get_distance(myhero->get_position())))
				{
					w->cast(myhero);
				}
				else return;
			}
		}
		if ((myhero->get_health_percent() < combo::w_if_ally_hp_below->get_int()))
		{
			w->cast(myhero);
		}
	}
#pragma endregion

#pragma region e_logic
	void e_logic()
	{
		for (auto&& ally : entitylist->get_ally_heroes())
		{
			if (ally->is_valid() && !ally->is_dead() && can_use_e_on(ally) && ally->get_distance(myhero->get_position()) <= e->range())
			{
				if (!utils::has_unkillable_buff(ally) && (!combo::e_only_when_enemies_nearby->get_bool() || ally->count_enemies_in_range(combo::e_enemies_search_radius->get_int()) != 0))
				{
					if ((ally->get_health_percent() < (ally->is_me() ? combo::e_myhero_hp_under->get_int() : combo::e_ally_hp_under->get_int())) || (combo::e_calculate_incoming_damage->get_bool() && health_prediction->get_incoming_damage(ally, combo::e_coming_damage_time->get_int() / 1000.0f, true) >= ally->get_health()))
					{
						if (e->cast(ally))
						{
							return;
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma region r_logic 
	void r_logic()
	{
		for (auto&& ally : entitylist->get_ally_heroes())
		{
			if (ally->is_valid() && !ally->is_dead() && can_use_r_on(ally) && ally->get_distance(myhero->get_position()) <= r->range())
			{
				if (!utils::has_unkillable_buff(ally))
				{
					if ((ally->get_health_percent() < (ally->is_me() ? combo::r_if_ally_hp_below->get_int() : myhero->get_position().count_allys_in_range(r->range()) >= combo::r_if_ally_nearby->get_int() && utils::has_crowd_control_buff(ally))))
					{
						if (r->cast())
						{
							return;
						}
					}
				}
			}
		}
	}
#pragma endregion

#pragma region get_hitchance
	hit_chance get_hitchance(TreeEntry* entry)
	{
		switch (entry->get_int())
		{
		case 0:
			return hit_chance::low;
		case 1:
			return hit_chance::medium;
		case 2:
			return hit_chance::high;
		case 3:
			return hit_chance::very_high;
		}
		return hit_chance::medium;
	}
#pragma endregion

	void on_gapcloser(game_object_script sender, antigapcloser::antigapcloser_args* args)
	{
		if (misc::use_q->get_bool())
		{
			if (sender->is_valid_target(q->range() + sender->get_bounding_radius()))
			{
				q->cast(sender);
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

		if (r->is_ready() && draw_settings::draw_range_r->get_bool())
			draw_manager->add_circle(myhero->get_position(), r->range(), draw_settings::r_color->get_color());
	}
}
