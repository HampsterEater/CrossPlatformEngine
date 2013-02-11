///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"
#include "Platform.h"

#include "CArray.h"
#include "CScriptLexer.h"
#include "CScriptASTNode.h"

namespace Engine
{
    namespace Scripting
    {
		struct CScriptToken;

		namespace AST
		{

			// Node containing an intrinsic part of an expression.
			class CScriptIntrinsicASTNode : public CScriptASTNode
			{
				private:

				public:
					bool PostFix;

					CScriptIntrinsicASTNode										(Engine::Scripting::CScriptToken token, CScriptASTNode* parent);
					~CScriptIntrinsicASTNode									();
					
					virtual Engine::Containers::CString	 GetName				();
					
					virtual void						 GenerateSymbols		(CScriptGenerator* gen);
					virtual u32							 GenerateInstructions	(CScriptGenerator* gen);

			};

		}
	}
}