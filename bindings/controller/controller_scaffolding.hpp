#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNIFFI_CPP_INTERNALSTRUCTS
#define UNIFFI_CPP_INTERNALSTRUCTS
struct ForeignBytes {
    int32_t len;
    uint8_t *data;
};

struct RustBuffer {
    uint64_t capacity;
    uint64_t len;
    uint8_t *data;
};

struct RustCallStatus {
    int8_t code;
    RustBuffer error_buf;
};

#endif
void * uniffi_controller_uniffi_fn_clone_controlleraccount(void * handle, RustCallStatus *out_status);
void uniffi_controller_uniffi_fn_free_controlleraccount(void * handle, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_constructor_controlleraccount_from_storage(RustBuffer app_id, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_constructor_controlleraccount_new(RustBuffer app_id, RustBuffer username, RustBuffer class_hash, RustBuffer rpc_url, void * owner, RustBuffer address, RustBuffer chain_id, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_constructor_controlleraccount_new_headless(RustBuffer app_id, RustBuffer username, RustBuffer class_hash, RustBuffer rpc_url, void * owner, RustBuffer chain_id, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_address(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_app_id(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_chain_id(void * ptr, RustCallStatus *out_status);
void uniffi_controller_uniffi_fn_method_controlleraccount_clear_last_error(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_delegate_account(void * ptr, RustCallStatus *out_status);
void uniffi_controller_uniffi_fn_method_controlleraccount_disconnect(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_error_message(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_execute(void * ptr, RustBuffer calls, RustCallStatus *out_status);
void uniffi_controller_uniffi_fn_method_controlleraccount_signup(void * ptr, RustBuffer signer_type, RustBuffer session_expiration, RustBuffer cartridge_api_url, RustCallStatus *out_status);
void uniffi_controller_uniffi_fn_method_controlleraccount_switch_chain(void * ptr, RustBuffer rpc_url, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_transfer(void * ptr, RustBuffer recipient, RustBuffer amount, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_controlleraccount_username(void * ptr, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_clone_owner(void * handle, RustCallStatus *out_status);
void uniffi_controller_uniffi_fn_free_owner(void * handle, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_constructor_owner_new(RustBuffer private_key, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_clone_sessionaccount(void * handle, RustCallStatus *out_status);
void uniffi_controller_uniffi_fn_free_sessionaccount(void * handle, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_constructor_sessionaccount_create_from_subscribe(RustBuffer private_key, RustBuffer policies, RustBuffer rpc_url, RustBuffer cartridge_api_url, RustCallStatus *out_status);
void * uniffi_controller_uniffi_fn_constructor_sessionaccount_new(RustBuffer rpc_url, RustBuffer private_key, RustBuffer address, RustBuffer owner_guid, RustBuffer chain_id, RustBuffer policies, uint64_t session_expiration, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_address(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_app_id(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_chain_id(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_execute(void * ptr, RustBuffer calls, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_execute_from_outside(void * ptr, RustBuffer calls, RustCallStatus *out_status);
uint64_t uniffi_controller_uniffi_fn_method_sessionaccount_expires_at(void * ptr, RustCallStatus *out_status);
int8_t uniffi_controller_uniffi_fn_method_sessionaccount_is_expired(void * ptr, RustCallStatus *out_status);
int8_t uniffi_controller_uniffi_fn_method_sessionaccount_is_revoked(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_owner_guid(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_session_id(void * ptr, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_method_sessionaccount_username(void * ptr, RustCallStatus *out_status);
int8_t uniffi_controller_uniffi_fn_func_controller_has_storage(RustBuffer app_id, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_func_get_controller_class_hash(RustBuffer version, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_func_get_public_key(RustBuffer private_key, RustCallStatus *out_status);
RustBuffer uniffi_controller_uniffi_fn_func_signer_to_guid(RustBuffer private_key, RustCallStatus *out_status);
int8_t uniffi_controller_uniffi_fn_func_validate_felt(RustBuffer felt, RustCallStatus *out_status);
RustBuffer ffi_controller_uniffi_rustbuffer_alloc(uint64_t size, RustCallStatus *out_status);
RustBuffer ffi_controller_uniffi_rustbuffer_from_bytes(ForeignBytes bytes, RustCallStatus *out_status);
void ffi_controller_uniffi_rustbuffer_free(RustBuffer buf, RustCallStatus *out_status);
RustBuffer ffi_controller_uniffi_rustbuffer_reserve(RustBuffer buf, uint64_t additional, RustCallStatus *out_status);
uint16_t uniffi_controller_uniffi_checksum_func_controller_has_storage();
uint16_t uniffi_controller_uniffi_checksum_func_get_controller_class_hash();
uint16_t uniffi_controller_uniffi_checksum_func_get_public_key();
uint16_t uniffi_controller_uniffi_checksum_func_signer_to_guid();
uint16_t uniffi_controller_uniffi_checksum_func_validate_felt();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_address();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_app_id();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_chain_id();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_clear_last_error();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_delegate_account();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_disconnect();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_error_message();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_execute();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_signup();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_switch_chain();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_transfer();
uint16_t uniffi_controller_uniffi_checksum_method_controlleraccount_username();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_address();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_app_id();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_chain_id();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_execute();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_execute_from_outside();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_expires_at();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_is_expired();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_is_revoked();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_owner_guid();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_session_id();
uint16_t uniffi_controller_uniffi_checksum_method_sessionaccount_username();
uint16_t uniffi_controller_uniffi_checksum_constructor_controlleraccount_from_storage();
uint16_t uniffi_controller_uniffi_checksum_constructor_controlleraccount_new();
uint16_t uniffi_controller_uniffi_checksum_constructor_controlleraccount_new_headless();
uint16_t uniffi_controller_uniffi_checksum_constructor_owner_new();
uint16_t uniffi_controller_uniffi_checksum_constructor_sessionaccount_create_from_subscribe();
uint16_t uniffi_controller_uniffi_checksum_constructor_sessionaccount_new();
uint32_t ffi_controller_uniffi_uniffi_contract_version();
#ifdef __cplusplus
}
#endif