// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

// IDWriteTextAnalysisSource Implementation

HRESULT STDMETHODCALLTYPE TextAnalysis::QueryInterface(
	REFIID riid,
	_COM_Outptr_ void** obj
)
{
	if (riid == __uuidof(IDWriteTextAnalysisSource) || riid == __uuidof(IUnknown))
	{
		AddRef();
		*obj = this;
		return S_OK;
	}
	else
	{
		*obj = nullptr;
		return E_NOINTERFACE;
	}
}

ULONG STDMETHODCALLTYPE TextAnalysis::AddRef(void)
{
	return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE TextAnalysis::Release(void)
{
	uint32_t newCount = InterlockedDecrement(&refCount_);
	if (newCount == 0)
		delete this;
	return newCount;
}

HRESULT STDMETHODCALLTYPE TextAnalysis::GetTextAtPosition(
	UINT32 textPosition,
	_Outptr_result_buffer_(*textLength) WCHAR const** textString,
	_Out_ UINT32* textLength
)
{
	uint32_t const length = static_cast<uint32_t>(text_.length());

	if (textPosition <= length)
	{
		*textString = text_.data() + textPosition;
		*textLength = length - textPosition;
		return S_OK;
	}
	else
	{
		*textString = nullptr;
		*textLength = 0;
		return E_INVALIDARG;
	}
}

HRESULT STDMETHODCALLTYPE TextAnalysis::GetTextBeforePosition(
	UINT32 textPosition,
	_Outptr_result_buffer_(*textLength) WCHAR const** textString,
	_Out_ UINT32* textLength
)
{
	uint32_t const length = static_cast<uint32_t>(text_.length());

	if (textPosition <= length)
	{
		*textString = text_.data();
		*textLength = textPosition;
		return S_OK;
	}
	else
	{
		*textString = nullptr;
		*textLength = 0;
		return E_INVALIDARG;
	}
}

DWRITE_READING_DIRECTION STDMETHODCALLTYPE TextAnalysis::GetParagraphReadingDirection()
{
	return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
}

HRESULT STDMETHODCALLTYPE TextAnalysis::GetLocaleName(
	UINT32 textPosition,
	_Out_ UINT32* textLength,
	_Outptr_result_z_ WCHAR const** localeName
)
{
	*localeName = localeName_.c_str(); 
	return S_OK;
}

HRESULT STDMETHODCALLTYPE TextAnalysis::GetNumberSubstitution(
	UINT32 textPosition,
	_Out_ UINT32* textLength,
	_COM_Outptr_ IDWriteNumberSubstitution** numberSubstitution
)
{
	return factory_->CreateNumberSubstitution(
		DWRITE_NUMBER_SUBSTITUTION_METHOD_NONE,
		localeName_.c_str(),
		TRUE,
		numberSubstitution
	);
}

// IDWriteTextAnalysisSink Implementation

HRESULT STDMETHODCALLTYPE TextAnalysisSink::QueryInterface(
	REFIID riid,
	_COM_Outptr_ void** obj
)
{
	if (riid == __uuidof(IDWriteTextAnalysisSource) || riid == __uuidof(IUnknown))
	{
		AddRef();
		*obj = this;
		return S_OK;
	}
	else
	{
		*obj = nullptr;
		return E_NOINTERFACE;
	}
}

ULONG STDMETHODCALLTYPE TextAnalysisSink::AddRef(void)
{
	return InterlockedIncrement(&refCount_);
}

ULONG STDMETHODCALLTYPE TextAnalysisSink::Release(void)
{
	uint32_t newCount = InterlockedDecrement(&refCount_);
	if (newCount == 0)
		delete this;
	return newCount;
}

HRESULT STDMETHODCALLTYPE TextAnalysisSink::SetScriptAnalysis(
	UINT32 textPosition,
	UINT32 textLength,
	_In_ DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis
)
{
	scriptAnalysis_ = *scriptAnalysis; 
	scriptAnalysisSet_ = true; 

	return S_OK; 
}

HRESULT STDMETHODCALLTYPE TextAnalysisSink::SetLineBreakpoints(
	UINT32 textPosition,
	UINT32 textLength,
	_In_reads_(textLength) DWRITE_LINE_BREAKPOINT const* lineBreakpoints
)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE TextAnalysisSink::SetBidiLevel(
	UINT32 textPosition,
	UINT32 textLength,
	UINT8 explicitLevel,
	UINT8 resolvedLevel
)
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE TextAnalysisSink::SetNumberSubstitution(
	UINT32 textPosition,
	UINT32 textLength,
	_In_ IDWriteNumberSubstitution* numberSubstitution
)
{
	return S_OK; 
}

const DWRITE_SCRIPT_ANALYSIS* TextAnalysisSink::GetScriptAnalysis()
{
	return &scriptAnalysis_;
}