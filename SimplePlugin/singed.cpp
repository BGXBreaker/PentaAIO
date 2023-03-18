#include "singed.h"
#include "../plugin_sdk/plugin_sdk.hpp"
#include "utils.h"

namespace singed
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
        TreeEntry* use_q = nullptr;;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* semi_e = nullptr;
        TreeEntry* use_r = nullptr;
        TreeEntry* r_safe_slider = nullptr;
        TreeEntry* aa_block = nullptr;

        TreeEntry* r_use_on_low_hp = nullptr;
        TreeEntry* r_myhero_hp_under = nullptr;
        TreeEntry* r_only_when_enemies_nearby = nullptr;
        TreeEntry* r_hp_enemies_search_radius = nullptr;
        TreeEntry* r_calculate_incoming_damage = nullptr;
        TreeEntry* r_coming_damage_time = nullptr;

        TreeEntry* r_use_if_x_enemies_nearby = nullptr;
        TreeEntry* r_enemies_nearby_minimum = nullptr;
        TreeEntry* r_enemies_nearby_search_radius = nullptr;
    }

    namespace harass
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_e = nullptr;
    }

    namespace laneclear
    {
        TreeEntry* spell_farm = nullptr;
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
    }

    namespace jungleclear
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_w = nullptr;
    }

    namespace misc
    {
        TreeEntry* use_w_antigapcloser = nullptr;
        TreeEntry* use_w_antimelee = nullptr;
    }

    namespace hitchance
    {
        TreeEntry* q_hitchance = nullptr;
        TreeEntry* w_hitchance = nullptr;
        TreeEntry* e_hitchance = nullptr;
        TreeEntry* r_hitchance = nullptr;
    }

    void on_update();
    void on_draw();
    void on_gapcloser(game_object_script sender, antigapcloser::antigapcloser_args* args);

    void q_logic();
    void w_logic();
    void e_logic();
    void r_logic();

    hit_chance get_hitchance(TreeEntry* entry);

    void load()
    {
        q = plugin_sdk->register_spell(spellslot::q, myhero->get_attack_range() + 25);
        w = plugin_sdk->register_spell(spellslot::w, 1000);
        w->set_skillshot(0.25f, 265.0f, FLT_MAX, { }, skillshot_type::skillshot_circle);
        e = plugin_sdk->register_spell(spellslot::e, 135);
        r = plugin_sdk->register_spell(spellslot::r, 0);

        main_tab = menu->create_tab("singed", "Singed");
        main_tab->set_assigned_texture(myhero->get_square_icon_portrait());

        main_tab->add_separator(myhero->get_model() + ".aio", "PentaAIO : " + myhero->get_model());

        auto combo = main_tab->add_tab(myhero->get_model() + ".combo", "Combo Settings");
        {
            combo->add_separator("Combo Q", " Q Settings ");

            combo::use_q = combo->add_checkbox(myhero->get_model() + ".combo.q", "Use Q", true);
            combo::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());

            combo->add_separator("Combo W", " W Settings ");

            combo::use_w = combo->add_checkbox(myhero->get_model() + ".combo.w.use_w", "Use W", true);
            combo::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());

            combo->add_separator("Combo E", " E Settings ");

            combo::use_e = combo->add_checkbox(myhero->get_model() + ".combo.w.use_e", "Use E", true);
            combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());

            combo->add_separator("Combo R", " R Settings ");

            combo::use_r = combo->add_checkbox(myhero->get_model() + ".combo.r", "Use R", true);
            combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());

            auto r_config = combo->add_tab(myhero->get_model() + ".combo.r.config", "R Config");
            {
                r_config->add_separator(myhero->get_model() + ".combo.r.separator1", "R on Low HP Settings");

                combo::r_use_on_low_hp = r_config->add_checkbox(myhero->get_model() + ".combo.r.use_on_low_hp", "Use R on Low HP", true);
                combo::r_use_on_low_hp->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                combo::r_myhero_hp_under = r_config->add_slider(myhero->get_model() + ".combo.r.myhero_hp_under", "Myhero HP is under (in %)", 35, 0, 100);
                combo::r_only_when_enemies_nearby = r_config->add_checkbox(myhero->get_model() + ".combo.r.only_when_enemies_nearby", "Only when enemies are nearby", true);
                combo::r_hp_enemies_search_radius = r_config->add_slider(myhero->get_model() + ".combo.r.hp_enemies_search_radius", "Enemies nearby search radius", 350, 150, 1600);
                combo::r_calculate_incoming_damage = r_config->add_checkbox(myhero->get_model() + ".combo.r.calculate_incoming_damage", "Calculate incoming damage", true);
                combo::r_coming_damage_time = r_config->add_slider(myhero->get_model() + ".combo.r.coming_damage_time", "Set coming damage time (in ms)", 750, 0, 1000);

                r_config->add_separator(myhero->get_model() + ".combo.r.separator2", "R if enemies nearby Settings");

                combo::r_use_if_x_enemies_nearby = r_config->add_checkbox(myhero->get_model() + ".combo.r.use_if_x_enemies_nearby", "Use R if enemies nearby", true);
                combo::r_use_if_x_enemies_nearby->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                combo::r_enemies_nearby_minimum = r_config->add_slider(myhero->get_model() + ".combo.r.enemies_nearby_minimum", "Minimum enemies nearby", 2, 1, 5);
                combo::r_enemies_nearby_search_radius = r_config->add_slider(myhero->get_model() + ".combo.r.myhero_hp_under_enemies_search_radius", "Enemies nearby search radius", 250, 150, 1600);
            }
        }

        auto harass = main_tab->add_tab(myhero->get_model() + ".harass", "Harass Settings");
        {
            harass::use_q = harass->add_checkbox(myhero->get_model() + ".harass.q", "Use Q", true);
            harass::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
            harass::use_w = harass->add_checkbox(myhero->get_model() + ".harass.w", "Use W", true);
            harass::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
            harass::use_e = harass->add_checkbox(myhero->get_model() + ".harass.e", "Use E", true);
            harass::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
        }

        auto laneclear = main_tab->add_tab(myhero->get_model() + ".laneclear", "Lane Clear Settings");
        {
            laneclear::spell_farm = laneclear->add_hotkey(myhero->get_model() + ".laneclear.enabled", "Toggle Spell Farm", TreeHotkeyMode::Toggle, 0x04, true);
            laneclear::use_q = laneclear->add_checkbox(myhero->get_model() + ".laneclear.q", "Use Q", false);
            laneclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
            laneclear::use_w = laneclear->add_checkbox(myhero->get_model() + ".laneclear.w", "Use W", false);
            laneclear::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
        }

        auto jungleclear = main_tab->add_tab(myhero->get_model() + ".jungleclear", "Jungle Clear Settings");
        {
            jungleclear::use_q = jungleclear->add_checkbox(myhero->get_model() + ".jungleclear.q", "Use Q", true);
            jungleclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
            jungleclear::use_w = jungleclear->add_checkbox(myhero->get_model() + ".jungleclear.w", "Use W", true);
            jungleclear::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
        }

        auto hitchance = main_tab->add_tab(myhero->get_model() + ".hitchance", "Hitchance Settings");
        {
            hitchance::w_hitchance = hitchance->add_combobox(myhero->get_model() + ".hitchance.e", "Hitchance E", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
        }

        auto misc = main_tab->add_tab(myhero->get_model() + ".misc", "Miscellaneous Settings");
        {
            misc::use_w_antigapcloser = misc->add_checkbox(myhero->get_model() + ".misc.use_w_antigapcloser", "W Anti-Gapcloser", false);
        }

        auto draw_settings = main_tab->add_tab(myhero->get_model() + ".draw", "Drawings Settings");
        {
            float color[] = { 0.0f, 1.0f, 1.0f, 1.0f };
            draw_settings::draw_range_q = draw_settings->add_checkbox(myhero->get_model() + ".draw.q", "Draw Q range", true);
            draw_settings::draw_range_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
            draw_settings::q_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.q.color", "Q Color", color);
            draw_settings::draw_range_w = draw_settings->add_checkbox(myhero->get_model() + ".draw.w", "Draw W range", true);
            draw_settings::draw_range_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
            draw_settings::w_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.w.color", "W Color", color);
            draw_settings::draw_range_e = draw_settings->add_checkbox(myhero->get_model() + ".draw.e", "Draw E range", true);
            draw_settings::draw_range_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
            draw_settings::e_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.e.color", "E Color", color);
            draw_settings::draw_range_r = draw_settings->add_checkbox(myhero->get_model() + ".draw.r", "Draw R range", true);
            draw_settings::draw_range_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
            draw_settings::r_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.r.color", "R Color", color);
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
                if (w->is_ready() && harass::use_w->get_bool())
                {
                    w_logic();
                }
                if (e->is_ready() && harass::use_e->get_bool())
                {
                    e_logic();
                }
            }
        }


        if (orbwalker->lane_clear_mode() && laneclear::spell_farm->get_bool())
        {
            auto lane_minions = entitylist->get_enemy_minions();

            auto monsters = entitylist->get_jugnle_mobs_minions();

            lane_minions.erase(std::remove_if(lane_minions.begin(), lane_minions.end(), [](game_object_script x)
                {
                    return !x->is_valid_target(q->range());
                }), lane_minions.end());

            monsters.erase(std::remove_if(monsters.begin(), monsters.end(), [](game_object_script x)
                {
                    return !x->is_valid_target(q->range());
                }), monsters.end());

            std::sort(lane_minions.begin(), lane_minions.end(), [](game_object_script a, game_object_script b)
                {
                    return a->get_position().distance(myhero->get_position()) < b->get_position().distance(myhero->get_position());
                });

            std::sort(monsters.begin(), monsters.end(), [](game_object_script a, game_object_script b)
                {
                    return a->get_max_health() > b->get_max_health();
                });

            if (!lane_minions.empty())
            {
                for (auto& minion : lane_minions)
                {
                    if (laneclear::use_q->get_bool())
                    {
                        if (utils::count_minions_in_range(minion, w->range()) >= 1)
                        {
                            if (q->toogle_state() == 1) q->cast();
                        }
                        else
                        {
                            if (q->toogle_state() == 2) q->cast();
                        }
                    }

                    if (w->is_ready() && laneclear::use_w->get_bool())
                    {
                        if (w->cast_on_best_farm_position(1))
                            return;
                    }
                }
            }

            if (!monsters.empty())
            {
                for (auto& monster : monsters)
                {
                    if (jungleclear::use_q->get_bool())
                    {
                        if (utils::count_monsters_in_range(monster, w->range()) >= 1)
                        {
                            if (q->toogle_state() == 1) q->cast();
                        }
                        else
                        {
                            if (q->toogle_state() == 2) q->cast();
                        }
                    }

                    if (jungleclear::use_w->get_bool() && w->is_ready())
                    {
                        if (monster->is_valid() && monster->get_distance(myhero->get_position()) <= w->range())
                        {
                            auto pred = w->get_prediction(monster);
                            if (pred.hitchance >= hit_chance::low)
                            {
                                if (w->cast(pred.get_cast_position()))
                                {
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

#pragma region q_logic
    void q_logic()
    {
        for (auto& enemy : entitylist->get_enemy_heroes())
        {
            if (combo::use_q->get_bool())
            {
                if (enemy->is_valid() && enemy->get_distance(myhero->get_position()) <= w->range() + 50)
                {
                    if (q->toogle_state() == 1)
                    {
                        q->cast();
                    }
                }
                else
                {
                    if (q->toogle_state() == 2)
                    {
                        q->cast();
                    }
                }

            }
        }
    }
#pragma endregion

    void w_logic()
    {
        if (combo::use_w->get_bool())
        {
            for (auto& enemy : entitylist->get_enemy_heroes())
            {
                if (enemy->is_valid_target(w->range()) && !enemy->is_dead())
                {
                    if (enemy->get_distance(myhero->get_position()) <= w->range())
                    {
                        auto pred = w->get_prediction(enemy);
                        if (pred.hitchance >= get_hitchance(hitchance::w_hitchance))
                        {
                            if (w->cast(pred.get_cast_position()))
                            {
                                if (e->is_ready() && !should_block_e_when_w())
                                {
                                    e->cast(enemy);
                                }
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
    bool should_block_e_when_w()
    {
        // spell block
        auto target = target_selector->get_target(e->range(), damage_type::magical);
        //no to e target out of w
        if (target->has_buff(buff_hash("SingedW")))//buff name shoulf be incorrect
        {
            return true;
        }
        return false;
    }

#pragma region e_logic

    void e_logic()
    {
        if (combo::use_e->get_bool())
        {
            for (auto& enemy : entitylist->get_enemy_heroes())
            {
                if (enemy->is_valid_target(e->range()) && !enemy->is_dead())
                {
                    if (enemy->get_distance(myhero->get_position()) <= e->range())
                    {
                        e->cast(enemy);
                    }
                }
            }
        }

    }

#pragma endregion

#pragma region r_logic
void r_logic()
{
    if (!utils::has_unkillable_buff(myhero))
    {
        if (combo::r_use_on_low_hp->get_bool())
        {
            if ((!combo::r_only_when_enemies_nearby->get_bool() || myhero->count_enemies_in_range(combo::r_hp_enemies_search_radius->get_int()) != 0))
            {
                if ((myhero->get_health_percent() < combo::r_myhero_hp_under->get_int()) || (combo::r_calculate_incoming_damage->get_bool() && health_prediction->get_incoming_damage(myhero, combo::r_coming_damage_time->get_int() / 1000.0f, true) >= myhero->get_health()))
                {
                    if (r->cast())
                    {
                        return;
                    }
                }
            }
        }

        if (combo::r_use_if_x_enemies_nearby->get_bool())
        {
            if (myhero->count_enemies_in_range(combo::r_enemies_nearby_search_radius->get_int()) >= combo::r_enemies_nearby_minimum->get_int())
            {
                r->cast();
            }
        }
    }
}
#pragma endregion

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
        if (misc::use_w_antigapcloser->get_bool())
        {
            if (sender->is_valid_target(e->range() + sender->get_bounding_radius()))
            {
                w->cast(sender);
            }
        }
    }

    

    float get_damage(game_object_script target)
    {
        float damage = 0.0f;

        if (q->is_ready())
            damage += q->get_damage(target);

        if (w->is_ready())
            damage += w->get_damage(target);

        if (e->is_ready())
            damage += e->get_damage(target);

        if (r->is_ready())
            damage += r->get_damage(target);

        damage += myhero->get_auto_attack_damage(target);
        return damage;
    }

    void on_draw()
    {
        if (myhero->is_dead())
        {
            return;
        }

        if (w->is_ready() && draw_settings::draw_range_w->get_bool())
            draw_manager->add_circle(myhero->get_position(), w->range(), draw_settings::w_color->get_color());

        if (e->is_ready() && draw_settings::draw_range_e->get_bool())
            draw_manager->add_circle(myhero->get_position(), e->range(), draw_settings::e_color->get_color());

        if (r->is_ready() && draw_settings::draw_range_r->get_bool())
            draw_manager->add_circle(myhero->get_position(), r->range(), draw_settings::r_color->get_color());
    }
}