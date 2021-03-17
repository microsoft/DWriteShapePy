
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Ignore unreferenced parameters, since they are very common
// when implementing callbacks.
#pragma warning(disable : 4100)

// Modify the following defines if you have to target a platform prior to the ones pecified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#ifndef WINVER                  // Minimum platform is Windows 7
#define WINVER  0x0A00
#endif

#ifndef _WIN32_WINNT            // Minimum platform is Windows 7
#define _WIN32_WINNT  0x0A00
#endif

#ifndef _WIN32_WINDOWS          // Minimum platform is Windows 7
#define _WIN32_WINDOWS  0x0A00
#endif

#define NOMINMAX

#include <string>
#include <map>
#include <algorithm>
#include <vector>
#include <climits>
#include <codecvt>

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <assert.h>
#include <atlbase.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite_2.h>
#include <dwrite_3.h>
#include <string.h>

#include "locale.h"

#include "PostTable.h"
#include "MacPost.h"

#include "TextAnalysis.h"
#include "TextRun.h"

#define likely(expr) (expr)
#define unlikely(expr) (expr)

/* ASCII tag/character handling */
static inline bool ISALPHA(unsigned char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static inline bool ISALNUM(unsigned char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}
static inline bool ISSPACE(unsigned char c)
{
	return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}
static inline unsigned char TOUPPER(unsigned char c)
{
	return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}
static inline unsigned char TOLOWER(unsigned char c)
{
	return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}
static inline bool ISHEX(unsigned char c)
{
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
static inline unsigned char TOHEX(uint8_t c)
{
	return (c & 0xF) <= 9 ? (c & 0xF) + '0' : (c & 0xF) + 'a' - 10;
}
static inline uint8_t FROMHEX(unsigned char c)
{
	return (c >= '0' && c <= '9') ? c - '0' : TOLOWER(c) - 'a' + 10;
}

static inline unsigned int DIV_CEIL(const unsigned int a, unsigned int b)
{
	return (a + (b - 1)) / b;
}


#undef  ARRAY_LENGTH
template <typename Type, unsigned int n>
static inline unsigned int ARRAY_LENGTH(const Type(&)[n]) { return n; }
/* A const version, but does not detect erratically being called on pointers. */
#define ARRAY_LENGTH_CONST(__array) ((signed int) (sizeof (__array) / sizeof (__array[0])))
#define hb_min std::min
#define hb_max std::max

#include "hb-number.hh"
#include "hb-common.h"

#define FS_2BYTE(p)  ( ((unsigned short)((p)[0]) << 8) |  (p)[1])
#define FS_4BYTE(p)  ( FS_2BYTE((p)+2) | ( (FS_2BYTE(p)+0L) << 16) )
#define FS_8BYTE(p)  ( (int64_t)FS_4BYTE((p)+4) | ( (int64_t)(FS_4BYTE(p)+0) << 32) )

#define SWAPW(a)        ((int16_t) FS_2BYTE( (unsigned char *)(&a) ))
#define SWAPL(a)        ((int32_t) FS_4BYTE( (unsigned char *)(&a) ))
#define SWAPQ(a)        ((int64_t) FS_8BYTE( (unsigned char *)(&a) ))

// compile-time endianness swap based on http://stackoverflow.com/a/36937049 
//template<class T, std::size_t... N>
//constexpr T bswap_impl(T i, std::index_sequence<N...>) {
//	return (((i >> N * CHAR_BIT & std::uint8_t(-1)) << (sizeof(T) - 1 - N) * CHAR_BIT) | ...);
//}
//template<class T, class U = std::make_unsigned_t<T>>
//constexpr U bswap(T i) {
//	return bswap_impl<U>(i, std::make_index_sequence<sizeof(T)>{});
//}

//static_assert(bswap<std::uint16_t>(0x1234u) == 0x3412u);
//static_assert(bswap<std::uint64_t>(0x0123456789abcdefULL) == 0xefcdab8967452301ULL);

#ifndef IFR
#define IFR(hr) {HRESULT tempHr = (hr); if (FAILED(tempHr)) return tempHr;}
#endif

// SafeRelease inline function.
template <typename Type> inline void SafeRelease(Type** ppComObject)
{
	if (*ppComObject != NULL)
	{
		(*ppComObject)->Release();
		*ppComObject = NULL;
	}
}
