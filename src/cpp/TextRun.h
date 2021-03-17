// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

class TextRunShapeOutput
{
public:
	TextRunShapeOutput() {};
	~TextRunShapeOutput() {};

	std::vector<DWRITE_GLYPH_OFFSET> glyphOffsets;
	std::vector<uint16_t> glyphClusters;
	std::vector<uint16_t> glyphIndices;
	std::vector<float>  glyphAdvances;
	UINT32 glyphCount = 0; 
};

class TextRun
{
public:
	TextRun(IDWriteFactory6* dwriteFactory, IDWriteFontFace5* fontFace, IDWriteTextAnalyzer* textAnalyzer, const std::vector<DWRITE_TYPOGRAPHIC_FEATURES>& features, const std::vector<uint32_t>& featureRangeLengths);
	~TextRun();

	HRESULT SetText(const std::wstring& text, const std::wstring& localeName);
	HRESULT ShapeRun(float fontEmSize, TextRunShapeOutput& output);
	
private:

	std::wstring text_;
	std::wstring localeName_;

	std::vector<DWRITE_SHAPING_TEXT_PROPERTIES>  textProps_;
	std::vector<DWRITE_SHAPING_GLYPH_PROPERTIES> glyphProps_;

	TextAnalysis* textAnalysis_ = nullptr;
	TextAnalysisSink* textAnalysisSink_ = nullptr;

	PostTable* postTable_ = nullptr;

	const std::vector<DWRITE_TYPOGRAPHIC_FEATURES> &features_;
	const std::vector<UINT32> &featureRangeLengths_; 	

	IDWriteFactory6* dwriteFactory_ = nullptr;
	IDWriteFontFace5* fontFace_ = nullptr;
	IDWriteTextAnalyzer* textAnalyzer_ = nullptr;
};
