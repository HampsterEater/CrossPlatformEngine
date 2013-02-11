///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "..\Engine\Conditionals.h"

// This file just contains a handful of defines that determine the behaviour
// of the game. Where things are saved, loaded, the name, etc etc.

// The long name is typically used anywhere the user will see it, such as 
// in the game windows titlebar, console, etc.
#define GAME_NAME							"Icarus Test Game"

// The short game name is typically used anywhere where the user is not 
// likely to notice, and the full game name is to long to use. Such as 
// directory names.
#define GAME_NAME_SHORT						"IcarusTG"

// This is the root folder that all raw assets are loaded from, reading
// should never be performed outside of this folder.
#define GAME_ASSET_ROOT_FOLDER				"assets"

// This is the root folder that all game saves are written into.
// No writing should ever be done to the disk except in this folder.
// This folder is rooted to %APPDATA%/GAME_NAME_SHORT
#define GAME_ASSET_SAVE_FOLDER				"saves"

// This is the package file that assets will be loaded from if 
// they are not available in the assets folder.
#define GAME_ASSET_DATA_FILE				"assets.pak"

// If defined then assets can not be loaded from files and will be 
// loaded from the data files instead.
#ifndef DEBUG
#define GAME_ASSET_FORCE_DATA_FILE			1
#endif

// The file name of the configuration file. This is loaded from the 
// root directory that the executable is stored in.
#define GAME_CONFIG_FILE					"game.script"

// This is the file name of the profile configuration file. This is loaded
// from within the save folder.
#define GAME_PROFILE_FILE					"game.script"