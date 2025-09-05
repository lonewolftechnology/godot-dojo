# Changelog

## 2025-09-05
* update gdscript testing (7470a14)
* fix negative numbers on i128 (72c6a8b)
* update enums (bd310c6)
* update dojo.c forward declarations (09c0f4d)
* update dojo.c submodule -> v1.7.0-alpha.5 (f256e4a)

## 2025-09-04
* remove unused int256_t (ad479ee)
* missing U256::from_variant (ea6da26)
* fix: made public static STARKNET_PRIME (80085f9)
* Add missing I128 on calldata (e011007)
* simplify big_int creation in one static method using Variant (4ea4f55)
* add floats to bigint (53458b2)
* it:feat(calldata): Use std::deque for iterative argument processing
    A std::deque is used as a worklist to serialize calldata arguments. Its efficiency at adding items to the front (push_front) is key for recursively breaking down complex types (like U256 or Vector3) into their basic components without performance penalties. This provides an elegant, iterative solution for handling nested data structures. (5762d47)
* Add static method to avoid using U256.new() and then initialize it (bc20767)
* fix U256 (6854a70)
* cleanup (8af9f46)

## 2025-09-03
* Make FieldElement and BigInt return PackedByteArray as default when printing (1d17268)
* missing changes to account related to d01d56e98da4b7a22eb352fe5644562a5d7188ca (9ecc1a1)
* update big_int to better handle u256 (21aac5e)
* make calldata array type safe so arrays like PackedByteArray can be used properly (d01d56e)

## 2025-09-02
* minor changes (22a350b)
* fix: crash on due to unsafe asynchronous callback (e54c71e)
* make 128 and 256 integers show as string as default get instead of [Wrapped:0] (baf6f0f)

## 2025-09-01
* simplify testing (021dd3a)
* feat: Starknet Prime as constant and static method for negative conversion (b466017)
* Update slot and demo to Dojo v1.7.0-alpha.3 (bf1bec5)

## 2025-08-30
* revert ldbus linking (31fc364)

## 2025-08-29
* ci: make linux build use pkg-config to link dbus (ee2bfad)
* better rpc_url failsafe (920bdc9)
* add check for rpc_url (ebd1f21)
* fix ControllerAccount null pointer dereference occurred (ad4406e)
* add missing execute_raw to ControllerAccount (47c8196)
* fix crashes when rebuilding gdextension on the background and add in-editor rpc_url field for ControllerAccount (527ae63)
* fix: Crash when creating Call (27bed96)
* update Account testing (056b2f2)
* update demo contract (dcb8e6a)
* fix demo crash by rebuilding contract :shrug: (7d41ddd)
* update test to properly handle validate_fp40 and reconversion (4d448db)

## 2025-08-28
* ci: add crash if ldbus not found on linux build (3b981dd)
* update tests (ea4b06d)
* better memory handling for FieldElement (f4b8978)
* fix floating point not using shift (e44d2fb)
* properly convert int to FieldElement + support floats inside arrays (de2ebcf)
* fixed crash in big int conversion (3b678dd)
* ci: possibly fix linux dbus (9b77233)
* test: bigint to felt (d63f55a)
* test: bigint as RefCounted (34962c4)
* feat: re-add Vector2, Vector3, Vector2i, Vector3i auto formatting for calldata (58a3550)
* sync Account tests output panels (1928520)
* Account: improved testing (ec68031)
* minor changes (2df296b)
* fix: properly handle negative numbers (0461576)

## 2025-08-27
* minor calldata refactor (11e46eb)
* chore: update dojo.c forward declarations (a0b31c7)
* feat: refactor Call to static helper method to keep parity between ControllerAccount and Account execute methods (26be7b5)

## 2025-08-26
* Scons: Remove forcing macos x86_64 to 1.85.0 rust toolchain (846c922)
* ci: revert toolchain to stable (cbca6b9)
* ci: force rust toolchain to 1.87.0 (3be97cf)
* bump: dojo.c -> v1.7.0.alpha.3 (76623b8)
* improve account Testing (19aaa5a)
* DojoHelpers: expose double_to_variant_fp and variant_to_double_fp (8a27cb3)
* update default fixed point presition to 40 (659742e)
* DojoHelpers: fix wrongly getting settings (2251bd8)

## 2025-08-25
* fix: Account failing all execute (1fbc80b)
* minor changes to account testing (aa51f03)
* reduce testing to spawn (debug) (71bb472)
* Account: print chain_id and address for easier debugging (55b980f)
* add use slot or use local katana/torii for unittesting (30cc77b)
* Account: better checks management and reduced spam (6b10011)
* Better success/error signals for Account (dfde1ff)
* Simple UnitTesting (05d41c5)
* minor changed to account (cf23cda)

## 2025-08-24
* Refactor: Account (deca76a)

## 2025-08-20
* update test/abi generator (4c5d350)

## 2025-08-19
* ControllerAccount: update float to use double_to_variant_fp (40c2254)
* update dojo.c with controller's account_sdk custom debug log (82fdfe2)
* update ci (f7310d0)

## 2025-08-18
* update tag (bff7df3)
* fix submodule path (32d889c)
* update dojo.c (9fe46bc)
* fix windows build (5233872)
* use custom dojo.c with logger/debugger (e7422c2)
* update ci (32ce279)
* update submodule (606daa2)
* fix selector freeing char pointer (5011ad6)
* wip: configurator/bindings generator from abi (6d7608b)
* update demo to address updated execute_from_outside (abbaa4c)
* chore: update gitignore to address plugin to addons folder (23cad14)
* update ControllerAccount to handle floats, i128, u128 and u256 (c04e619)
* wip web build (5e3059d)
* demo cleanup (7b35ed9)
* feat: reworked ToriiClient + update subscriptions (only for the ones that are implemented in dojo.c) (fbac72c)
* missing file -> 797d19d49526ebe2508bf3cd0c1dd534555148db (c51d289)
* chore: cleanup DojoHelpers (797d19d)
* feat: added update callback to DojoSubscription (c070af5)
* use custom dojo.c with logger (42d2941)
* update dojo.c -> 1.6.1 (9cf6738)

## 2025-08-14
* chore: update buildsystem to output to addons/godot-dojo instead of bin folder (e541549)
* cleanup and fixed subscriptions (d2d8c33)
* fix demo to use new slot (katana 1.6.3, torii 1.6.1) (a3ca1d4)
* update demo (1e4c85c)

## 2025-08-13
* update Rust toolchain on CI to remove 'set-output' deprecation warning (ffeb3bd)
* chore: reduce clutter in release body (6ee653a)
* feat: update ci to show dojo.c version on release title (3911036)
* update CHANGELOG (4e77e7e)
* fix Queries (aadecb0)
* update Queries handling (80f3c93)

## 2025-08-12
* cleanup (3736912)
* chore: Update build system, configuration and documentation (3287d17)
* feat: Update demo and tools (714cc52)
* feat: Update Torii client (fc29659)
* feat: Implement Account class (0b36be7)
* refactor: Reorganize Dojo resource files (ed604cbd)

## 2025-08-01
* minor refactor to felt arrays (d0fb209)
* added utility functions to filter controllers by usernames or addresses (b027a57)
* update includes paths (811421e)
* add off screen indicator to find controllers outside screen (933af19)
* remove temp folder (9c73480)

## 2025-07-29
* Minor refactor to entities and spawning (a1919fe)

## 2025-07-28
* ArrayDojo -> DojoArray + working filtering controllers by username (d62d9f9)
* remove duplicated lines (6d64e65)
* fix crash of null queries by populating them with its tags (b7b698c)
* wip experimental integers (cd63403)
* ignore android folder on demo (35e4d18)
* fix calldata + fix move call on demo (78eaf1c)
* missing update policies resource (05fdbfd)
* update demo
    - default dojo starter to match slot katana 1.6.3 torii 1.6.0
    - removed/disabled debug/testing on main scene (201a911)
* partial upgrade to dojo.c 1.6.0 (Queries are all null for now) (3ebbc72)

## 2025-07-25
* Some ui cleanup (8d43890)
* Changed buttons types
    changed background asset (ee91be5)
* Removed autoload Connection to a regular scene
    Fixed spawning only correct entities (df1e63b)

## 2025-07-16
* update ui (c3eea95)

## 2025-07-14
* remove unused header (91caedf)
* cleanup (1111de3)
* u256/u128 boost/Num testing (a43788e)
* update new world addr (a69f651)
* update cmake (c0740ba)

## 2025-07-12
* Merge remote-tracking branch 'origin/boost-big-numbers' into dev (920b9e0)

## 2025-07-11
* message testing (64694cb)
* feat: messages (fce4f79)
* DojoQuery resource (6ffe02b)
* controllers filtering from api testing (8fcdfa8)
* update readme (f03d235)
* minor changes, prepping major refactor to build query with a resource or dictionary (e2fc05e)
* adding boost multiprecision (c3a9574)
* compiles using boost::multiprecision (e30e757)
* adding convertors using boost (4293590)

## 2025-07-10
* rollback dojo.c -> 1.5.9 (179d007)
* update ci to make simpler changelog (2d05b15)
* update changelog (62638a8)
* remove custom dojo.c logger (2812597)
* fix calldata (33973cf)
* cleanup (dfe81f9)
* minor refactors + optimized FieldElement::from_string (1ad80ca)
* Tokens boilerplate (1f187ed)
* optimized felt generation (36140ae)
* minor changes (33fd787)
* demo reorg (047bfed)
* fix cmake (12e8481)
* minor changes (aee3ad6)

## 2025-07-08
* minor changes + added num lib (679e3ce)
* remove depecrated workflow (b28f105)
* 1.6.0.alpha.0 changelog update (1f52352)
* if branch = dev and new tag: pre-release (2a74555)
* update cmake (7720d83)
* update to 1.6.0.alpha.0 (e83c77d)
* update dojo.c 1.6.0.alpha.0 (e72810a)
* Merge branch '1.6.0_wip' into dev (7751792)
* :shrug: workflows deserve to look good (4e215ce)
* workflow matrix refactor (a4338ab)
* workflow testing (6c751f2)
* minor fixes + combine github actions (02c7899)
* update changelog (b1f1019)
* add double precision (66430aa)
* update changelog (5e788e8)
* add double precision (2712957)
* update dojo.c to latest dev 1.6.0 (a43a9be)
* temp revert to test dojo.c 1.5.9 (9722d07)
* Revert "latest support dojo.c -> hardcoded empty PageController (retrieves all controllers)"
    
    This reverts commit 2e25b92f20d5e65abb5b4ffee216e14b2547225b. (1885138)

## 2025-07-07
* latest support dojo.c -> hardcoded empty PageController (retrieves all controllers) (2e25b92)
* updates to 1.5.8 + world config for 1.6.0.alpha.1 (3f3b51d)
* revert 1.6.0 changes (6ed1ba5)
* rollback dojo.c (7146991)
* checkout dojo.c (ef27432)
* initial changes (ee4fdba)

## 2025-07-05
* remove act build (8ba400c)
* final changes to ci (8af6e1c)
* remove unnecessary cp on ci (311a9dd)
* possibly finally fix ci (2fe21e1)
* add missing protoc dependency to windows and macos ci (294a8c2)
* update scons (0346348)
* update ci dependecies (fc2b17a)
* add cache to ci (176b9b0)
* update ci (80f6fce)
* update scons (14d6166)
* update ci (25f845c)
* update github workflow and readme (33ec64c)

## 2025-07-04
* set default fixed point precision to 24 (763da8e)
* Possibly more reliable fixed point conversion (bc32a35)
* update changelog + project state (8dd5a03)
* added DojoHelpers, can convert any float to fixed point and vice-versa with utility tools for 64, 128, 256 integers using config in project settings
    ⚠️ 128 and 256 not guaranteed to work, yet (4aa8560)
* types conversion cleanup (4c227c8)
* add default config (a5e7080)
* .idea (8e3c57c)

## 2025-07-02
* calldata refactor, should support any struct as array (1e5ec40)
* proper calldata generation + removed deprecated methods (4824c11)

## 2025-07-01
* wip DojoHelpers (hopefully fixes calldata array) (9b07c3f)

## 2025-06-30
* reduce prints spam + generic event data response (07c1da5)
* minor cahnges (dcc67bd)
* add android experimental build (6d34b41)
* simplify cmakelists (82534e3)
* update gitignore (c7daafc)

## 2025-06-27
* fix msvc linking (3046778)
* disable renaming dojo_c.dll (7c8be97)
* update scons (72c4caa)
* readd missing linkflag to windows msvc build (414d707)
* update scons (f167ee4)
* add note about raw event may crash instance (14a3323)
* add makefile note (130f219)
* change build usage for scons (688e645)
* update readme (d2e608b)
* simplify project state (2f8c549)
* update readme (553df2d)
* minor fix (39a649a)
* update readme (5240a3f)
* fix moves on demo (3d64670)
* Added awaits after connection
    Spawning and moving controllers (b9c5dc3)
* fix player movement (aa5d872)
* make status indicators more visible (5e4a9ba)
* fix wrong entity id serialization + cleanup + extra logs (b2819ad)
* Merge remote-tracking branch 'origin/demo_refactor' into FullRefactor (49b17e9)
* username and address (2b472a9)
* Merge branch 'demo_refactor' into FullRefactor (8e25372)
* New autoload Connection
    Demo game (44afd04)
* minor changes (67c98e4)
* fix move call (6e84e6c)
* update demo + fix missing data names (0a283d1)
* fix updates without data name (cb130e3)
* fix targets (8e7f2b3)
* act support for local github action testing (33c254f)
* cleanup (0fb3d8b)
* add arm64 support for act (local build github actions) (1e6b3ef)
* fix ci (525fc0c)
* update ci (3a39bed)
* fix submodules not initializing (eb2b437)

## 2025-06-26
* fix needing requeriments.txt (476f635)
* python requeriments (scons) (cf416a7)
* update buildsystem, possible github using cache for faster builds (e7cbda6)
* use msvc to build windows (47e54c0)
* remove linux arm build (18ddd3a)
* update makefile and github workflow (eaa3c1b)
* added more tests to demo (cf7db39)
* minor refactor (469f352)
* move chain_id to Controller (6139899)
* github actions - draft (f67d013)
* update demo (fd14b54)
* completed enough array to variant (1edcccb)

## 2025-06-25
* update gdextension template (1c182bf)
* minor change to scons (35d54d9)
* update features draft (10e07f8)
* cleanup (cc54c6d)
* expanded DojoArray support (3b8fab2)
* update windows dll to use debug/release dojo_c dependency (be2cb9f)
* fix windows build (b9d4530)
* removed logger macros (c627498)
* features draft (c0ac507)
* reconnect signals (5738aec)
* policies and calls as resources on disk (5c5bb3e)
* [ArrayDojo] added CArrayFieldElement Constructor (23cb465)
* minor changes (1f04fe7)
* missing header (fe1bb3a)

## 2025-06-24
* enum support + minor changes/cleanup (9ed2a1b)
* debug_macros.h -> tools/logger.h (8fc3c6b)
* fix wrong dictionary idx (779bae2)
* update demo project (b02ec71)
* minor change (1f6e801)
* update classes to new data structure (70c5b0f)
* Refactor types (b693623)
* minor update on debug macros (a65dc57)

## 2025-06-23
* Fixed subscriptions + cleanup (cb3a6c6)
* fix event submission (dac5db6)

## 2025-06-22
* Policies and Calls as Resources (9f452cb)

## 2025-06-21
* rollback to dojo.h and minor refactor (87bb144)
* simplified scons (4447c1d)

## 2025-06-19
* wip refactor demo proyect (2196fd9)
* debug macros + controller account (5fc38c8)
* types (aea9831)
* simplyfied build system? (25ee8fa)

## 2025-06-17
* fix slow msvc checking for rust (fe919f5)
* remove redundant scons code (337440a)
* possibly final scons refactor (261d25f)
* remove quiet cargo check --target (d15cd5c)
* robust msvc detection (5b06778)
* fix missing declaration (5ada422)
* possibly fix msvc detection (bd37b51)
* simplify msvc detection to just trust rust (a978efb)
* potencially fix colors on scons print under windows (6f644aa)
* fix wrongly patched scons (3f253fb)
* fix msvc detection and forced if found on windows (db12fe7)
* Reworked and over-engineered Sconstruct.
    Now the build pipeline is:
    - Compiles Rust dojo.c library with proper target detection
    - Patches dojo.hpp with missing struct declarations
    - Builds godot-cpp and integrates its environment
    - Compiles GDExtension library with recursive source discovery
    - Generates .gdextension configuration files
    - Supports Linux, Windows (MSVC/MinGW), and macOS builds
    - Includes clean mode with cargo clean integration
    - Auto-detects platform/architecture with manual override options (e8aa83d)
* separate getting metadata button for testing (f6f9f4d)
* cleanup + migrated to dojo.hpp (cb98b14)
* dojo.c and godot-cpp as submodules (f05127e)

## 2025-06-13
* minor refactor on demo (75ed430)
* implement ToriiClient Class with partially complete support to create Query from Dictionary (472b6f2)
* update toriiclient name class (8373e88)
* remove ALL hardcoded code (e29eb55)
* field element utility get_as_string (e11e79c)
* update controller account (d46819c)
* Refactor wip (0b172d2)

## 2025-06-11
* remove extra frame awaits (f80c348)
* Fix spawn controllers on login (967e3da)
* Almost fully working multiplayer (needs to get position of other players) (5820f19)
* allow fetch client metadata + return controllers data (b30fa3f)
* update debug macros (613f52a)

## 2025-06-10
* added custom tagged debug log (9d3add5)
* fix map offset (b08d9b5)
* fix macros formatting (0f86622)
* adding zoom and drag to camera (0e8db57)
* disable get_entities - crashes on windows (24b8b03)
* fix build on windows msvc (679ed79)
* fix player updating on same event (69afddc)

## 2025-06-09
* fix movement (40bf650)
* ui overhaul (8cd7ad5)
* cleanup + extended api (fd80ca1)
* fixed missing last changes
    parallax (84909ef)
* fix player movement (79fe2ca)
* player in demo (748ed9f)
* fixed wrong node paths (e30c448)
* missing vector2 update (17a18a7)
* minor cleanup demo project (803564e)
* updated logging macros + minor change (c03c411)
* update dojo.c 1.5.5 -> 1.5.7 (765ecba)
* fix crash when abi file not found (a9ab247)
* fix c++17 constructors (f93b499)
* Fix msvc build (82f4be7)
* update libname under msvc (abfaa76)
* Fix build + linking on mingw (547d8d9)

## 2025-06-08
* linker changes, still not linking on windows (e1f0ba9)
* compiles on windows with msvc (75a08ef)

## 2025-06-06
* WIP hardcoded events + Callable from gdscript (aefb70c)

## 2025-06-04
* minial cleanup + move action (7ee426e)
* draft types (7839086)
* updated demo project with move option (0b1858b)
* wip generic status indicator (a4f3555)

## 2025-05-30
* possible fix printing crash (681a476)
* update cmakelist (10722b3)
* Updated demo for working spawning events (43584be)
* Can send event through library (abf68aa)

## 2025-05-28
* addded missing sources (46dd862)
* compiles with scons (d1c01e4)
* cleanup + updated PackedByteArray conversion (0ce896f)
* cleanup + FieldElement to PackedByteArray (92f482e)

## 2025-05-27
* Draft Primitives to Variant + added sozo to export in demo project (3b918f6)

## 2025-05-24
* - fixed issue loding library on osx, changed library name to libdojoc
    - fixed comple issue on osx, some std:: dependency not found, converted function field_element_to_hex to use pure godot calls (bed7f72)

## 2025-05-23
* :shrug: Super Hyper Mega Inneficient & Hardcoded type filtering (21b0634)
* added easier testing by executing sozo (d805dd5)

## 2025-05-21
* entities fetching + update cmake (010e916)
* enable full stacktrace on rust (5a34abc)

## 2025-05-20
* Account creation + address receival (ee3c8bf)
* Experiments + debug env for rust stacktrace (6f97e64)

## 2025-05-19
* add dbus flag only to linux (5adc6e0)
* added account testing button (86412c8)
* updated readme (8ef7495)
* Testing session creation (a250d76)
* update cmake (254cbe9)
* add macos targets (2334dca)

## 2025-05-17
* fix ignored .gdextension (scons) (6af618a)

## 2025-05-16
* Add demo project info to readme (da35eeb)
* update readme (9982bc3)
* fix readme naming (150454b)
* Add readme tuto (63c7172)
* fix linking issue on scons build (e47d178)
* Godot Demo Project (fe9ad93)
* GDextension implementation (0d8dbd5)
* update gitignore (24157b3)
* CmakeConfig + Scons (2db557e)
* Initial commit (1fc565c)