//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include "godot_cpp/classes/project_settings.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "tools/logger.h"
#include "godot_cpp/core/version.hpp"
#include "tools/dojo_helper.h"
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
        set_setting("dojo/config/disconnect_controller_on_exit", false, reset);
        set_setting("dojo/config/katana_url", "http://localhost:5050", reset);
        set_setting("dojo/config/contract_address", String, reset);
        // Torii Config
        set_setting("dojo/config/torii/torii_url", "http://localhost:8080", reset);
        set_setting("dojo/config/torii/contract_addresses", TypedArray<String>(), reset);
        set_setting("dojo/config/torii/worlds", TypedArray<String>(), reset);
        // Policies
        Dictionary policies_info = {};
        policies_info["name"] = "dojo/config/policies";
        policies_info["type"] = Variant::DICTIONARY;
        // Account
        set_setting("dojo/config/account/address", "0x0",
                    reset);
        set_setting("dojo/config/account/public_key",
                    "0x0", reset);
        set_setting("dojo/config/account/private_key",
                    "0x0", reset);
        // Policies
        set_setting("dojo/config/policies", Dictionary(), reset);
        // Format: [VariantType]/[Hint]:[ClassName]

        if (DojoHelpers::can_use_typed_dictionaries())
        {
            // PROPERTY_HINT_DICTIONARY_TYPE = 38 in Godot 4.4+
            policies_info["hint"] = vformat("%d/%d", Variant::DICTIONARY, 38);
            policies_info["hint_string"] = vformat("%s;%s", "String", "String");

        }

        ProjectSettings::get_singleton()->add_property_info(policies_info);

        // set_setting("dojo/config/chain_id", "KATANA", reset);
        // Fixed Point
        set_setting("dojo/config/fixed_point/default", 128, reset);
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
