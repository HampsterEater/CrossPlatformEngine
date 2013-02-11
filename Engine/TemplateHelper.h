///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

namespace Engine
{
    namespace Misc
    {

		// Alignment macros, I do so love metaprogramming ;_;
		#if (_MSC_VER >= 1300)
			#define alignof(x) __alignof(x)
		#elif defined(__GNUC__)
			#define alignof(x) __alignof__(x)
		#else
			#ifndef alignof
				#define MINIMUM_ALIGNMENT 4

				template <typename T> struct alignof;

				template <int size_diff>
				struct alignof_sub_helper
				{
					template <typename T> struct Val { enum { value = size_diff }; };
				};

				template <>
				struct alignof_sub_helper<0>
				{
					template <typename T> struct Val { enum { value = alignof<T>::value }; };
				};

				template <typename T>
				struct alignof_helper
				{
					struct Big { T x; u8 c; };

					enum { diff = sizeof (Big) - sizeof (T),
						value = alignof_sub_helper<diff>::template Val<Big>::value };
				};

				#define alignof(t) (alignof_helper<t>::value)
			#endif
		#endif

		// Methods used to work out if we are a pointer type or not.
		typedef u8 IsPointerType;
		typedef u32 IsNotPointerType;

		template<typename T>
		struct IsPointer
		{
			typedef IsNotPointerType Result;
		};

		template<typename T>
		struct IsPointer<T*>
		{
			typedef IsPointerType Result;
		};

	}
}