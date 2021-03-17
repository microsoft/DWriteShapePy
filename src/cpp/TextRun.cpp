// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

UINT32 EstimateGlyphCount(UINT32 textLength)
{
	return 3 * textLength / 2 + 16;
}

TextRun::TextRun(IDWriteFactory6* dwriteFactory, IDWriteFontFace5* fontFace, IDWriteTextAnalyzer* textAnalyzer, const std::vector<DWRITE_TYPOGRAPHIC_FEATURES>& features, const std::vector<uint32_t>& featureRangeLengths) :
	dwriteFactory_(dwriteFactory), fontFace_(fontFace), textAnalyzer_(textAnalyzer), features_(features), featureRangeLengths_(featureRangeLengths)
{
}

TextRun::~TextRun()
{
	// Derived interfaces
	SafeRelease(&textAnalysis_);
	SafeRelease(&textAnalysisSink_);
}

HRESULT TextRun::SetText(const std::wstring& text, const std::wstring& localeName)
{
	text_ = text;
	localeName_ = localeName;

	textAnalysis_ = new TextAnalysis(dwriteFactory_, text, localeName);
	textAnalysis_->AddRef();

	textAnalysisSink_ = new TextAnalysisSink();
	textAnalysisSink_->AddRef();

	IFR(textAnalyzer_->AnalyzeScript(textAnalysis_, 0, static_cast<UINT32>(text_.length()), textAnalysisSink_));

	return S_OK;
}

HRESULT TextRun::ShapeRun(float fontEmSize, TextRunShapeOutput& output)
{
	HRESULT hr = S_OK;

	uint32_t textLength = static_cast<uint32_t>(text_.size());

	uint32_t maxGlyphCount = EstimateGlyphCount(textLength);
	uint32_t actualGlyphCount = 0;	

	output.glyphIndices.resize(maxGlyphCount);
	output.glyphOffsets.resize(maxGlyphCount);
	output.glyphAdvances.resize(maxGlyphCount);
	output.glyphClusters.resize(maxGlyphCount);
	textProps_.resize(textLength);
	glyphProps_.resize(maxGlyphCount);

	assert(features_.size() == featureRangeLengths_.size()); 

	bool useOpenTypeFetures = features_.size() != 0;

	std::vector<const DWRITE_TYPOGRAPHIC_FEATURES*> features1; 

	for (auto& it : features_)
	{
		const DWRITE_TYPOGRAPHIC_FEATURES* pfeature = &it;
		features1.push_back(pfeature); 
	}
	
	if (textLength == 0)
		return hr;

	int tries = 0;
	do
	{
		hr = textAnalyzer_->GetGlyphs(
			&text_[0],
			textLength,
			fontFace_,
			false,						// isSideways
			false,						// isRightToLeft
			textAnalysisSink_->GetScriptAnalysis(),
			localeName_.length() > 0 ? localeName_.c_str() : nullptr,
			nullptr,					// numberSubstitution
			useOpenTypeFetures ? features1.data() : nullptr,
			useOpenTypeFetures ? featureRangeLengths_.data() : nullptr,
			useOpenTypeFetures ? static_cast<UINT32>(featureRangeLengths_.size()) : 0, // featureCount
			maxGlyphCount,				// maxGlyphCount
			& output.glyphClusters[0],
			& textProps_[0],
			& output.glyphIndices[0],
			& glyphProps_[0],
			& actualGlyphCount
		);
		tries++;

		if (hr == E_NOT_SUFFICIENT_BUFFER)
		{
			maxGlyphCount = EstimateGlyphCount(maxGlyphCount);

			glyphProps_.resize(maxGlyphCount);
			output.glyphIndices.resize(maxGlyphCount);
		}
		else
		{
			break;
		}

	} while (tries < 2);

	if (FAILED(hr))
		return hr;

	output.glyphAdvances.resize(std::max(static_cast<size_t>(actualGlyphCount), output.glyphAdvances.size()));
	output.glyphOffsets.resize(std::max(static_cast<size_t>(actualGlyphCount), output.glyphOffsets.size()));

	output.glyphCount = actualGlyphCount;

	hr = textAnalyzer_->GetGlyphPlacements(
		&text_[0],
		&output.glyphClusters[0],
		&textProps_[0],
		textLength,
		&output.glyphIndices[0],
		&glyphProps_[0],
		actualGlyphCount,
		fontFace_,
		fontEmSize,
		false,				// isSideways
		false,				// isRightToLeft
		textAnalysisSink_->GetScriptAnalysis(),
		localeName_.length() > 0 ? localeName_.c_str() : nullptr,
		useOpenTypeFetures ? features1.data() : nullptr,
		useOpenTypeFetures ? featureRangeLengths_.data() : nullptr,
		useOpenTypeFetures ? static_cast<UINT32>(featureRangeLengths_.size()) : 0, // featureCount
		&output.glyphAdvances[0],
		&output.glyphOffsets[0]
	);

	if (FAILED(hr))
		return hr;

	return S_OK;
}