# Godot Dojo Starter Project

## Overview

This project provides a foundational setup for integrating the **DojoC GDExtension** within a Godot game. It utilizes the vanilla **Dojo-Starter** contract from https://github.com/dojoengine/dojo-starter to showcase basic functionalities. This document serves as a comprehensive guide to understanding and utilizing this project.

## Project Structure

The project is organized into the following directories:

-   **/game**: Contains the core gameplay logic and scenes.
    -   **/entities**: Holds the various game entities.
        -   **/player**: The player scene and associated script.
        -   **/generic**: A generic entity that can be used as a template.
    -   `game.tscn`: The main game scene.
    -   `game.gd`: The script governing the main game logic.
-   **/ui**: Houses user interface elements and scenes.
-   **/assets**: Stores all the visual and audio assets for the project.
-   **/connection**: Manages the connection to the Dojo network, including handling calls and responses.
-   **/tools**: For custom editor tools and utilities.

## Core Concepts

### Scenes and Scripts

-   **game.tscn**: The entry point of the game. It orchestrates the various elements, including the player, enemies, and UI.
-   **game.gd**: The backbone of the game, this script manages game states, player interactions, and communicates with the Dojo network via the scripts in `/connection`.
-   **player.tscn**: The player's scene, containing its visual representation and collision shapes.
-   **player.gd**: This script defines the player's behavior, such as movement and actions. It interacts with `game.gd` to send and receive information from the Dojo network.

### Dojo Integration

The project communicates with a Dojo-powered backend. The `/connection` directory contains the logic for sending transactions and fetching state from the Dojo network. This allows for decentralized game logic and persistent player data.

## Glossary

-   **Dojo**: A provable game engine for building onchain games.
-   **GDExtension**: A Godot feature that allows for the creation of native extensions, in this case, for Dojo integration.
-   **Entity**: Any object in the game, such as the player, an enemy, or an item.
-   **Scene**: In Godot, a scene is a collection of nodes that form a part of the game, like a level or a character.
-   **Script**: A file containing code that defines the behavior of nodes and scenes.

## How to Use

This project is configured by default to use a public deployment on Slot. If you wish to use your own local deployment, follow the steps below.

### Prerequisites
- Godot Engine installed.
- Dojo toolchain installed. Follow the guide at https://book.dojoengine.org/getting-started/quick-start.html.
- **godot-dojo GDExtension**: If the `addons/godot-dojo` directory does not exist, download the latest release from https://github.com/lonewolftechnology/godot-dojo/releases. Unzip it and place the contents into the `addons/godot-dojo` directory within your project.

### Default Configuration (Slot)
The project works out-of-the-box with a pre-deployed contract on the Slot network.
1.  Open the project in Godot.
2.  Run the `game.tscn` scene.

### Local Development Setup
If the public Slot deployment is down or you want to use your own contract:
1.  **Clone dojo-starter**:
    ```bash
    git clone https://github.com/dojoengine/dojo-starter
    cd dojo-starter
    ```
2.  **Start Katana**: Run a local Katana node from the `dojo-starter` directory.
    ```bash
    katana
    ```
    Keep this terminal open.
3.  **Build and Deploy**: In a new terminal, build and deploy the contracts to your local Katana instance.
    ```bash
    sozo build && sozo migrate
    ```
    Take note of the world address and actions contract address from the output.
4.  **Update Connection Settings**:
    - Open the `connection/connection.gd` file in Godot.
    - Replace the `world_address`, `actions_address`, `torii_url`, and `rpc_url` variables with the values from your local deployment.
      - `rpc_url` will be the Katana RPC endpoint (usually `http://0.0.0.0:5050`).
      - `torii_url` will be the Torii endpoint (usually `http://0.0.0.0:8080`).
      - `world_address` and `actions_address` are the addresses you noted from the `sozo migrate` output.
5.  **Run the Game**: Open the project in Godot and run the `game.tscn` scene.

This starter project is designed to be a launchpad. Feel free to expand upon it by adding new entities, game mechanics, and UI elements.
