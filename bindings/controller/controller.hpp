#pragma once

#include <algorithm>
#include <bit>
#include <chrono>
#include <cstdint>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <streambuf>
#include <type_traits>
#include <variant>
#include <vector>

#include "controller_scaffolding.hpp"



// Polyfill for std::endian (C++20) when using C++17 or older.
// UniFFI-generated code requires this.
#if __cplusplus < 202002L
#ifndef GODOT_DOJO_STD_ENDIAN_POLYFILL
#define GODOT_DOJO_STD_ENDIAN_POLYFILL
#include <cstdint>
namespace std {
    enum class endian {
#if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        little = 0,
        big = 1,
        native = little
#elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        little = 1,
        big = 0,
        native = big
#else
#  error "Cannot determine endianness"
#endif
    };
}
#endif
#endif

namespace controller {
struct ControllerAccount;
struct Owner;
struct SessionAccount;
struct Call;
struct SessionPolicies;
struct SessionPolicy;
struct ControllerError;
enum class SignerType;
enum class Version;
typedef std::string ControllerFieldElement;


struct Call {
    ControllerFieldElement contract_address;
    std::string entrypoint;
    std::vector<ControllerFieldElement> calldata;
};


struct SessionPolicy {
    ControllerFieldElement contract_address;
    std::string entrypoint;
};


struct SessionPolicies {
    std::vector<std::shared_ptr<SessionPolicy>> policies;
    ControllerFieldElement max_fee;
};


namespace uniffi {
    struct FfiConverterControllerAccount;
} // namespace uniffi

struct ControllerAccount



{
    friend uniffi::FfiConverterControllerAccount;

    ControllerAccount() = delete;

    ControllerAccount(ControllerAccount &&) = delete;

    ControllerAccount &operator=(const ControllerAccount &) = delete;
    ControllerAccount &operator=(ControllerAccount &&) = delete;

    ~ControllerAccount();
    static std::shared_ptr<ControllerAccount> init(const std::string &app_id, const std::string &username, const ControllerFieldElement &class_hash, const std::string &rpc_url, const std::shared_ptr<Owner> &owner, const ControllerFieldElement &address, const ControllerFieldElement &chain_id);
    static std::shared_ptr<ControllerAccount> from_storage(const std::string &app_id);
    static std::shared_ptr<ControllerAccount> new_headless(const std::string &app_id, const std::string &username, const ControllerFieldElement &class_hash, const std::string &rpc_url, const std::shared_ptr<Owner> &owner, const ControllerFieldElement &chain_id);
    ControllerFieldElement address();
    std::string app_id();
    ControllerFieldElement chain_id();
    void clear_last_error();
    ControllerFieldElement delegate_account();
    void disconnect();
    std::string error_message();
    ControllerFieldElement execute(const std::vector<std::shared_ptr<Call>> &calls);
    void signup(const SignerType &signer_type, std::optional<uint64_t> session_expiration, std::optional<std::string> cartridge_api_url);
    void switch_chain(const std::string &rpc_url);
    ControllerFieldElement transfer(const ControllerFieldElement &recipient, const ControllerFieldElement &amount);
    std::string username();

    private:
    ControllerAccount(const ControllerAccount &);

    ControllerAccount(void *);

    void *_uniffi_internal_clone_pointer() const;

    void *instance = nullptr;
};


namespace uniffi {
    struct FfiConverterOwner;
} // namespace uniffi

struct Owner



{
    friend uniffi::FfiConverterOwner;

    Owner() = delete;

    Owner(Owner &&) = delete;

    Owner &operator=(const Owner &) = delete;
    Owner &operator=(Owner &&) = delete;

    ~Owner();
    static std::shared_ptr<Owner> init(const std::string &private_key);

    private:
    Owner(const Owner &);

    Owner(void *);

    void *_uniffi_internal_clone_pointer() const;

    void *instance = nullptr;
};


namespace uniffi {
    struct FfiConverterSessionAccount;
} // namespace uniffi

struct SessionAccount



{
    friend uniffi::FfiConverterSessionAccount;

    SessionAccount() = delete;

    SessionAccount(SessionAccount &&) = delete;

    SessionAccount &operator=(const SessionAccount &) = delete;
    SessionAccount &operator=(SessionAccount &&) = delete;

    ~SessionAccount();
    static std::shared_ptr<SessionAccount> init(const std::string &rpc_url, const std::string &private_key, const ControllerFieldElement &address, const ControllerFieldElement &owner_guid, const ControllerFieldElement &chain_id, const SessionPolicies &policies, uint64_t session_expiration);
    static std::shared_ptr<SessionAccount> create_from_subscribe(const std::string &private_key, const SessionPolicies &policies, const std::string &rpc_url, const std::string &cartridge_api_url);
    std::string address();
    std::optional<std::string> app_id();
    std::string chain_id();
    ControllerFieldElement execute(const std::vector<std::shared_ptr<Call>> &calls);
    ControllerFieldElement execute_from_outside(const std::vector<std::shared_ptr<Call>> &calls);
    uint64_t expires_at();
    bool is_expired();
    bool is_revoked();
    std::string owner_guid();
    std::optional<std::string> session_id();
    std::optional<std::string> username();

    private:
    SessionAccount(const SessionAccount &);

    SessionAccount(void *);

    void *_uniffi_internal_clone_pointer() const;

    void *instance = nullptr;
};

namespace uniffi {
struct FfiConverterControllerError;
} // namespace uniffi

struct ControllerError: std::runtime_error {
    friend uniffi::FfiConverterControllerError;

    ControllerError() : std::runtime_error("") {}
    ControllerError(const std::string &what_arg) : std::runtime_error(what_arg) {}

    virtual ~ControllerError() = default;

    virtual void throw_underlying() {
        throw *this;
    }

protected:
    virtual int32_t get_variant_idx() const {
        return 0;
    };
};
/**
 * Contains variants of ControllerError
 */
namespace controller_error {

struct InitializationError: ControllerError {

    InitializationError() : ControllerError("") {}
    InitializationError(const std::string &what_arg) : ControllerError(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return 1;
    }
};

struct SignupError: ControllerError {

    SignupError() : ControllerError("") {}
    SignupError(const std::string &what_arg) : ControllerError(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return 2;
    }
};

struct ExecutionError: ControllerError {

    ExecutionError() : ControllerError("") {}
    ExecutionError(const std::string &what_arg) : ControllerError(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return 3;
    }
};

struct NetworkError: ControllerError {

    NetworkError() : ControllerError("") {}
    NetworkError(const std::string &what_arg) : ControllerError(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return 4;
    }
};

struct StorageError: ControllerError {

    StorageError() : ControllerError("") {}
    StorageError(const std::string &what_arg) : ControllerError(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return 5;
    }
};

struct InvalidInput: ControllerError {

    InvalidInput() : ControllerError("") {}
    InvalidInput(const std::string &what_arg) : ControllerError(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return 6;
    }
};

struct DisconnectError: ControllerError {

    DisconnectError() : ControllerError("") {}
    DisconnectError(const std::string &what_arg) : ControllerError(what_arg) {}

    void throw_underlying() override {
        throw *this;
    }

protected:
    int32_t get_variant_idx() const override {
        return 7;
    }
};
} // namespace controller_error


enum class SignerType: int32_t {
    kWebauthn = 1,
    kStarknet = 2
};


enum class Version: int32_t {
    kV104 = 1,
    kV105 = 2,
    kV106 = 3,
    kV107 = 4,
    kV108 = 5,
    kV109 = 6,
    kLatest = 7
};

namespace uniffi {struct RustStreamBuffer: std::basic_streambuf<char> {
    RustStreamBuffer(RustBuffer *buf) {
        char* data = reinterpret_cast<char*>(buf->data);
        this->setg(data, data, data + buf->len);
        this->setp(data, data + buf->capacity);
    }
    ~RustStreamBuffer() = default;

private:
    RustStreamBuffer() = delete;
    RustStreamBuffer(const RustStreamBuffer &) = delete;
    RustStreamBuffer(RustStreamBuffer &&) = delete;

    RustStreamBuffer &operator=(const RustStreamBuffer &) = delete;
    RustStreamBuffer &operator=(RustStreamBuffer &&) = delete;
};

struct RustStream: std::basic_iostream<char> {
    RustStream(RustBuffer *buf):
        std::basic_iostream<char>(&streambuf), streambuf(RustStreamBuffer(buf)) { }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    RustStream &operator>>(T &val) {
        read(reinterpret_cast<char *>(&val), sizeof(T));

        if (std::endian::native != std::endian::big) {
            auto bytes = reinterpret_cast<char *>(&val);

            std::reverse(bytes, bytes + sizeof(T));
        }

        return *this;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    RustStream &operator<<(T val) {
        if (std::endian::native != std::endian::big) {
            auto bytes = reinterpret_cast<char *>(&val);

            std::reverse(bytes, bytes + sizeof(T));
        }

        write(reinterpret_cast<char *>(&val), sizeof(T));

        return *this;
    }
private:
    RustStreamBuffer streambuf;
};


RustBuffer rustbuffer_alloc(uint64_t);
RustBuffer rustbuffer_from_bytes(const ForeignBytes &);
void rustbuffer_free(RustBuffer);
template <typename T> struct HandleMap {
    HandleMap() = default;

    std::shared_ptr<T> at(uint64_t handle) {
        std::lock_guard<std::mutex> guard(this->mutex);

        return this->map.at(handle);
    }

    uint64_t insert(std::shared_ptr<T> impl) {
        std::lock_guard<std::mutex> guard(this->mutex);

        auto handle = this->cur_handle;

        this->map.insert({ handle, impl });
        this->cur_handle += 1;

        return handle;
    }

    void erase(uint64_t handle) {
        // We store the object here to avoid re-entrant locking
        std::shared_ptr<T> cleanup;
        {
            std::lock_guard<std::mutex> guard(this->mutex);
            auto it = this->map.find(handle);
            if (it != this->map.end()) {
                cleanup = it->second;
                this->map.erase(it);
            }
        }
    }
    private:
        HandleMap(const HandleMap<T> &) = delete;
        HandleMap(HandleMap<T> &&) = delete;

        HandleMap<T> &operator=(const HandleMap<T> &) = delete;
        HandleMap<T> &operator=(HandleMap<T> &&) = delete;

        std::mutex mutex;
        uint64_t cur_handle = 0;
        std::map<uint64_t, std::shared_ptr<T>> map;
};
struct FfiConverterUInt64 {
    static uint64_t lift(uint64_t);
    static uint64_t lower(uint64_t);
    static uint64_t read(RustStream &);
    static void write(RustStream &, uint64_t);
    static uint64_t allocation_size(uint64_t);
};
struct FfiConverterBool {
    static bool lift(uint8_t);
    static uint8_t lower(bool);
    static bool read(RustStream &);
    static void write(RustStream &, bool);
    static uint64_t allocation_size(bool);
};
struct FfiConverterString {
    static std::string lift(RustBuffer buf);
    static RustBuffer lower(const std::string &);
    static std::string read(RustStream &);
    static void write(RustStream &, const std::string &);
    static uint64_t allocation_size(const std::string &);
};


struct FfiConverterControllerAccount {
    static std::shared_ptr<ControllerAccount> lift(void *);
    static void *lower(const std::shared_ptr<ControllerAccount> &);
    static std::shared_ptr<ControllerAccount> read(RustStream &);
    static void write(RustStream &, const std::shared_ptr<ControllerAccount> &);
    static uint64_t allocation_size(const std::shared_ptr<ControllerAccount> &);
private:
};


struct FfiConverterOwner {
    static std::shared_ptr<Owner> lift(void *);
    static void *lower(const std::shared_ptr<Owner> &);
    static std::shared_ptr<Owner> read(RustStream &);
    static void write(RustStream &, const std::shared_ptr<Owner> &);
    static uint64_t allocation_size(const std::shared_ptr<Owner> &);
private:
};


struct FfiConverterSessionAccount {
    static std::shared_ptr<SessionAccount> lift(void *);
    static void *lower(const std::shared_ptr<SessionAccount> &);
    static std::shared_ptr<SessionAccount> read(RustStream &);
    static void write(RustStream &, const std::shared_ptr<SessionAccount> &);
    static uint64_t allocation_size(const std::shared_ptr<SessionAccount> &);
private:
};

struct FfiConverterTypeCall {
    static Call lift(RustBuffer);
    static RustBuffer lower(const Call &);
    static Call read(RustStream &);
    static void write(RustStream &, const Call &);
    static uint64_t allocation_size(const Call &);
};

struct FfiConverterTypeSessionPolicies {
    static SessionPolicies lift(RustBuffer);
    static RustBuffer lower(const SessionPolicies &);
    static SessionPolicies read(RustStream &);
    static void write(RustStream &, const SessionPolicies &);
    static uint64_t allocation_size(const SessionPolicies &);
};

struct FfiConverterTypeSessionPolicy {
    static SessionPolicy lift(RustBuffer);
    static RustBuffer lower(const SessionPolicy &);
    static SessionPolicy read(RustStream &);
    static void write(RustStream &, const SessionPolicy &);
    static uint64_t allocation_size(const SessionPolicy &);
};

struct FfiConverterControllerError {
    static std::shared_ptr<ControllerError> lift(RustBuffer buf);
    static RustBuffer lower(const ControllerError &);
    static std::shared_ptr<ControllerError> read(RustStream &stream);
    static void write(RustStream &stream, const ControllerError &);
    static uint64_t allocation_size(const ControllerError &);
};
struct FfiConverterSignerType {
    static SignerType lift(RustBuffer);
    static RustBuffer lower(const SignerType &);
    static SignerType read(RustStream &);
    static void write(RustStream &, const SignerType &);
    static uint64_t allocation_size(const SignerType &);
};
struct FfiConverterVersion {
    static Version lift(RustBuffer);
    static RustBuffer lower(const Version &);
    static Version read(RustStream &);
    static void write(RustStream &, const Version &);
    static uint64_t allocation_size(const Version &);
};
struct FfiConverterOptionalUInt64 {
    static std::optional<uint64_t> lift(RustBuffer buf);
    static RustBuffer lower(const std::optional<uint64_t>& val);
    static std::optional<uint64_t> read(RustStream &stream);
    static void write(RustStream &stream, const std::optional<uint64_t>& value);
    static uint64_t allocation_size(const std::optional<uint64_t> &val);
};
struct FfiConverterOptionalString {
    static std::optional<std::string> lift(RustBuffer buf);
    static RustBuffer lower(const std::optional<std::string>& val);
    static std::optional<std::string> read(RustStream &stream);
    static void write(RustStream &stream, const std::optional<std::string>& value);
    static uint64_t allocation_size(const std::optional<std::string> &val);
};

struct FfiConverterSequenceTypeCall {
    static std::vector<std::shared_ptr<Call>> lift(RustBuffer);
    static RustBuffer lower(const std::vector<std::shared_ptr<Call>> &);
    static std::vector<std::shared_ptr<Call>> read(RustStream &);
    static void write(RustStream &, const std::vector<std::shared_ptr<Call>> &);
    static uint64_t allocation_size(const std::vector<std::shared_ptr<Call>> &);
};

struct FfiConverterSequenceTypeSessionPolicy {
    static std::vector<std::shared_ptr<SessionPolicy>> lift(RustBuffer);
    static RustBuffer lower(const std::vector<std::shared_ptr<SessionPolicy>> &);
    static std::vector<std::shared_ptr<SessionPolicy>> read(RustStream &);
    static void write(RustStream &, const std::vector<std::shared_ptr<SessionPolicy>> &);
    static uint64_t allocation_size(const std::vector<std::shared_ptr<SessionPolicy>> &);
};

struct FfiConverterSequenceTypeControllerFieldElement {
    static std::vector<ControllerFieldElement> lift(RustBuffer);
    static RustBuffer lower(const std::vector<ControllerFieldElement> &);
    static std::vector<ControllerFieldElement> read(RustStream &);
    static void write(RustStream &, const std::vector<ControllerFieldElement> &);
    static uint64_t allocation_size(const std::vector<ControllerFieldElement> &);
};
typedef struct FfiConverterString FfiConverterTypeControllerFieldElement;
} // namespace uniffi

bool controller_has_storage(const std::string &app_id);
ControllerFieldElement get_controller_class_hash(const Version &version);
ControllerFieldElement get_public_key(const ControllerFieldElement &private_key);
ControllerFieldElement signer_to_guid(const ControllerFieldElement &private_key);
bool validate_felt(const std::string &felt);
} // namespace controller