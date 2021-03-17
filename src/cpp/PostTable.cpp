// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

typedef short FWord;
typedef unsigned short uFWord;
typedef short F2Dot14;
typedef long Fixed;

#define POSTSCRIPTNAMEINDICES_NUMBERGLYPHS          32

bool PostTable::Initialize()
{
	// https://docs.microsoft.com/en-us/typography/opentype/spec/post

	const char* data = static_cast<const char*>(postData_);
	uint32_t offset = 0; 

	Fixed version = SWAPL(*data + offset);
	if (version == 0x00020000)
	{
		if (POSTSCRIPTNAMEINDICES_NUMBERGLYPHS + sizeof(uint16_t) > postSize_)
			return false;

		offset += POSTSCRIPTNAMEINDICES_NUMBERGLYPHS;
		numGlyphs_ = SWAPW(*(data + offset));		
		offset += sizeof(uint16_t); //numGlyphs

		for (int index = 0; index < numGlyphs_; index++)
		{
			if (offset + sizeof(uint16_t) > postSize_)
				return false;

			uint16_t glyphNameIndex = SWAPW(*(data + offset)); 
			glyphNameIndex_.push_back(glyphNameIndex); 
			offset += sizeof(uint16_t);
		}

		while (offset < postSize_ && offset + *(data + offset) < postSize_)
		{
			uint8_t size = *(data + offset); 
			offset += sizeof(uint8_t); 
			std::string str(data + offset, size); 
			glyphNames_.push_back(str); 
			offset += size; 
		}

		havePostData_ = true; 
	}
	
	return true; 
}

std::string PostTable::GlyphToString(uint16_t glyphId)
{
	std::string result = "gid" + std::to_string(glyphId);

	if (havePostData_ && glyphId < numGlyphs_)
	{
		uint16_t glyphNameIndex = glyphNameIndex_[glyphId]; 
		if (glyphNameIndex < 258)
		{
			result = macPostStrings[glyphNameIndex]; 
		}
		else
		{
			const uint16_t ordinal = glyphNameIndex - 258;
			if (ordinal < glyphNames_.size())
			{
				result = glyphNames_[ordinal];
			}
		}
	}
	
	return result; 
}

