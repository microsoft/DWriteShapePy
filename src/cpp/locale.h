// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

struct ci_less
{
	// case-independent (ci) compare_less binary function
	struct nocase_compare
	{
		bool operator() (const wchar_t& c1, const wchar_t& c2) const {
			return tolower(c1) < tolower(c2);
		}
	};
	bool operator() (const std::wstring& s1, const std::wstring& s2) const {
		return std::lexicographical_compare
		(s1.begin(), s1.end(),   // source range
			s2.begin(), s2.end(),   // dest range
			nocase_compare());  // comparison
	}
};

class LocaleMap
{
public:
	LocaleMap();
	~LocaleMap();

	bool GetWindowsLocale(const std::wstring& openTypeLocale, std::wstring& windowsLocale);

private:
	std::map<std::wstring, std::wstring, ci_less> localeMap_;

	static BOOL CALLBACK LocaleCallback(LPWSTR pStr, DWORD dwFlags, LPARAM lparam);
};
