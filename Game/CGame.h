///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "..\Engine\Conditionals.h"
#include "..\Engine\CGameEngine.h"

#include "Config.h"

namespace Game
{
	namespace Core
	{

		class CGame : public Engine::Core::CGameEngine
		{
			private:

			protected:
				
				virtual void PrintHeader	();
				virtual void Update			();
				virtual void Render			();
				virtual bool Initialize		();
				virtual bool Deinitialize	();

			public:
				
				CGame						(const u8* game_title="Unnamed Game", const u8* game_title_short="UG");
				
		};

	}
}