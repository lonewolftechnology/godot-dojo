# Project State & Support Matrix

This file details the current state of the project, what features are working, and what is still missing or experimental.

---

## General Status

- **Enum-based calldata is fully supported and safe.**
- **Struct and array calldata is now fully compatible:** you can send any struct as an array containing each of its members in order.
- **Error handling is very basic and still being improved.**
- **Clause types are not implemented yet.**
- **Nothing related to Dojo tokens is implemented yet.**
- **Advanced features or complex entity schemas may require manual code or extensions.**
- **WASM builds should be considered experimental.**
- **Breaking backend/schema changes may require a rebuild.**
- Executing raw events may crash the game.

---

## Recent Changes (as of 2025-07-04)

- Added `DojoHelpers` utility for conversion between float and fixed point for 64, 128, and 256-bit integers (configurable in project settings; support for 128 and 256-bit is experimental).
- Type conversion system cleaned up and refactored for reliability and maintainability.
- Calldata generation refactored: now supports any struct as array; legacy methods removed.
- Improved print logging and event data responses.
- Experimental Android build support added.
- WebAssembly build process improved, including proper RUSTFLAGS configuration and Rust library handling.
- Default project configuration added.
- Various small fixes in demo scenes and build scripts.

---

## Missing/Not Implemented

- Clause type and features that rely on it
- Anything involving Dojo Token primitives:
    - Creating, querying, or managing tokens
    - Token metadata, balances, transfers
    - Token filters in queries
- Advanced query composition and attributes
- Deep inheritance or nested mapping
- Complete error reporting

---

Check back for updates as the project evolves.

For a detailed list of changes, see the [CHANGELOG.md](CHANGELOG.md).