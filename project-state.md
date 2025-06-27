## Current Working Features & Limitations

### What Works

- **Enum-based calldata for Calls:**  
  Calls using calldata parameters with enums are stable and tested.
- **Entity/event subscription:**  
  You can subscribe to entities and events; these will trigger your callback as expected.
- **Receiving and parsing enum types:**  
  Enums received from backend/data are parsed and mapped correctly.
- **Basic DojoStruct, Member, and Array support:**  
  Simple structs, DojoMembers, and supported array types are passed and received without crashes.
- **Cross-platform builds via Makefile:**  
  Linux, macOS, Windows, and WebAssembly builds are supported with the provided instructions and Makefile targets.
- **Automatic cache management:**  
  Build system caches and manages SCons/Rust output to speed up repeated builds.
- **Basic demo scene:**  
  The demo can connect, subscribe, send basic enum-based calls, and receive results.

---

### Limitations / Known Issues

- **Calldata Safety:**  
  Only enum types are guaranteed to work safely in calldata.  
  *Passing structs, arrays, or other custom types as calldata parameters is experimental and may fail or cause undefined behavior.*
- **Clause not implemented:**  
  The `Clause` type is not yet implemented. Any features or calls requiring `Clause` will not work at this time.
- **Complex/nested types:**  
  Complex/nested structs or deep arrays may not work as expected in all cases.
- **Dynamic schema changes:**  
  If the backend schema changes at runtime, type mapping may need a project rebuild.
- **Error handling:**  
  Error feedback may be basic or lacking for some operations.
- **WebAssembly:**  
  WASM builds are experimental and not all features may be available.
- **No full support for all Dojo features:**  
  Advanced features (like field attributes, mixed arrays, etc.) might require manual extension in Rust and C++.

---

If your use case fits within the working features (mainly enums for calldata), the project should be stable.  
For any advanced needs, please see the roadmap or open an issue for guidance or feature requests.