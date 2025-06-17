//
// Created by hazel on 27/05/25.
//

#ifndef TORII_CLIENT_H
#define TORII_CLIENT_H

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/callable.hpp"

#include "dojo_types.hpp"
#include "variant/field_element.h"

using namespace godot;

/**
 * @class ToriiClient
 * @brief Wrapper completo para DOJO::ToriiClient
 *
 * Esta clase proporciona una interfaz Godot completa para interactuar con el cliente Torii
 * del ecosistema Dojo, incluyendo consultas de entidades, metadatos, tokens, suscripciones
 * y publicación de mensajes.
 */
class ToriiClient : public RefCounted {
    GDCLASS(ToriiClient, RefCounted)

private:
    static ToriiClient* singleton;
    DOJO::ToriiClient* client;
    bool is_connected;


protected:
    static void _bind_methods();
    String torii_url;
    String world_address;
    Callable logger_callable;
    DOJO::FieldElement* world;

public:
    ToriiClient();
    ~ToriiClient();

    enum QueryOrderDirection {
        ASC,
        DESC,
      };

    enum QueryPaginationDirection {
        FORWARD,
        BACKWARD,
      };
    // Singleton pattern
    static ToriiClient* get_singleton();

    // Métodos de conexión
    bool create_client(const String& world_addr, const String& torii_url = "http://localhost:8080");
    void disconnect_client();
    bool is_client_connected() const;
    String get_torii_url() const;
    String get_world_address() const;
    bool is_calable_valid();
    void callable_call(const char* msg);
    FieldElement get_world() const;
    void set_world(const FieldElement& n_world);

    // Metadatos del mundo
    Dictionary get_world_metadata();
    bool refresh_metadata();

    // Consulta de entidades
    TypedArray<Dictionary> get_entities(const Dictionary& query_params = Dictionary());

    // Gestión de controladores
    TypedArray<Dictionary> get_controllers(const String& player_address = "");
    Dictionary get_controller_info(const String& controller_address);

    // Tokens y balances
    // TypedArray<Dictionary> get_tokens(const Dictionary& query_params = Dictionary());
    // TypedArray<Dictionary> get_token_balances(const String& account_address);
    // TypedArray<Dictionary> get_token_collections();
    // Dictionary get_token_info(const String& token_address);

    // Suscripciones en tiempo real
    bool create_entity_subscription(const Callable& callback, const Dictionary& filter_params = Dictionary());
    bool create_event_subscription(const Callable& callback, const Dictionary& filter_params = Dictionary());
    bool create_token_subscription(const Callable& callback, const String& account_address);
    void cancel_all_subscriptions();

    // Publicación de mensajes
    bool publish_message(const String& message_data, const Array& signature_felts);
    bool publish_typed_message(const Dictionary& typed_data, const Array& signature_felts);

    // Métodos de consulta avanzados

    // Configuración del cliente
    void set_logger_callback(const Callable& logger_callback);
    Dictionary get_client_info() const;

    // Métodos de utilidad
    Dictionary get_connection_status() const;

    // Métodos internos de conversión
    DOJO::Query create_query_from_dict(const Dictionary& query_params) const;
    DOJO::Pagination create_pagination_from_dict(const Dictionary& pagination_params) const;
    Dictionary entity_to_dictionary(const DOJO::Entity& entity) const;
    Dictionary controller_to_dictionary(const DOJO::Controller& controller) const;
    // Dictionary token_to_dictionary(const DOJO::Token& token) const;

    // Callbacks para suscripciones
    // void on_entity_update_internal(DOJO::FieldElement entity_id, DOJO::CArrayStruct models);
    // void on_event_update_internal(DOJO::Event event);

    // Getters para uso interno
    DOJO::ToriiClient* get_client() const { return client; }
};
VARIANT_ENUM_CAST(ToriiClient::QueryOrderDirection);
VARIANT_ENUM_CAST(ToriiClient::QueryPaginationDirection);

#endif // TORII_CLIENT_H