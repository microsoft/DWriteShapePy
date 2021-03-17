#cython: language_level=3
#distutils: language = c++
from libc.stdint cimport uint8_t, uint16_t, uint32_t
from libcpp cimport bool as bool_t
from libcpp cimport bool as hb_bool_t
from libcpp.vector cimport vector

cdef extern from "..\cpp\DWriteShapeLib.h":
   
    ctypedef struct glyph_info_t:
        uint32_t codepoint
        uint32_t cluster

    ctypedef struct glyph_position_t:
        float x_advance
        float y_advance
        float x_offset
        float y_offset

    ctypedef uint32_t hb_tag_t

    ctypedef struct hb_variation_t:
        hb_tag_t tag
        float value

    ctypedef struct hb_feature_t:
        hb_tag_t tag
        unsigned long value
        unsigned int start
        unsigned int end

    bool_t hb_feature_from_string(const char* str, int len, hb_feature_t* feature)
    
    hb_tag_t hb_tag_from_string(const char* str, int len)   
    void hb_tag_to_string(hb_tag_t tag, char* buf)    

    cdef cppclass Face_:
       Face_() except +
       bool_t Create(const char* fontData, unsigned int fontDataSize, unsigned int ttcIndex)
           
    cdef cppclass Font_:
        Font_(Face_ *face) except +
        bool_t Create()
        uint16_t GetDesignUnitsPerEm()
        void SetFontEmSize(float size)
        float GetFontEmSize()
        bool_t SetVariations(const vector[hb_variation_t])
        void GlyphToString(uint16_t glyphId, char* string, unsigned int size)

    cdef cppclass Buffer_:
        Buffer_() except +
        void Add_Utf32(const uint32_t* text, int text_length, unsigned int item_offset, int item_length)
        void Add_Utf8(const uint8_t* text, int text_length, unsigned int item_offset, int item_length)
        void Add_Utf16(const uint16_t* text, int text_length, unsigned int item_offset, int item_length)
        void Add_Latin(const uint8_t* text, int text_length, unsigned int  item_offset, int item_length)
        void SetLocale(const char *locale)
        const char* GetLocale()
        glyph_info_t* GetGlyphInfos(unsigned int* count)
        glyph_position_t* GetGlyphPositions(unsigned int* count)
        

    void Shape(Font_* font, Buffer_* buffer, const vector[hb_feature_t] features)
    
    
    
