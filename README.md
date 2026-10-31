# Skyrim Ingredient Exporter

> 🎮 **A personal hobby project — just for fun.**
> This is a weekend Skyrim mod I built for my own alchemy habit. It is **not** a work
> project, not affiliated with or endorsed by my employer, and comes with no warranty or
> support. Play with it, fork it, break it. 🐉

A tiny [SKSE](https://skse.silverlock.org/) plugin for **Skyrim Special Edition** that dumps
your carried alchemy **ingredients** to a JSON file so you (or an AI assistant) can get
potion/alchemy recommendations from it.

## What it does

Press **F8** in-game and the plugin writes every alchemy ingredient in your inventory —
name, count, and its (up to four) effect names — to:

```
Documents\My Games\Skyrim Special Edition\SKSE\IngredientExporter_Inventory.json
```

Example entry:

```json
{ "name": "Blue Mountain Flower", "count": 32,
  "effects": ["Restore Health", "Fortify Conjuration", "Fortify Health", "Damage Magicka Regen"] }
```

The file is overwritten each time you press F8, so it always reflects your current inventory.

> Note: the dump lists *all* of each ingredient's effects, including ones you haven't
> discovered in-game yet. Combining the ingredients still works; the Alchemy menu just
> won't *show* an effect until you've learned it.

## Requirements

- Skyrim Special Edition **1.7.104** (built/tested against this runtime)
- **SKSE64** 2.3.1
- **Address Library for SKSE Plugins** (for the 1.7.x / format-5 database)

## Install (Vortex)

1. Grab `IngredientExporter-*.zip` (from the Releases page, or build it yourself below).
2. In Vortex: **Mods → Install From File**, pick the zip, then **Enable** and **Deploy**.
3. Launch the game through SKSE, load a save, and press **F8**.

The archive installs `SKSE/Plugins/IngredientExporter.dll` into your game's `Data` folder,
so it also works with Mod Organizer 2 or a manual drop into `Data\SKSE\Plugins\`.

## Build from source

The build is standard [CommonLibSSE-NG](https://github.com/alandtse/CommonLibSSE-NG) +
CMake + vcpkg, with two machine-specific notes:

- **MSVC v143 toolset** is required. Newer toolsets (VS2026 / 14.5x) drop an STL symbol the
  dependencies need, so `cmake/x64-windows-static.cmake` pins vcpkg to toolset **14.44**;
  configure from a v143 developer environment (`vcvars64.bat -vcvars_ver=14.44`).
- CommonLibSSE-NG is vendored as a **submodule**, so clone recursively.

```bash
git clone --recursive https://github.com/fullera8/SkyrimIngredientExporter.git
cd SkyrimIngredientExporter
# set VCPKG_ROOT to your vcpkg checkout, and (optional) SKYRIM_FOLDER to auto-deploy the DLL
cmake --preset release
cmake --build build/release
```

The built DLL lands in `build/release/IngredientExporter.dll` (and is copied into
`%SKYRIM_FOLDER%\Data\SKSE\Plugins\` when `SKYRIM_FOLDER` is set).

## License

**GPL-3.0-or-later.** This plugin links [alandtse/CommonLibSSE-NG](https://github.com/alandtse/CommonLibSSE-NG),
which is GPL-3.0-or-later, so the distributed plugin is GPL-3.0 too. See [LICENSE](LICENSE).

## Credits

- [CommonLibSSE-NG](https://github.com/alandtse/CommonLibSSE-NG) (alandtse fork) — the library that makes SKSE plugins possible across game versions.
- [SkyrimScripting SKSE templates](https://github.com/SkyrimScripting/SKSE_Templates) — the starting point.
