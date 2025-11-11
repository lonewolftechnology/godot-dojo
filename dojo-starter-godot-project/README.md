## Overview

This project contains the basic setup required to use the **DojoC GDExtension** in Godot.  
The contract used is a modified version of **Dojo-Starter**.  
This project is **not fully configured** yet.

This document serves as a user guide to complete the final configuration steps needed to run the project.

---

## Setup

First, download the latest version of the **Godot Dojo.c** plugin from:  
`https://github.com/lonewolftechnology/godot-dojo`

Unzip the archive and copy its contents into this project’s files.

---

## Configuration

To run this game, you need to configure the parameters of both the **ToriiClient** and **ControllerAccount**.

There are three possible ways to configure these parameters, listed below in order of precedence:

1. Using the parameters provided directly in each node’s methods.  
2. Setting the parameters from the editor.  
3. Setting them in the **Project Settings**.

In this document, we’ll use the third option — configuring through **Project Settings**, which has the lowest precedence.

### Steps

1. Open the **Project Settings** menu.  
2. Navigate to the **Dojo** section (it should appear near the end of the list).  
3. Configure the following:

   - **Katana URL**: The URL of your Katana server.  
	 You can use:  
	 `https://api.cartridge.gg/x/godot-demo-rookie/katana`

   - **Torii URL**: The URL of your Torii server.  
	 You can use:  
	 `https://api.cartridge.gg/x/godot-demo-rookie/torii`

   - **Torii World Address**: The addresses of the Torii worlds.  
	 This demo only uses the following address:  
	 `0x03a7e28319f3617da4135893c711c79a1306adcd87cca4fbd0ceda50ae397683`

   - **Contract Address**: The address of your contract.  
	 You can use:  
	 `0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da`

   - **Policies**: The policies used by your contract.  
	 The policies used by this contract are:
	 - `"move"`: “Move in the world”
	 - `"spawn"`: “Spawn in the world”
