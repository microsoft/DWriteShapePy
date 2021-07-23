// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DWriteShapeLib.h"

// https://stackoverflow.com/questions/955484/is-it-possible-to-convert-utf32-text-to-utf16-using-only-windows-api
unsigned int UTF32ToUTF16(char32_t cUTF32, wchar_t& h, wchar_t& l)
{
	if (cUTF32 < 0x10000)
	{
		h = 0;
		l = (wchar_t)cUTF32;
		return cUTF32;
	}
	unsigned int t = cUTF32 - 0x10000;
	h = (((t << 12) >> 22) + 0xD800);
	l = (((t << 22) >> 22) + 0xDC00);
	unsigned int ret = ((h << 16) | (l & 0x0000FFFF));
	return ret;
}

//
// Buffer_ Implementations
//

Buffer_::Buffer_()
{
	// Sniff default locale
	std::wstring locale = L"en-US";
	wchar_t szLocale[LOCALE_NAME_MAX_LENGTH];
	if (GetUserDefaultLocaleName(szLocale, LOCALE_NAME_MAX_LENGTH) > 0)
	{
		locale = szLocale;
	}

	locale_ = locale;
}

Buffer_::~Buffer_()
{
	if (localeMap_ != nullptr)
		delete localeMap_;
}

void Buffer_::Add_Utf32(const uint32_t* text, int text_length, unsigned int item_offset, int item_length)
{
	assert(text != nullptr);

	if ((int)item_offset > text_length)
		return;

	std::u32string str(reinterpret_cast<const char32_t*>(text), text_length); 
	std::u32string str32;

	if (item_length != -1)
	{
		str32.append(str, item_offset, item_length);
	}
	else
	{
		str32.append(str);
	}

	std::wstring str16; 
	for (auto& it : str32)
	{
		wchar_t h, l; 
		UTF32ToUTF16(it, h, l); 
		if (h != 0)
		{
			str16.push_back(h);
		}
		str16.push_back(l);
	}

	str_.append(str16); 
}

void Buffer_::Add_Utf16(const uint16_t* text, int text_length, unsigned int item_offset, int item_length)
{
	assert(text != nullptr);

	if ((int)item_offset > text_length)
		return;

	std::wstring stdText(reinterpret_cast<const wchar_t*>(text), text_length);

	if (item_length != -1)
	{
		str_.append(stdText, item_offset, item_length);
	}
	else
	{
		str_.append(stdText);
	}
}

void Buffer_::Add_Utf8(const uint8_t* text, int text_length, unsigned int item_offset, int item_length)
{
	assert(text != nullptr);

	if ((int)item_offset > text_length)
		return;

	std::string str(reinterpret_cast<const char*>(text), text_length);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstr = converter.from_bytes(str);

	if (item_length != -1)
	{
		str_.append(wstr, item_offset, item_length);
	}
	else
	{
		str_.append(wstr);
	}
}

void Buffer_::Add_Latin(const uint8_t* text, int text_length, unsigned int  item_offset, int item_length)
{
	assert(text != nullptr);

	if ((int)item_offset > text_length)
		return;

	std::string str(reinterpret_cast<const char*>(text), text_length);
	std::wstring wstr;
	for (auto& it : str)
	{
		wchar_t ch = it & 0x00FF; 
		wstr.push_back(ch); 
	}
	
	if (item_length != -1)
	{
		str_.append(wstr, item_offset, item_length);
	}
	else
	{
		str_.append(wstr);
	}
}

int Buffer_::Length()
{
	return static_cast<int>(str_.length());
}

const std::wstring& Buffer_::GetString()
{
	return str_;
}

void Buffer_::SetLocale(const char* locale)
{
	// Build Locale mapping
	if(localeMap_ == nullptr)
		localeMap_ = new LocaleMap();

	if (localeMap_ == nullptr)
		return; 

	std::string str = reinterpret_cast<const char*>(locale);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring openTypeLocale = converter.from_bytes(str);

	std::wstring windowsLocale; 
	if (localeMap_->GetWindowsLocale(openTypeLocale, windowsLocale))
		locale_ = windowsLocale; 
}

const char* Buffer_::GetLocale()
{	
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	locale8_ = converter.to_bytes(locale_); 
	
	return locale8_.c_str(); 
}

const std::wstring& Buffer_::Locale()
{
	return locale_; 
}

void Buffer_::SetOutput(TextRunShapeOutput& output)
{
	
	for (size_t i = 0; i < output.glyphCount; i++)
	{
		glyph_position_t glyphPosition;
		glyphPosition.x_advance = output.glyphAdvances[i];
		glyphPosition.y_advance = 0; // for now until we support y advances
		glyphPosition.x_offset = output.glyphOffsets[i].advanceOffset;
		glyphPosition.y_offset = output.glyphOffsets[i].ascenderOffset;
		
		glyphPositions_.push_back(glyphPosition);

		glyph_info_t glyphInfo;
		glyphInfo.cluster = output.glyphClusters[i];
		glyphInfo.codepoint = output.glyphIndices[i];

		glyphInfos_.push_back(glyphInfo); 
	}
}

glyph_info_t* Buffer_::GetGlyphInfos(unsigned int* count)
{
	*count = static_cast<unsigned int>(glyphInfos_.size());

	return glyphInfos_.data();
}

glyph_position_t* Buffer_::GetGlyphPositions(unsigned int* count)
{
	*count = static_cast<unsigned int>(glyphPositions_.size());

	return glyphPositions_.data();
}

//
// Face Implementations
// 

Face_::Face_() 
{
	pface_ = new DWriteShapeInternal::Face(); 
	if (pface_ == nullptr)
		return; 	

	HRESULT hr = pface_->Initialize();
}

Face_::~Face_() 
{ 
	if (pface_ != nullptr)
	{
		delete pface_;
		pface_ = nullptr; 
	}
}

bool Face_::Create(const char* fontData, unsigned int fontDataSize, unsigned int ttcIndex)
{
	HRESULT hr = pface_->OpenFontFile(fontData, fontDataSize, ttcIndex); 
	if (FAILED(hr))
		return false; 

	return true; 
}

//
// Font_ Implementations
// 

Font_::Font_(Face_* pface) : pface_(pface)
{
}

Font_::~Font_()
{
}

bool Font_::Create()
{
	pfont_ = new DWriteShapeInternal::Font(*(pface_->pface_)); 
	if (pfont_ == nullptr)
		return false;  

	return true; 
}

uint16_t Font_::GetDesignUnitsPerEm()
{	
	return  pfont_->GetDesignUnitsPerEm();
}

void Font_::SetFontEmSize(float size)
{
	fontEmSizeSet_ = true; 

	fontEmSize_ = size; 
}

float Font_::GetFontEmSize()
{
	float fontEmSize; 

	if (!fontEmSizeSet_)
	{
		fontEmSize = pfont_->GetDesignUnitsPerEm();
	}
	else
	{
		fontEmSize = fontEmSize_; 
	}

	return fontEmSize;
}

bool Font_::SetVariations(const std::vector<hb_variation_t> &axisValues)
{
	std::vector<DWRITE_FONT_AXIS_VALUE> dWriteAxisValues;

	for (auto& it : axisValues)
	{
		DWRITE_FONT_AXIS_VALUE value;
		// DWrite tags are opposite HarfBuzz tags
		value.axisTag = static_cast<DWRITE_FONT_AXIS_TAG>(SWAPL(it.tag));
		value.value =  it.value;
		dWriteAxisValues.push_back(value); 
	}

	return (pfont_->SetVariationAxis(dWriteAxisValues) == S_OK);	
}

bool  Font_::SetFeatures(uint32_t textLength, const std::vector<hb_feature_t>& features)
{
	std::vector<DWRITE_TYPOGRAPHIC_FEATURES> typographicFeatures;
	std::vector<UINT32> featureRangeLengths;

	if (textLength == 0 || features.size() == 0)
		return true; 

	std::vector<bool> featureRangeBounderies;
	featureRangeBounderies.assign(textLength, false);

	// beginning is always a boundry
	featureRangeBounderies[0] = true; 

	// look though features and determine range boundries
	for (auto& it : features)
	{
		if (it.start < featureRangeBounderies.size())
		{
			featureRangeBounderies[it.start] = true;
			if (it.end < featureRangeBounderies.size())
			{
				featureRangeBounderies[it.end] = true;
			}
		}
	}

	// look at each boundry and determine features of influence
	UINT32 featureRangeLength = 0;

	for (unsigned int i = 0; i < featureRangeBounderies.size(); i++)
	{		
		if (featureRangeBounderies[i])
		{
			// temporaraly store features in a vector
			std::vector<DWRITE_FONT_FEATURE> localFeatures;
			for (auto& it : features)
			{
				if (i >= it.start && i < it.end)
				{
					DWRITE_FONT_FEATURE localFeature;
					// DWrite tags are opposite HarfBuzz tags
					localFeature.nameTag = static_cast<DWRITE_FONT_FEATURE_TAG>(SWAPL(it.tag));
					localFeature.parameter = it.value;

					// Do we already have this feature for this range?
					bool found = false; 
					for (auto& it1 : localFeatures)
					{
						if (it1.nameTag == localFeature.nameTag)
						{
							found = true; 
							// Update parameter to most recent one encountered
							it1.parameter = it.value; 
						}
					}

					// If we don't already have it then add it
					if (!found)
					{
						localFeatures.push_back(localFeature);
					}
				}
			}

			// allocate an array for DWRITE_FONT_FEATURE
			DWRITE_FONT_FEATURE* pLocalFeatures = new DWRITE_FONT_FEATURE[localFeatures.size()];
			for (int j = 0; j < localFeatures.size(); j++)
			{
				pLocalFeatures[j].nameTag = localFeatures[j].nameTag;
				pLocalFeatures[j].parameter = localFeatures[j].parameter;
			}

			DWRITE_TYPOGRAPHIC_FEATURES typographicFeature;
			typographicFeature.featureCount = static_cast<UINT32>(localFeatures.size());
			typographicFeature.features = pLocalFeatures;

			typographicFeatures.push_back(typographicFeature);

			// Calculate featureRangeLengths
			if (i > 0)
			{
				featureRangeLengths.push_back(featureRangeLength);
				featureRangeLength = 0;
			}
		}

		featureRangeLength++;
	}

	// Final length
	featureRangeLengths.push_back(featureRangeLength);
	featureRangeLength = 0;

	assert(typographicFeatures.size() == featureRangeLengths.size());

	// Set features in DWriteShape::Font object
	HRESULT hr = pfont_->SetFontFeatures(typographicFeatures, featureRangeLengths);

	// Clean up
	for (auto& it : typographicFeatures)
	{
		if (it.features != nullptr)
		{
			delete[] it.features;
			it.features = nullptr;
		}
	}

	return hr == S_OK; 
}

void Font_::GlyphToString(uint16_t glyphId, char* string, unsigned int size)
{
	try
	{
		std::string name = pfont_->GlyphToString(glyphId);
		std::size_t len = name.copy(string, size);
		string[len] = '\0';
	}
	catch (...)
	{		
		return;
	}
}

bool Font_::Shape(Buffer_* buffer)
{
	TextRunShapeOutput output; 

	const std::wstring& text = buffer->GetString();

	float fontEmSize = this->GetFontEmSize(); 

	// Shape DWriteShape::Font
	HRESULT hr = pfont_->Shape(text, buffer->Locale(), fontEmSize, output);

	buffer->SetOutput(output); 

	return true; 
}

void Shape(Font_* font, Buffer_* buffer, const std::vector<hb_feature_t> &features)
{
	assert(font != nullptr);

	// Set font features
	font->SetFeatures(buffer->Length(), features); 	

	// Call Shape
	font->Shape(buffer); 	
}












