///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

// This file just contains a handful of defines that determine the behaviour
// of the game. Where things are saved, loaded, the name, etc etc.

// This is the root folder that all raw assets are loaded from, reading
// should never be performed outside of this folder.
#define ENGINE_ASSET_ROOT_FOLDER			"assets"

// This is the root folder that all game saves are written into.
// No writing should ever be done to the disk except in this folder.
// This folder is rooted to %APPDATA%/GAME_NAME_SHORT
#define ENGINE_ASSET_SAVE_FOLDER			""

// This is the package file that assets will be loaded from if 
// they are not available in the assets folder.
#define ENGINE_ASSET_DATA_FILE				"assets.pak"

// If defined then assets can not be loaded from files and will be 
// loaded from the data files instead.
#ifndef DEBUG
#define ENGINE_ASSET_FORCE_DATA_FILE		1
#endif

// The file name of the configuration file. This is loaded from the 
// root directory that the executable is stored in.
#define ENGINE_CONFIG_FILE					"game.script"

// This is the file name of the profile configuration file. This is loaded
// from within the save folder.
#define ENGINE_PROFILE_FILE					"game.script"