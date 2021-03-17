// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "DWriteShapeInternal.h"

typedef struct glyph_info_t
{
	uint32_t codepoint;
	uint32_t cluster;	
} glyph_info_t;

typedef struct glyph_position_t 
{
	float  x_advance;
	float  y_advance;
	float  x_offset;
	float  y_offset;	
} glyph_position_t;

class Buffer_
{
public:
	Buffer_();
	~Buffer_();

	void Add_Utf32(const uint32_t* text, int text_length, unsigned int item_offset, int item_length);
	void Add_Utf16(const uint16_t* text, int text_length, unsigned int item_offset, int item_length);
	void Add_Utf8(const uint8_t* text, int text_length, unsigned int  item_offset, int item_length);
	void Add_Latin(const uint8_t* text, int text_length, unsigned int  item_offset, int item_length); 

	int Length();
	const std::wstring& GetString();

	void SetLocale(const char* locale);
	const char* GetLocale();
	const std::wstring& Locale();

	void SetOutput(TextRunShapeOutput& output); 

	glyph_info_t* GetGlyphInfos(unsigned int *count);
	glyph_position_t* GetGlyphPositions(unsigned int *count);


private:
	LocaleMap* localeMap_ = nullptr;

	std::wstring str_;
	std::wstring locale_;
	std::string locale8_; 

	std::vector< glyph_info_t> glyphInfos_;
	std::vector<glyph_position_t> glyphPositions_; 
};

class Face_
{
public:
	Face_();
	~Face_();

	bool Create(const char* fontData, unsigned int fontDataSize, unsigned int ttcIndex = 0);	 

	DWriteShapeInternal::Face *pface_ = nullptr;
};

class Font_
{
public:
	Font_(Face_* pface);
	~Font_();

	bool Create();

	uint16_t GetDesignUnitsPerEm();

	void SetFontEmSize(float size); 
	float GetFontEmSize();

	bool SetVariations(const std::vector<hb_variation_t> &axisValues); 
	bool SetFeatures(uint32_t textLength, const std::vector<hb_feature_t>& features);

	void GlyphToString(uint16_t glyphId, char* string, unsigned int size);

	bool Shape(Buffer_* buffer); 
	

private:

	Face_ *pface_; 
	DWriteShapeInternal::Font *pfont_ = nullptr; 

	float fontEmSize_ = 0; 
	bool fontEmSizeSet_ = false; 
};

void Shape(Font_* font, Buffer_* buffer, const std::vector<hb_feature_t>& features);
