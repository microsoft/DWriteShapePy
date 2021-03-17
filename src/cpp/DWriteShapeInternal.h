// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace DWriteShapeInternal 
{
	class Face
	{
	public:
		Face();
		~Face();

		HRESULT Initialize();
		HRESULT OpenFontFile(const void* fontData, UINT32 fontDataSize, UINT32 ttcIndex = 0);
		HRESULT OpenFontFile(const std::wstring& file, UINT32 ttcIndex = 0);

		IDWriteFactory6* dwriteFactory_ = nullptr;
		IDWriteInMemoryFontFileLoader* dwriteFontFileLoader_ = nullptr;
		IDWriteFontFile* fontFile_ = nullptr;
		IDWriteTextAnalyzer* textAnalyzer_ = nullptr;

		UINT32 ttcIndex_ = 0;	
	};

	class Font
	{
	public:
		Font(Face& face);
		~Font();

		HRESULT SetVariationAxis(const std::vector<DWRITE_FONT_AXIS_VALUE>& axisValues);
		std::string GlyphToString(uint16_t glyphId);

		HRESULT SetFontFeatures(const std::vector<DWRITE_TYPOGRAPHIC_FEATURES>& features, const std::vector<UINT32>& featureRangeLengths);
		HRESULT Shape(const std::wstring& text, const std::wstring& localeName, float fontEmSize, TextRunShapeOutput& output);

		UINT16 GetDesignUnitsPerEm();

	private:
		Face& face_; 

		std::vector<DWRITE_FONT_AXIS_VALUE> axisValues_;
		std::vector<DWRITE_TYPOGRAPHIC_FEATURES> features_;
		std::vector<UINT32> featureRangeLengths_;

		DWRITE_FONT_METRICS dwriteFontMetrics_ = { 0 };

		IDWriteFontFaceReference1* fontFaceReference_ = nullptr;
		IDWriteFontFace5* fontFace_ = nullptr;

		PostTable* postTable_ = nullptr;

		bool fontRealized_ = false; 
		HRESULT RealizeFont();
		HRESULT InitializePostTable();
	};
}

