#include "../plugin_sdk/plugin_sdk.hpp"
#include "leesin.h"
#include "utils.h"
#include "permashow.h"

namespace leesin
{
    script_spell* q = nullptr;
    script_spell* w = nullptr;
    script_spell* e = nullptr;
    script_spell* r = nullptr;
    script_spell* ward = nullptr;
    script_spell* flash = nullptr;

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
        TreeEntry* Draw_Damage = nullptr;
        TreeEntry* Draw_Damage_color = nullptr;
        TreeEntry* Draw_DMG_rl = nullptr;
        TreeEntry* draw_transparency = nullptr;

        namespace draw_damage_settings
        {
            TreeEntry* draw_damage = nullptr;
            TreeEntry* q_damage = nullptr;
            TreeEntry* e_damage = nullptr;
            TreeEntry* r_damage = nullptr;
        }
    }
    namespace combo
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_q2 = nullptr;
        TreeEntry* q2_if_target_is_under_turret = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_w2 = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_e2 = nullptr;
        TreeEntry* use_r = nullptr;
    }
    namespace harass
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_q2 = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_e2 = nullptr;
    }
    namespace laneclear
    {
        TreeEntry* spell_farm = nullptr;
        TreeEntry* use_q = nullptr;
        TreeEntry* use_q2 = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_e2 = nullptr;
        TreeEntry* e_use_if_hit_minions = nullptr;
    }
    namespace jungleclear
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_q2 = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* use_w2 = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_e2 = nullptr;
    }
    namespace fleemode
    {
        TreeEntry* use_w = nullptr;
        TreeEntry* w_jump_on_ally_champions = nullptr;
        TreeEntry* w_jump_on_ally_minions = nullptr;
        TreeEntry* w_ward_jump = nullptr;
        TreeEntry* ward_jump = nullptr;
    }
    namespace insec
    {
        TreeEntry* use_flash = nullptr;
        TreeEntry* wait_q = nullptr;
        TreeEntry* q_other = nullptr;
    }
    namespace hitchance
    {
        TreeEntry* q_hitchance = nullptr;
    }
    void on_update();
    void on_draw();
    void q_logic();
    void w_logic();
    void e_logic();
    void r_logic();
    /*void ward_jump_logic();
    void insec_logic();*/
    void on_process_spell_cast(game_object_script sender, spell_instance_script spell);
    hit_chance get_hitchance(TreeEntry* entry);
    inline void draw_dmg_rl(game_object_script target, float damage, unsigned long color);
    float last_Q_time;
    float last_W_time;
    float last_E_time;
    bool q_active = myhero->has_buff(buff_hash("blindmonkqmanager"));
    bool w_active = myhero->has_buff(buff_hash("blindmonkwmanager"));
    bool e_active = myhero->has_buff(buff_hash("blindmonkemanager"));
    float calculate_q1_damage(game_object_script target);
    float calculate_q2_damage(game_object_script target);
    float calculate_e_damage(game_object_script target);
    float calculate_r_damage(game_object_script target);

    std::vector<float> q1_damages = { 55,80,105,130,155 };
    float q1_coef = 1.1;

    std::vector<float> q2_damages = { 110,160,210,260,310 };
    float q2_coef = 2.2;

    std::vector<float> e_damages = { 35,65,95,125,155 };
    float e_coef = 1.0;

    std::vector<float> r_damages = { 175,400,625 };
    float r_coef = 2.0;


    void load()
    {
        q = plugin_sdk->register_spell(spellslot::q, 1200);
        q->set_skillshot(0.25f, 120.0f, 1800.0f, { collisionable_objects::minions, collisionable_objects::yasuo_wall, collisionable_objects::heroes }, skillshot_type::skillshot_line);
        w = plugin_sdk->register_spell(spellslot::w, 700);
        e = plugin_sdk->register_spell(spellslot::e, 450);
        r = plugin_sdk->register_spell(spellslot::r, 375);
        ward = plugin_sdk->register_spell(spellslot::trinket, 600);

        q->set_spell_lock(false);
        w->set_spell_lock(false);
        e->set_spell_lock(false);
        r->set_spell_lock(false);

        if (myhero->get_spell(spellslot::summoner1)->get_spell_data()->get_name_hash() == spell_hash("SummonerFlash"))
            flash = plugin_sdk->register_spell(spellslot::summoner1, 400.f);
        else if (myhero->get_spell(spellslot::summoner2)->get_spell_data()->get_name_hash() == spell_hash("SummonerFlash"))
            flash = plugin_sdk->register_spell(spellslot::summoner2, 400.f);

        main_tab = menu->create_tab("", "Leesin");
        main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
        {
            main_tab->add_separator(myhero->get_model() + "", "PentaAIO : " + myhero->get_model());
            auto hitchance = main_tab->add_tab(myhero->get_model() + "", "Hitchance Settings");
            {
                hitchance::q_hitchance = hitchance->add_combobox(myhero->get_model() + "", "Hitchance Q", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
            }
            auto draw_settings = main_tab->add_tab(myhero->get_model() + ".drawings", "Drawings Settings");
            {
                float color_red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
                float color_blue[] = { 0.0f, 0.4f, 1.0f, 1.0f };
                float color_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float color_dark_blue[] = { 0.4f, 0.3f, 1.0f, 1.0f };
                float color11[] = { 0.5f, 0.0f, 0.9f, 1.0f };

                draw_settings::draw_range_q = draw_settings->add_checkbox(myhero->get_model() + "", "Draw Q range", true);
                draw_settings::draw_range_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                draw_settings::q_color = draw_settings->add_colorpick(myhero->get_model() + "", "Q Color", color_red, true);

                draw_settings::draw_range_w = draw_settings->add_checkbox(myhero->get_model() + "", "Draw W range", true);
                draw_settings::draw_range_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                draw_settings::w_color = draw_settings->add_colorpick(myhero->get_model() + "", "w Color", color_blue, true);

                draw_settings::draw_range_e = draw_settings->add_checkbox(myhero->get_model() + "", "Draw E range", true);
                draw_settings::e_color = draw_settings->add_colorpick(myhero->get_model() + "", "e Color", color_dark_blue, true);

                draw_settings::draw_range_r = draw_settings->add_checkbox(myhero->get_model() + "", "Draw R range", true);
                draw_settings::draw_range_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                draw_settings::r_color = draw_settings->add_colorpick(myhero->get_model() + "", "R Color", color11, true);


                auto draw_damage = draw_settings->add_tab(myhero->get_model() + "drawdamage", "Draw Damage");
                {
                    draw_settings::draw_damage_settings::draw_damage = draw_damage->add_checkbox(myhero->get_model() + "", "Draw Combo Damage", true);
                    draw_settings::draw_transparency = draw_settings->add_colorpick("color", "Draw Damage color", color_green, true);
                    draw_settings::draw_damage_settings::q_damage = draw_damage->add_checkbox(myhero->get_model() + "", "Draw Q Damage", true);
                    draw_settings::draw_damage_settings::q_damage->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                    draw_settings::draw_damage_settings::e_damage = draw_damage->add_checkbox(myhero->get_model() + "", "Draw E Damage", true);
                    draw_settings::draw_damage_settings::e_damage->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                    draw_settings::draw_damage_settings::r_damage = draw_damage->add_checkbox(myhero->get_model() + "", "Draw R Damage", true);
                    draw_settings::draw_damage_settings::r_damage->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                }
            }
        }
        {
            Permashow::Instance.Init(main_tab);
            Permashow::Instance.AddElement("Spell Farm", laneclear::spell_farm);
            Permashow::Instance.AddElement("Q2 under turret", combo::q2_if_target_is_under_turret);
        }
        event_handler<events::on_update>::add_callback(on_update);
        event_handler<events::on_draw>::add_callback(on_draw);
        event_handler<events::on_process_spell_cast>::add_callback(on_process_spell_cast);
    }
    void unload()
    {
        plugin_sdk->remove_spell(q);
        plugin_sdk->remove_spell(w);
        plugin_sdk->remove_spell(e);
        plugin_sdk->remove_spell(r);
        if (flash)
            plugin_sdk->remove_spell(flash);

        menu->delete_tab(main_tab);
        Permashow::Instance.Destroy();

        event_handler<events::on_update>::remove_handler(on_update);
        event_handler<events::on_draw>::remove_handler(on_draw);
        event_handler<events::on_process_spell_cast>::remove_handler(on_process_spell_cast);

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
            std::sort(monsters.begin(), monsters.end(), [](game_object_script a, game_object_script b)
                {
                    return a->get_max_health() > b->get_max_health();
                });

            if (!lane_minions.empty())
            {
                if ((laneclear::use_q->get_bool() && !q_active && last_Q_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                {
                    if (q->cast(monsters.front()->get_position()))
                    {
                        return;
                    }
                }
                if (laneclear::use_q2->get_bool() && q_active)
                {
                    if ((last_Q_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                    {
                        if (q->cast())
                        {
                            return;
                        }
                    }
                }

                if ((laneclear::use_e->get_bool() && !e_active && last_E_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                {
                    if (e->cast_on_best_farm_position(laneclear::e_use_if_hit_minions->get_int()))
                    {
                        return;
                    }
                }
                if (laneclear::use_e2->get_bool() && e_active)
                {
                    if ((last_E_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                    {
                        if (e->cast())
                        {
                            return;
                        }
                    }
                }
            }
            if (!monsters.empty())
            {
                if ((jungleclear::use_q->get_bool() && !q_active && last_Q_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                {
                    if (q->cast(monsters.front()->get_position()))
                    {
                        return;
                    }
                }
                if (jungleclear::use_q2->get_bool() && q_active)
                {
                    if (last_Q_time + 3.0 < gametime->get_time() || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                    {
                        if (q->cast())
                        {
                            return;
                        }
                    }
                }
                if ((jungleclear::use_w->get_bool() && !w_active && last_W_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                {
                    if (w->cast(myhero))
                    {
                        return;
                    }
                }
                if (jungleclear::use_w2->get_bool() && w_active)
                {
                    if ((last_W_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                    {
                        if (w->cast())
                        {
                            return;
                        }
                    }
                }
                if ((jungleclear::use_e->get_bool() && !e_active && last_E_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                {
                    if (e->cast(monsters.front()->get_position()))
                    {
                        return;
                    }
                }
                if (jungleclear::use_e2->get_bool() && e_active)
                {
                    if ((last_E_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                    {
                        if (e->cast())
                        {
                            return;
                        }
                    }
                }
            }
        }
    }

#pragma region q_logic
    void q_logic()
    {
        auto target = target_selector->get_target(q->range(), damage_type::physical);
        if (target != nullptr)
        {
            if (combo::use_q->get_bool() && !q_active && last_Q_time + 3.0 < gametime->get_time())
            {
                if (q->cast(target, utils::get_hitchance(hitchance::q_hitchance)))
                {
                    return;
                }
            }
            if (combo::use_q2->get_bool() && q_active)
            {
                if (last_Q_time + 3.0 < gametime->get_time())
                {
                    if (q->cast())
                    {
                        return;
                    }
                }
            }
        }
    }
#pragma endregion

    #pragma region w_logic
        void w_logic()
        {
            auto target = target_selector->get_target(myhero->get_attackRange(), damage_type::magical);
            if ((combo::use_w->get_bool() && !w_active && last_W_time + 3.0 < gametime->get_time()) || !myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
            {
                if (w->cast(myhero))
                {
                    return;
                }
            }
            if (combo::use_w2->get_bool() && w_active)
            {
                if (last_W_time + 3.0 < gametime->get_time() && myhero->get_position().count_enemies_in_range(myhero->get_attackRange()) >= 1)
                {
                    w->cast();
                }
                else if (!myhero->has_buff(buff_hash("blindmonkpassive_cosemtic")))
                {
                    w->cast();
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
                if (combo::use_e->get_bool() && !e_active && last_E_time + 3.0 < gametime->get_time())
                {
                    if (e->cast())
                    {
                        return;
                    }
                }
                if (combo::use_e2->get_bool() && e_active)
                {
                    if (last_E_time + 3.0 < gametime->get_time())
                    {
                        if (e->cast())
                        {
                            return;
                        }
                    }
                }
            }
        }
    #pragma endregion

    #pragma region r_logic
        void r_logic()
        {
            auto target = target_selector->get_target(r->range(), damage_type::physical);
            if (target == nullptr || !target->is_valid_target() || target->is_zombie() || target->is_invulnerable()) return;
            {
                if (combo::use_r->get_bool())
                {
                    if (calculate_r_damage(target) >= target->get_real_health())
                    {
                        r->cast(target);
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

#pragma region on_process_spell_cast
    void on_process_spell_cast(game_object_script sender, spell_instance_script spell)
    {
        auto spell_hash = spell->get_spell_data()->get_name_hash();
        if (sender->is_me() && spell_hash == spell_hash("BlindMonkQOne"))
        {
            last_Q_time = gametime->get_time();
        }
        if (sender->is_me() && spell_hash == spell_hash("BlindMonkWOne"))
        {
            last_W_time = gametime->get_time();
        }
        if (sender->is_me() && spell_hash == spell_hash("BlindMonkEOne"))
        {
            last_E_time = gametime->get_time();
        }
    }
#pragma endregion

    float calculate_q1_damage(game_object_script target)
    {

        float q1_raw_damage = q1_damages[q->level() - 1] + q1_coef * myhero->get_additional_attack_damage();
        float q1_calculate_damage = damagelib->calculate_damage_on_unit(myhero, target, damage_type::physical, q1_raw_damage);

        return q1_calculate_damage;
    }

    float calculate_q2_damage(game_object_script target)
    {
        float q2_damage = q2_damages[q->level() - 1] + q2_coef * myhero->get_additional_attack_damage();
        float missing_hp_perc = 100 - target->get_health_percent();
        q2_damage += missing_hp_perc * q2_damage;
        float q2_calculate_damage = damagelib->calculate_damage_on_unit(myhero, target, damage_type::physical, q2_damage);

        return q2_calculate_damage;
    }

    float calculate_e_damage(game_object_script target)
    {
        float e_damage = e_damages[e->level() - 1] + (myhero->get_additional_attack_damage() * 1.0);
        float e_calculate_damage = damagelib->calculate_damage_on_unit(myhero, target, damage_type::magical, e_damage);

        return e_calculate_damage;
    }
    float calculate_r_damage(game_object_script target)
    {
        float r_damage = r_damages[r->level() - 1] + (r_coef* myhero->get_total_attack_damage());
        float r_calculate_damage = damagelib->calculate_damage_on_unit(myhero, target, damage_type::physical, r_damage);

        return r_calculate_damage;
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
        if (draw_settings::draw_damage_settings::draw_damage->get_bool())
        {
            for (auto& enemy : entitylist->get_enemy_heroes())
            {
                if (enemy->is_valid() && !enemy->is_dead() && enemy->is_hpbar_recently_rendered())
                {
                    float damage = 0.0f;

                    if (q->is_ready() && draw_settings::draw_damage_settings::q_damage->get_bool())
                    {
                        if (myhero->get_spell(spellslot::q)->get_name() == "BlindMonkQOne")
                            damage += calculate_q2_damage(enemy);
                        else
                            damage += calculate_q1_damage(enemy);
                    }

                    if (e->is_ready() && draw_settings::draw_damage_settings::e_damage->get_bool())
                        damage += calculate_e_damage(enemy);

                    if (r->is_ready() && draw_settings::draw_damage_settings::r_damage->get_bool())
                        damage += calculate_r_damage(enemy);

                    if (damage != 0.0f)
                        utils::draw_dmg_rl(enemy, damage, 0x8000ff00);
                }
            }
        }

    }
}