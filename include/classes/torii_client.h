//
// Created by hazel on 27/05/25.
//

#ifndef TORII_CLIENT_H
#define TORII_CLIENT_H
#include <godot_cpp/godot.hpp>
#include <classes/ref_counted.hpp>
#include <dojo_types.h>

using namespace godot;

class DojoToriiClient : public RefCounted
{
    GDCLASS(DojoToriiClient, RefCounted);

protected:
    dojo_bindings::ToriiClient* client;
    static void _bind_methods();

public:
    DojoToriiClient();
    ~DojoToriiClient();


};

#endif //TORII_CLIENT_H
