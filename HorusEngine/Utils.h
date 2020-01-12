#pragma once
#include <string>
#include <boost/locale/encoding.hpp>

#pragma comment(lib, "libboost_locale-vc141-mt-gd-x64-1_71.lib")

inline std::wstring toUtf8(const std::string & s)
{
	return boost::locale::conv::to_utf<wchar_t>(s, "UTF-8");
}