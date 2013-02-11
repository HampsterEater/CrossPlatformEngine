///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////

#include "..\Engine\Engine.h"
#include "Version.h"

#include "CGame.h"

// ----------------------------------------------------------------------------
//  Platform independent entry point.
//  This function itself does very little, it just boots up the main engine.
// ----------------------------------------------------------------------------
s32 PlatformMain()
{
	Game::Core::CGame game("Icarus Test Game", "IcarusTG");
	return game.Run();
}
