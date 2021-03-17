#cython: language_level=3
#distutils: language = c++
from enum import IntEnum
from .cdwriteshape cimport *
from libc.stdlib cimport free, malloc
from libc.string cimport const_char
from typing import Callable, Dict, List, Sequence, Tuple, Union

cdef extern from "Python.h":
    # PEP 393
    bint PyUnicode_IS_READY(object u)
    Py_ssize_t PyUnicode_GET_LENGTH(object u)
    int PyUnicode_KIND(object u)
    void* PyUnicode_DATA(object u)
    ctypedef uint8_t Py_UCS1
    ctypedef uint16_t Py_UCS2
    Py_UCS1 PyUnicode_1BYTE_DATA(object u)
    Py_UCS2 PyUnicode_2BYTE_DATA(object u)
    Py_UCS4 PyUnicode_4BYTE_DATA(object u)
    int PyUnicode_1BYTE_KIND
    int PyUnicode_2BYTE_KIND
    int PyUnicode_4BYTE_KIND
    char* PyUnicode_AsUTF8(object unicode)

cdef class GlyphInfo:
    cdef glyph_info_t _glyph_info
    # could maybe store Buffer to prevent GC

    cdef set(self, glyph_info_t info):
        self._glyph_info = info

    @property
    def codepoint(self):
        return self._glyph_info.codepoint

    @property
    def cluster(self):
        return self._glyph_info.cluster

cdef class GlyphPosition:
    cdef glyph_position_t _glyph_position
    
    cdef set(self, glyph_position_t position):
        self._glyph_position = position

    @property
    def position(self):
        return (
            self._glyph_position.x_offset,
            self._glyph_position.y_offset,
            self._glyph_position.x_advance,
            self._glyph_position.y_advance
        )

    @property
    def x_advance(self):
        return self._glyph_position.x_advance

    @property
    def y_advance(self):
        return self._glyph_position.y_advance

    @property
    def x_offset(self):
        return self._glyph_position.x_offset

    @property
    def y_offset(self):
        return self._glyph_position.y_offset

cdef class Face:
     cdef Face_* c_face_ # Hold C++ instance
     cdef object fontData_ 

     def __cinit__(self, bytes fontData, int ttcIndex=0):
         if fontData is not None:
             self.fontData_ = fontData
             self.c_face_ = new Face_()
             self.c_face_.Create(fontData, len(fontData), ttcIndex)
         else:
             self.c_face_ = NULL              
         
     def __dealloc__(self):
         del self.c_face_    
     
cdef class Font:
     cdef Font_* c_font_ # Hold C++ instance
     cdef Face face_ # Hold associated Face

     def __cinit__(self, Face face):
         if face is not None:
             self.c_font_ = new Font_(face.c_face_)
             self.c_font_.Create()
             self.face_ = face
         else:
             self.c_font_ = NULL

     def __dealloc__(self):
         del self.c_font_ 
         self.face_ = None 

     @property
     def designupem(self) -> int:
         return self.c_font_.GetDesignUnitsPerEm()

     @property
     def upem(self) -> float:
         return self.c_font_.GetFontEmSize()

     @upem.setter
     def upem(self, value: float):
         self.c_font_.SetFontEmSize(value)

     @property
     def face(self):
         return self.face_

     def set_variations(self, variations: Dict[str, float]) -> None:
         cdef unsigned int size
         cdef bytes packed
         cdef hb_variation_t variation
         cdef vector[hb_variation_t] variations1
         size = len(variations)

         for i, (name, value) in enumerate(variations.items()):
             packed = name.encode()
             variation.tag = hb_tag_from_string(packed, -1)
             variation.value = value
             variations1.push_back(variation)
         self.c_font_.SetVariations(variations1)

     def glyph_to_string(self, gid: int):
         cdef char name[64]
         cdef bytes packed
         self.c_font_.GlyphToString(gid, name, 64)
         packed = name
         return packed.decode()

cdef class Buffer:
     cdef Buffer_* c_buffer_ # Hold C++ instance

     def __cinit__(self):
         self.c_buffer_ = new Buffer_()

     def __dealloc_(self):
         del self.c_buffer_

     @property
     def glyph_infos(self) -> List[GlyphInfo]:
        cdef unsigned int count        
        cdef glyph_info_t* glyph_infos = \
            self.c_buffer_.GetGlyphInfos(&count)        
        cdef list infos = []
        cdef GlyphInfo info
        cdef unsigned int i
        for i in range(count):
            info = GlyphInfo()
            info.set(glyph_infos[i])
            infos.append(info)
        return infos        

     @property
     def glyph_positions(self) -> List[GlyphPosition]:
        cdef unsigned int count
        cdef glyph_position_t* glyph_positions = \
            self.c_buffer_.GetGlyphPositions(&count)
        cdef list positions = []
        cdef GlyphPosition position
        cdef unsigned int i
        for i in range(count):
            position = GlyphPosition()
            position.set(glyph_positions[i])
            positions.append(position)
        return positions

     @property
     def language(self) -> str:
        cdef const_char* cstr = self.c_buffer_.GetLocale()
        cdef bytes packed = cstr
        return packed.decode()

     @language.setter
     def language(self, value: str):
        cdef bytes packed = value.encode()
        cdef const char* cstr = packed
        self.c_buffer_.SetLocale(cstr)

     def add_codepoints(self, codepoints: List[int], item_offset: int = 0, item_length: int = -1) -> None:
        cdef unsigned int size = len(codepoints)
        cdef uint32_t* points
        if not size:
            points = NULL
        else:
            points = <uint32_t*>malloc(
                size * sizeof(uint32_t))
            for i in range(size):
                points[i] = codepoints[i]
        self.c_buffer_.Add_Utf32(points, size, item_offset, item_length) 
        if points is not NULL:
            free(points)
                 
     def add_utf8(self, text: bytes, item_offset: int = 0, item_length: int = -1) -> None:
         self.c_buffer_.Add_Utf8(text, len(text), item_offset, item_length)
         
     def add_str(self, text: str, item_offset: int = 0, item_length: int = -1) -> None:
         cdef Py_ssize_t length = PyUnicode_GET_LENGTH(text)
         cdef int kind = PyUnicode_KIND(text)

         if kind == PyUnicode_1BYTE_KIND:
            self.c_buffer_.Add_Latin(<uint8_t*>PyUnicode_1BYTE_DATA(text), length, item_offset, item_length)
         elif kind == PyUnicode_2BYTE_KIND:
            self.c_buffer_.Add_Utf16(<uint16_t*>PyUnicode_2BYTE_DATA(text), length, item_offset, item_length)            
         elif kind == PyUnicode_4BYTE_KIND:
            self.c_buffer_.Add_Utf32(<uint32_t*>PyUnicode_4BYTE_DATA(text), length, item_offset, item_length) 
         else:
             raise AssertionError(kind)

def shape(font: Font, buffer: Buffer, features: Dict[str,Union[int,bool,Sequence[Tuple[int,int,Union[int,bool]]]]] = None,
        shapers: List[str] = None) -> None:
     cdef unsigned int size     
     cdef bytes packed
     cdef hb_feature_t feat
     cdef vector[hb_feature_t] hd_features
     size = 0
     hb_features = NULL
     if features:
        for value in features.values():
            if isinstance(value, int):
                size += 1
            else:
                size += len(value)            
        i = 0
        for name, value in features.items():
            assert i < size, "index out of range for feature array capacity"
            packed = name.encode()
            if isinstance(value, int):
                hb_feature_from_string(packed, len(packed), &feat)
                feat.value = value                   
                hd_features.push_back(feat)
                i += 1
            else:
                feat.tag = hb_tag_from_string(packed, -1)
                for start, end, value in value:
                    feat.value = value
                    feat.start = start
                    feat.end = end                        
                    hd_features.push_back(feat)
                    i += 1
     #no support for shapers            
       
     Shape(font.c_font_, buffer.c_buffer_, hd_features)
            
    







             




