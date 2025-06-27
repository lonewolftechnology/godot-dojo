# Project State & Support Matrix

This file details the current state of the project, what features are working, and what is still missing or experimental.

---

## General Status

- **Enum-based calldata is fully supported and safe.**
- **Simple struct and array handling (basic cases) are supported.**
- **Error handling is very basic and still being improved.**
- **Clause types are not implemented yet.**
- **Nothing related to Dojo tokens is implemented yet.**
- **Advanced features or complex entity schemas may require manual code or extensions.**
- **WASM builds should be considered experimental.**
- **Breaking backend/schema changes may require a rebuild.**
- Executing raw events may crash the game.
- 
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