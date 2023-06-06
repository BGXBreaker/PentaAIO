#include "anivia.h"
#include "../plugin_sdk/plugin_sdk.hpp"
#include "utils.h"
#include "permashow.hpp"
#include "translate.h"
#include "SpellDB.h"
#include "cmath"

namespace anivia
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
        TreeEntry* Draw_Damage = nullptr;
        TreeEntry* Draw_Damage_color = nullptr;
        TreeEntry* Draw_DMG_rl = nullptr;
        TreeEntry* draw_transparency = nullptr;
        TreeEntry* glow_strong = nullptr;
        TreeEntry* glow_size = nullptr;
        TreeEntry* glow_outside_power = nullptr;
        TreeEntry* glow_outside_size = nullptr;
        TreeEntry* glow_thickness = nullptr;

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
        TreeEntry* q_range = nullptr;
        TreeEntry* use_q = nullptr;
        TreeEntry* r_slow_q = nullptr;
        TreeEntry* use_w = nullptr;
        TreeEntry* w_pull = nullptr;
        TreeEntry* w_push = nullptr;
        TreeEntry* r_w = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* e_frosted = nullptr;
        TreeEntry* r_min_mana = nullptr;
        TreeEntry* use_r = nullptr;
        TreeEntry* wq = nullptr;
        TreeEntry* close_q = nullptr;
        TreeEntry* unkillable = nullptr;
        TreeEntry* delay_r = nullptr;
        TreeEntry* delay_int = nullptr;
    }
    namespace lasthit
    {
        TreeEntry* AE = nullptr;
    }
    namespace harass
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* e_frosted = nullptr;
    }
    namespace laneclear
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_r = nullptr;
        TreeEntry* spell_farm = nullptr;
    }
    namespace jungleclear
    {
        TreeEntry* use_q = nullptr;
        TreeEntry* use_e = nullptr;
        TreeEntry* use_r = nullptr;
    }
    namespace misc
    {
        TreeEntry* auto_q = nullptr;
        TreeEntry* use_w_antigapcloser = nullptr;
        TreeEntry* use_q_antigapcloser = nullptr;
    }
    namespace hitchance
    {
        TreeEntry* q_hitchance = nullptr;
        TreeEntry* w_hitchance = nullptr;
        TreeEntry* r_hitchance = nullptr;
    }
    namespace killsteal
    {
        TreeEntry* q_killsteal = nullptr;
        TreeEntry* e_killsteal = nullptr;
    }
    enum Position
    {
        Line,
        Jungle
    };

    Position my_hero_region;

    void on_update();
    void on_draw();
    void draw_dmg_rl();
    void on_create_object(game_object_script sender);
    void on_delete_object(game_object_script sender);
    void on_gapcloser(game_object_script sender, antigapcloser::antigapcloser_args* args);
    void on_before_attack(game_object_script target, bool* process);
    float calculate_q1_damage(game_object_script enemy);
    float calculate_q2_damage(game_object_script enemy);
    float calculate_e_damage(game_object_script enemy);
    float distance_between_positions(const vector& pos1, const vector& pos2);
    int count_enemy_minions_in_range(float range, vector from);
    int count_enemy_monsters_in_range(float range, vector from);
    game_object_script q_missile;
    void q_logic();
    void harass_q();
    void q_auto_logic();
    void w_logic();
    void e_logic();
    void harass_e();
    void r_logic();
    void w_pull();
    void w_push();
    float q_missile_speed = 950.0f;
    float buff_remaining_time_additional_time = 0.1f;
    std::vector<float> q1_damages = { 50,70,90,110,130 };
    float q1_coef = 0.25f;
    std::vector<float> q2_damages = { 60,95,130,165,200 };
    float q2_coef = 0.45f;
    std::vector<float> e_damages = { 50,75,100,125,150 };
    float e_coef = 0.6f;
    std::vector<float> frostbite_damages = { 100,150,200,250,300 };
    float frostbite_coef = 1.2f;
    float ult_range = 400.0f;
    bool is_r_casted = false;
    float r_cast_time = 0.0f;
    bool r_active();
    vector last_r_pos;
    vector last_r_farm_pos;
    std::vector<game_object_script> get_enemies_in_range(vector position, float range)
    {
        std::vector<game_object_script> enemies_in_range;

        for (auto&& enemy : entitylist->get_enemy_heroes())
        {
            if (!enemy->is_dead() && enemy->is_visible() && distance_between_positions(enemy->get_position(), position) < range)
            {
                enemies_in_range.push_back(enemy);
            }
        }
        return enemies_in_range;
    }
    void load()
    {
        myhero->print_chat(0x3, "<font color=\"#FFFFFF\">[<b><font color=\"#3F704D\">Anivia | PentaAIO</font></b>]</font><font color=\"#3F704D\">:</font><font color=\"#90EE90\"> Loaded</font>");
        myhero->print_chat(0x3, "<font color=\"#3F704D\"><b> Prediction: </b><font color=\"#90EE90\">only support Aurora</font></font>");
        translate::load();
        q = plugin_sdk->register_spell(spellslot::q, 1090);
        q->set_skillshot(0.25f, 55.0f, 950.0f, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);
        w = plugin_sdk->register_spell(spellslot::w, 1000);
        w->set_skillshot(0.25f, 255.0f, FLT_MAX, { }, skillshot_type::skillshot_line);
        e = plugin_sdk->register_spell(spellslot::e, 650);
        e->set_skillshot(0.25f, 0.0f, 1600.0f, { collisionable_objects::yasuo_wall }, skillshot_type::skillshot_line);
        r = plugin_sdk->register_spell(spellslot::r, 750);
        r->set_skillshot(0.0f, 400.0f, FLT_MAX, { }, skillshot_type::skillshot_circle);

        q->set_spell_lock(false);
        w->set_spell_lock(false);
        e->set_spell_lock(false);
        r->set_spell_lock(false);

        main_tab = menu->create_tab("anivia", "Anivia");
        main_tab->set_assigned_texture(myhero->get_square_icon_portrait());
        {
            main_tab->add_separator(myhero->get_model() + ".aio", "PentaAIO : " + myhero->get_model());

            auto combo = main_tab->add_tab(myhero->get_model() + ".combo", "Combo Settings");
            {
                combo::use_q = combo->add_checkbox(myhero->get_model() + ".combo.q", "Use Q", true);
                combo::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                auto q_config = combo->add_tab(myhero->get_model() + "combo.q.config", "Q Settings");
                {
                    combo::r_slow_q = q_config->add_checkbox(myhero->get_model() + ".slowq", "Wait R slow for Q", true);
                    combo::r_slow_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                    combo::close_q = q_config->add_checkbox(myhero->get_model() + ".closeq", "when enemy is on your face", true);
                    combo::close_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                    combo::close_q->set_tooltip("eg:YiQ , ZedR, etc");
                }
                combo::use_w = combo->add_checkbox(myhero->get_model() + ".combo.w.use_w", "Use W", true);
                combo::use_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                auto w_config = combo->add_tab(myhero->get_model() + "combo.w.config", "W Settings");
                {
                    combo::wq = w_config->add_checkbox(myhero->get_model() + ".combo.wq", "Use WQ combo", true);
                    combo::wq->set_tooltip("only WQ before LV6");
                    combo::wq->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                    combo::r_w = w_config->add_checkbox(myhero->get_model() + ".combo.w.use_block", "Use W when leaving out of R range", true);
                    combo::r_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                }
                combo::use_e = combo->add_checkbox(myhero->get_model() + ".combo.e.use_e", "Use E", true);
                combo::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                auto e_config = combo->add_tab(myhero->get_model() + "combo.e.config", "E Settings");
                {
                    combo::e_frosted = e_config->add_checkbox(myhero->get_model() + ".combo.e.drosted", "Only E on frosted", true);
                    combo::e_frosted->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                }
                combo::use_r = combo->add_checkbox(myhero->get_model() + ".combo.r", "Use R", true);
                combo::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                auto r_config = combo->add_tab(myhero->get_model() + "combo.r.config", "R Settings");
                {
                    combo::unkillable = r_config->add_checkbox(myhero->get_model() + ".buff", "Don't close R if target have unkillable buff", true);
                    combo::unkillable->set_tooltip("eg:KayleR,TaricR,etc");
                    combo::unkillable->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                    combo::delay_r = r_config->add_checkbox(myhero->get_model() + ".dealy", "Delay close R", false);
                    combo::delay_int = r_config->add_slider("delayint", "Delay close R after X seconds", 2, 1, 10);
                    combo::delay_int->set_tooltip("start when no enemy in ult range");
                    combo::delay_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                    combo::r_min_mana = r_config->add_slider("minmana", "Min. % Mana For Use R", 30, 0, 100);
                    combo::r_min_mana->set_tooltip("0 = disable");
                    combo::r_min_mana->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                }
            }
            TreeTab* spells = main_tab->add_tab("spells", "Spells to Cancel:");
            {
                Database::InitializeCancelMenu(spells);
            }
            auto semi = main_tab->add_tab(myhero->get_model() + ".semi", "Semi Manual");
            {
                combo::w_pull = semi->add_hotkey(myhero->get_model() + ".w.pull", "W pull", TreeHotkeyMode::Hold, 'T', false, true);
                combo::w_push = semi->add_hotkey(myhero->get_model() + ".w.push", "W push", TreeHotkeyMode::Hold, 'T', false, true);
            }
            auto harass = main_tab->add_tab(myhero->get_model() + ".harass", "Harass Settings");
            {
                harass::use_q = harass->add_checkbox(myhero->get_model() + ".harass.q", "Use Q", true);
                harass::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                harass::use_e = harass->add_checkbox(myhero->get_model() + ".harass.e", "Use E", false);
                harass::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                harass::e_frosted = harass->add_checkbox(myhero->get_model() + ".harass.e.drosted", "Only E on frosted", true);
                harass::e_frosted->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
            }
            auto lasthit = main_tab->add_tab(myhero->get_model() + ".lasthit", "Lasthit Settings");
            {
                lasthit::AE = lasthit->add_checkbox(myhero->get_model() + ".lasthit.use_e", "Use E if unable to AA", true);
                lasthit::AE->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
            }
            auto laneclear = main_tab->add_tab(myhero->get_model() + ".laneclear", "Lane Clear Settings");
            {
                laneclear::spell_farm = laneclear->add_hotkey(myhero->get_model() + ".laneclear.enabled", "Toggle Spell Farm", TreeHotkeyMode::Toggle, 0x04, true);
                laneclear::use_q = laneclear->add_checkbox(myhero->get_model() + ".laneclear.use_q", "Use Q", true);
                laneclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                laneclear::use_e = laneclear->add_checkbox(myhero->get_model() + ".laneclear.use_e", "Use E", true);
                laneclear::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                laneclear::use_r = laneclear->add_checkbox(myhero->get_model() + ".laneclear.use_r", "Use R", true);
                laneclear::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
            }
            auto jungleclear = main_tab->add_tab(myhero->get_model() + ".jungleclear", "Jungle Clear Settings");
            {
                jungleclear::use_q = jungleclear->add_checkbox(myhero->get_model() + ".jungleclear.use_q", "Use Q", true);
                jungleclear::use_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                jungleclear::use_e = jungleclear->add_checkbox(myhero->get_model() + ".jungleclear.use_e", "Use E", true);
                jungleclear::use_e->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                jungleclear::use_r = jungleclear->add_checkbox(myhero->get_model() + ".jungleclear.use_r", "Use R", true);
                jungleclear::use_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
            }
            auto hitchance = main_tab->add_tab(myhero->get_model() + ".hitchance", "Hitchance Settings");
            {
                hitchance::q_hitchance = hitchance->add_combobox(myhero->get_model() + ".hitchance.q", "Hitchance Q", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 2);
                hitchance::w_hitchance = hitchance->add_combobox(myhero->get_model() + ".hitchance.w", "Hitchance W", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 3);
                hitchance::r_hitchance = hitchance->add_combobox(myhero->get_model() + ".hitchance.r", "Hitchance R", { {"Low",nullptr},{"Medium",nullptr },{"High", nullptr},{"Very High",nullptr} }, 1);
            }
            auto misc = main_tab->add_tab(myhero->get_model() + ".misc", "Miscellaneous Settings");
            {
                misc::auto_q = misc->add_hotkey(myhero->get_model() + ".auto.enabled", "Auto Q", TreeHotkeyMode::Toggle, 'A', false);
                misc::use_q_antigapcloser = misc->add_checkbox(myhero->get_model() + ".misc.use_q_antigapcloser", "Q Anti-Gapcloser", true);
                misc::use_w_antigapcloser = misc->add_checkbox(myhero->get_model() + ".misc.use_w_antigapcloser", "W Anti-Gapcloser", false);
            }
            auto killsteal = main_tab->add_tab(myhero->get_model() + ".killsteal", "Killsteal Settings");
            {
                killsteal::q_killsteal = killsteal->add_checkbox(myhero->get_model() + ".killsteal.use_q", "Q KS", true);
                killsteal::e_killsteal = killsteal->add_checkbox(myhero->get_model() + ".killsteal.use_e", "E KS", true);
            }
            auto draw_settings = main_tab->add_tab(myhero->get_model() + ".drawings", "Drawings Settings");
            {
                float color_red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
                float color_blue[] = { 0.0f, 0.4f, 1.0f, 1.0f };
                float color_green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
                float color_dark_blue[] = { 0.4f, 0.3f, 1.0f, 1.0f };
                float color11[] = { 0.5f, 0.0f, 0.9f, 1.0f };

                draw_settings::draw_range_q = draw_settings->add_checkbox(myhero->get_model() + ".drawingQ", "Draw Q range", true);
                draw_settings::draw_range_q->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                draw_settings::q_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.q.color", "Q Color", color_red, true);

                draw_settings::draw_range_w = draw_settings->add_checkbox(myhero->get_model() + ".drawingw", "Draw W range", true);
                draw_settings::draw_range_w->set_texture(myhero->get_spell(spellslot::w)->get_icon_texture());
                draw_settings::w_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.w.color", "W Color", color_blue, true);

                draw_settings::draw_range_e = draw_settings->add_checkbox(myhero->get_model() + ".drawinge2", "Draw E range", true);
                draw_settings::e_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.e2.color", "E Color", color_dark_blue, false);

                draw_settings::draw_range_r = draw_settings->add_checkbox(myhero->get_model() + ".drawingr", "Draw R range", true);
                draw_settings::draw_range_r->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                draw_settings::r_color = draw_settings->add_colorpick(myhero->get_model() + ".draw.r.color", "R Color", color11, true);

                /*auto draw_glow = draw_settings->add_tab(myhero->get_model() + ".draw.glow", "Glow Settings");
                {
                    draw_settings::glow_thickness = draw_glow->add_slider(myhero->get_model() + ".thickness", "Thickness", 2, 1, 5);
                    draw_settings::glow_size = draw_glow->add_slider(myhero->get_model() + ".glowsize", "Glow Size", 40, 1, 100);
                    draw_settings::glow_strong = draw_glow->add_slider(myhero->get_model() + ".glowstrong", "Glow Strong", 50, 1, 100);
                    draw_settings::glow_outside_size = draw_glow->add_slider(myhero->get_model() + ".glowoutsize", "Glow Outside Size", 100, 1, 100);
                    draw_settings::glow_outside_power = draw_glow->add_slider(myhero->get_model() + ".glowoutpower", "Glow Outside Power", 50, 1, 100);

                }*/

                auto draw_damage = draw_settings->add_tab(myhero->get_model() + ".draw.damage", "Draw Damage");
                {
                    draw_settings::draw_damage_settings::draw_damage = draw_damage->add_checkbox(myhero->get_model() + ".draw.damage.enabled", "Draw Combo Damage", true);
                    draw_settings::draw_transparency = draw_settings->add_colorpick("color", "Draw Damage color", color_green, true);
                    draw_settings::draw_damage_settings::q_damage = draw_damage->add_checkbox(myhero->get_model() + ".draw.damage.q", "Draw Q Damage", true);
                    draw_settings::draw_damage_settings::q_damage->set_texture(myhero->get_spell(spellslot::q)->get_icon_texture());
                    draw_settings::draw_damage_settings::e_damage = draw_damage->add_checkbox(myhero->get_model() + ".draw.damage.e", "Draw E Damage", true);
                    draw_settings::draw_damage_settings::e_damage->set_texture(myhero->get_spell(spellslot::e)->get_icon_texture());
                    draw_settings::draw_damage_settings::r_damage = draw_damage->add_checkbox(myhero->get_model() + ".draw.damage.r", "Draw R Damage", true);
                    draw_settings::draw_damage_settings::r_damage->set_texture(myhero->get_spell(spellslot::r)->get_icon_texture());
                }

            }


            antigapcloser::add_event_handler(on_gapcloser);
            event_handler<events::on_update>::add_callback(on_update);
            event_handler<events::on_env_draw>::add_callback(on_draw);
            //event_handler<events::on_after_attack_orbwalker>::add_callback(on_after_attack_orbwalker);
            event_handler<events::on_before_attack_orbwalker>::add_callback(on_before_attack);
            event_handler<events::on_draw>::add_callback(draw_dmg_rl);
            event_handler<events::on_create_object>::add_callback(on_create_object);
            event_handler<events::on_delete_object>::add_callback(on_delete_object);
            utils::on_load();
        }
        {
            Permashow::Instance.Init(main_tab);
            Permashow::Instance.AddElement("Spell Farm", laneclear::spell_farm);
            Permashow::Instance.AddElement("Auto Q", misc::auto_q);
            Permashow::Instance.AddElement("W push", combo::w_push);
            Permashow::Instance.AddElement("W pull", combo::w_pull);
        }
        main_tab->add_separator("separator_1", "~~Version: 1.0.7~~");
        main_tab->add_separator("separator_2", "Author: GameBreaker#3051");
        main_tab->add_separator("separator_3", "~~Enjoy~~");
    }
    void unload()
    {
        plugin_sdk->remove_spell(q);
        plugin_sdk->remove_spell(w);
        plugin_sdk->remove_spell(e);
        plugin_sdk->remove_spell(r);

        menu->delete_tab(main_tab);

        Permashow::Instance.Destroy();

        antigapcloser::remove_event_handler(on_gapcloser);

        event_handler<events::on_update>::remove_handler(on_update);
        event_handler<events::on_env_draw>::remove_handler(on_draw);
        event_handler<events::on_before_attack_orbwalker>::remove_handler(on_before_attack);
        //event_handler<events::on_after_attack_orbwalker>::remove_handler(on_after_attack_orbwalker);
        event_handler< events::on_draw >::remove_handler(draw_dmg_rl);
        event_handler<events::on_create_object>::remove_handler(on_create_object);
        event_handler<events::on_delete_object>::remove_handler(on_delete_object);
    }
    void on_update()
    {
        if (myhero->is_dead())
        {
            return;
        }
        if (r_active() && myhero->get_mana_percent() < combo::r_min_mana->get_int() && orbwalker->combo_mode())
        {
            r->cast();
            last_r_pos = vector::zero;
            //myhero->print_chat(1, "no mana r");
        }
        if (r_active())
        {
            if (myhero->is_dead() || myhero->is_recalling())
            {
                last_r_pos = vector::zero;
                return;
            }
            float current_time = gametime->get_time();
            for (auto&& enemy : entitylist->get_enemy_heroes())
            {
                if (combo::unkillable->get_bool() && (utils::has_unkillable_buff(enemy) || enemy->get_buff(1036096934) || enemy->get_buff(-718911512)))
                {
                    //myhero->print_chat(1, "unkillable r");
                    continue;
                }
                if (r->is_ready() && last_r_pos.is_valid() && last_r_pos.count_enemies_in_range(ult_range) == 0)
                {
                    if (combo::delay_r->get_bool())
                    {
                        if (!is_r_casted)
                        {
                            r_cast_time = current_time;
                            is_r_casted = true;
                        }
                        if (current_time - r_cast_time >= combo::delay_int->get_int())
                        {
                            r->cast();
                            last_r_pos = vector::zero;
                            //myhero->print_chat(1, "delay close r");
                            is_r_casted = false;
                            r_cast_time = 0.0f;
                        }
                    }
                    else
                    {
                        r->cast();
                        last_r_pos = vector::zero;
                        //myhero->print_chat(1, "close r");
                    }
                }
            }
        }

        if (q->is_ready() && combo::use_q->get_bool() && combo::close_q->get_bool())
        {
            for (auto&& enemy : entitylist->get_enemy_heroes())
            {
                if (myhero->get_distance(enemy->get_position()) < 300.f && !enemy->is_dead() && enemy->is_valid_target(q->range()))
                {
                    if (q_missile == nullptr)
                    {
                        q->cast(enemy);
                        //myhero->print_chat(1, "enemy too close !");
                    }
                }
            }
        }
        if (w->is_ready() && !q->is_ready())
        {
            for (auto&& enemy : entitylist->get_enemy_heroes())
            {
                if (Database::canCancel(enemy) && enemy->is_valid_target(w->range()))
                {
                    w->cast(enemy);
                    //myhero->print_chat(1, "DataBase W");
                }
            }
        }
        if (q->is_ready() && !w->is_ready())
        {
            for (auto&& enemy : entitylist->get_enemy_heroes())
            {
                if (Database::canCancel(enemy) && enemy->is_valid_target(q->range()) && !enemy->has_buff(buff_hash("SionR")))
                {
                    if (myhero->has_buff(buff_hash("FlashFrost")))
                    {
                        if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead())
                        {

                            if (q_missile->get_distance(enemy) < 225.0f)
                            {
                                if (q->cast())
                                {
                                    //myhero->print_chat(1, "data base q recast");
                                }
                            }

                        }
                    }
                    else if (q_missile == nullptr)
                    {
                        if (q->cast(enemy))
                        {
                            //myhero->print_chat(1, "database q");
                        }
                    }
                }
            }
        }
        for (auto&& enemy : entitylist->get_enemy_heroes())
        {
            if (myhero->has_buff(buff_hash("FlashFrost")) && !enemy->is_dead())
            {
                if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(enemy) < 225.0f)
                {
                    q->cast();
                    //myhero->print_chat(1, "q2 update enemy");
                }
            }
        }
        /*for (auto&& minion : entitylist->get_enemy_minions())
        {
            if (myhero->has_buff(buff_hash("FlashFrost")))
            {
                if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(minion) < 225.0f)
                {
                    q->cast();
                  //myhero->print_chat(1, "q2 update minion");
                }
            }
        }
        for (auto&& monster : entitylist->get_jugnle_mobs_minions())
        {
            if (myhero->has_buff(buff_hash("FlashFrost")))
            {
                if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(monster) < 225.0f)
                {
                    q->cast();
                  //myhero->print_chat(1, "q2 update monster");
                }
            }
        }*/
        if (combo::w_pull->get_bool() && w->is_ready())
        {
            w_pull();
            myhero->issue_order(hud->get_hud_input_logic()->get_game_cursor_position());
        }
        if (combo::w_push->get_bool() && w->is_ready())
        {
            w_push();
            myhero->issue_order(hud->get_hud_input_logic()->get_game_cursor_position());
        }
        /*console->print("[PentaAIO] [DEBUG] Buff list:");
        for (auto& enemy : entitylist->get_enemy_heroes())
        {
            for (auto&& enemy_buff : enemy->get_bufflist())
            {
                if (enemy_buff->is_valid() && enemy_buff->is_alive())
                {
                    console->print("[PentaAIO] [DEBUG] Enemy Buff name %s, count: %d", enemy_buff->get_name_cstr(), enemy_buff->get_count());
                }

            }
        }*/
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
                    harass_q();
                }
                if (e->is_ready() && harass::use_e->get_bool())
                {
                    harass_e();
                }
            }
        }
        if (orbwalker->can_move())
        {
            if (!myhero->is_under_enemy_turret())
            {
                if (!myhero->is_recalling())
                {
                    if (q->is_ready() && misc::auto_q->get_bool() && combo::use_q->get_bool())
                    {
                        q_auto_logic();
                    }
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
                for (auto&& minion : lane_minions)
                {
                    if (q->is_ready() && laneclear::use_q->get_bool())
                    {
                        if (myhero->has_buff(buff_hash("FlashFrost")))
                        {
                            if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead())
                            {
                                for (auto& minion : lane_minions)
                                {
                                    if (q_missile->get_distance(minion) < 225.0f)
                                    {
                                        if (q->cast())
                                        {
                                            //myhero->print_chat(1, "laneclear q recast");
                                        }
                                    }
                                }
                            }
                        }
                        else if (q_missile == nullptr && !myhero->count_enemies_in_range(1300))
                        {
                            if (q->cast_on_best_farm_position())
                            {
                                //myhero->print_chat(1, "laneclear q");
                            }
                        }
                    }
                    if (e->is_ready() && laneclear::use_e->get_bool())
                    {
                        if (minion->get_distance(myhero->get_position()) <= e->range())
                        {
                            e->cast(minion);
                        }

                    }
                    if (r->is_ready() && laneclear::use_r->get_bool() && minion->get_distance(myhero->get_position()) <= r->range())
                    {
                        auto r_position = r->get_cast_on_best_farm_position();
                        auto minions_around = count_enemy_minions_in_range(ult_range, last_r_farm_pos);
                        if (minions_around != 0)
                        {
                            if (r->toogle_state() == 1)
                            {
                                r->cast(r_position);
                                //myhero->print_chat(1, "jungleclear farm r");
                                last_r_farm_pos = r_position;
                            }
                        }
                        else
                            if (minions_around == 0)
                            {
                                if (r->toogle_state() == 2)
                                {
                                    r->cast();
                                    last_r_farm_pos = vector::zero;
                                    //myhero->print_chat(1, "jungle farm r close");
                                }
                            }
                    }
                }
            }
            else if (monsters.empty() && r->toogle_state() == 2)
            {
                r->cast();
                last_r_farm_pos = vector::zero;
                //myhero->print_chat(1, "disable 2");
            }
            for (auto&& monster : monsters)
            {
                if (!monsters.empty())
                {
                    if (q->is_ready() && jungleclear::use_q->get_bool())
                    {
                        if (myhero->has_buff(buff_hash("FlashFrost")))
                        {
                            if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead())
                            {
                                for (auto& monster : monsters)
                                {
                                    if (q_missile->get_distance(monster) < 225.0f)
                                    {
                                        if (q->cast())
                                            return;
                                    }
                                }
                            }
                        }
                        else if (q_missile == nullptr)
                        {
                            if (q->cast_on_best_farm_position(1, true))
                                return;
                        }
                    }
                    if (e->is_ready() && jungleclear::use_e->get_bool() && monster->get_distance(myhero->get_position()) <= e->range())
                    {
                        if (e->cast(monsters.front()))
                            return;
                    }
                    if (r->is_ready() && jungleclear::use_r->get_bool())
                    {
                        auto r_position = r->get_cast_on_best_farm_position(1, true);
                        auto monsters_around = count_enemy_monsters_in_range(ult_range, last_r_farm_pos);
                        if (monsters_around != 0)
                        {
                            if (r->toogle_state() == 1)
                            {
                                r->cast(r_position);
                                //myhero->print_chat(1, "jungleclear farm r");
                                last_r_farm_pos = r_position;
                            }
                        }
                        else
                            if (monsters_around == 0)
                            {
                                if (r->toogle_state() == 2)
                                {
                                    r->cast();
                                    last_r_farm_pos = vector::zero;
                                    //myhero->print_chat(1, "jungle farm r close");
                                }
                            }
                    }
                }
            }
        }
        if (orbwalker->last_hit_mode() && laneclear::spell_farm->get_bool())
        {
            auto lane_minions = entitylist->get_enemy_minions();

            lane_minions.erase(std::remove_if(lane_minions.begin(), lane_minions.end(), [](game_object_script x)
                {
                    return !x->is_valid_target(e->range());
                }), lane_minions.end());

            std::sort(lane_minions.begin(), lane_minions.end(), [](game_object_script a, game_object_script b)
                {
                    return a->get_position().distance(myhero->get_position()) < b->get_position().distance(myhero->get_position());
                });

            if (!lane_minions.empty())
            {
                if (e->is_ready() && lasthit::AE->get_bool())
                {
                    if (!lane_minions.empty())
                    {
                        if (e->is_ready() && lasthit::AE->get_bool())
                        {
                            int killable_minions = 0;

                            for (auto& minion : lane_minions)
                            {
                                if (myhero->get_auto_attack_damage(minion) > minion->get_health())
                                {
                                    killable_minions++;
                                }

                                if (killable_minions >= 2)
                                {
                                    if (e->cast(minion))
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
    }
#pragma region q_logic  
    void q_logic()
    {
        for (auto&& enemy : entitylist->get_enemy_heroes())
        {
            if (utils::has_unccable_buff(enemy))
            {
                //myhero->print_chat(1, "enemy have magic shield");
                continue;
            }
        }
        if ((q->is_ready() && combo::r_slow_q->get_bool() && myhero->get_level() >= 6))
        {
            auto target = target_selector->get_target(r->range(), damage_type::magical);
            if (target != nullptr && (target->has_buff(buff_hash("aniviachilled")) || utils::has_unslowable_buff(target)))
            {
                if (q_missile == nullptr)
                {
                    auto target = target_selector->get_target(r->range(), damage_type::magical);
                    if (target != nullptr && r_active())
                    {
                        if (q->cast(target, utils::get_hitchance(hitchance::q_hitchance)))
                        {
                            //myhero->print_chat(1, "q slow or unslowable target");
                            return;
                        }
                    }
                }
            }
        }
        else if ((q->is_ready() && (!r->is_ready() || r->is_ready()) && !combo::r_slow_q->get_bool() && myhero->get_level() <= 18) || (combo::r_slow_q->get_bool() && myhero->get_level() <= 18 && q->is_ready() && (!r->is_ready() || r->is_ready())))
        {
            if (q_missile == nullptr)
            {
                auto target = target_selector->get_target(q->range(), damage_type::magical);
                auto pred = q->get_prediction(target);
                auto pred_pos = pred.get_cast_position();
                if (target != nullptr)
                {
                    if (q->cast(pred_pos))
                    {
                        //myhero->print_chat(1, "q in nomal 1");
                        return;
                    }
                }
            }
        }
        if (q->is_ready() && killsteal::q_killsteal->get_bool())
        {
            auto target = target_selector->get_target(q->range() + 675, damage_type::magical);
            if (q_missile == nullptr)
            {
                auto target = target_selector->get_target(q->range(), damage_type::magical);
                if (target != nullptr && (calculate_q1_damage(target) + calculate_q2_damage(target)) > target->get_health())
                {
                    if (q->cast(target, utils::get_hitchance(hitchance::q_hitchance)))
                    {
                        //myhero->print_chat(1, "ks q1");
                        return;
                    }
                }
            }
        }
        if (q->is_ready())
        {
            for (auto&& enemy : entitylist->get_enemy_heroes())
            {
                if (enemy != nullptr && myhero->get_distance(enemy->get_position()) <= q->range())
                {
                    auto buff = enemy->get_buff_by_type({ buff_type::Stun, buff_type::Snare, buff_type::Knockup, buff_type::Asleep, buff_type::Suppression, buff_type::Taunt });
                    auto zhonya = enemy->get_buff(1036096934);
                    auto ga = enemy->get_buff(-718911512);
                    if (buff != nullptr)
                    {
                        float remaining_time = buff->get_remaining_time();
                        float q_travel_time = enemy->get_distance(myhero) / q_missile_speed;

                        if (remaining_time + buff_remaining_time_additional_time >= q->get_delay() + q_travel_time)
                        {
                            if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(enemy) < 225.0f)
                            {
                                if (q->cast())
                                {
                                    return;
                                }
                            }
                            else
                            {
                                if (q_missile == nullptr)
                                {
                                    if (enemy != nullptr)
                                    {
                                        if (q->cast(enemy, utils::get_hitchance(hitchance::q_hitchance)))
                                        {
                                            //myhero->print_chat(1, "q cc");
                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (zhonya != nullptr)
                    {
                        float remaining_time = zhonya->get_remaining_time();
                        float q_travel_time = enemy->get_distance(myhero) / q_missile_speed;

                        if (remaining_time + buff_remaining_time_additional_time >= q->get_delay() + q_travel_time)
                        {
                            if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(enemy) < 225.0f)
                            {
                                if (q->cast())
                                {
                                    return;
                                }
                            }
                            else
                            {
                                if (q_missile == nullptr)
                                {
                                    if (enemy != nullptr)
                                    {
                                        if (q->cast(enemy, utils::get_hitchance(hitchance::q_hitchance)))
                                        {
                                            //myhero->print_chat(1, "q zhonya");
                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (ga != nullptr)
                    {
                        float remaining_time = ga->get_remaining_time();
                        float q_travel_time = enemy->get_distance(myhero) / q_missile_speed;

                        if (remaining_time + buff_remaining_time_additional_time >= q->get_delay() + q_travel_time)
                        {
                            if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(enemy) < 225.0f)
                            {
                                if (q->cast())
                                {
                                    return;
                                }
                            }
                            else
                            {
                                if (q_missile == nullptr)
                                {
                                    if (enemy != nullptr)
                                    {
                                        if (q->cast(enemy, utils::get_hitchance(hitchance::q_hitchance)))
                                        {
                                            //myhero->print_chat(1, "q ga");
                                            return;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    }
#pragma endregion

#pragma region auto_q
    void q_auto_logic()
    {
        for (auto&& enemy : entitylist->get_enemy_heroes())
        {
            if (enemy->has_buff(buff_hash("MorganaE")) || enemy->has_buff(buff_hash("OlafRagnarok")) || enemy->has_buff(buff_hash("SivirE")) || enemy->has_buff(buff_hash("NocturneShroudofDarkness")))
            {
                continue;
            }
        }
        if (myhero->has_buff(buff_hash("FlashFrost")))
        {
            auto target = target_selector->get_target(q->range() + 675, damage_type::magical);
            if (target != nullptr)
            {
                if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(target) < 225.0f)
                {
                    if (q->cast())
                    {
                        //myhero->print_chat(1, "auto q2");
                        return;
                    }
                }
            }
        }
        else if (q_missile == nullptr)
        {
            auto target = target_selector->get_target(q->range(), damage_type::magical);
            if (target != nullptr)
            {
                if (q->cast(target, utils::get_hitchance(hitchance::q_hitchance)))
                {
                    //myhero->print_chat(1, "auto q1");
                    return;
                }
            }
        }
    }
#pragma endregion

#pragma region w_logic
    void w_logic()
    {
        if (combo::r_w->get_bool())
        {
            auto targets_in_range = get_enemies_in_range(last_r_pos, ult_range);
            if (!targets_in_range.empty())
            {
                auto target = targets_in_range[0];
                if (!target->is_facing(myhero) && r_active() && distance_between_positions(target->get_position(), last_r_pos) > (ult_range - 150) && last_r_pos.is_valid() && !target->has_buff(buff_hash("YoneE")) && !target->is_dashing() && target->has_buff(buff_hash("aniviachilled")))
                {
                    auto pred = w->get_prediction(target);
                    if (pred.hitchance >= utils::get_hitchance(hitchance::w_hitchance))
                    {
                        vector pred_pos = pred.get_cast_position();
                        vector pos = myhero->get_position().extend(pred_pos, myhero->get_distance(pred_pos) + 50);
                        if (w->cast(pos))
                        {
                            //myhero->print_chat(1, "w not facing block");
                            return;
                        }
                    }
                }
                else if (target->is_facing(myhero) && r_active() && distance_between_positions(target->get_position(), last_r_pos) > (ult_range - 150) && last_r_pos.is_valid() && !target->has_buff(buff_hash("YoneE")) && !target->is_dashing() && target->has_buff(buff_hash("aniviachilled")))
                {
                    auto pred = w->get_prediction(target);
                    if (pred.hitchance >= utils::get_hitchance(hitchance::w_hitchance))
                    {
                        vector pred_pos = pred.get_cast_position();
                        vector pos = myhero->get_position().extend(pred_pos, myhero->get_distance(pred_pos) - 50);
                        if (w->cast(pos))
                        {
                            //myhero->print_chat(1, "w facing block");
                            return;
                        }
                    }
                }
            }
        }
        if (combo::wq->get_bool())
        {
            if (myhero->get_level() < 6)
            {
                if (q->is_ready() && combo::use_q->get_bool())
                {
                    if (myhero->has_buff(buff_hash("FlashFrost")))
                    {
                        if (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead())
                        {
                            auto target = target_selector->get_target(w->range(), damage_type::magical);
                            if (target != nullptr && !target->is_facing(myhero))
                            {
                                auto pred = w->get_prediction(target);
                                if (pred.hitchance >= utils::get_hitchance(hitchance::w_hitchance))
                                {
                                    vector pred_pos = pred.get_cast_position();
                                    vector pos = myhero->get_position().extend(pred_pos, myhero->get_distance(pred_pos) + 150);
                                    //vector pos = pred_pos.extend(myhero->get_position(), myhero->get_distance(pred_pos) + 200);
                                    if (w->cast(pos))
                                    {
                                        //myhero->print_chat(1, "combo wq");
                                        return;
                                    }
                                }
                            }
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
        if (combo::e_frosted->get_bool())
        {
            if (target != nullptr)
            {
                if (target->has_buff(buff_hash("aniviaiced")))
                {
                    if (e->cast(target))
                    {
                        //myhero->print_chat(1, "e on q");
                        return;
                    }
                }
                if (q_missile == nullptr && r_active() && !q->is_ready())
                {
                    if (target->has_buff(buff_hash("aniviachilled")))
                    {
                        if (e->cast(target))
                        {
                            //myhero->print_chat(1, "e on r");
                            return;
                        }
                    }
                }
            }
        }
        else if (target != nullptr)
        {
            if (e->cast(target))
            {
                //myhero->print_chat(1, "normal e");
                return;
            }
        }
        if (e->is_ready() && killsteal::e_killsteal->get_bool())
        {
            auto target = target_selector->get_target(e->range(), damage_type::magical);
            if (target != nullptr && target->get_distance(myhero) <= e->range() && calculate_e_damage(target) > target->get_real_health())
            {
                if (e->cast(target))
                {
                    return;
                }
            }
        }

    }
#pragma endregion

#pragma region r_logic
    void r_logic()
    {
        auto target = target_selector->get_target(r->range(), damage_type::magical);
        {
            if (target != nullptr && target->is_valid_target(r->range()) && myhero->get_mana_percent() > combo::r_min_mana->get_int())
            {
                auto r_position = target->get_position();
                //auto enemies_in_ult_range = predicted_position.count_enemies_in_range(ult_range);
                if (r->toogle_state() == 1)
                {
                    r->cast(r_position);
                    last_r_pos = r_position;
                    //myhero->print_chat(1, "open r");
                }
            }
        }
    }

#pragma region harass_q
    void harass_q()
    {
        for (auto&& enemy : entitylist->get_enemy_heroes())
        {
            if (utils::has_unccable_buff(enemy))
            {
                //myhero->print_chat(1, "enemy have magic shield");
                continue;
            }
        }
        if (q->is_ready())
        {
            if (q_missile == nullptr)
            {
                auto target = target_selector->get_target(q->range(), damage_type::magical);
                if (target != nullptr)
                {
                    if (q->cast(target, utils::get_hitchance(hitchance::q_hitchance)))
                    {
                        //myhero->print_chat(1, "harass q1");
                        return;
                    }
                }
            }
        }
    }
#pragma region harass_e
    void harass_e()
    {
        if (harass::e_frosted->get_bool())
        {
            auto target = target_selector->get_target(e->range(), damage_type::magical);
            if (target != nullptr)
            {
                if (target->has_buff(buff_hash("aniviaiced")) || target->has_buff(buff_hash("aniviachilled")))
                {
                    if (e->cast(target))
                    {
                        //myhero->print_chat(1, "harass on frosted");
                        return;
                    }
                }
            }
        }
        else if (harass::use_e->get_bool())
        {
            auto target = target_selector->get_target(e->range(), damage_type::magical);
            if (target != nullptr)
            {
                if (e->cast(target))
                {
                    //myhero->print_chat(1, "harass on e");
                    return;
                }
            }
        }
    }
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
        if (misc::use_w_antigapcloser->get_bool() && !q->is_ready() && w->is_ready())
        {
            auto pred = w->get_prediction(sender);
            if (sender->is_valid_target(w->range() + sender->get_bounding_radius()))
            {
                vector pred_pos = pred.get_cast_position();
                vector pos = myhero->get_position().extend(pred_pos, myhero->get_distance(pred_pos) - 50);
                w->cast(pos);
            }
        }

        if (misc::use_q_antigapcloser->get_bool())
        {
            if (sender->is_valid_target(q->range() + sender->get_bounding_radius()))
            {
                q->cast(sender);
                //myhero->print_chat(1, "anti gapcloser Q");
            }
            else (q_missile != nullptr && q_missile->is_valid() && !q_missile->is_dead() && q_missile->get_distance(sender) < 225.0f);
            {
                if (q->cast())
                {
                    //myhero->print_chat(1, "anti gapcloser recast Q");
                    return;
                }
            }
        }

    }
    void w_push()
    {
        for (auto&& enemy : entitylist->get_enemy_heroes())
        {
            if (enemy != nullptr)
            {
                auto pred = w->get_prediction(enemy);
                if (pred.hitchance >= utils::get_hitchance(hitchance::w_hitchance))
                {
                    vector pred_pos = pred.get_cast_position();
                    vector pos = myhero->get_position().extend(pred_pos, myhero->get_distance(pred_pos) - 50);
                    if (w->cast(pos))
                    {
                        //myhero->print_chat(1, "w push");
                        return;
                    }
                }
            }
        }
    }

    void w_pull()
    {
        for (auto&& enemy : entitylist->get_enemy_heroes())
        {
            if (enemy != nullptr)
            {
                auto pred = w->get_prediction(enemy);
                if (pred.hitchance >= utils::get_hitchance(hitchance::w_hitchance))
                {
                    vector pred_pos = pred.get_cast_position();
                    vector pos = myhero->get_position().extend(pred_pos, myhero->get_distance(pred_pos) + 50);
                    if (w->cast(pos))
                    {
                        //myhero->print_chat(1, "w pull");
                        return;
                    }
                }
            }
        }
    }

    void on_before_attack(game_object_script target, bool* process)
    {
        if (e->is_ready() && q->cooldown_time() >= 2)
        {
            if (target->is_ai_hero() && !combo::e_frosted->get_bool() && (orbwalker->combo_mode() || orbwalker->harass()) && combo::use_e->get_bool() && myhero->is_in_auto_attack_range(target))
            {
                if (e->cast(target))
                {
                    return;
                }
            }
        }
        if (e->is_ready())
        {
            if (target->is_ai_hero() && combo::e_frosted->get_bool() && (orbwalker->combo_mode() || orbwalker->harass()) && combo::use_e->get_bool() && myhero->is_in_auto_attack_range(target))
            {
                if (target->has_buff(buff_hash("aniviachilled")) || target->has_buff(buff_hash("aniviaiced")))
                {
                    if (e->cast(target))
                    {
                        return;
                    }
                }
            }
        }
    }
    void on_create_object(game_object_script sender)
    {
        if (sender->is_valid() && !sender->is_dead() && sender->get_name() == "FlashFrostSpell")
        {
            q_missile = sender;
        }
    }

    void on_delete_object(game_object_script sender)
    {
        if (sender->is_valid() && !sender->is_dead() && sender->get_name() == "FlashFrostSpell")
        {
            q_missile = nullptr;
        }
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

                    if ((q->is_ready() || q_missile != nullptr) && e->is_ready() && draw_settings::draw_damage_settings::q_damage->get_bool() && draw_settings::draw_damage_settings::e_damage->get_bool())
                    {
                        damage += calculate_q1_damage(enemy) + calculate_e_damage(enemy) + calculate_q2_damage(enemy);
                    }
                    else
                    {
                        if ((q->is_ready() || q_missile != nullptr) && draw_settings::draw_damage_settings::q_damage->get_bool())
                        {
                            damage += calculate_q1_damage(enemy) + calculate_q2_damage(enemy);
                        }
                    }

                    if (e->is_ready() && draw_settings::draw_damage_settings::e_damage->get_bool())
                        damage += calculate_e_damage(enemy);

                    if (r->is_ready() && (e->is_ready() || enemy->has_buff(buff_hash("aniviachilled"))) && draw_settings::draw_damage_settings::r_damage->get_bool() && draw_settings::draw_damage_settings::e_damage->get_bool())
                        damage += (r->get_damage(enemy) + calculate_e_damage(enemy));
                    else
                    {
                        if (r->is_ready() && draw_settings::draw_damage_settings::r_damage->get_bool())
                        {
                            damage += r->get_damage(enemy);
                        }
                    }
                    if (damage != 0.0f)
                        utils::draw_dmg_rl(enemy, damage, 0x8000ff00);
                }
            }
        }
    }
    bool r_active()
    {
        auto buff = myhero->get_buff(buff_hash("GlacialStorm"));
        return buff != nullptr && buff->is_valid() && buff->is_alive();
    }

    void on_draw()
    {

        if (myhero->is_dead())
        {
            return;
        }
        if (q->is_ready() && draw_settings::draw_range_q->get_bool())
            draw_manager->add_circle_with_glow(myhero->get_position(), draw_settings::q_color->get_color(), q->range(), 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));
        if (w->is_ready() && draw_settings::draw_range_w->get_bool())
            draw_manager->add_circle_with_glow(myhero->get_position(), draw_settings::w_color->get_color(), w->range(), 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));

        if (e->is_ready() && draw_settings::draw_range_e->get_bool())
            draw_manager->add_circle_with_glow(myhero->get_position(), draw_settings::e_color->get_color(), e->range(), 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));
        if (r->is_ready() && draw_settings::draw_range_r->get_bool())
            draw_manager->add_circle_with_glow(myhero->get_position(), draw_settings::r_color->get_color(), r->range(), 2.0f, glow_data(0.1f, 0.75f, 0.f, 1.f));
    }
    float calculate_q1_damage(game_object_script enemy)
    {
        float q_raw_damage = q1_damages[q->level() - 1] + (myhero->get_total_ability_power() * q1_coef);
        float q_calculated_damage = damagelib->calculate_damage_on_unit(myhero, enemy, damage_type::magical, q_raw_damage);

        return q_calculated_damage;
    }
    float calculate_q2_damage(game_object_script enemy)
    {
        float q2_raw_damage = q2_damages[q->level() - 1] + (myhero->get_total_ability_power() * q2_coef);
        float q2_calculated_damage = damagelib->calculate_damage_on_unit(myhero, enemy, damage_type::magical, q2_raw_damage);

        return q2_calculated_damage;
    }
    float calculate_e_damage(game_object_script enemy)
    {
        if (enemy->has_buff(buff_hash("aniviachilled")) || enemy->has_buff(buff_hash("aniviaiced")))
        {
            float e_raw_damage = frostbite_damages[e->level() - 1] + (myhero->get_total_ability_power() * frostbite_coef);
            float e_calculated_damage = damagelib->calculate_damage_on_unit(myhero, enemy, damage_type::magical, e_raw_damage);

            return e_calculated_damage;
        }
        else
        {
            {
                float e_raw_damage = e_damages[e->level() - 1] + (myhero->get_total_ability_power() * e_coef);
                float e_calculated_damage = damagelib->calculate_damage_on_unit(myhero, enemy, damage_type::magical, e_raw_damage);

                return e_calculated_damage;
            }
        }
    }
    float distance_between_positions(const vector& pos1, const vector& pos2)
    {
        float dx = pos2.x - pos1.x;
        float dy = pos2.y - pos1.y;
        float dz = pos2.z - pos1.z;

        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
    int count_enemy_minions_in_range(float range, vector from)
    {
        int count = 0;
        for (auto&& t : entitylist->get_enemy_minions())
        {
            if (t->is_valid_target(range, from))
                count++;
        }
        return count;
    }
    int count_enemy_monsters_in_range(float range, vector from)
    {
        int count = 0;
        for (auto&& t : entitylist->get_jugnle_mobs_minions())
        {
            if (t->is_valid_target(range, from))
                count++;
        }
        return count;
    }
}
