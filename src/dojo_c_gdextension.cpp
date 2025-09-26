//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include <resources/dojo_policy.h>

#include "godot_cpp/classes/project_settings.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "tools/logger.h"
#include "godot_cpp/core/version.hpp"
DojoC* DojoC::singleton = nullptr;


DojoC::DojoC()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        Logger::debug_extra("DojoC", "built in editor mode");
    }
    singleton = this;
    enabled = true;
    init_config();
}

DojoC::~DojoC()
{
    Logger::debug_extra("DojoC", "Destroyed");
    singleton = nullptr;
    enabled = false;
}

void DojoC::init_config(bool reset)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        set_setting("dojo/config/free_controller_on_exit", false, reset);
        set_setting("dojo/config/katana_url", "http://localhost:5050", reset);
        set_setting("dojo/config/torii_url", "http://localhost:8080", reset);

        set_setting("dojo/config/contract_address", "0x0", reset);
        set_setting("dojo/config/world_address", "0x0", reset);

        Dictionary policies_info = {};
        policies_info["name"] = "dojo/config/policies";
        policies_info["type"] = Variant::DICTIONARY;

        // The account default setting is a random one from KATANA
        set_setting("dojo/config/account/address", "0x6677fe62ee39c7b07401f754138502bab7fac99d2d3c5d37df7d1c6fab10819",
                    reset);
        set_setting("dojo/config/account/public_key",
                    "0x3e3979c1ed728490308054fe357a9f49cf67f80f9721f44cc57235129e090f4", reset);
        set_setting("dojo/config/account/private_key",
                    "0x1e8965b7d0b20b91a62fe515dd991dc9fcb748acddf6b2cf18cec3bdd0f9f9a", reset);

        // Format: [VariantType]/[Hint]:[ClassName]
        Dictionary godot_info = Engine::get_singleton()->get_version_info();
        set_setting("dojo/config/policies", Dictionary(), reset);

        if ((int)godot_info["major"] > 4 || ((int)godot_info["major"] == 4 && (int)godot_info["minor"] >= 4))
        {
            // PROPERTY_HINT_DICTIONARY_TYPE = 38 in Godot 4.4+
            policies_info["hint"] = vformat("%d/%d", Variant::DICTIONARY, 38);
            policies_info["hint_string"] = vformat("%s;%s", "String", "String");
            // policies_info["hint_string"] = vformat("%d/%d:%s;%s", Variant::DICTIONARY, PROPERTY_HINT_TYPE_STRING,
            //                                        "String", "String");
        }


        ProjectSettings::get_singleton()->add_property_info(policies_info);

        // set_setting("dojo/config/chain_id", "KATANA", reset);

        set_setting("dojo/config/fixed_point/default", 40, reset);
        set_setting("dojo/config/fixed_point/64", 40, reset);
        set_setting("dojo/config/fixed_point/128", 60, reset);
        set_setting("dojo/config/fixed_point/256", 123, reset);
    }
}

void DojoC::set_setting(const String& setting, const Variant& value, const bool& force)
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    if (force)
    {
        Logger::info("Forcing init of config", setting);
        settings->set_setting(setting, value);
    }
    if (settings->has_setting(setting) == false)
    {
        settings->set_setting(setting, value);
        settings->set_as_basic(setting, true);
        settings->set_initial_value(setting, value);
        Logger::info("Inited config", setting, "with value", value);
    }
    settings->save();
}
