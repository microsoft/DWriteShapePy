// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

class PostTable
{
public:
	PostTable(const void* postData, uint32_t postSize) : postData_(postData), postSize_(postSize)
	{
	}
	~PostTable() {}

	bool Initialize();
	std::string GlyphToString(uint16_t glyphId); 

private:
	const void* postData_ = nullptr; 
	uint32_t postSize_ = 0; 
	bool havePostData_ = false; 
	uint16_t numGlyphs_ = 0; 

	std::vector<uint16_t> glyphNameIndex_;
	std::vector<std::string> glyphNames_;
};
