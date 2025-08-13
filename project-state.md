# Project State & Support Matrix

This file details the current state of the project, what features are working, and what is still missing or experimental.

---

## General Status

- **Enum-based calldata is fully supported and safe.**
- **Struct and array calldata is now fully compatible:** you can send any struct as an array containing each of its members in order.
- **Error handling is very basic and still being improved.**
- **Clause types are not implemented yet.**
- **Advanced features or complex entity schemas may require manual code or extensions.**
- **WASM builds should be considered experimental.**
- **Breaking backend/schema changes may require a rebuild.**
- Executing raw events may crash the game.

---

## Recent Changes (as of 2025-08-12)

- **Account Management:** Implemented a new `Account` class to simplify user management.
- **Simplified Queries:** Introduced the `DojoQuery` resource to make data fetching more intuitive.
- **Controller Filtering:** Added new utility functions to filter controllers by username or address.
- **UI Enhancements:** Implemented off-screen indicators to help locate controllers outside the screen view.
- **Torii Client Update:** The Torii client has been updated with new features and improvements.
- **Demo Overhaul:** The demo has been significantly updated with new features and a reorganized structure.
- **Build System:** The build system has been improved, with updates to CMake and CI configurations.

---

## Missing/Not Implemented

- TESTING
- 
---

Check back for updates as the project evolves.

For a detailed list of changes, see the [CHANGELOG.md](CHANGELOG.md).
