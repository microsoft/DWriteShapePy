// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

class TextAnalysis : public IDWriteTextAnalysisSource
{
public:
	TextAnalysis(IDWriteFactory3* factory, const std::wstring& text, const std::wstring& localeName) : factory_(factory), text_(text), localeName_(localeName)
	{}

	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid,
		_COM_Outptr_ void** obj
	) override;

	ULONG STDMETHODCALLTYPE AddRef(void) override;

	ULONG STDMETHODCALLTYPE Release(void) override;

	HRESULT STDMETHODCALLTYPE GetTextAtPosition(
		UINT32 textPosition,
		_Outptr_result_buffer_(*textLength) WCHAR const** textString,
		_Out_ UINT32* textLength
	) override;

	HRESULT STDMETHODCALLTYPE GetTextBeforePosition(
		UINT32 textPosition,
		_Outptr_result_buffer_(*textLength) WCHAR const** textString,
		_Out_ UINT32* textLength
	) override;

	DWRITE_READING_DIRECTION STDMETHODCALLTYPE GetParagraphReadingDirection() override;

	HRESULT STDMETHODCALLTYPE GetLocaleName(
		UINT32 textPosition,
		_Out_ UINT32* textLength,
		_Outptr_result_z_ WCHAR const** localeName
	) override;

	HRESULT STDMETHODCALLTYPE GetNumberSubstitution(
		UINT32 textPosition,
		_Out_ UINT32* textLength,
		_COM_Outptr_ IDWriteNumberSubstitution** numberSubstitution
	) override;

private:
	std::wstring text_; 
	std::wstring localeName_;
	uint32_t refCount_ = 0;
	CComPtr<IDWriteFactory3> factory_;

};

class TextAnalysisSink : public IDWriteTextAnalysisSink
{
public:
	TextAnalysisSink() {};

	HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid,
		_COM_Outptr_ void** obj
	) override;

	ULONG STDMETHODCALLTYPE AddRef(void) override;

	ULONG STDMETHODCALLTYPE Release(void) override;

	HRESULT STDMETHODCALLTYPE SetScriptAnalysis(
		UINT32 textPosition,
		UINT32 textLength,
		__in DWRITE_SCRIPT_ANALYSIS const* scriptAnalysis
		) override;

	HRESULT STDMETHODCALLTYPE SetLineBreakpoints(
		UINT32 textPosition,
		UINT32 textLength,
		__in_ecount(textLength) DWRITE_LINE_BREAKPOINT const* lineBreakpoints
		) override;

	HRESULT STDMETHODCALLTYPE SetBidiLevel(
		UINT32 textPosition,
		UINT32 textLength,
		UINT8 explicitLevel,
		UINT8 resolvedLevel
		) override;

	HRESULT STDMETHODCALLTYPE SetNumberSubstitution(
		UINT32 textPosition,
		UINT32 textLength,
		_In_ IDWriteNumberSubstitution* numberSubstitution
		) override;

	const DWRITE_SCRIPT_ANALYSIS* GetScriptAnalysis(); 

private:
	uint32_t refCount_ = 0;
	DWRITE_SCRIPT_ANALYSIS  scriptAnalysis_ = { 0 };
	bool scriptAnalysisSet_ = false; 
};