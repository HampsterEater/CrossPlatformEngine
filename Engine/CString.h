///////////////////////////////////////////////////////////////////////////////
//  Icarus Game Engine
//  Copyright © 2011 Timothy Leonard
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Conditionals.h"

#include "TemplateHelper.h"

#include <cstdarg>

namespace Engine
{
	namespace Memory
	{
		namespace Allocators
		{
			class CProxyAllocator;
		}
	}

    namespace Containers
    {
		template <typename t> class CArray;

        #define STRING_ALLOC_START                  64
        #define STRING_ALLOC_INTERVAL               2.0f
        #define STRING_FORMAT_MAX_ARGUMENT_LENGTH   64

        // Handy conversion macro so you don't have to type CString each time you convert a u8 array :3
        #define S(x) Engine::Containers::CString(x)

		// Allocators!
		extern Engine::Memory::Allocators::CProxyAllocator* g_string_allocator;
		void InitStringAllocator();
		void FreeStringAllocator();
		inline Engine::Memory::Allocators::CProxyAllocator* GetStringAllocator() { return Engine::Containers::g_string_allocator; }

        // Our lovely ex-string class :3
        // It's like std::string but far more awesome!
        class CString
        {
            protected:

                u8*        _data;
                u8         _startBuffer[STRING_ALLOC_START]; // This allocates initial space for us on the stack, much better than a call to new :3
                u32        _length;
                u32        _allocated;

                // Helper functions!
                void         Initialize     ();
                void         Allocate       (u32 size, bool keepOld=false);
                void         CopyFrom       (const u8* buffer, u32 length);
                void         Append         (const u8* buffer, u32 length);
                void         Append         (const CString& a);
                static bool  Compare        (const u8* a, u32 alen, const u8* b, u32 blen);

            public:

                // Other type to string constructors!
                ~CString                           ();
                CString                            ();
                CString                            (const u8*          v, u32 length);
                CString                            (const u8*          v);
                CString                            (const bool         v);
                CString                            (const u8           v);
                CString                            (const s32          v);
                CString                            (const u32          v);
                CString                            (const s64          v);
                CString                            (const u64          v);
                CString                            (const f32         v);
                CString                            (const CString&    v);
                CString                            (const u8           v, u32 length);
                CString                            (const CString&    v, u32 length);
                CString                            (const CString&    v1,const CString& v2, const CString& v3="", const CString& v4="", const CString& v5="", const CString& v6="", const CString& v7="", const CString& v8="", const CString& v9="", const CString& v10="");

                // Properties.
                bool            Empty               ()               const { return _length <= 0; }
                u32             Length              ()               const { return _length; }
                const u8 *      c_str               ()               const { return _data; }

                // Conversion to other type operators.
                operator        const u8 *        (void)             const { return c_str(); }

                // Operators, delicious operators!
                const u8        operator[]          (u32 index) const;
                const u8        operator[]          (s32 index) const;
                void            operator=           (const CString &str);
                void            operator=           (const u8* text);

                friend CString operator+           (const CString& a, const CString& b);
                friend CString operator+           (const u8* a,       const CString& b);
                friend CString operator+           (const CString& a, const u8* b);
                friend CString operator+           (const CString& a, const bool b);
                friend CString operator+           (const CString& a, const f32 b);
                friend CString operator+           (const CString& a, const s32 b);
                friend CString operator+           (const CString& a, const u32 b);
                friend CString operator+           (const CString& a, const s64 b);
                friend CString operator+           (const CString& a, const u64 b);
                friend CString operator+           (const CString& a, const u8 b);

                CString&       operator+=          (const CString&  a);
                CString&       operator+=          (const u8*        a);
                CString&       operator+=          (const f32        a);
                CString&       operator+=          (const u8         a);
                CString&       operator+=          (const s32        a);
                CString&       operator+=          (const u32        a);
                CString&       operator+=          (const s64        a);
                CString&       operator+=          (const u64        a);
                CString&       operator+=          (const bool       a);

                friend bool     operator==          (const CString& a, const CString& b);
                friend bool     operator==          (const CString& a, const u8*       b);
                friend bool     operator==          (const u8*       a, const CString& b);
                friend bool     operator!=          (const CString& a, const CString& b);
                friend bool     operator!=          (const CString& a, const u8*       b);
                friend bool     operator!=          (const u8*       a, const CString& b);

                // General string manipulation methods.
                CString        ToLower             () const; 
                CString        ToUpper             () const;
                s32             ToInt               () const;
                f32             ToFloat             () const;
                bool            ToBool              () const;
                u32             ToHashCode          () const;

                CString        TrimStart           (const CString& a=" \t\v\n\r\f\b\0", bool aschars=true) const;
                CString        TrimEnd             (const CString& a=" \t\v\n\r\f\b\0", bool aschars=true) const;
                CString        Trim                (const CString& a=" \t\v\n\r\f\b\0", bool aschars=true) const;

                CString        SubString           (u32 start, u32 length=0) const;
                CString        Start               (u32 length) const;
                CString        End                 (u32 length) const;
                u8              Last                () const;
                u8              First               () const;

                CString        PadStart            (u32 length, const CString& fill=" ");
                CString        PadStart            (u32 length, const u8 fill=' ');
                CString        PadEnd              (u32 length, const CString& fill=" ");
                CString        PadEnd              (u32 length, const u8 fill=' ');

                CString        Repeat              (u32 count);
                CString        Fill                (u8 c);
                CString        Fill                (const CString& s);

                s32             IndexOf             (const CString& a, u32 start=0, u32 length=0);
                s32             LastIndexOf         (const CString& b, u32 start=0, u32 length=0);
                s32             IndexOf             (u8 a, u32 start=0, u32 length=0);
                s32             LastIndexOf         (u8 b, u32 start=0, u32 length=0);

//                s32             IndexOfAny          (CString a[], u32 size, u32 start=0, u32 length=0);         // TODO: Replace with CArray
//                s32             LastIndexOfAny      (CString b[], u32 size, u32 start=0, u32 length=0);         // TODO: Replace with CArray
//                s32             IndexOfAny          (u8 a[], u32 size, u32 start=0, u32 length=0);              // TODO: Replace with CArray
 //               s32             LastIndexOfAny      (u8 b[], u32 size, u32 start=0, u32 length=0);              // TODO: Replace with CArray
                s32             IndexOfAny       (Engine::Containers::CArray<CString>, u32 start=0, u32 length=0, CString* deliminator=NULL);
                s32             LastIndexOfAny   (Engine::Containers::CArray<CString>, u32 start=0, u32 length=0, CString* deliminator=NULL);

                s32             IndexNotOf          (const CString& a, u32 start=0, u32 length=0);
                s32             LastIndexNotOf      (const CString& b, u32 start=0, u32 length=0);
                s32             IndexNotOf          (u8 a, u32 start=0, u32 length=0);
                s32             LastIndexNotOf      (u8 b, u32 start=0, u32 length=0);

               // s32             IndexNotOfAny       (CString a[], u32 size, u32 start=0, u32 length=0);
               // s32             LastIndexNotOfAny   (CString b[], u32 size, u32 start=0, u32 length=0);
               // s32             IndexNotOfAny       (u8 a[], u32 size, u32 start=0, u32 length=0);
               // s32             LastIndexNotOfAny   (u8 b[], u32 size, u32 start=0, u32 length=0);
                s32             IndexNotOfAny       (Engine::Containers::CArray<CString>, u32 start=0, u32 length=0, CString* deliminator=NULL);
                s32             LastIndexNotOfAny   (Engine::Containers::CArray<CString>, u32 start=0, u32 length=0, CString* deliminator=NULL);

                CString        Remove              (u32 start, u32 length);
                CString        Insert              (const CString& a, u32 start);

                s32             Count               (const CString& a, u32 start=0, u32 length=0);
                s32             Count               (u8 a, u32 start=0, u32 length=0);

                CString        Replace             (const CString& a, const CString& b, u32 start=0, u32 length=0);
                CString        Replace             (u8 a, u8 b, u32 start=0, u32 length=0);

                CString        Limit               (u32 length);

                bool            IsAlphaNumeric      ();
                bool            IsAlpha             ();
                bool            IsNumeric           ();
                bool            IsUppercase         ();
                bool            IsLowercase         ();
                bool            IsWhitespace        ();
                bool            IsHex               ();

                bool            HasAlpha            ();
                bool            HasNumeric          ();
                bool            HasPunctuation      ();
                bool            HasUppercase        ();
                bool            HasLowercase        ();
                bool            HasWhitespace       ();
                bool            HasHex              ();

                //CArray<CString>        Split               (const CString& delims=" ", u32 maxSplits=0, bool treatAsChars=false, bool ignoreDuplicates=false);
                //CArray<CString>        Split               (u8 delims[], u32 delimCount, u32 maxSplits=0, bool treatAsChars=false, bool ignoreDuplicates=false);         // TODO: Replace with CArray
                Engine::Containers::CArray<CString>        Split               (Engine::Containers::CArray<CString> delims, u32 maxSplits=0, bool ignoreDuplicates=false);
                Engine::Containers::CArray<CString>        Split               (u8 split, u32 maxSplits=0, bool ignoreDuplicates=false);

                //CString                Join                (CString bits[], u32 bitsCount);
                //CString                Join                (u8 bits[], u32 bitsCount);
                CString                                 Join                (Engine::Containers::CArray<CString> bits);

                // .Format(x,y,z) - We need to use templates here because there is no such thing in C++ as variable arguments with no named params :(
                template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
                CString Format(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8, T9 a9)         { return FormatString(*this, a1, a2, a3, a4, a5, a6, a7, a8, a9); }
                template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
                CString Format(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7, T8 a8)                { return FormatString(*this, a1, a2, a3, a4, a5, a6, a7, a8); }
                template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
                CString Format(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)                       { return FormatString(*this, a1, a2, a3, a4, a5, a6, a7); }
                template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
                CString Format(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6)                              { return FormatString(*this, a1, a2, a3, a4, a5, a6);}
                template <typename T1, typename T2, typename T3, typename T4, typename T5>
                CString Format(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)                                     { return FormatString(*this, a1, a2, a3, a4, a5); }
                template <typename T1, typename T2, typename T3, typename T4>
                CString Format(T1 a1, T2 a2, T3 a3, T4 a4)                                            { return FormatString(*this, a1, a2, a3, a4); }
                template <typename T1, typename T2, typename T3>
                CString Format(T1 a1, T2 a2, T3 a3)                                                   { return FormatString(*this, a1, a2, a3); }
                template <typename T1, typename T2>
                CString Format(T1 a1, T2 a2)                                                          { return FormatString(*this, a1, a2); }
                template <typename T1>
                CString Format(T1 a1)                                                                 { return FormatString(*this, a1); }

                // Helper functions
                static CString FormatString        (CString a, ...);
                static CString FormatStringVA      (CString a, va_list&);
/*
                CList          Tokenize             ();
                s32             ToCRC                ();
                CString        ToMD5                ();
                CString        ToSHA1               ();
                CString        ToSHA256             ();

                CString        FormatNumber
                CString        FormatTime
*/
        };

    }

}

