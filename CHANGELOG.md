# CHANGELOG

## [v0.0.4-alpha] - 2025-07-10

### Added
- Tokens boilerplate
- Optimized FieldElement generation

### Improved
- Optimized FieldElement::from_string
- Removed custom dojo.c logger
- Fixed calldata generation
- Code cleanup and minor refactors
- Demo reorganization

### Fixed
- Fixed CMake configuration

## [v0.0.3-alpha] - 2025-07-08

### Added
- Double precision support with dedicated build workflow
- `DojoHelpers` can now convert any `float` to fixed point and vice versa, with utility tools for 64, 128, and 256-bit integers (configurable in Project Settings).
- Added default project configuration.
- Experimental build support for Android.
- Initial WebAssembly support: RUSTFLAGS configuration and Rust library handling in SConstruct.

### Improved
- Updated to dojo.c version 1.6.0.alpha.0
- Improved GitHub workflow: tags created from dev branch are now automatically marked as pre-releases
- Cleanup and refactor of type conversions.
- Refactored calldata generation: now supports any struct as an array.
- Improved calldata generation and removed deprecated methods.
- Reduced print spam and added a generic event data response.
- Simplified CMakeLists.
- Various minor fixes and compatibility improvements (MSVC linking, renaming flags, scons adjustments, updated .gitignore).

### Fixed
- Corrected texture region coordinates in certain demo scenes.
- World metadata is now printed upon client connection in the Godot demo autoload.

### Notes
- 128-bit and 256-bit fixed conversions are **not guaranteed to work yet**.

## [v0.0.2-dev]
Initial development release.
