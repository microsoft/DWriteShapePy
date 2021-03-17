// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License. 

#include "pch.h"
#include "DWriteShapeInternal.h"

namespace DWriteShapeInternal 
{
	Face::Face()
	{
	}

	Face::~Face()
	{
		dwriteFactory_->UnregisterFontFileLoader(dwriteFontFileLoader_); 

		SafeRelease(&dwriteFontFileLoader_);
		SafeRelease(&fontFile_);	
		SafeRelease(&textAnalyzer_);
		SafeRelease(&dwriteFactory_);		
	}

	HRESULT Face::Initialize()
	{
		// Create a shared DirectWrite factory.
		IFR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory6), reinterpret_cast<IUnknown**>(&dwriteFactory_)));

		IFR(dwriteFactory_->CreateInMemoryFontFileLoader(&dwriteFontFileLoader_));

		IFR(dwriteFactory_->RegisterFontFileLoader(dwriteFontFileLoader_)); 

		IFR(dwriteFactory_->CreateTextAnalyzer(&textAnalyzer_));

		return S_OK;
	}

	HRESULT Face::OpenFontFile(const void* fontData, UINT32 fontDataSize, UINT32 ttcIndex)
	{
		ttcIndex = ttcIndex;

		IFR(dwriteFontFileLoader_->CreateInMemoryFontFileReference(dwriteFactory_, fontData, fontDataSize, nullptr, &fontFile_));	
				
		return S_OK;
	};

	HRESULT Face::OpenFontFile(const std::wstring& file, UINT32 ttcIndex)
	{
		ttcIndex_ = ttcIndex;

		IFR(dwriteFactory_->CreateFontFileReference(file.c_str(), nullptr, &fontFile_));

		return S_OK;
	}

	Font::Font(Face& face) : face_(face)
	{
	}

	Font::~Font()
	{		
		SafeRelease(&fontFaceReference_);
		SafeRelease(&fontFace_);	

		if (postTable_ != nullptr)
			delete postTable_;
	}

	HRESULT Font::SetVariationAxis(const std::vector<DWRITE_FONT_AXIS_VALUE>& axisValues)
	{
		fontRealized_ = false; 

		axisValues_ = axisValues;

		return S_OK; 
	}

	std::string Font::GlyphToString(uint16_t glyphId)
	{
		if(FAILED(this->RealizeFont()) || (postTable_ == nullptr))
		{
			return "";
		}

		return postTable_->GlyphToString(glyphId); 
	}

	HRESULT Font::RealizeFont()
	{
		if (!fontRealized_)
		{
			const DWRITE_FONT_AXIS_VALUE* axis = axisValues_.data();

			SafeRelease(&fontFaceReference_);
			SafeRelease(&fontFace_);

			IFR(face_.dwriteFactory_->CreateFontFaceReference(face_.fontFile_, face_.ttcIndex_, DWRITE_FONT_SIMULATIONS::DWRITE_FONT_SIMULATIONS_NONE, axis, static_cast<UINT32>(axisValues_.size()), &fontFaceReference_));

			IFR(fontFaceReference_->CreateFontFace(&fontFace_));

			fontFace_->GetMetrics(&dwriteFontMetrics_);

			IFR(InitializePostTable());

			fontRealized_ = true;
		}

		return S_OK; 
	}

	HRESULT Font::SetFontFeatures(const std::vector<DWRITE_TYPOGRAPHIC_FEATURES>& features, const std::vector<UINT32>& featureRangeLengths)
	{
		fontRealized_ = false; 

		features_ = features;
		featureRangeLengths_ = featureRangeLengths; 

		return S_OK;
	}

	HRESULT Font::Shape(const std::wstring& text, const std::wstring& localeName, float fontEmSize, TextRunShapeOutput& output)
	{
		TextRun* textRun = nullptr;

		IFR(RealizeFont()); 

		textRun = new TextRun(face_.dwriteFactory_, fontFace_, face_.textAnalyzer_, features_, featureRangeLengths_);

		// Set text and locale
		IFR(textRun->SetText(text, localeName));		

		// The call
		IFR(textRun->ShapeRun(fontEmSize, output));	

		delete textRun;

		return S_OK;
	}

	UINT16 Font::GetDesignUnitsPerEm()
	{
		if (FAILED(RealizeFont()))
			return 0; 

		return dwriteFontMetrics_.designUnitsPerEm;
	}

	HRESULT Font::InitializePostTable()
	{
		UINT32 postTag = DWRITE_MAKE_OPENTYPE_TAG('p', 'o', 's', 't');
		const void* postData;
		UINT32 postSize = 0;
		void* tableContext;
		BOOL postExists;

		if (postTable_ == nullptr)
		{
			HRESULT hr = fontFace_->TryGetFontTable(postTag, &postData, &postSize, &tableContext, &postExists);

			if (SUCCEEDED(hr) && postExists)
			{
				postTable_ = new PostTable(postData, postSize);
			}
			else
			{
				postTable_ = new PostTable(nullptr, 0);
			}

			if (postTable_ == nullptr)
			{
				fontFace_->ReleaseFontTable(tableContext);
				return E_FAIL;
			}

			postTable_->Initialize();

			fontFace_->ReleaseFontTable(tableContext);
		}

		return S_OK;
	}
}