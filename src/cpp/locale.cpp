// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(A)               (sizeof(A)/sizeof((A)[0]))
#endif

LocaleMap::LocaleMap()
{
	// Sniff supported locales
	EnumSystemLocalesEx(LocaleMap::LocaleCallback, LOCALE_ALL, reinterpret_cast<LPARAM>(this), 0);
};

LocaleMap::~LocaleMap()
{
};

// Lookup Windows locale from OpenType language tag
bool LocaleMap::GetWindowsLocale(const std::wstring& openTypeLocale, std::wstring& windowsLocale)
{
	std::map<std::wstring, std::wstring, ci_less>::iterator it;
	std::wstring str = openTypeLocale;

	if (str.size() < 4)
	{
		str.append(4 - str.size(), L' ');
	}

	it = localeMap_.find(str);
	if (it == localeMap_.end())
		return false;

	windowsLocale = it->second;
	return true;
}

BOOL CALLBACK LocaleMap::LocaleCallback(LPWSTR pStr, DWORD dwFlags, LPARAM lparam)
{
	LocaleMap* pThis = reinterpret_cast<LocaleMap*>(lparam);

	std::wstring localeStr = pStr;
	std::wstring opentypeTag;
	int charsRead = 0;
	wchar_t tagBuffer[5] = { 0 };     // 4 chars plus null
	wchar_t const* tag = tagBuffer;

	if (pStr[0] != L'\0')
	{
		// Lookup OpenType tag from this Windows locale
		charsRead = GetLocaleInfoEx(pStr, LOCALE_SOPENTYPELANGUAGETAG, tagBuffer, ARRAY_SIZE(tagBuffer));
	}

	if (charsRead == 5)
	{
		opentypeTag = tagBuffer;

		// If we have two tags save them to a std::map
		pThis->localeMap_.insert(std::make_pair(opentypeTag, localeStr));
	}

	return TRUE;
}