#include "../plugin_sdk/plugin_sdk.hpp"

PLUGIN_NAME("PentaAIO");
SUPPORTED_CHAMPIONS(champion_id::Singed /*,champion_id::LeeSin*/ );
PLUGIN_TYPE(plugin_type::champion);

#include "singed.h"
#include"leesin.h"


PLUGIN_API bool on_sdk_load(plugin_sdk_core* plugin_sdk_good)
{
    DECLARE_GLOBALS(plugin_sdk_good);

    switch (myhero->get_champion())
    {
    case champion_id::Singed:
        singed::load();
        break;
    /*case champion_id::LeeSin:
        leesin::load();
        break;*/
    default:
        break;
        console->print("Champion %s is not supported!", myhero->get_model_cstr());
        return false;
    }

    console->print("[PentaAIO] Champion %s loaded successfully.", myhero->get_model_cstr());
    return true;
}


PLUGIN_API void on_sdk_unload()
{
    switch (myhero->get_champion())
    {
   case champion_id::Singed:
        singed::unload();
        break;
    /*case champion_id::LeeSin:
        leesin::unload();
        break;*/
    default:
        break;
    }
}
