//
// Created by hazel on 27/05/25.
//

#include <classes/torii_client.h>

#include <variant/utility_functions.hpp>

void DojoToriiClient::_bind_methods()
{
    UtilityFunctions::print_rich("Bindings Torii");
}

DojoToriiClient::DojoToriiClient()
{
    UtilityFunctions::print("Torii constructor called");
}

DojoToriiClient::~DojoToriiClient()
{
    UtilityFunctions::print("Torii destructor called");
}
