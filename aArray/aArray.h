    /**\
 /*/
/*  Public domain.  No restriction, no need to accredit
**    "As freely as you have received, freely give" -- Jesus
**
\*  An array and string library.  Generated: 14 Sep 2020
 \*\
    \**/




#if !defined(AARRAY_define)

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>   /* for memcmp */
#if defined(_MSC_VER)
#include <malloc.h>   /* for msvc realloc */
#endif
#if defined(__cplusplus)
#include <functional> /* for lambda */
#endif

/* $defines are for pre-pre-processed macros
 * this simplifies end-user compiler messages
 * if this is aArray.h, they will have already reduced to the final exposed api
 */



 
/* handle warnings
 * many parameters take an arbitrary mix of pointers/integers
 * so optionally suppress warnings only for those parameters
 */
/* c++ is available but not advertised and not advised
 * start-end block suppresses some c++ warnings
 * i.e. (cast) not being static_cast<cast>(), c++98-compat, etc
 */
#define AARRAY_nowarn_pedantic_cpp_start
#define AARRAY_nowarn_pedantic_cpp_end
#if defined(__clang__)
    /* these are slightly redundant, but handle both clang and gcc */
  #if defined(__cplusplus)
    #define AARRAY_nowarn_start \
      _Pragma("clang diagnostic push") \
      _Pragma("clang diagnostic ignored \"-Wconversion\"") \
      _Pragma("clang diagnostic ignored \"-Wconversion-null\"") \
      _Pragma("clang diagnostic ignored \"-Woverflow\"") /* older versions of gcc */ \
      _Pragma("clang diagnostic ignored \"-Wnarrowing\"")
  #else
    #define AARRAY_nowarn_start \
      _Pragma("clang diagnostic push") \
      _Pragma("clang diagnostic ignored \"-Wconversion\"") \
      _Pragma("clang diagnostic ignored \"-Wint-conversion\"") \
      _Pragma("clang diagnostic ignored \"-Wpointer-to-int-cast\"") \
      _Pragma("clang diagnostic ignored \"-Wbad-function-cast\"")
  #endif
  #define AARRAY_nowarn_end    _Pragma("clang diagnostic pop")
  #if defined(__cplusplus)
    #undef  AARRAY_nowarn_pedantic_cpp_start
    #define AARRAY_nowarn_pedantic_cpp_start_helper \
      _Pragma("clang diagnostic push") \
      _Pragma("clang diagnostic ignored \"-Wc99-extensions\"") \
      _Pragma("clang diagnostic ignored \"-Wc++98-compat-pedantic\"") \
      _Pragma("clang diagnostic ignored \"-Wold-style-cast\"") \
      _Pragma("clang diagnostic ignored \"-Wcast-qual\"")
    #if __clang_major__ < 5
      #define AARRAY_nowarn_pedantic_cpp_start \
        AARRAY_nowarn_pedantic_cpp_start_helper
    #else
      #define AARRAY_nowarn_pedantic_cpp_start \
        AARRAY_nowarn_pedantic_cpp_start_helper \
        _Pragma("clang diagnostic ignored \"-Wzero-as-null-pointer-constant\"")
    #endif
    #undef  AARRAY_nowarn_pedantic_cpp_end
    #define AARRAY_nowarn_pedantic_cpp_end _Pragma("clang diagnostic pop")
  #endif
#elif defined(__GNUC__)
    /* these are slightly redundant, but handle both clang and gcc */
  #if defined(__cplusplus)
    #define AARRAY_nowarn_start \
      _Pragma("GCC diagnostic push") \
      _Pragma("GCC diagnostic ignored \"-Wconversion\"") \
      _Pragma("GCC diagnostic ignored \"-Wconversion-null\"") \
      _Pragma("GCC diagnostic ignored \"-Woverflow\"") /* older versions of gcc */ \
      _Pragma("GCC diagnostic ignored \"-Wnarrowing\"")
  #else
    #define AARRAY_nowarn_start \
      _Pragma("GCC diagnostic push") \
      _Pragma("GCC diagnostic ignored \"-Wconversion\"") \
      _Pragma("GCC diagnostic ignored \"-Wint-conversion\"") \
      _Pragma("GCC diagnostic ignored \"-Wpointer-to-int-cast\"") \
      _Pragma("GCC diagnostic ignored \"-Wbad-function-cast\"")
  #endif
  #define AARRAY_nowarn_end    _Pragma("GCC diagnostic pop")
  #if defined(__cplusplus)
    #undef  AARRAY_nowarn_pedantic_cpp_start
    #undef  AARRAY_nowarn_pedantic_cpp_end
    #define AARRAY_nowarn_pedantic_cpp_start \
      _Pragma("GCC diagnostic push") \
      _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
    #define AARRAY_nowarn_pedantic_cpp_end  _Pragma("GCC diagnostic pop")
  #endif
#elif defined(_MSC_VER)
  #define AARRAY_nowarn_start \
    __pragma(warning(push)) \
    __pragma(warning(disable:4047))
  #define AARRAY_nowarn_end __pragma(warning(pop))
#else
  /* don't hide int-pointer conversion warnings for this compiler */
  #define AARRAY_nowarn_start
  #define AARRAY_nowarn_end
#endif

#if defined(__cplusplus)
  AARRAY_nowarn_pedantic_cpp_start
  #define AARRAY_move std::move
#else
  #define AARRAY_move
#endif




/* compile as .c .h, or as a single header with 'static inline' functions */
#if defined(AARRAY_c)
#define AARRAY_define(name, ...) name __VA_ARGS__
#elif defined(AARRAY_h)
#define AARRAY_define(name, ...) extern name;
#else
#define AARRAY_define(name, ...) static inline name __VA_ARGS__
#endif

/* general compiler compatibility */
#if !defined(__has_feature)
#define __has_feature(x) 0
#endif
#if !defined(__has_extension)
/* pre-3.0 clang */
#define __has_extension __has_feature
#endif

#if defined(__builtin_prefetch)
#define AARRAY_prefetch(A, B, C) __builtin_prefetch(A, B, C)
#else
#define AARRAY_prefetch(A, B, C)
#endif

/* switch on/off safety checks */
#if defined(AARRAY_UNSAFE)
#define AARRAY_safety(UNSAFE, ...) UNSAFE
#else
#define AARRAY_safety(UNSAFE, ...) __VA_ARGS__
#endif

/* switch on/off type warnings for generics */
#if defined(AARRAY_WARN) || (defined(__GNUC__) && !defined(__clang__))
  /* gcc can't handle pragmas within expressions, and so misses out */
  #define AARRAY_nowarn_internal_start
  #define AARRAY_nowarn_internal_end
#else
  #define AARRAY_nowarn_internal_start AARRAY_nowarn_start
  #define AARRAY_nowarn_internal_end   AARRAY_nowarn_end
#endif

/* set the size of aSort's cache */
#if !defined(AARRAY_sortCache)
  #define AARRAY_sortCache 512
#endif




/* error handling */
AARRAY_define(__attribute((noreturn))
void AARRAY_aError(char errLocation[], char errMsg[]), {
  fflush(stdout); fprintf(stderr, "%s: %s\n", errLocation, errMsg); abort(); })
/* set the default handler */
#if defined(AARRAY_c)
void (*aError)(char[], char[]) = &AARRAY_aError;
#elif defined(AARRAY_h)
extern void (*aError)(char[], char[]);
#else
static void (*aError)(char[], char[]) = &AARRAY_aError;
#endif

/* generate "file.c:line_number" for error messages */
#define AARRAY_STRINGIFY(x) #x
#define AARRAY_TOSTRING(x) AARRAY_STRINGIFY(x)
#define AARRAY_LINE (char*)__FILE__ ":" AARRAY_TOSTRING(__LINE__)

/* generate error messages */
#define AARRAY_aError_MsgLen 52 + 3*20 + 1 /* 52 characters + 3*size_t + NULL */
#define AARRAY_aError_CALL(MSG) { aError(errLoc, MSG); abort(); }
#define AARRAY_Error_OutOfMemory(SIZE) \
  { char AARRAY_aError_Msg[AARRAY_aError_MsgLen]; \
    if(0>snprintf(AARRAY_aError_Msg, AARRAY_aError_MsgLen,  \
          "out of memory (allocating=%zu)", \
          SIZE)) \
      AARRAY_aError_CALL((char*)"out of memory " \
                      "(can I interest you in a banana instead? 🍌)") \
    else AARRAY_aError_CALL(AARRAY_aError_Msg) }
#define AARRAY_Error_OutOfBounds(LENGTH, POS) \
  { char AARRAY_aError_Msg[AARRAY_aError_MsgLen]; \
    if(0>snprintf(AARRAY_aError_Msg, AARRAY_aError_MsgLen,  \
          "out of bounds (length=%zu but pos=%zu)", \
          LENGTH, POS)) \
      AARRAY_aError_CALL((char*)"out of bounds (no info)") \
    else AARRAY_aError_CALL(AARRAY_aError_Msg) }
#define AARRAY_Error_RemovalIsOutOfBounds(LENGTH, POS, RLEN) \
  { char AARRAY_aError_Msg[AARRAY_aError_MsgLen]; \
    if(0>snprintf(AARRAY_aError_Msg, AARRAY_aError_MsgLen, \
          "removal is out of bounds (length=%zu but pos=%zu removal=%zu)", \
          LENGTH, POS, RLEN)) \
      AARRAY_aError_CALL((char*)"removal is out of bounds (no info)") \
    else AARRAY_aError_CALL(AARRAY_aError_Msg) }
#define AARRAY_Error_OutOfCapacity(CAPACITY, REQ) \
  { char AARRAY_aError_Msg[AARRAY_aError_MsgLen]; \
    if(0>snprintf(AARRAY_aError_Msg, AARRAY_aError_MsgLen, \
          "out of capacity (size=%zu but require=%zu)", CAPACITY, REQ)) \
      AARRAY_aError_CALL((char*)"array is STATIC (no info)") \
    else AARRAY_aError_CALL(AARRAY_aError_Msg) }
#define AARRAY_Error_ArrayIsNull(ARRAY_NO) \
  { char AARRAY_aError_Msg[AARRAY_aError_MsgLen]; \
    if(0>snprintf(AARRAY_aError_Msg, AARRAY_aError_MsgLen,   \
          "array is NULL (array no=%zu)", ARRAY_NO)) \
      AARRAY_aError_CALL((char*)"array is NULL (no info)") \
    else AARRAY_aError_CALL(AARRAY_aError_Msg) }
#define AARRAY_Error_ArrayIsWide \
  (aError(AARRAY_LINE, (char*)"array type too wide (max 8 bytes)"), 0)
#define AARRAY_Error_WrongArgCount(ARG_NUM, MULTIPLE, ADDITION) \
  { char AARRAY_aError_Msg[AARRAY_aError_MsgLen]; \
    if(0>snprintf(AARRAY_aError_Msg, AARRAY_aError_MsgLen, \
          "wrong arg count (args=%zu but should be %i + multiple of %i)", \
          ARG_NUM, ADDITION, MULTIPLE)) \
      AARRAY_aError_CALL((char*)"wrong arg count (no info)") \
    else AARRAY_aError_CALL(AARRAY_aError_Msg) }
#define AARRAY_Error_InfiniteLoop \
  AARRAY_aError_CALL((char*)"infinite loop (jump=0)")
#define AARRAY_Error_FormatStringArgs \
  AARRAY_aError_CALL((char*)"format requires more arguments")
#define AARRAY_Error_FormatStringMalformed \
  AARRAY_aError_CALL((char*)"format is malformed")
#define AARRAY_Error_NullParameter \
  AARRAY_aError_CALL((char*)"parameter is NULL")




/* foundations for array allocation */
AARRAY_define(size_t AARRAY_upper_power_of_two(size_t v), {
  /* from: graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
   * very slightly faster than:  1<<(64-AARRAY_BUILTIN_LL(clz,v-1))
   */
  v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8;
  v |= v >> 16; if(sizeof(size_t)==8) v |= v >> 32; v++; return v; })
/* cross platform count-leading-zeros */
#if defined(__GNUC__)
#define AARRAY_builtin_ll(name,with) \
  (sizeof(with)<=sizeof(int)?__builtin_##name(with) : \
   (sizeof(with)<=sizeof(long)?__builtin_##name##l(with) : \
    __builtin_##name##ll(with)))
AARRAY_define(int AARRAY_clz(size_t value), {
	if(!value) return sizeof(value)*8;
  return AARRAY_builtin_ll(clz,value); })
#elif defined(_MSC_VER)
#include <intrin.h>
#if defined(_WIN64)
#pragma intrinsic(_BitScanReverse,_BitScanReverse64)
AARRAY_define(int AARRAY_clz(size_t value), {
  unsigned long result;
  return (int)(sizeof(size_t) <= 4?
    (_BitScanReverse(&result, (unsigned long)value)? 31-result : 32) :
    _BitScanReverse64(&result, value)? 63-result : 64); })
#else
#pragma intrinsic(_BitScanReverse)
AARRAY_define(int AARRAY_clz(size_t value), {
  unsigned long result;
  return (int)(_BitScanReverse(&result, value)? 31-result : 32); })
#endif
#else
/* from www.hackersdelight.org/hdcodetxt/nlz.c.txt */
AARRAY_define(int AARRAY_clz(size_t x), {
  int n = 0;
  if(sizeof(size_t)==8 &&
     x <= 0x00000000FFFFFFFF) { n += 32; x <<= 32; }
  if(x <= 0x0000FFFFFFFFFFFF) { n += 16; x <<= 16; }
  if(x <= 0x00FFFFFFFFFFFFFF) { n +=  8; x <<=  8; }
  if(x <= 0x0FFFFFFFFFFFFFFF) { n +=  4; x <<=  4; }
  if(x <= 0x3FFFFFFFFFFFFFFF) { n +=  2; x <<=  2; }
  if(x <= 0x7FFFFFFFFFFFFFFF) { n +=  1; x <<=  1; }
  return n; })
#endif

/* array allocators, for 3 types of array
 * fix pointers to realloced memory, to point back into memory
 */
#define AARRAY_FIX_POINTERS(TYPE) \
  if(!*vec) *length = 0; \
  else if(vecsIncr) { \
    /* parameters can contain pointers \
       so update any that overlaped vec's old position */ \
    size_t m = vecsIncr==5? 2: vecsIncr-1; \
    while(m < vecsCount) { \
      if(vecs[m] >= (uintptr_t)(*vec) && \
        vecs[m] < (uintptr_t)(*vec)+(*length)*sizeof(TYPE)) \
        vecs[m] += \
            (uintptr_t)length-(uintptr_t)(*(size_t**)vec-1); \
        m += vecsIncr; } }

/* calculate capacity from length */
#define AARRAY_ALLOC_NOCAPACITY(TYPE) \
  size_t curSize = (*length) * sizeof(TYPE) + sizeof(size_t); \
  size_t newSize = ((*length) + (ilen-rlen)) * sizeof(TYPE) + sizeof(size_t); \
  if((!*vec || AARRAY_clz(newSize-1) < AARRAY_clz(curSize-1))) { \
    length = (size_t*)realloc(!*vec? NULL : length, \
                              AARRAY_upper_power_of_two(newSize)); \
    AARRAY_safety(, if(!length) AARRAY_Error_OutOfMemory(newSize)); \
    AARRAY_FIX_POINTERS(TYPE) } \
  *vec = (TYPE*)(length+1);

/* error if allocation required */
#define AARRAY_ALLOC_STATIC(TYPE) \
  (void)vecsIncr; (void)vecsCount; (void)vecs; \
  size_t curSize = *vec? *(*(size_t**)vec-2) : 0; \
  size_t newSize = ((*length) + (ilen-rlen)) * sizeof(TYPE) + sizeof(size_t) * 2; \
  if(!*vec) AARRAY_Error_OutOfCapacity((size_t)0, newSize); \
  AARRAY_safety((void)curSize;, \
    if(newSize>curSize) \
      AARRAY_Error_OutOfCapacity(curSize, newSize))

/* store capacity
 * alternative growth strategies might be
 * size += (size >> 3) + (size < 9 ? 3 : 6);
 * size += size >> 1; // *1.5
 * size = AARRAY_upper_power_of_two(size)
 */
#define AARRAY_ALLOC_STD(TYPE) \
  size_t curSize = *vec? *(*(size_t**)vec-2) : 0; \
  size_t newSize = ((*length) + (ilen-rlen)) * sizeof(TYPE) + sizeof(size_t) * 2; \
  if((!*vec || newSize > curSize)) { \
    newSize += newSize >> 1; \
    length = (size_t*)realloc(!*vec? NULL : length-1, newSize); \
    AARRAY_safety(, if(!length) AARRAY_Error_OutOfMemory(newSize)); \
    *length = newSize; \
    length += 1; \
    AARRAY_FIX_POINTERS(TYPE) } \
  *vec = (TYPE*)(length+1);

/* handle array allocation */
#define AARRAY_Expand(TYPE, GROWTH) \
  /* use rlen (remove length) and ilen (insert length), \
     to setup vec ready for any new data to be inserted/appended \
     -- essentially a realloc + a memmove */ \
  size_t lengthHolder = 0, *length = &lengthHolder; \
  if(*vec) length = *(size_t**)vec-1; \
  AARRAY_safety((void)errLoc;, \
  if(pos > *length) AARRAY_Error_OutOfBounds(*length, pos) \
  if(rlen > (*length) - pos) \
    AARRAY_Error_RemovalIsOutOfBounds(*length, pos, rlen))  \
  if(rlen > ilen &&   (*length)-(pos+rlen)) \
    /* move when still have items (before realloc clips them) */ \
    memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]), \
            sizeof(TYPE) * ((*length) - (pos+rlen))); \
  /* calculate curSize and newSize */ \
  AARRAY_ALLOC_##GROWTH(TYPE) \
  if(rlen < ilen &&   (*length)-(pos+rlen)) { \
    /* move when have space to put items (after realloc creates it) */ \
    memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]), \
            sizeof(TYPE) * ((*length) - (pos+rlen))); }




/* generate type specific and growth-strategy specific functions */
/* GENERATE_GENERICS creates functions for each data type,
 * FTYPE also make it specific to a block|lambda|function-pointer
 * These functions are then put into an array,
 * letting us select the right one at compile time
 */
AARRAY_define(uint8_t*AARRAY_Replace_NOCAPACITY_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t ilen, uint8_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint8_t, NOCAPACITY);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint8_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint16_t*AARRAY_Replace_NOCAPACITY_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t ilen, uint16_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint16_t, NOCAPACITY);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint16_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint32_t*AARRAY_Replace_NOCAPACITY_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t ilen, uint32_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint32_t, NOCAPACITY);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint32_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint64_t*AARRAY_Replace_NOCAPACITY_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t ilen, uint64_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint64_t, NOCAPACITY);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint64_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
static void(*const AARRAY_Replace_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Replace_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_Replace_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_Replace_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Replace_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_Replace_STATIC_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t ilen, uint8_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint8_t, STATIC);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint8_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint16_t*AARRAY_Replace_STATIC_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t ilen, uint16_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint16_t, STATIC);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint16_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint32_t*AARRAY_Replace_STATIC_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t ilen, uint32_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint32_t, STATIC);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint32_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint64_t*AARRAY_Replace_STATIC_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t ilen, uint64_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint64_t, STATIC);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint64_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
static void(*const AARRAY_Replace_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Replace_STATIC_uint8_t,
  (void(*)(void))&AARRAY_Replace_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_Replace_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Replace_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_Replace_STD_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t ilen, uint8_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint8_t, STD);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint8_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint16_t*AARRAY_Replace_STD_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t ilen, uint16_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint16_t, STD);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint16_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint32_t*AARRAY_Replace_STD_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t ilen, uint32_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint32_t, STD);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint32_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
AARRAY_define(uint64_t*AARRAY_Replace_STD_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t ilen, uint64_t items[]), {
  /* replaces section of a array with N items */
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  /* vecs is {array1 ... arrayN}, but doesn't contain pointers;
     so vsIncr skipped with setting it to 0 */
  size_t vecsCount = 1; size_t vecsIncr = 0;
  uintptr_t*vecs = (uintptr_t*)(void*)items;
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint64_t, STD);
  if(*vec) {
    if(ilen) memcpy(&((*vec)[pos]), vecs, sizeof(uint64_t) * ilen);
    *((*(size_t**)vec)-1) += ilen-rlen; }
  return *vec; })
 
static void(*const AARRAY_Replace_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Replace_STD_uint8_t,
  (void(*)(void))&AARRAY_Replace_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_Replace_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Replace_STD_uint64_t };

AARRAY_define(uint8_t*AARRAY_Append_NOCAPACITY_uint8_t(char errLoc[],
  uint8_t*vec[], size_t ilen, uint8_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_NOCAPACITY_uint8_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint16_t*AARRAY_Append_NOCAPACITY_uint16_t(char errLoc[],
  uint16_t*vec[], size_t ilen, uint16_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_NOCAPACITY_uint16_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint32_t*AARRAY_Append_NOCAPACITY_uint32_t(char errLoc[],
  uint32_t*vec[], size_t ilen, uint32_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_NOCAPACITY_uint32_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint64_t*AARRAY_Append_NOCAPACITY_uint64_t(char errLoc[],
  uint64_t*vec[], size_t ilen, uint64_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_NOCAPACITY_uint64_t(
      errLoc, vec, pos, 0, ilen, items); })
static void(*const AARRAY_Append_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Append_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_Append_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_Append_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Append_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_Append_STATIC_uint8_t(char errLoc[],
  uint8_t*vec[], size_t ilen, uint8_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STATIC_uint8_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint16_t*AARRAY_Append_STATIC_uint16_t(char errLoc[],
  uint16_t*vec[], size_t ilen, uint16_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STATIC_uint16_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint32_t*AARRAY_Append_STATIC_uint32_t(char errLoc[],
  uint32_t*vec[], size_t ilen, uint32_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STATIC_uint32_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint64_t*AARRAY_Append_STATIC_uint64_t(char errLoc[],
  uint64_t*vec[], size_t ilen, uint64_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STATIC_uint64_t(
      errLoc, vec, pos, 0, ilen, items); })
static void(*const AARRAY_Append_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Append_STATIC_uint8_t,
  (void(*)(void))&AARRAY_Append_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_Append_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Append_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_Append_STD_uint8_t(char errLoc[],
  uint8_t*vec[], size_t ilen, uint8_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STD_uint8_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint16_t*AARRAY_Append_STD_uint16_t(char errLoc[],
  uint16_t*vec[], size_t ilen, uint16_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STD_uint16_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint32_t*AARRAY_Append_STD_uint32_t(char errLoc[],
  uint32_t*vec[], size_t ilen, uint32_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STD_uint32_t(
      errLoc, vec, pos, 0, ilen, items); })
AARRAY_define(uint64_t*AARRAY_Append_STD_uint64_t(char errLoc[],
  uint64_t*vec[], size_t ilen, uint64_t items[]), {
  size_t pos = vec && *vec?
                      /* get array length */
                      *(*(size_t**)vec-1) : 0;
  return AARRAY_Replace_STD_uint64_t(
      errLoc, vec, pos, 0, ilen, items); })
static void(*const AARRAY_Append_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Append_STD_uint8_t,
  (void(*)(void))&AARRAY_Append_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_Append_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Append_STD_uint64_t };
  
AARRAY_define(uint8_t*AARRAY_Concat_NOCAPACITY_uint8_t(char errLoc[],
  uint8_t*vec[], size_t vecsCount, uint8_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint8_t, NOCAPACITY);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_Concat_NOCAPACITY_uint16_t(char errLoc[],
  uint16_t*vec[], size_t vecsCount, uint16_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint16_t, NOCAPACITY);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_Concat_NOCAPACITY_uint32_t(char errLoc[],
  uint32_t*vec[], size_t vecsCount, uint32_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint32_t, NOCAPACITY);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_Concat_NOCAPACITY_uint64_t(char errLoc[],
  uint64_t*vec[], size_t vecsCount, uint64_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint64_t, NOCAPACITY);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
static void(*const AARRAY_Concat_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Concat_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_Concat_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_Concat_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Concat_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_Concat_STATIC_uint8_t(char errLoc[],
  uint8_t*vec[], size_t vecsCount, uint8_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint8_t, STATIC);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_Concat_STATIC_uint16_t(char errLoc[],
  uint16_t*vec[], size_t vecsCount, uint16_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint16_t, STATIC);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_Concat_STATIC_uint32_t(char errLoc[],
  uint32_t*vec[], size_t vecsCount, uint32_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint32_t, STATIC);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_Concat_STATIC_uint64_t(char errLoc[],
  uint64_t*vec[], size_t vecsCount, uint64_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint64_t, STATIC);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
static void(*const AARRAY_Concat_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Concat_STATIC_uint8_t,
  (void(*)(void))&AARRAY_Concat_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_Concat_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Concat_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_Concat_STD_uint8_t(char errLoc[],
  uint8_t*vec[], size_t vecsCount, uint8_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint8_t, STD);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_Concat_STD_uint16_t(char errLoc[],
  uint16_t*vec[], size_t vecsCount, uint16_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint16_t, STD);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_Concat_STD_uint32_t(char errLoc[],
  uint32_t*vec[], size_t vecsCount, uint32_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint32_t, STD);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_Concat_STD_uint64_t(char errLoc[],
  uint64_t*vec[], size_t vecsCount, uint64_t*vecs_[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  uintptr_t*vecs = (uintptr_t*)vecs_;
  size_t rlen = 0; size_t ilen = 0;
  size_t n = (size_t)-1;
  while(++n < vecsCount) if(vecs[n])
      ilen += *((size_t*)vecs[n]-1);
  /* vecs is {vec0, ... ,vecN}, so vsIncr is 1 */
  size_t vecsIncr = 1;
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  size_t pos = *vec? *(*(size_t**)vec-1) : 0;
  AARRAY_Expand(uint64_t, STD);
  if(*vec) {
    size_t vLen = *(*(size_t**)vec-1);
    n = (size_t)-1; while(++n < vecsCount) {
      if(!vecs[n]) continue;
      size_t ns = (size_t)-1, vsnLen;
      vsnLen = *((size_t*)vecs[n]-1);
      while(++ns < vsnLen) (*vec)[vLen+ns] = vecs_[n][ns];
      vLen += vsnLen; }
    *(*(size_t**)vec-1) += ilen; }
  return *vec; })
static void(*const AARRAY_Concat_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Concat_STD_uint8_t,
  (void(*)(void))&AARRAY_Concat_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_Concat_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Concat_STD_uint64_t };

AARRAY_define(uint8_t*AARRAY_GenericArray_NOCAPACITY_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint8_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint8_t, NOCAPACITY);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint8_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_GenericArray_NOCAPACITY_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint16_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint16_t, NOCAPACITY);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint16_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_GenericArray_NOCAPACITY_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint32_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint32_t, NOCAPACITY);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint32_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_GenericArray_NOCAPACITY_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint64_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint64_t, NOCAPACITY);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint64_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
static void(*const AARRAY_GenericArray_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_GenericArray_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_GenericArray_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_GenericArray_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_GenericArray_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_GenericArray_STATIC_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint8_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint8_t, STATIC);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint8_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_GenericArray_STATIC_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint16_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint16_t, STATIC);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint16_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_GenericArray_STATIC_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint32_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint32_t, STATIC);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint32_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_GenericArray_STATIC_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint64_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint64_t, STATIC);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint64_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
static void(*const AARRAY_GenericArray_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_GenericArray_STATIC_uint8_t,
  (void(*)(void))&AARRAY_GenericArray_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_GenericArray_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_GenericArray_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_GenericArray_STD_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint8_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint8_t, STD);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint8_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_GenericArray_STD_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint16_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint16_t, STD);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint16_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_GenericArray_STD_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint32_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint32_t, STD);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint32_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_GenericArray_STD_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  size_t n = 0; size_t alen, ilen = 0;
  size_t vecsIncr = 2;
  while(n < vecsCount) {
    alen = vecs[n];
    AARRAY_safety(, if(alen && vecs[n+1]==(uintptr_t)NULL) 
        AARRAY_Error_ArrayIsNull((n+2)/2));
    if(alen==SIZE_MAX) { while(((uint64_t*)vecs[n+1])[++alen]) { }
                         vecs[n] = alen; }
    ilen += alen; n+=vecsIncr; }
  /* vecs is {len0, array0, ... lenN, arrayN}, so vsIncr is 2 */
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_Expand(uint64_t, STD);
  if(*vec) {
    n = 0; while(n < vecsCount) {
      alen = vecs[n];
      if(alen) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+1], sizeof(uint64_t)*alen);
        pos += alen; } n+=vecsIncr; }
    *(*(size_t**)vec-1) += ilen-rlen; }
  return *vec; })
static void(*const AARRAY_GenericArray_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_GenericArray_STD_uint8_t,
  (void(*)(void))&AARRAY_GenericArray_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_GenericArray_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_GenericArray_STD_uint64_t };

AARRAY_define(uint8_t*AARRAY_ReplaceArray_NOCAPACITY_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_NOCAPACITY_uint8_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint16_t*AARRAY_ReplaceArray_NOCAPACITY_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_NOCAPACITY_uint16_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint32_t*AARRAY_ReplaceArray_NOCAPACITY_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_NOCAPACITY_uint32_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint64_t*AARRAY_ReplaceArray_NOCAPACITY_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_NOCAPACITY_uint64_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
static void(*const AARRAY_ReplaceArray_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ReplaceArray_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_ReplaceArray_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_ReplaceArray_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ReplaceArray_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_ReplaceArray_STATIC_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STATIC_uint8_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint16_t*AARRAY_ReplaceArray_STATIC_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STATIC_uint16_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint32_t*AARRAY_ReplaceArray_STATIC_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STATIC_uint32_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint64_t*AARRAY_ReplaceArray_STATIC_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STATIC_uint64_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
static void(*const AARRAY_ReplaceArray_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ReplaceArray_STATIC_uint8_t,
  (void(*)(void))&AARRAY_ReplaceArray_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_ReplaceArray_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ReplaceArray_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_ReplaceArray_STD_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STD_uint8_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint16_t*AARRAY_ReplaceArray_STD_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STD_uint16_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint32_t*AARRAY_ReplaceArray_STD_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STD_uint32_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
AARRAY_define(uint64_t*AARRAY_ReplaceArray_STD_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+3) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 2, 3));
  return AARRAY_GenericArray_STD_uint64_t(
    errLoc, vec, pos, rlen, vecsCount, vecs); })
static void(*const AARRAY_ReplaceArray_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ReplaceArray_STD_uint8_t,
  (void(*)(void))&AARRAY_ReplaceArray_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_ReplaceArray_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ReplaceArray_STD_uint64_t };

AARRAY_define(uint8_t*AARRAY_AppendArray_NOCAPACITY_uint8_t(char errLoc[],
  uint8_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_NOCAPACITY_uint8_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint16_t*AARRAY_AppendArray_NOCAPACITY_uint16_t(char errLoc[],
  uint16_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_NOCAPACITY_uint16_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint32_t*AARRAY_AppendArray_NOCAPACITY_uint32_t(char errLoc[],
  uint32_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_NOCAPACITY_uint32_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint64_t*AARRAY_AppendArray_NOCAPACITY_uint64_t(char errLoc[],
  uint64_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_NOCAPACITY_uint64_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
static void(*const AARRAY_AppendArray_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_AppendArray_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_AppendArray_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_AppendArray_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_AppendArray_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_AppendArray_STATIC_uint8_t(char errLoc[],
  uint8_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STATIC_uint8_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint16_t*AARRAY_AppendArray_STATIC_uint16_t(char errLoc[],
  uint16_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STATIC_uint16_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint32_t*AARRAY_AppendArray_STATIC_uint32_t(char errLoc[],
  uint32_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STATIC_uint32_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint64_t*AARRAY_AppendArray_STATIC_uint64_t(char errLoc[],
  uint64_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STATIC_uint64_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
static void(*const AARRAY_AppendArray_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_AppendArray_STATIC_uint8_t,
  (void(*)(void))&AARRAY_AppendArray_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_AppendArray_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_AppendArray_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_AppendArray_STD_uint8_t(char errLoc[],
  uint8_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STD_uint8_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint16_t*AARRAY_AppendArray_STD_uint16_t(char errLoc[],
  uint16_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STD_uint16_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint32_t*AARRAY_AppendArray_STD_uint32_t(char errLoc[],
  uint32_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STD_uint32_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
AARRAY_define(uint64_t*AARRAY_AppendArray_STD_uint64_t(char errLoc[],
  uint64_t*vec[], size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  AARRAY_safety(, if((vecsCount+1) % 2 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+1, 2, 1));
  size_t pos = vec && *vec? *(*(size_t**)vec-1) : 0;
  return AARRAY_GenericArray_STD_uint64_t(
    errLoc, vec, pos, 0, vecsCount, vecs); })
static void(*const AARRAY_AppendArray_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_AppendArray_STD_uint8_t,
  (void(*)(void))&AARRAY_AppendArray_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_AppendArray_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_AppendArray_STD_uint64_t };

AARRAY_define(uint8_t*AARRAY_Multi_NOCAPACITY_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint8_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_NOCAPACITY(uint8_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint8_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint8_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_Multi_NOCAPACITY_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint16_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_NOCAPACITY(uint16_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint16_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint16_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_Multi_NOCAPACITY_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint32_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_NOCAPACITY(uint32_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint32_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint32_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_Multi_NOCAPACITY_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint64_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_NOCAPACITY(uint64_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint64_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint64_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
static void(*const AARRAY_Multi_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Multi_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_Multi_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_Multi_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Multi_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_Multi_STATIC_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint8_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STATIC(uint8_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint8_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint8_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_Multi_STATIC_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint16_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STATIC(uint16_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint16_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint16_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_Multi_STATIC_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint32_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STATIC(uint32_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint32_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint32_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_Multi_STATIC_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint64_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STATIC(uint64_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint64_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint64_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
static void(*const AARRAY_Multi_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Multi_STATIC_uint8_t,
  (void(*)(void))&AARRAY_Multi_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_Multi_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Multi_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_Multi_STD_uint8_t(char errLoc[],
  uint8_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint8_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint8_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STD(uint8_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint8_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint8_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint16_t*AARRAY_Multi_STD_uint16_t(char errLoc[],
  uint16_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint16_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint16_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STD(uint16_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint16_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint16_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint32_t*AARRAY_Multi_STD_uint32_t(char errLoc[],
  uint32_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint32_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint32_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STD(uint32_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint32_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint32_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
AARRAY_define(uint64_t*AARRAY_Multi_STD_uint64_t(char errLoc[],
  uint64_t*vec[], size_t pos, size_t rlen, size_t vecsCount, uintptr_t vecs[]), {
  if(vec) { AARRAY_prefetch((size_t*)*vec-1, 1, 1); }
  size_t lengthHolder = 0, *length = &lengthHolder;
  if(vec && *vec) length = *(size_t**)vec-1;
  size_t n = 0, alen, ilen = 0, atimes, newlen = *length, maxlen = newlen;
  size_t saved_pos = pos, saved_rlen = rlen;
  size_t vecsIncr = 5;
  AARRAY_safety(, if((vecsCount+3) % 5 != 1)
      AARRAY_Error_WrongArgCount(vecsCount+3, 5, 1));
  while(n < vecsCount) {
    atimes = vecs[n]; alen = vecs[n+1];
    AARRAY_safety(, if(atimes && alen  && !vecs[n+2])
        AARRAY_Error_ArrayIsNull(n/5+1));
    if(alen==SIZE_MAX) { while(((uint64_t*)vecs[n+2])[++alen]) { }
                         vecs[n+1] = alen; }
    /* check input, and work out max length required for array */
    if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
    if(pos > newlen) AARRAY_Error_OutOfBounds(*length, pos)
    if(rlen > newlen - pos) AARRAY_Error_RemovalIsOutOfBounds(newlen, pos, rlen)
    ilen = atimes? atimes * alen : alen;
    newlen += ilen - rlen;
    if(newlen > maxlen) maxlen = newlen;
    n+=vecsIncr; }
  ilen = maxlen - *length; rlen = 0;
  /* vecs is {times0, len0, array0, ... timesN, lenN, arrayN}, so vsIncr is 3 */
  uint64_t*vecHolder = NULL; if(!vec) vec = &vecHolder;
  AARRAY_ALLOC_STD(uint64_t)
  if(*vec) {
    n = 0; pos = saved_pos; rlen = saved_rlen;
    do {
      atimes = vecs[n]; alen = vecs[n+1];
      if(n) { pos = vecs[n-2]; rlen = vecs[n-1]; }
      ilen = atimes? atimes * alen : alen;
      if(ilen-rlen) {
        memmove(&((*vec)[pos+ilen]), &((*vec)[pos+rlen]),
                sizeof(uint64_t) * ((*length) - (pos+rlen)));
        *length += ilen-rlen; }
      if(!atimes) {}
      else while(atimes--) {
        memcpy(&((*vec)[pos]), (void*)vecs[n+2], sizeof(uint64_t) * alen);
        pos += alen; }
      n+=vecsIncr;
    } while(n < vecsCount); }
  return *vec; })
static void(*const AARRAY_Multi_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Multi_STD_uint8_t,
  (void(*)(void))&AARRAY_Multi_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_Multi_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Multi_STD_uint64_t };

AARRAY_define(uint8_t*AARRAY_Mem_NOCAPACITY_uint8_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(1) {

    return (uint8_t*)(vec-1); }
  return (uint8_t*)(vec-2); })
AARRAY_define(uint16_t*AARRAY_Mem_NOCAPACITY_uint16_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(1) {

    return (uint16_t*)(vec-1); }
  return (uint16_t*)(vec-2); })
AARRAY_define(uint32_t*AARRAY_Mem_NOCAPACITY_uint32_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(1) {

    return (uint32_t*)(vec-1); }
  return (uint32_t*)(vec-2); })
AARRAY_define(uint64_t*AARRAY_Mem_NOCAPACITY_uint64_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(1) {

    return (uint64_t*)(vec-1); }
  return (uint64_t*)(vec-2); })
static void(*const AARRAY_Mem_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Mem_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_Mem_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_Mem_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Mem_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_Mem_STATIC_uint8_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint8_t*)(vec-1); }
  return (uint8_t*)(vec-2); })
AARRAY_define(uint16_t*AARRAY_Mem_STATIC_uint16_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint16_t*)(vec-1); }
  return (uint16_t*)(vec-2); })
AARRAY_define(uint32_t*AARRAY_Mem_STATIC_uint32_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint32_t*)(vec-1); }
  return (uint32_t*)(vec-2); })
AARRAY_define(uint64_t*AARRAY_Mem_STATIC_uint64_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint64_t*)(vec-1); }
  return (uint64_t*)(vec-2); })
static void(*const AARRAY_Mem_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Mem_STATIC_uint8_t,
  (void(*)(void))&AARRAY_Mem_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_Mem_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Mem_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_Mem_STD_uint8_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint8_t*)(vec-1); }
  return (uint8_t*)(vec-2); })
AARRAY_define(uint16_t*AARRAY_Mem_STD_uint16_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint16_t*)(vec-1); }
  return (uint16_t*)(vec-2); })
AARRAY_define(uint32_t*AARRAY_Mem_STD_uint32_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint32_t*)(vec-1); }
  return (uint32_t*)(vec-2); })
AARRAY_define(uint64_t*AARRAY_Mem_STD_uint64_t(
    char errLoc[], size_t vec[]), {
  (void)errLoc;
  if(!vec) return NULL;
  if(0) {

    return (uint64_t*)(vec-1); }
  return (uint64_t*)(vec-2); })
static void(*const AARRAY_Mem_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Mem_STD_uint8_t,
  (void(*)(void))&AARRAY_Mem_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_Mem_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Mem_STD_uint64_t };

AARRAY_define(uint8_t*AARRAY_Array_NOCAPACITY_uint8_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(1) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint8_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint8_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint8_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint8_t*)(vec+2); } })
AARRAY_define(uint16_t*AARRAY_Array_NOCAPACITY_uint16_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(1) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint16_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint16_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint16_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint16_t*)(vec+2); } })
AARRAY_define(uint32_t*AARRAY_Array_NOCAPACITY_uint32_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(1) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint32_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint32_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint32_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint32_t*)(vec+2); } })
AARRAY_define(uint64_t*AARRAY_Array_NOCAPACITY_uint64_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(1) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint64_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint64_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint64_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint64_t*)(vec+2); } })
static void(*const AARRAY_Array_NOCAPACITY_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Array_NOCAPACITY_uint8_t,
  (void(*)(void))&AARRAY_Array_NOCAPACITY_uint16_t, 0,
  (void(*)(void))&AARRAY_Array_NOCAPACITY_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Array_NOCAPACITY_uint64_t };
AARRAY_define(uint8_t*AARRAY_Array_STATIC_uint8_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint8_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint8_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint8_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint8_t*)(vec+2); } })
AARRAY_define(uint16_t*AARRAY_Array_STATIC_uint16_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint16_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint16_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint16_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint16_t*)(vec+2); } })
AARRAY_define(uint32_t*AARRAY_Array_STATIC_uint32_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint32_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint32_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint32_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint32_t*)(vec+2); } })
AARRAY_define(uint64_t*AARRAY_Array_STATIC_uint64_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint64_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint64_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint64_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint64_t*)(vec+2); } })
static void(*const AARRAY_Array_STATIC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Array_STATIC_uint8_t,
  (void(*)(void))&AARRAY_Array_STATIC_uint16_t, 0,
  (void(*)(void))&AARRAY_Array_STATIC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Array_STATIC_uint64_t };
AARRAY_define(uint8_t*AARRAY_Array_STD_uint8_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint8_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint8_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint8_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint8_t*)(vec+2); } })
AARRAY_define(uint16_t*AARRAY_Array_STD_uint16_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint16_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint16_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint16_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint16_t*)(vec+2); } })
AARRAY_define(uint32_t*AARRAY_Array_STD_uint32_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint32_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint32_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint32_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint32_t*)(vec+2); } })
AARRAY_define(uint64_t*AARRAY_Array_STD_uint64_t(
    char errLoc[], size_t vec[], size_t sizebytes, int maximize), {
  AARRAY_safety((void)errLoc;, if(!vec) AARRAY_Error_ArrayIsNull((size_t)0));
  if(0) {

	  AARRAY_safety(,if(sizebytes < sizeof(size_t)  ) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)  ))
	  /* greatest len power-of-two that fits sizebytes */
    size_t len = ((size_t)1)<<(63-AARRAY_clz((sizebytes-sizeof(size_t))/sizeof(uint64_t)));
    if(maximize || len < *vec) *vec = len;
    return (uint64_t*)(vec+1); }
  else {
	  AARRAY_safety(,if(sizebytes < sizeof(size_t)*2) AARRAY_Error_OutOfCapacity(sizebytes, sizeof(size_t)*2))
    size_t len = (sizebytes - sizeof(size_t)*2) / sizeof(uint64_t);
    if(maximize || len < *(vec+1)) *(vec+1) = len;
    *vec = sizebytes;
    return (uint64_t*)(vec+2); } })
static void(*const AARRAY_Array_STD_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Array_STD_uint8_t,
  (void(*)(void))&AARRAY_Array_STD_uint16_t, 0,
  (void(*)(void))&AARRAY_Array_STD_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Array_STD_uint64_t };




/* get the number of var-arg arguments, and make them generic */
/* as above, but for 64 bit only, such as aFmt */
/* make c++ happy */
/* as above, but hide compiler warnings for mixed int/pointer arrays
 * uintptr_t conversion of first arg resolves gcc issue
 */
/* get around requirement for __VA_ARGS__ to not be empty */
#define AARRAY_ArgsTail(A, ...) __VA_ARGS__

#if defined(AARRAY_NOTYPEOF) \
    || (defined(_MSC_VER) && !__has_feature(cxx_decltype))
/* create api functions without type-casts */
#define AARRAY_typeof(TYPE, EXPR) EXPR
#elif __has_feature(cxx_decltype)
/* +0 resolves c++ compliance */
#define AARRAY_typeof(TYPE, EXPR) (__decltype(TYPE+0))(EXPR)
#else
#define AARRAY_typeof(TYPE, EXPR) \
  AARRAY_nowarn_internal_start (__typeof(TYPE))(EXPR) AARRAY_nowarn_internal_end
#endif

/* generate the main apis for c, c++, and compilers without __typeof support */
#define aAppend_NOCAPACITY(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, void*)) \
         AARRAY_Append_NOCAPACITY_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, AARRAY_nowarn_internal_start \
  (sizeof(**vec)==1? sizeof((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? sizeof((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? sizeof((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) / sizeof(**vec), \
  (sizeof(**vec)==1? (void*)AARRAY_move((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? (void*)AARRAY_move((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? (void*)AARRAY_move((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   (void*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) \
   AARRAY_nowarn_internal_end))
#define aReplace_NOCAPACITY(vec, pos, rlen, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, void*)) \
         AARRAY_Replace_NOCAPACITY_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, \
                              AARRAY_nowarn_internal_start \
  (sizeof(**vec)==1? sizeof((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? sizeof((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? sizeof((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) / sizeof(**vec), \
  (sizeof(**vec)==1? (void*)AARRAY_move((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? (void*)AARRAY_move((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? (void*)AARRAY_move((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   (void*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) \
   AARRAY_nowarn_internal_end))
#define aDelete_NOCAPACITY(vec, pos, rlen) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, void*)) \
         AARRAY_Replace_NOCAPACITY_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, 0, NULL))
#define aConcat_NOCAPACITY(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, uintptr_t*)) \
         AARRAY_Concat_NOCAPACITY_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, \
  sizeof((void*[]){(void*)__VA_ARGS__}) / sizeof(void*), \
  (uintptr_t*)AARRAY_move((void*[]){(void*)__VA_ARGS__})))
#define aAppendArray_NOCAPACITY(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, uintptr_t*)) \
         AARRAY_AppendArray_NOCAPACITY_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aReplaceArray_NOCAPACITY(vec, pos, rlen, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, uintptr_t*)) \
          AARRAY_ReplaceArray_NOCAPACITY_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aMulti_NOCAPACITY(vec, pos, rlen, arrTimes, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, uintptr_t*)) \
         AARRAY_Multi_NOCAPACITY_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
  (AARRAY_LINE, (void**)vec, pos, rlen, \
   AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)arrTimes, __VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)arrTimes, __VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aMem_NOCAPACITY(vec) \
  (AARRAY_typeof(vec, (void*(*)(char[], void*)) \
         AARRAY_Mem_NOCAPACITY_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, (void*)vec))
#define aArray_NOCAPACITY(vec, size, maximize) \
  (AARRAY_typeof(vec, (void*(*)(char[], void*, size_t, int)) \
         AARRAY_Array_NOCAPACITY_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, (void*)vec, size, maximize))
#define aAppend_STATIC(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, void*)) \
         AARRAY_Append_STATIC_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, AARRAY_nowarn_internal_start \
  (sizeof(**vec)==1? sizeof((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? sizeof((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? sizeof((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) / sizeof(**vec), \
  (sizeof(**vec)==1? (void*)AARRAY_move((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? (void*)AARRAY_move((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? (void*)AARRAY_move((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   (void*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) \
   AARRAY_nowarn_internal_end))
#define aReplace_STATIC(vec, pos, rlen, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, void*)) \
         AARRAY_Replace_STATIC_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, \
                              AARRAY_nowarn_internal_start \
  (sizeof(**vec)==1? sizeof((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? sizeof((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? sizeof((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) / sizeof(**vec), \
  (sizeof(**vec)==1? (void*)AARRAY_move((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? (void*)AARRAY_move((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? (void*)AARRAY_move((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   (void*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) \
   AARRAY_nowarn_internal_end))
#define aDelete_STATIC(vec, pos, rlen) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, void*)) \
         AARRAY_Replace_STATIC_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, 0, NULL))
#define aConcat_STATIC(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, uintptr_t*)) \
         AARRAY_Concat_STATIC_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, \
  sizeof((void*[]){(void*)__VA_ARGS__}) / sizeof(void*), \
  (uintptr_t*)AARRAY_move((void*[]){(void*)__VA_ARGS__})))
#define aAppendArray_STATIC(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, uintptr_t*)) \
         AARRAY_AppendArray_STATIC_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aReplaceArray_STATIC(vec, pos, rlen, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, uintptr_t*)) \
          AARRAY_ReplaceArray_STATIC_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aMulti_STATIC(vec, pos, rlen, arrTimes, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, uintptr_t*)) \
         AARRAY_Multi_STATIC_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
  (AARRAY_LINE, (void**)vec, pos, rlen, \
   AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)arrTimes, __VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)arrTimes, __VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aMem_STATIC(vec) \
  (AARRAY_typeof(vec, (void*(*)(char[], void*)) \
         AARRAY_Mem_STATIC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, (void*)vec))
#define aArray_STATIC(vec, size, maximize) \
  (AARRAY_typeof(vec, (void*(*)(char[], void*, size_t, int)) \
         AARRAY_Array_STATIC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, (void*)vec, size, maximize))
#define aAppend(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, void*)) \
         AARRAY_Append_STD_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, AARRAY_nowarn_internal_start \
  (sizeof(**vec)==1? sizeof((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? sizeof((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? sizeof((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) / sizeof(**vec), \
  (sizeof(**vec)==1? (void*)AARRAY_move((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? (void*)AARRAY_move((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? (void*)AARRAY_move((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   (void*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) \
   AARRAY_nowarn_internal_end))
#define aReplace(vec, pos, rlen, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, void*)) \
         AARRAY_Replace_STD_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, \
                              AARRAY_nowarn_internal_start \
  (sizeof(**vec)==1? sizeof((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? sizeof((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? sizeof((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) / sizeof(**vec), \
  (sizeof(**vec)==1? (void*)AARRAY_move((uint8_t []){(uint8_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)==2? (void*)AARRAY_move((uint16_t[]){(uint16_t)(uintptr_t)__VA_ARGS__}) : \
   sizeof(**vec)<=4? (void*)AARRAY_move((uint32_t[]){(uint32_t)(uintptr_t)__VA_ARGS__}) : \
   (void*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__})) \
   AARRAY_nowarn_internal_end))
#define aDelete(vec, pos, rlen) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, void*)) \
         AARRAY_Replace_STD_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, 0, NULL))
#define aConcat(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, uintptr_t*)) \
         AARRAY_Concat_STD_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, \
  sizeof((void*[]){(void*)__VA_ARGS__}) / sizeof(void*), \
  (uintptr_t*)AARRAY_move((void*[]){(void*)__VA_ARGS__})))
#define aAppendArray(vec, ...) \
  (AARRAY_typeof(*vec, (void*(*)(char[], void**, size_t, uintptr_t*)) \
         AARRAY_AppendArray_STD_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aReplaceArray(vec, pos, rlen, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, uintptr_t*)) \
          AARRAY_ReplaceArray_STD_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
   (AARRAY_LINE, (void**)vec, pos, rlen, AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aMulti(vec, pos, rlen, arrTimes, ...) \
  (AARRAY_typeof(*vec, (void*(*)( \
     char[], void**, size_t, size_t, size_t, uintptr_t*)) \
         AARRAY_Multi_STD_FUNCTIONS[sizeof(**vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(**vec)]) \
  (AARRAY_LINE, (void**)vec, pos, rlen, \
   AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)arrTimes, __VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)arrTimes, __VA_ARGS__}) \
  AARRAY_nowarn_internal_end))
#define aMem(vec) \
  (AARRAY_typeof(vec, (void*(*)(char[], void*)) \
         AARRAY_Mem_STD_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, (void*)vec))
#define aArray(vec, size, maximize) \
  (AARRAY_typeof(vec, (void*(*)(char[], void*, size_t, int)) \
         AARRAY_Array_STD_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, (void*)vec, size, maximize))

/* make pointer casts safer: ensuring data can become (voidstars)data */
AARRAY_define(void AARRAY_Free(void*vec[]), {
  if(*vec) { free((size_t*)*vec-2); *vec = NULL; } })
AARRAY_define(void AARRAY_Free_STATIC(void*vec[]), {
   AARRAY_Free(vec); }) /* to make stack traces clearer */
AARRAY_define(void AARRAY_Free_NOCAPACITY(void*vec[]), {
  if(*vec) { free((size_t*)*vec-1); *vec = NULL; } })
#define aFree(vec) \
  AARRAY_Free(0?(void**)(uintptr_t)sizeof( \
  **vec/*  **ptr check failed --- use (type**) --- */ \
  ):(void**)vec)
#define aFree_STATIC(vec) \
  AARRAY_Free_STATIC(0?(void**)(uintptr_t)sizeof( \
  **vec/*  **ptr check failed --- use (type**) --- */ \
  ):(void**)vec)
#define aFree_NOCAPACITY(vec) \
  AARRAY_Free_NOCAPACITY(0?(void**)(uintptr_t)sizeof( \
  **vec/*  **ptr check failed --- use (type**) --- */ \
  ):(void**)vec)




/* supporting api */
AARRAY_define(size_t AARRAY_aCapacity(void*vec), {
  return !vec? 0 : *((size_t*)vec-2) - sizeof(size_t)*2; })
AARRAY_define(size_t AARRAY_aCapacity_STATIC(void*vec), {
  return AARRAY_aCapacity(vec); })
AARRAY_define(size_t AARRAY_aCapacity_NOCAPACITY(void*vec), {
  return !vec? 0 : ((size_t)1) << (64-AARRAY_clz((*((size_t*)vec-1))-1)); })
#define aCapacity(vec)            (AARRAY_aCapacity(0?(void*)(uintptr_t)sizeof( \
  *vec/*  *ptr check failed --- use (type**) --- */ \
  ):(void*)vec)/sizeof(*vec))
#define aCapacity_STATIC(vec)     (AARRAY_aCapacity_STATIC(0?(void*)(uintptr_t)sizeof( \
  *vec/*  *ptr check failed --- use (type**) --- */ \
  ):(void*)vec)/sizeof(*vec))
#define aCapacity_NOCAPACITY(vec)  AARRAY_aCapacity_NOCAPACITY(0?(void*)(uintptr_t)sizeof( \
  *vec/*  *ptr check failed --- use (type**) --- */ \
  ):(void*)vec)

AARRAY_define(size_t AARRAY_aLength(void*vec), {
  return !vec? 0 : *((size_t*)vec-1); })
#define aLength(vec) AARRAY_aLength(0?(void*)(uintptr_t)sizeof( \
  *vec/*  *ptr check failed --- use (type**) --- */ \
  ):(void*)vec)

AARRAY_define(uint8_t AARRAY_Length2__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos), {
  if(!vec) AARRAY_safety(return 0; (void)errLoc;,
    { if(pos==0) return 0; else AARRAY_Error_OutOfBounds(aLength(vec), pos); })
  size_t*length = (size_t*)(void*)vec-1;
  AARRAY_safety(, if(pos > *length)
      AARRAY_Error_OutOfBounds(aLength(vec), pos));
  *length = pos;
  /* if(pos==0) { pos-1 would index into *length (which is 0);
                  so vec[pos-1] is safe and returns 0 } */
  if(pos==0) return 0; /* but keep it sanitizer safe */
  return vec[pos-1]; })
AARRAY_define(uint16_t AARRAY_Length2__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos), {
  if(!vec) AARRAY_safety(return 0; (void)errLoc;,
    { if(pos==0) return 0; else AARRAY_Error_OutOfBounds(aLength(vec), pos); })
  size_t*length = (size_t*)(void*)vec-1;
  AARRAY_safety(, if(pos > *length)
      AARRAY_Error_OutOfBounds(aLength(vec), pos));
  *length = pos;
  /* if(pos==0) { pos-1 would index into *length (which is 0);
                  so vec[pos-1] is safe and returns 0 } */
  if(pos==0) return 0; /* but keep it sanitizer safe */
  return vec[pos-1]; })
AARRAY_define(uint32_t AARRAY_Length2__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos), {
  if(!vec) AARRAY_safety(return 0; (void)errLoc;,
    { if(pos==0) return 0; else AARRAY_Error_OutOfBounds(aLength(vec), pos); })
  size_t*length = (size_t*)(void*)vec-1;
  AARRAY_safety(, if(pos > *length)
      AARRAY_Error_OutOfBounds(aLength(vec), pos));
  *length = pos;
  /* if(pos==0) { pos-1 would index into *length (which is 0);
                  so vec[pos-1] is safe and returns 0 } */
  if(pos==0) return 0; /* but keep it sanitizer safe */
  return vec[pos-1]; })
AARRAY_define(uint64_t AARRAY_Length2__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos), {
  if(!vec) AARRAY_safety(return 0; (void)errLoc;,
    { if(pos==0) return 0; else AARRAY_Error_OutOfBounds(aLength(vec), pos); })
  size_t*length = (size_t*)(void*)vec-1;
  AARRAY_safety(, if(pos > *length)
      AARRAY_Error_OutOfBounds(aLength(vec), pos));
  *length = pos;
  /* if(pos==0) { pos-1 would index into *length (which is 0);
                  so vec[pos-1] is safe and returns 0 } */
  if(pos==0) return 0; /* but keep it sanitizer safe */
  return vec[pos-1]; })
static void(*const AARRAY_Length2__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Length2__uint8_t,
  (void(*)(void))&AARRAY_Length2__uint16_t, 0,
  (void(*)(void))&AARRAY_Length2__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Length2__uint64_t };
#define aLength2(vec, len) \
  AARRAY_typeof(*vec, (uint64_t(*)(char[], void*, size_t)) \
            AARRAY_Length2__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, len)

AARRAY_define(uint8_t AARRAY_ZLength2__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos), {
  return AARRAY_Length2__uint8_t(errLoc, vec, aLength(vec) - pos); })
AARRAY_define(uint16_t AARRAY_ZLength2__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos), {
  return AARRAY_Length2__uint16_t(errLoc, vec, aLength(vec) - pos); })
AARRAY_define(uint32_t AARRAY_ZLength2__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos), {
  return AARRAY_Length2__uint32_t(errLoc, vec, aLength(vec) - pos); })
AARRAY_define(uint64_t AARRAY_ZLength2__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos), {
  return AARRAY_Length2__uint64_t(errLoc, vec, aLength(vec) - pos); })
static void(*const AARRAY_ZLength2__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ZLength2__uint8_t,
  (void(*)(void))&AARRAY_ZLength2__uint16_t, 0,
  (void(*)(void))&AARRAY_ZLength2__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ZLength2__uint64_t };
#define aZLength2(vec, len) \
  (AARRAY_typeof(*vec, (uint64_t(*)(char[], void*, size_t)) \
            AARRAY_ZLength2__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, len))

AARRAY_define(uint8_t*AARRAY_AtPtr__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return &(vec[pos]); })
AARRAY_define(uint16_t*AARRAY_AtPtr__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return &(vec[pos]); })
AARRAY_define(uint32_t*AARRAY_AtPtr__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return &(vec[pos]); })
AARRAY_define(uint64_t*AARRAY_AtPtr__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return &(vec[pos]); })
static void(*const AARRAY_AtPtr__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_AtPtr__uint8_t,
  (void(*)(void))&AARRAY_AtPtr__uint16_t, 0,
  (void(*)(void))&AARRAY_AtPtr__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_AtPtr__uint64_t };
#define aAtPtr(vec, pos) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, size_t)) \
   AARRAY_AtPtr__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, pos))

AARRAY_define(uint8_t*AARRAY_ZAtPtr__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos), {
  return AARRAY_AtPtr__uint8_t(errLoc, vec, aLength(vec) - (pos+1)); })
AARRAY_define(uint16_t*AARRAY_ZAtPtr__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos), {
  return AARRAY_AtPtr__uint16_t(errLoc, vec, aLength(vec) - (pos+1)); })
AARRAY_define(uint32_t*AARRAY_ZAtPtr__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos), {
  return AARRAY_AtPtr__uint32_t(errLoc, vec, aLength(vec) - (pos+1)); })
AARRAY_define(uint64_t*AARRAY_ZAtPtr__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos), {
  return AARRAY_AtPtr__uint64_t(errLoc, vec, aLength(vec) - (pos+1)); })
static void(*const AARRAY_ZAtPtr__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ZAtPtr__uint8_t,
  (void(*)(void))&AARRAY_ZAtPtr__uint16_t, 0,
  (void(*)(void))&AARRAY_ZAtPtr__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ZAtPtr__uint64_t };
#define aZAtPtr(vec, pos) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, size_t)) \
   AARRAY_ZAtPtr__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, pos))

AARRAY_define(uint8_t AARRAY_At__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return vec[pos]; })
AARRAY_define(uint16_t AARRAY_At__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return vec[pos]; })
AARRAY_define(uint32_t AARRAY_At__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return vec[pos]; })
AARRAY_define(uint64_t AARRAY_At__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  return vec[pos]; })
static void(*const AARRAY_At__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_At__uint8_t,
  (void(*)(void))&AARRAY_At__uint16_t, 0,
  (void(*)(void))&AARRAY_At__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_At__uint64_t };
#define aAt(vec, pos) \
  (AARRAY_typeof(*vec, (uint64_t(*)(char[], void*, size_t)) \
   AARRAY_At__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, pos))

AARRAY_define(uint8_t AARRAY_ZAt__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos), {
  return AARRAY_At__uint8_t(errLoc, vec, aLength(vec) - (pos+1)); })
AARRAY_define(uint16_t AARRAY_ZAt__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos), {
  return AARRAY_At__uint16_t(errLoc, vec, aLength(vec) - (pos+1)); })
AARRAY_define(uint32_t AARRAY_ZAt__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos), {
  return AARRAY_At__uint32_t(errLoc, vec, aLength(vec) - (pos+1)); })
AARRAY_define(uint64_t AARRAY_ZAt__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos), {
  return AARRAY_At__uint64_t(errLoc, vec, aLength(vec) - (pos+1)); })
static void(*const AARRAY_ZAt__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ZAt__uint8_t,
  (void(*)(void))&AARRAY_ZAt__uint16_t, 0,
  (void(*)(void))&AARRAY_ZAt__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ZAt__uint64_t };
#define aZAt(vec, pos) \
  (AARRAY_typeof(*vec, (uint64_t(*)(char[], void*, size_t)) \
   AARRAY_ZAt__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, pos))

AARRAY_define(uint8_t AARRAY_At2__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, uint8_t item), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  vec[pos] = item;
  return item; })
AARRAY_define(uint16_t AARRAY_At2__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, uint16_t item), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  vec[pos] = item;
  return item; })
AARRAY_define(uint32_t AARRAY_At2__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, uint32_t item), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  vec[pos] = item;
  return item; })
AARRAY_define(uint64_t AARRAY_At2__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, uint64_t item), {
  AARRAY_safety((void)errLoc;,
    if(pos >= aLength(vec)) AARRAY_Error_OutOfBounds(aLength(vec), pos));
  vec[pos] = item;
  return item; })
static void(*const AARRAY_At2__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_At2__uint8_t,
  (void(*)(void))&AARRAY_At2__uint16_t, 0,
  (void(*)(void))&AARRAY_At2__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_At2__uint64_t };
#define aAt2(vec, pos, item) \
  (AARRAY_typeof(*vec, (uint64_t(*)(char[], void*, size_t, uint64_t)) \
   AARRAY_At2__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, pos, \
    AARRAY_nowarn_internal_start (uint64_t)item AARRAY_nowarn_internal_end))

AARRAY_define(uint8_t AARRAY_ZAt2__uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, uint8_t item), {
  return AARRAY_At2__uint8_t(errLoc, vec, aLength(vec) - (pos+1), item); })
AARRAY_define(uint16_t AARRAY_ZAt2__uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, uint16_t item), {
  return AARRAY_At2__uint16_t(errLoc, vec, aLength(vec) - (pos+1), item); })
AARRAY_define(uint32_t AARRAY_ZAt2__uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, uint32_t item), {
  return AARRAY_At2__uint32_t(errLoc, vec, aLength(vec) - (pos+1), item); })
AARRAY_define(uint64_t AARRAY_ZAt2__uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, uint64_t item), {
  return AARRAY_At2__uint64_t(errLoc, vec, aLength(vec) - (pos+1), item); })
static void(*const AARRAY_ZAt2__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ZAt2__uint8_t,
  (void(*)(void))&AARRAY_ZAt2__uint16_t, 0,
  (void(*)(void))&AARRAY_ZAt2__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ZAt2__uint64_t };
#define aZAt2(vec, pos, item) \
  (AARRAY_typeof(*vec, (uint64_t(*)(char[], void*, size_t, uint64_t)) \
   AARRAY_ZAt2__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, (void*)vec, pos, \
    AARRAY_nowarn_internal_start item AARRAY_nowarn_internal_end))

AARRAY_define(int AARRAY_Cmp__uint8_t(
    uint8_t vec[], size_t n, uint8_t*vecs[]), {
  while(n--) {
    if(vec == vecs[n]) continue;
    if(aLength(vec) != aLength(vecs[n])) return 0;
    if(aLength(vec)==0) continue;
    /* surely memcmp would work, but this upsets MSan
       memcmp(((size_t*)(uintptr_t)vec-1), ((size_t*)(uintptr_t)vecs[n]-1),
              aLength(vec)*sizeof(uint8_t)+sizeof(size_t)); */
    size_t m = SIZE_MAX; while(++m < aLength(vec)) if(vec[m]!=vecs[n][m]) return 0; }
  return 1; })
AARRAY_define(int AARRAY_Cmp__uint16_t(
    uint16_t vec[], size_t n, uint16_t*vecs[]), {
  while(n--) {
    if(vec == vecs[n]) continue;
    if(aLength(vec) != aLength(vecs[n])) return 0;
    if(aLength(vec)==0) continue;
    /* surely memcmp would work, but this upsets MSan
       memcmp(((size_t*)(uintptr_t)vec-1), ((size_t*)(uintptr_t)vecs[n]-1),
              aLength(vec)*sizeof(uint16_t)+sizeof(size_t)); */
    size_t m = SIZE_MAX; while(++m < aLength(vec)) if(vec[m]!=vecs[n][m]) return 0; }
  return 1; })
AARRAY_define(int AARRAY_Cmp__uint32_t(
    uint32_t vec[], size_t n, uint32_t*vecs[]), {
  while(n--) {
    if(vec == vecs[n]) continue;
    if(aLength(vec) != aLength(vecs[n])) return 0;
    if(aLength(vec)==0) continue;
    /* surely memcmp would work, but this upsets MSan
       memcmp(((size_t*)(uintptr_t)vec-1), ((size_t*)(uintptr_t)vecs[n]-1),
              aLength(vec)*sizeof(uint32_t)+sizeof(size_t)); */
    size_t m = SIZE_MAX; while(++m < aLength(vec)) if(vec[m]!=vecs[n][m]) return 0; }
  return 1; })
AARRAY_define(int AARRAY_Cmp__uint64_t(
    uint64_t vec[], size_t n, uint64_t*vecs[]), {
  while(n--) {
    if(vec == vecs[n]) continue;
    if(aLength(vec) != aLength(vecs[n])) return 0;
    if(aLength(vec)==0) continue;
    /* surely memcmp would work, but this upsets MSan
       memcmp(((size_t*)(uintptr_t)vec-1), ((size_t*)(uintptr_t)vecs[n]-1),
              aLength(vec)*sizeof(uint64_t)+sizeof(size_t)); */
    size_t m = SIZE_MAX; while(++m < aLength(vec)) if(vec[m]!=vecs[n][m]) return 0; }
  return 1; })
static void(*const AARRAY_Cmp__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Cmp__uint8_t,
  (void(*)(void))&AARRAY_Cmp__uint16_t, 0,
  (void(*)(void))&AARRAY_Cmp__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Cmp__uint64_t };
#define aCmp(vec, ...) \
  (((int(*)(void*, size_t, void*)) \
   AARRAY_Cmp__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, \
  sizeof((void*[]){(void*)__VA_ARGS__}) / sizeof(void*), \
  (uintptr_t*)AARRAY_move((void*[]){(void*)__VA_ARGS__})))

AARRAY_define(size_t AARRAY_IndexOf__uint8_t(uint8_t vec[], uint8_t item), {
  size_t length = aLength(vec), i = (size_t)-1;
  while(++i < length) if(vec[i]==item) return i;
  return (size_t)-1; })
AARRAY_define(size_t AARRAY_IndexOf__uint16_t(uint16_t vec[], uint16_t item), {
  size_t length = aLength(vec), i = (size_t)-1;
  while(++i < length) if(vec[i]==item) return i;
  return (size_t)-1; })
AARRAY_define(size_t AARRAY_IndexOf__uint32_t(uint32_t vec[], uint32_t item), {
  size_t length = aLength(vec), i = (size_t)-1;
  while(++i < length) if(vec[i]==item) return i;
  return (size_t)-1; })
AARRAY_define(size_t AARRAY_IndexOf__uint64_t(uint64_t vec[], uint64_t item), {
  size_t length = aLength(vec), i = (size_t)-1;
  while(++i < length) if(vec[i]==item) return i;
  return (size_t)-1; })
static void(*const AARRAY_IndexOf__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_IndexOf__uint8_t,
  (void(*)(void))&AARRAY_IndexOf__uint16_t, 0,
  (void(*)(void))&AARRAY_IndexOf__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_IndexOf__uint64_t };
#define aIndexOf(vec, item) \
  ((size_t(*)(void*, uint64_t)) \
   AARRAY_IndexOf__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(vec, \
  AARRAY_nowarn_internal_start item AARRAY_nowarn_internal_end)

AARRAY_define(size_t AARRAY_ZIndexOf__uint8_t(uint8_t vec[], uint8_t item), {
  size_t i = aLength(vec);
  while(i--) if(vec[i]==item) return i;
  return (size_t)-1; })
AARRAY_define(size_t AARRAY_ZIndexOf__uint16_t(uint16_t vec[], uint16_t item), {
  size_t i = aLength(vec);
  while(i--) if(vec[i]==item) return i;
  return (size_t)-1; })
AARRAY_define(size_t AARRAY_ZIndexOf__uint32_t(uint32_t vec[], uint32_t item), {
  size_t i = aLength(vec);
  while(i--) if(vec[i]==item) return i;
  return (size_t)-1; })
AARRAY_define(size_t AARRAY_ZIndexOf__uint64_t(uint64_t vec[], uint64_t item), {
  size_t i = aLength(vec);
  while(i--) if(vec[i]==item) return i;
  return (size_t)-1; })
static void(*const AARRAY_ZIndexOf__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_ZIndexOf__uint8_t,
  (void(*)(void))&AARRAY_ZIndexOf__uint16_t, 0,
  (void(*)(void))&AARRAY_ZIndexOf__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_ZIndexOf__uint64_t };
#define aZIndexOf(vec, item) \
  ((size_t(*)(void*, uint64_t)) \
   AARRAY_ZIndexOf__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(vec, \
  AARRAY_nowarn_internal_start item AARRAY_nowarn_internal_end)
  



AARRAY_define(void AARRAY_Map_FUNC_3_uint8_t(
    char errLoc[], uint8_t vec[], void(*f)(uint8_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_FUNC_3_uint16_t(
    char errLoc[], uint16_t vec[], void(*f)(uint16_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_FUNC_3_uint32_t(
    char errLoc[], uint32_t vec[], void(*f)(uint32_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_FUNC_3_uint64_t(
    char errLoc[], uint64_t vec[], void(*f)(uint64_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

static void(*const AARRAY_Map_FUNC_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Map_FUNC_3_uint8_t,
  (void(*)(void))&AARRAY_Map_FUNC_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Map_FUNC_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Map_FUNC_3_uint64_t };
AARRAY_define(void AARRAY_Map_FUNC_2_uint8_t(
    char errLoc[], uint8_t vec[], void(*f)(uint8_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_FUNC_2_uint16_t(
    char errLoc[], uint16_t vec[], void(*f)(uint16_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_FUNC_2_uint32_t(
    char errLoc[], uint32_t vec[], void(*f)(uint32_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_FUNC_2_uint64_t(
    char errLoc[], uint64_t vec[], void(*f)(uint64_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

static void(*const AARRAY_Map_FUNC_2_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Map_FUNC_2_uint8_t,
  (void(*)(void))&AARRAY_Map_FUNC_2_uint16_t, 0,
  (void(*)(void))&AARRAY_Map_FUNC_2_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Map_FUNC_2_uint64_t };
#define AARRAY_aMap_FUNC_3(vec, f, data) \
  ((void(*)(char[], void*, void(*)(void), void*)) \
  AARRAY_Map_FUNC_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, (void(*)(void))f, data)
#define AARRAY_aMap_FUNC_2(vec, f) \
  ((void(*)(char[], void*, void(*)(void))) \
  AARRAY_Map_FUNC_2_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, (void(*)(void))f)
AARRAY_define(int AARRAY_Loop_FUNC_4_uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, int(*f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_FUNC_4_uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, int(*f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_FUNC_4_uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, int(*f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_FUNC_4_uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, int(*f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
static void(*const AARRAY_Loop_FUNC_4_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Loop_FUNC_4_uint8_t,
  (void(*)(void))&AARRAY_Loop_FUNC_4_uint16_t, 0,
  (void(*)(void))&AARRAY_Loop_FUNC_4_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Loop_FUNC_4_uint64_t };
AARRAY_define(int AARRAY_Loop_FUNC_3_uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, int(*f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_FUNC_3_uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, int(*f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_FUNC_3_uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, int(*f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_FUNC_3_uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, int(*f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
static void(*const AARRAY_Loop_FUNC_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Loop_FUNC_3_uint8_t,
  (void(*)(void))&AARRAY_Loop_FUNC_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Loop_FUNC_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Loop_FUNC_3_uint64_t };
#define AARRAY_aLoop_FUNC_4(vec, pos, f, data) \
  ((int(*)(char[], void*, size_t, int(*)(size_t, void*), void*)) \
  AARRAY_Loop_FUNC_4_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, pos, f, data)
#define AARRAY_aLoop_FUNC_3(vec, pos, f) \
  ((int(*)(char[], void*, size_t, int(*)(size_t))) \
  AARRAY_Loop_FUNC_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, pos, f)
AARRAY_define(uint8_t* AARRAY_Filter_FUNC_3_uint8_t(
    char errLoc[], uint8_t vec[], int(*f)(uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint16_t* AARRAY_Filter_FUNC_3_uint16_t(
    char errLoc[], uint16_t vec[], int(*f)(uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint32_t* AARRAY_Filter_FUNC_3_uint32_t(
    char errLoc[], uint32_t vec[], int(*f)(uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint64_t* AARRAY_Filter_FUNC_3_uint64_t(
    char errLoc[], uint64_t vec[], int(*f)(uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
static void(*const AARRAY_Filter_FUNC_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Filter_FUNC_3_uint8_t,
  (void(*)(void))&AARRAY_Filter_FUNC_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Filter_FUNC_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Filter_FUNC_3_uint64_t };
AARRAY_define(uint8_t* AARRAY_Filter_FUNC_2_uint8_t(
    char errLoc[], uint8_t vec[], int(*f)(uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint16_t* AARRAY_Filter_FUNC_2_uint16_t(
    char errLoc[], uint16_t vec[], int(*f)(uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint32_t* AARRAY_Filter_FUNC_2_uint32_t(
    char errLoc[], uint32_t vec[], int(*f)(uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint64_t* AARRAY_Filter_FUNC_2_uint64_t(
    char errLoc[], uint64_t vec[], int(*f)(uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
static void(*const AARRAY_Filter_FUNC_2_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Filter_FUNC_2_uint8_t,
  (void(*)(void))&AARRAY_Filter_FUNC_2_uint16_t, 0,
  (void(*)(void))&AARRAY_Filter_FUNC_2_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Filter_FUNC_2_uint64_t };
#define AARRAY_aFilter_FUNC_3(vec, f, data) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, void(*)(void), void*)) \
   AARRAY_Filter_FUNC_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec,(void(*)(void))f, data))
#define AARRAY_aFilter_FUNC_2(vec, f) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, void(*)(void))) \
   AARRAY_Filter_FUNC_2_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec,(void(*)(void))f))

#if __has_extension(blocks)
AARRAY_define(void AARRAY_Map_BLOCK_3_uint8_t(
    char errLoc[], uint8_t vec[], void(^f)(uint8_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_BLOCK_3_uint16_t(
    char errLoc[], uint16_t vec[], void(^f)(uint16_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_BLOCK_3_uint32_t(
    char errLoc[], uint32_t vec[], void(^f)(uint32_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_BLOCK_3_uint64_t(
    char errLoc[], uint64_t vec[], void(^f)(uint64_t*, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

static void(*const AARRAY_Map_BLOCK_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Map_BLOCK_3_uint8_t,
  (void(*)(void))&AARRAY_Map_BLOCK_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Map_BLOCK_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Map_BLOCK_3_uint64_t };
AARRAY_define(void AARRAY_Map_BLOCK_2_uint8_t(
    char errLoc[], uint8_t vec[], void(^f)(uint8_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_BLOCK_2_uint16_t(
    char errLoc[], uint16_t vec[], void(^f)(uint16_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_BLOCK_2_uint32_t(
    char errLoc[], uint32_t vec[], void(^f)(uint32_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_BLOCK_2_uint64_t(
    char errLoc[], uint64_t vec[], void(^f)(uint64_t*)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

static void(*const AARRAY_Map_BLOCK_2_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Map_BLOCK_2_uint8_t,
  (void(*)(void))&AARRAY_Map_BLOCK_2_uint16_t, 0,
  (void(*)(void))&AARRAY_Map_BLOCK_2_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Map_BLOCK_2_uint64_t };
#define AARRAY_aMap_BLOCK_3(vec, f, data) \
  ((void(*)(char[], void*, void(^)(void*, void*), void*)) \
   AARRAY_Map_BLOCK_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, vec, (void(^)(void*, void*))f, data)
#define AARRAY_aMap_BLOCK_2(vec, f) \
  ((void(*)(char[], void*, void(^)(void*))) \
   AARRAY_Map_BLOCK_2_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
  (AARRAY_LINE, vec, (void(^)(void*))f)
AARRAY_define(int AARRAY_Loop_BLOCK_4_uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, int(^f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_BLOCK_4_uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, int(^f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_BLOCK_4_uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, int(^f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_BLOCK_4_uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, int(^f)(size_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
static void(*const AARRAY_Loop_BLOCK_4_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Loop_BLOCK_4_uint8_t,
  (void(*)(void))&AARRAY_Loop_BLOCK_4_uint16_t, 0,
  (void(*)(void))&AARRAY_Loop_BLOCK_4_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Loop_BLOCK_4_uint64_t };
AARRAY_define(int AARRAY_Loop_BLOCK_3_uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, int (^f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_BLOCK_3_uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, int (^f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_BLOCK_3_uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, int (^f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_BLOCK_3_uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, int (^f)(size_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
static void(*const AARRAY_Loop_BLOCK_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Loop_BLOCK_3_uint8_t,
  (void(*)(void))&AARRAY_Loop_BLOCK_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Loop_BLOCK_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Loop_BLOCK_3_uint64_t };
#define AARRAY_aLoop_BLOCK_4(vec, pos, f, data) \
  ((int(*)(char[], void*, size_t, int(^)(size_t, void*), void*)) \
   AARRAY_Loop_BLOCK_4_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, pos, f, data)
#define AARRAY_aLoop_BLOCK_3(vec, pos, f) \
  ((int(*)(char[], void*, size_t, int(^)(size_t))) \
   AARRAY_Loop_BLOCK_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, pos, f)
AARRAY_define(uint8_t* AARRAY_Filter_BLOCK_3_uint8_t(
    char errLoc[], uint8_t vec[], int(^f)(uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint16_t* AARRAY_Filter_BLOCK_3_uint16_t(
    char errLoc[], uint16_t vec[], int(^f)(uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint32_t* AARRAY_Filter_BLOCK_3_uint32_t(
    char errLoc[], uint32_t vec[], int(^f)(uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint64_t* AARRAY_Filter_BLOCK_3_uint64_t(
    char errLoc[], uint64_t vec[], int(^f)(uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
static void(*const AARRAY_Filter_BLOCK_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Filter_BLOCK_3_uint8_t,
  (void(*)(void))&AARRAY_Filter_BLOCK_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Filter_BLOCK_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Filter_BLOCK_3_uint64_t };
AARRAY_define(uint8_t* AARRAY_Filter_BLOCK_2_uint8_t(
    char errLoc[], uint8_t vec[], int(^f)(uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint16_t* AARRAY_Filter_BLOCK_2_uint16_t(
    char errLoc[], uint16_t vec[], int(^f)(uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint32_t* AARRAY_Filter_BLOCK_2_uint32_t(
    char errLoc[], uint32_t vec[], int(^f)(uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint64_t* AARRAY_Filter_BLOCK_2_uint64_t(
    char errLoc[], uint64_t vec[], int(^f)(uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
static void(*const AARRAY_Filter_BLOCK_2_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Filter_BLOCK_2_uint8_t,
  (void(*)(void))&AARRAY_Filter_BLOCK_2_uint16_t, 0,
  (void(*)(void))&AARRAY_Filter_BLOCK_2_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Filter_BLOCK_2_uint64_t };
#define AARRAY_aFilter_BLOCK_3(vec, f, data) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, int(^)(uint64_t, void*), void*)) \
   AARRAY_Filter_BLOCK_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec,(int(^)(uint64_t, void*))f, data))
#define AARRAY_aFilter_BLOCK_2(vec, f) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, int(^)(uint64_t))) \
   AARRAY_Filter_BLOCK_2_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec,(int(^)(uint64_t))f))
#endif

#if defined(__cplusplus)
AARRAY_define(void AARRAY_Map_LAMBDA_3_uint8_t(
    char errLoc[], uint8_t vec[], std::function<void(uint8_t*, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_LAMBDA_3_uint16_t(
    char errLoc[], uint16_t vec[], std::function<void(uint16_t*, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_LAMBDA_3_uint32_t(
    char errLoc[], uint32_t vec[], std::function<void(uint32_t*, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

AARRAY_define(void AARRAY_Map_LAMBDA_3_uint64_t(
    char errLoc[], uint64_t vec[], std::function<void(uint64_t*, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) , data
); })

static void(*const AARRAY_Map_LAMBDA_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Map_LAMBDA_3_uint8_t,
  (void(*)(void))&AARRAY_Map_LAMBDA_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Map_LAMBDA_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Map_LAMBDA_3_uint64_t };
AARRAY_define(void AARRAY_Map_LAMBDA_2_uint8_t(
    char errLoc[], uint8_t vec[], std::function<void(uint8_t*)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_LAMBDA_2_uint16_t(
    char errLoc[], uint16_t vec[], std::function<void(uint16_t*)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_LAMBDA_2_uint32_t(
    char errLoc[], uint32_t vec[], std::function<void(uint32_t*)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

AARRAY_define(void AARRAY_Map_LAMBDA_2_uint64_t(
    char errLoc[], uint64_t vec[], std::function<void(uint64_t*)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  /* only FUNC versions use extra data parameter */
  while(++n < aLength(vec)) f(&(vec[n]) ); })

static void(*const AARRAY_Map_LAMBDA_2_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Map_LAMBDA_2_uint8_t,
  (void(*)(void))&AARRAY_Map_LAMBDA_2_uint16_t, 0,
  (void(*)(void))&AARRAY_Map_LAMBDA_2_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Map_LAMBDA_2_uint64_t };
#define AARRAY_aMap_LAMBDA_3(vec, f, data) \
  ((void(*)(char[], void*, std::function<void(void*, void*)>, void*)) \
   AARRAY_Map_LAMBDA_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, f, data)
#define AARRAY_aMap_LAMBDA_2(vec, f) \
  ((void(*)(char[], void*, std::function<void(void*)>)) \
   AARRAY_Map_LAMBDA_2_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, f)
AARRAY_define(int AARRAY_Loop_LAMBDA_4_uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, std::function<int(size_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_LAMBDA_4_uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, std::function<int(size_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_LAMBDA_4_uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, std::function<int(size_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_LAMBDA_4_uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, std::function<int(size_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos , data
);

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
static void(*const AARRAY_Loop_LAMBDA_4_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Loop_LAMBDA_4_uint8_t,
  (void(*)(void))&AARRAY_Loop_LAMBDA_4_uint16_t, 0,
  (void(*)(void))&AARRAY_Loop_LAMBDA_4_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Loop_LAMBDA_4_uint64_t };
AARRAY_define(int AARRAY_Loop_LAMBDA_3_uint8_t(
    char errLoc[], uint8_t vec[], size_t pos, std::function<int(size_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_LAMBDA_3_uint16_t(
    char errLoc[], uint16_t vec[], size_t pos, std::function<int(size_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_LAMBDA_3_uint32_t(
    char errLoc[], uint32_t vec[], size_t pos, std::function<int(size_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
AARRAY_define(int AARRAY_Loop_LAMBDA_3_uint64_t(
    char errLoc[], uint64_t vec[], size_t pos, std::function<int(size_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  int offset = 0;
  while(pos < aLength(vec)) {
    if((offset > 0 && pos < (size_t)offset) ||
       (offset < 0 && pos > (size_t)offset)) return offset;
    offset = f(pos );

    if(!offset) return 0;
    /* play safe with int promotion */
    if(offset > 0) pos += (size_t) offset;
    else           pos -= (size_t)-offset; }
  return offset; })
static void(*const AARRAY_Loop_LAMBDA_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Loop_LAMBDA_3_uint8_t,
  (void(*)(void))&AARRAY_Loop_LAMBDA_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Loop_LAMBDA_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Loop_LAMBDA_3_uint64_t };
#define AARRAY_aLoop_LAMBDA_4(vec, pos, f, data) \
  ((int(*)(char[], void*, size_t, std::function<int(size_t, void*)>, void*)) \
   AARRAY_Loop_LAMBDA_4_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, pos, f, data)
#define AARRAY_aLoop_LAMBDA_3(vec, pos, f) \
  ((int(*)(char[], void*, size_t, std::function<int(size_t)>)) \
   AARRAY_Loop_LAMBDA_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, pos, f)
AARRAY_define(uint8_t* AARRAY_Filter_LAMBDA_3_uint8_t(
    char errLoc[], uint8_t vec[], std::function<int(uint8_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint16_t* AARRAY_Filter_LAMBDA_3_uint16_t(
    char errLoc[], uint16_t vec[], std::function<int(uint16_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint32_t* AARRAY_Filter_LAMBDA_3_uint32_t(
    char errLoc[], uint32_t vec[], std::function<int(uint32_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint64_t* AARRAY_Filter_LAMBDA_3_uint64_t(
    char errLoc[], uint64_t vec[], std::function<int(uint64_t, void*)> f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] , data
))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
static void(*const AARRAY_Filter_LAMBDA_3_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Filter_LAMBDA_3_uint8_t,
  (void(*)(void))&AARRAY_Filter_LAMBDA_3_uint16_t, 0,
  (void(*)(void))&AARRAY_Filter_LAMBDA_3_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Filter_LAMBDA_3_uint64_t };
AARRAY_define(uint8_t* AARRAY_Filter_LAMBDA_2_uint8_t(
    char errLoc[], uint8_t vec[], std::function<int(uint8_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint16_t* AARRAY_Filter_LAMBDA_2_uint16_t(
    char errLoc[], uint16_t vec[], std::function<int(uint16_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint32_t* AARRAY_Filter_LAMBDA_2_uint32_t(
    char errLoc[], uint32_t vec[], std::function<int(uint32_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
AARRAY_define(uint64_t* AARRAY_Filter_LAMBDA_2_uint64_t(
    char errLoc[], uint64_t vec[], std::function<int(uint64_t)> f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1, nn = (size_t)-1;
  while(++n < aLength(vec)) {
    if(f(vec[n] ))

      vec[++nn] = vec[n]; }
  (void)aLength2(vec, nn+1);
  return vec; })
static void(*const AARRAY_Filter_LAMBDA_2_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Filter_LAMBDA_2_uint8_t,
  (void(*)(void))&AARRAY_Filter_LAMBDA_2_uint16_t, 0,
  (void(*)(void))&AARRAY_Filter_LAMBDA_2_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Filter_LAMBDA_2_uint64_t };
#define AARRAY_aFilter_LAMBDA_3(vec, f, data) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, std::function<int(uint64_t, void*)>, void*))\
   AARRAY_Filter_LAMBDA_3_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, f, data))
#define AARRAY_aFilter_LAMBDA_2(vec, f) \
  (AARRAY_typeof(vec, (uint64_t*(*)(char[], void*, std::function<int(uint64_t)>))\
   AARRAY_Filter_LAMBDA_2_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(AARRAY_LINE, vec, f))
#endif

/* aFold is generic over array_type, function_type, AND base_type
 * Rather than make previous cppp macros more generic, we treat aFold as unique
 * Though it's possible to create macros that genericize any c function
 * Which would be useful...
 */
AARRAY_define(uint8_t AARRAY_Fold_FUNC_4_uint8_t_uint8_t(
    char errLoc[], uint8_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_4_uint16_t_uint8_t(
    char errLoc[], uint8_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_4_uint32_t_uint8_t(
    char errLoc[], uint8_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_4_uint64_t_uint8_t(
    char errLoc[], uint8_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_FUNC_4_uint8_t_uint16_t(
    char errLoc[], uint16_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_4_uint16_t_uint16_t(
    char errLoc[], uint16_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_4_uint32_t_uint16_t(
    char errLoc[], uint16_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_4_uint64_t_uint16_t(
    char errLoc[], uint16_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_FUNC_4_uint8_t_uint32_t(
    char errLoc[], uint32_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_4_uint16_t_uint32_t(
    char errLoc[], uint32_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_4_uint32_t_uint32_t(
    char errLoc[], uint32_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_4_uint64_t_uint32_t(
    char errLoc[], uint32_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_FUNC_4_uint8_t_uint64_t(
    char errLoc[], uint64_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_4_uint16_t_uint64_t(
    char errLoc[], uint64_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_4_uint32_t_uint64_t(
    char errLoc[], uint64_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_4_uint64_t_uint64_t(
    char errLoc[], uint64_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
static void(*const AARRAY_Fold_FUNC_4_FUNCTIONS[9][9])(void) = { {
	NULL }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint8_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint8_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint8_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint8_t_uint64_t }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint16_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint16_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint16_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint16_t_uint64_t }, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint32_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint32_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint32_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint32_t_uint64_t }, {0}, {0}, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint64_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint64_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint64_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_4_uint64_t_uint64_t } };
AARRAY_define(uint8_t AARRAY_Fold_FUNC_3_uint8_t_uint8_t(
    char errLoc[], uint8_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_3_uint16_t_uint8_t(
    char errLoc[], uint8_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_3_uint32_t_uint8_t(
    char errLoc[], uint8_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_3_uint64_t_uint8_t(
    char errLoc[], uint8_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_FUNC_3_uint8_t_uint16_t(
    char errLoc[], uint16_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_3_uint16_t_uint16_t(
    char errLoc[], uint16_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_3_uint32_t_uint16_t(
    char errLoc[], uint16_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_3_uint64_t_uint16_t(
    char errLoc[], uint16_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_FUNC_3_uint8_t_uint32_t(
    char errLoc[], uint32_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_3_uint16_t_uint32_t(
    char errLoc[], uint32_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_3_uint32_t_uint32_t(
    char errLoc[], uint32_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_3_uint64_t_uint32_t(
    char errLoc[], uint32_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_FUNC_3_uint8_t_uint64_t(
    char errLoc[], uint64_t*vec, uint8_t base,
    uint8_t(*f)(uint8_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_FUNC_3_uint16_t_uint64_t(
    char errLoc[], uint64_t*vec, uint16_t base,
    uint16_t(*f)(uint16_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_FUNC_3_uint32_t_uint64_t(
    char errLoc[], uint64_t*vec, uint32_t base,
    uint32_t(*f)(uint32_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_FUNC_3_uint64_t_uint64_t(
    char errLoc[], uint64_t*vec, uint64_t base,
    uint64_t(*f)(uint64_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
static void(*const AARRAY_Fold_FUNC_3_FUNCTIONS[9][9])(void) = { {
	NULL }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint8_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint8_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint8_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint8_t_uint64_t }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint16_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint16_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint16_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint16_t_uint64_t }, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint32_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint32_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint32_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint32_t_uint64_t }, {0}, {0}, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint64_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint64_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint64_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_FUNC_3_uint64_t_uint64_t } };
#define AARRAY_aFold_FUNC_4(vec, base, f, data) \
  (AARRAY_typeof(base, (uint64_t(*)(char[], void*, uint64_t, void(*)(void), void*)) \
   AARRAY_Fold_FUNC_4_FUNCTIONS[sizeof(base) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(base)][sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec, (uint64_t)base, (void(*)(void))f, data))
#define AARRAY_aFold_FUNC_3(vec, base, f) \
  (AARRAY_typeof(base, (uint64_t(*)(char[], void*, uint64_t, void(*)(void))) \
   AARRAY_Fold_FUNC_3_FUNCTIONS[sizeof(base) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(base)][sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec, (uint64_t)base, (void(*)(void))f))

#if __has_extension(blocks)
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_4_uint8_t_uint8_t(
    char errLoc[], uint8_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_4_uint16_t_uint8_t(
    char errLoc[], uint8_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_4_uint32_t_uint8_t(
    char errLoc[], uint8_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_4_uint64_t_uint8_t(
    char errLoc[], uint8_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint8_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_4_uint8_t_uint16_t(
    char errLoc[], uint16_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_4_uint16_t_uint16_t(
    char errLoc[], uint16_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_4_uint32_t_uint16_t(
    char errLoc[], uint16_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_4_uint64_t_uint16_t(
    char errLoc[], uint16_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint16_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_4_uint8_t_uint32_t(
    char errLoc[], uint32_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_4_uint16_t_uint32_t(
    char errLoc[], uint32_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_4_uint32_t_uint32_t(
    char errLoc[], uint32_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_4_uint64_t_uint32_t(
    char errLoc[], uint32_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint32_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_4_uint8_t_uint64_t(
    char errLoc[], uint64_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_4_uint16_t_uint64_t(
    char errLoc[], uint64_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_4_uint32_t_uint64_t(
    char errLoc[], uint64_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_4_uint64_t_uint64_t(
    char errLoc[], uint64_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint64_t, void*), void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
static void(*const AARRAY_Fold_BLOCK_4_FUNCTIONS[9][9])(void) = { {
	NULL }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint8_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint8_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint8_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint8_t_uint64_t }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint16_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint16_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint16_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint16_t_uint64_t }, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint32_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint32_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint32_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint32_t_uint64_t }, {0}, {0}, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint64_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint64_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint64_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_4_uint64_t_uint64_t } };
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_3_uint8_t_uint8_t(
    char errLoc[], uint8_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_3_uint16_t_uint8_t(
    char errLoc[], uint8_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_3_uint32_t_uint8_t(
    char errLoc[], uint8_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_3_uint64_t_uint8_t(
    char errLoc[], uint8_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint8_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_3_uint8_t_uint16_t(
    char errLoc[], uint16_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_3_uint16_t_uint16_t(
    char errLoc[], uint16_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_3_uint32_t_uint16_t(
    char errLoc[], uint16_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_3_uint64_t_uint16_t(
    char errLoc[], uint16_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint16_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_3_uint8_t_uint32_t(
    char errLoc[], uint32_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_3_uint16_t_uint32_t(
    char errLoc[], uint32_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_3_uint32_t_uint32_t(
    char errLoc[], uint32_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_3_uint64_t_uint32_t(
    char errLoc[], uint32_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint32_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_BLOCK_3_uint8_t_uint64_t(
    char errLoc[], uint64_t*vec, uint8_t base,
    uint8_t(^f)(uint8_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_BLOCK_3_uint16_t_uint64_t(
    char errLoc[], uint64_t*vec, uint16_t base,
    uint16_t(^f)(uint16_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_BLOCK_3_uint32_t_uint64_t(
    char errLoc[], uint64_t*vec, uint32_t base,
    uint32_t(^f)(uint32_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_BLOCK_3_uint64_t_uint64_t(
    char errLoc[], uint64_t*vec, uint64_t base,
    uint64_t(^f)(uint64_t,uint64_t)), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
static void(*const AARRAY_Fold_BLOCK_3_FUNCTIONS[9][9])(void) = { {
	NULL }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint8_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint8_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint8_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint8_t_uint64_t }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint16_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint16_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint16_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint16_t_uint64_t }, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint32_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint32_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint32_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint32_t_uint64_t }, {0}, {0}, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint64_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint64_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint64_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_BLOCK_3_uint64_t_uint64_t } };
#define AARRAY_aFold_BLOCK_4(vec, base, f, data) \
  (AARRAY_typeof(base, (uint64_t(*)(char[], void*, uint64_t, \
                        uint64_t(^)(uint64_t,uint64_t,void*), void*)) \
   AARRAY_Fold_BLOCK_4_FUNCTIONS[sizeof(base) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(base)][sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec, (uint64_t)base, (uint64_t(^)(uint64_t,uint64_t,void*))f, data))
#define AARRAY_aFold_BLOCK_3(vec, base, f) \
  (AARRAY_typeof(base, (uint64_t(*)(char[], void*, uint64_t, \
                        uint64_t(^)(uint64_t,uint64_t))) \
   AARRAY_Fold_BLOCK_3_FUNCTIONS[sizeof(base) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(base)][sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec, (uint64_t)base, (uint64_t(^)(uint64_t,uint64_t))f))
#endif
#if defined(__cplusplus)
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_4_uint8_t_uint8_t(
    char errLoc[], uint8_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint8_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_4_uint16_t_uint8_t(
    char errLoc[], uint8_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint8_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_4_uint32_t_uint8_t(
    char errLoc[], uint8_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint8_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_4_uint64_t_uint8_t(
    char errLoc[], uint8_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint8_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_4_uint8_t_uint16_t(
    char errLoc[], uint16_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint16_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_4_uint16_t_uint16_t(
    char errLoc[], uint16_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint16_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_4_uint32_t_uint16_t(
    char errLoc[], uint16_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint16_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_4_uint64_t_uint16_t(
    char errLoc[], uint16_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint16_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_4_uint8_t_uint32_t(
    char errLoc[], uint32_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint32_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_4_uint16_t_uint32_t(
    char errLoc[], uint32_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint32_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_4_uint32_t_uint32_t(
    char errLoc[], uint32_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint32_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_4_uint64_t_uint32_t(
    char errLoc[], uint32_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint32_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_4_uint8_t_uint64_t(
    char errLoc[], uint64_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint64_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_4_uint16_t_uint64_t(
    char errLoc[], uint64_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint64_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_4_uint32_t_uint64_t(
    char errLoc[], uint64_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint64_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_4_uint64_t_uint64_t(
    char errLoc[], uint64_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint64_t, void*)>f, void*data), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] , data
);

  return base; })
static void(*const AARRAY_Fold_LAMBDA_4_FUNCTIONS[9][9])(void) = { {
	NULL }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint8_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint8_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint8_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint8_t_uint64_t }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint16_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint16_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint16_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint16_t_uint64_t }, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint32_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint32_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint32_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint32_t_uint64_t }, {0}, {0}, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint64_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint64_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint64_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_4_uint64_t_uint64_t } };
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_3_uint8_t_uint8_t(
    char errLoc[], uint8_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint8_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_3_uint16_t_uint8_t(
    char errLoc[], uint8_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint8_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_3_uint32_t_uint8_t(
    char errLoc[], uint8_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint8_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_3_uint64_t_uint8_t(
    char errLoc[], uint8_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint8_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_3_uint8_t_uint16_t(
    char errLoc[], uint16_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint16_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_3_uint16_t_uint16_t(
    char errLoc[], uint16_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint16_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_3_uint32_t_uint16_t(
    char errLoc[], uint16_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint16_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_3_uint64_t_uint16_t(
    char errLoc[], uint16_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint16_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_3_uint8_t_uint32_t(
    char errLoc[], uint32_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint32_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_3_uint16_t_uint32_t(
    char errLoc[], uint32_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint32_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_3_uint32_t_uint32_t(
    char errLoc[], uint32_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint32_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_3_uint64_t_uint32_t(
    char errLoc[], uint32_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint32_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint8_t AARRAY_Fold_LAMBDA_3_uint8_t_uint64_t(
    char errLoc[], uint64_t*vec, uint8_t base,
    std::function<uint8_t(uint8_t,uint64_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint16_t AARRAY_Fold_LAMBDA_3_uint16_t_uint64_t(
    char errLoc[], uint64_t*vec, uint16_t base,
    std::function<uint16_t(uint16_t,uint64_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint32_t AARRAY_Fold_LAMBDA_3_uint32_t_uint64_t(
    char errLoc[], uint64_t*vec, uint32_t base,
    std::function<uint32_t(uint32_t,uint64_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
AARRAY_define(uint64_t AARRAY_Fold_LAMBDA_3_uint64_t_uint64_t(
    char errLoc[], uint64_t*vec, uint64_t base,
    std::function<uint64_t(uint64_t,uint64_t)>f), {
  AARRAY_safety((void)errLoc, if(!f) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1;
  while(++n < aLength(vec)) base = f(base,vec[n] );

  return base; })
static void(*const AARRAY_Fold_LAMBDA_3_FUNCTIONS[9][9])(void) = { {
	NULL }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint8_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint8_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint8_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint8_t_uint64_t }, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint16_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint16_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint16_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint16_t_uint64_t }, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint32_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint32_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint32_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint32_t_uint64_t }, {0}, {0}, {0}, {
	NULL,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint64_t_uint8_t,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint64_t_uint16_t, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint64_t_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Fold_LAMBDA_3_uint64_t_uint64_t } };
#define AARRAY_aFold_LAMBDA_4(vec, base, f, data) \
  (AARRAY_typeof(base, (uint64_t(*)(char[], void*, uint64_t, \
                        std::function<uint64_t(uint64_t,uint64_t,void*)>, void*)) \
   AARRAY_Fold_LAMBDA_4_FUNCTIONS[sizeof(base) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(base)][sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec, (uint64_t)base, f, data))
#define AARRAY_aFold_LAMBDA_3(vec, base, f) \
  (AARRAY_typeof(base, (uint64_t(*)(char[], void*, uint64_t, \
                        std::function<uint64_t(uint64_t,uint64_t)>)) \
   AARRAY_Fold_LAMBDA_3_FUNCTIONS[sizeof(base) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(base)][sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (AARRAY_LINE, vec, (uint64_t)base, f))
#endif

/* make void*data parameter optional  */
#define AARRAY_ARGS_(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N 
#define AARRAY_ARGS(...) AARRAY_ARGS_(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define AARRAY_CONCAT(a, b)        a##b
#define AARRAY_CONCAT_EXPAND(a, b) AARRAY_CONCAT(a, b)
#define aMap_FUNC(...)      AARRAY_CONCAT_EXPAND(AARRAY_aMap_FUNC_,      AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aMap_BLOCK(...)     AARRAY_CONCAT_EXPAND(AARRAY_aMap_BLOCK_,     AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aMap_LAMBDA(...)    AARRAY_CONCAT_EXPAND(AARRAY_aMap_LAMBDA_,    AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aLoop_FUNC(...)     AARRAY_CONCAT_EXPAND(AARRAY_aLoop_FUNC_,     AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aLoop_BLOCK(...)    AARRAY_CONCAT_EXPAND(AARRAY_aLoop_BLOCK_,    AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aLoop_LAMBDA(...)   AARRAY_CONCAT_EXPAND(AARRAY_aLoop_LAMBDA_,   AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aFilter_FUNC(...)   AARRAY_CONCAT_EXPAND(AARRAY_aFilter_FUNC_,   AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aFilter_BLOCK(...)  AARRAY_CONCAT_EXPAND(AARRAY_aFilter_BLOCK_,  AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aFilter_LAMBDA(...) AARRAY_CONCAT_EXPAND(AARRAY_aFilter_LAMBDA_, AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aFold_FUNC(...)     AARRAY_CONCAT_EXPAND(AARRAY_aFold_FUNC_,     AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aFold_BLOCK(...)    AARRAY_CONCAT_EXPAND(AARRAY_aFold_BLOCK_,    AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define aFold_LAMBDA(...)   AARRAY_CONCAT_EXPAND(AARRAY_aFold_LAMBDA_,   AARRAY_ARGS(__VA_ARGS__))(__VA_ARGS__)



/* search and sort
 * aSort needs a whole bunch of helper macros and functions
 * thanks goes to: https://github.com/BonzaiThePenguin/WikiSort
 */
typedef struct {
  size_t size, power_of_two;
  size_t numerator, decimal;
  size_t denominator, decimal_step, numerator_step; } AARRAY_sortIt;
AARRAY_define(void AARRAY_sortReverse__uint8_t(
    uint8_t vec[], size_t start, size_t end), {
  for(size_t n = (end-start)/2; n > 0; n--) {
    uint8_t temp = vec[start+n-1];
    vec[start+n-1] = vec[end-n];
    vec[end-n] = temp; } })
AARRAY_define(void AARRAY_sortReverse__uint16_t(
    uint16_t vec[], size_t start, size_t end), {
  for(size_t n = (end-start)/2; n > 0; n--) {
    uint16_t temp = vec[start+n-1];
    vec[start+n-1] = vec[end-n];
    vec[end-n] = temp; } })
AARRAY_define(void AARRAY_sortReverse__uint32_t(
    uint32_t vec[], size_t start, size_t end), {
  for(size_t n = (end-start)/2; n > 0; n--) {
    uint32_t temp = vec[start+n-1];
    vec[start+n-1] = vec[end-n];
    vec[end-n] = temp; } })
AARRAY_define(void AARRAY_sortReverse__uint64_t(
    uint64_t vec[], size_t start, size_t end), {
  for(size_t n = (end-start)/2; n > 0; n--) {
    uint64_t temp = vec[start+n-1];
    vec[start+n-1] = vec[end-n];
    vec[end-n] = temp; } })
static void(*const AARRAY_sortReverse__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortReverse__uint8_t,
  (void(*)(void))&AARRAY_sortReverse__uint16_t, 0,
  (void(*)(void))&AARRAY_sortReverse__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortReverse__uint64_t };
#define AARRAY_aSortReverse(vec, start, end) \
  (((int(*)(void*, size_t, size_t)) \
    AARRAY_sortReverse__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])((void*)vec, start, end))
  
AARRAY_define(void AARRAY_sortRotate__uint8_t(
    uint8_t array[], const size_t amount, size_t start, size_t end,
    uint8_t cache[], const size_t cacheSize), {
  if(end-start == 0) return;
  size_t sA = start, eA = start+amount, sB = start+amount, eB = end;
  if(eA-sA <= eB-sB) {
    if(eA-sA <= cacheSize) {
      memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
      memmove(&array[sA], &array[sB], (eB-sB)*sizeof(array[0]));
      memcpy(&array[sA+(eB-sB)], &cache[0], (eA-sA)*sizeof(array[0]));
      return; } }
  else {
    if(eB-sB <= cacheSize) {
      memcpy(&cache[0], &array[sB], (eB-sB)*sizeof(array[0]));
      memmove(&array[eB-(eA-sA)], &array[sA], (eA-sA)*sizeof(array[0]));
      memcpy(&array[sA], &cache[0], (eB-sB)*sizeof(array[0]));
      return; } }
  AARRAY_aSortReverse(array, sA, eA);
  AARRAY_aSortReverse(array, sB, eB);
  AARRAY_aSortReverse(array, start, end); })
AARRAY_define(void AARRAY_sortRotate__uint16_t(
    uint16_t array[], const size_t amount, size_t start, size_t end,
    uint16_t cache[], const size_t cacheSize), {
  if(end-start == 0) return;
  size_t sA = start, eA = start+amount, sB = start+amount, eB = end;
  if(eA-sA <= eB-sB) {
    if(eA-sA <= cacheSize) {
      memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
      memmove(&array[sA], &array[sB], (eB-sB)*sizeof(array[0]));
      memcpy(&array[sA+(eB-sB)], &cache[0], (eA-sA)*sizeof(array[0]));
      return; } }
  else {
    if(eB-sB <= cacheSize) {
      memcpy(&cache[0], &array[sB], (eB-sB)*sizeof(array[0]));
      memmove(&array[eB-(eA-sA)], &array[sA], (eA-sA)*sizeof(array[0]));
      memcpy(&array[sA], &cache[0], (eB-sB)*sizeof(array[0]));
      return; } }
  AARRAY_aSortReverse(array, sA, eA);
  AARRAY_aSortReverse(array, sB, eB);
  AARRAY_aSortReverse(array, start, end); })
AARRAY_define(void AARRAY_sortRotate__uint32_t(
    uint32_t array[], const size_t amount, size_t start, size_t end,
    uint32_t cache[], const size_t cacheSize), {
  if(end-start == 0) return;
  size_t sA = start, eA = start+amount, sB = start+amount, eB = end;
  if(eA-sA <= eB-sB) {
    if(eA-sA <= cacheSize) {
      memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
      memmove(&array[sA], &array[sB], (eB-sB)*sizeof(array[0]));
      memcpy(&array[sA+(eB-sB)], &cache[0], (eA-sA)*sizeof(array[0]));
      return; } }
  else {
    if(eB-sB <= cacheSize) {
      memcpy(&cache[0], &array[sB], (eB-sB)*sizeof(array[0]));
      memmove(&array[eB-(eA-sA)], &array[sA], (eA-sA)*sizeof(array[0]));
      memcpy(&array[sA], &cache[0], (eB-sB)*sizeof(array[0]));
      return; } }
  AARRAY_aSortReverse(array, sA, eA);
  AARRAY_aSortReverse(array, sB, eB);
  AARRAY_aSortReverse(array, start, end); })
AARRAY_define(void AARRAY_sortRotate__uint64_t(
    uint64_t array[], const size_t amount, size_t start, size_t end,
    uint64_t cache[], const size_t cacheSize), {
  if(end-start == 0) return;
  size_t sA = start, eA = start+amount, sB = start+amount, eB = end;
  if(eA-sA <= eB-sB) {
    if(eA-sA <= cacheSize) {
      memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
      memmove(&array[sA], &array[sB], (eB-sB)*sizeof(array[0]));
      memcpy(&array[sA+(eB-sB)], &cache[0], (eA-sA)*sizeof(array[0]));
      return; } }
  else {
    if(eB-sB <= cacheSize) {
      memcpy(&cache[0], &array[sB], (eB-sB)*sizeof(array[0]));
      memmove(&array[eB-(eA-sA)], &array[sA], (eA-sA)*sizeof(array[0]));
      memcpy(&array[sA], &cache[0], (eB-sB)*sizeof(array[0]));
      return; } }
  AARRAY_aSortReverse(array, sA, eA);
  AARRAY_aSortReverse(array, sB, eB);
  AARRAY_aSortReverse(array, start, end); })
static void(*const AARRAY_sortRotate__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortRotate__uint8_t,
  (void(*)(void))&AARRAY_sortRotate__uint16_t, 0,
  (void(*)(void))&AARRAY_sortRotate__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortRotate__uint64_t };
#define AARRAY_aSortRotate(vec, value, start, end, vecb, cacheSize) \
  (((void(*)(void*, size_t, size_t, size_t, void*, size_t)) \
    AARRAY_sortRotate__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, value, start, end, vecb, cacheSize))
AARRAY_define(size_t AARRAY_sortBinaryFirst_FUNC_uint8_t(
    const uint8_t array[], const uint8_t value,
    size_t start, size_t end, int(*f)(uint8_t,uint8_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_FUNC_uint16_t(
    const uint16_t array[], const uint16_t value,
    size_t start, size_t end, int(*f)(uint16_t,uint16_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_FUNC_uint32_t(
    const uint32_t array[], const uint32_t value,
    size_t start, size_t end, int(*f)(uint32_t,uint32_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_FUNC_uint64_t(
    const uint64_t array[], const uint64_t value,
    size_t start, size_t end, int(*f)(uint64_t,uint64_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
static void(*const AARRAY_sortBinaryFirst_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortBinaryFirst_FUNC_uint8_t,
  (void(*)(void))&AARRAY_sortBinaryFirst_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_sortBinaryFirst_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortBinaryFirst_FUNC_uint64_t };
AARRAY_define(size_t AARRAY_sortBinaryLast_FUNC_uint8_t(
    const uint8_t array[], const uint8_t value,
    size_t start, size_t end, int(*f)(uint8_t,uint8_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_FUNC_uint16_t(
    const uint16_t array[], const uint16_t value,
    size_t start, size_t end, int(*f)(uint16_t,uint16_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_FUNC_uint32_t(
    const uint32_t array[], const uint32_t value,
    size_t start, size_t end, int(*f)(uint32_t,uint32_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_FUNC_uint64_t(
    const uint64_t array[], const uint64_t value,
    size_t start, size_t end, int(*f)(uint64_t,uint64_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
static void(*const AARRAY_sortBinaryLast_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortBinaryLast_FUNC_uint8_t,
  (void(*)(void))&AARRAY_sortBinaryLast_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_sortBinaryLast_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortBinaryLast_FUNC_uint64_t };
#define AARRAY_aSortBinaryFirst_FUNC(vec, value, start, end, f) \
  (((size_t(*)(void*, uint64_t, size_t, size_t, void(*)(void))) \
    AARRAY_sortBinaryFirst_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, value, start, end, (void(*)(void))f))
#define AARRAY_aSortBinaryLast_FUNC(vec, value, start, end, f) \
  (((size_t(*)(void*, uint64_t, size_t, size_t, void(*)(void))) \
    AARRAY_sortBinaryLast_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, value, start, end, (void(*)(void))f))
  
#if __has_extension(blocks)
AARRAY_define(size_t AARRAY_sortBinaryFirst_BLOCK_uint8_t(
    const uint8_t array[], const uint8_t value,
    size_t start, size_t end, int(^f)(uint8_t,uint8_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_BLOCK_uint16_t(
    const uint16_t array[], const uint16_t value,
    size_t start, size_t end, int(^f)(uint16_t,uint16_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_BLOCK_uint32_t(
    const uint32_t array[], const uint32_t value,
    size_t start, size_t end, int(^f)(uint32_t,uint32_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_BLOCK_uint64_t(
    const uint64_t array[], const uint64_t value,
    size_t start, size_t end, int(^f)(uint64_t,uint64_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
static void(*const AARRAY_sortBinaryFirst_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortBinaryFirst_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_sortBinaryFirst_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_sortBinaryFirst_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortBinaryFirst_BLOCK_uint64_t };
AARRAY_define(size_t AARRAY_sortBinaryLast_BLOCK_uint8_t(
    const uint8_t array[], const uint8_t value,
    size_t start, size_t end, int(^f)(uint8_t,uint8_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_BLOCK_uint16_t(
    const uint16_t array[], const uint16_t value,
    size_t start, size_t end, int(^f)(uint16_t,uint16_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_BLOCK_uint32_t(
    const uint32_t array[], const uint32_t value,
    size_t start, size_t end, int(^f)(uint32_t,uint32_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_BLOCK_uint64_t(
    const uint64_t array[], const uint64_t value,
    size_t start, size_t end, int(^f)(uint64_t,uint64_t)), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
static void(*const AARRAY_sortBinaryLast_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortBinaryLast_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_sortBinaryLast_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_sortBinaryLast_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortBinaryLast_BLOCK_uint64_t };
#define AARRAY_aSortBinaryFirst_BLOCK(vec, value, start, end, f) \
  (((size_t(*)(void*, uint64_t, size_t, size_t, void*)) \
    AARRAY_sortBinaryFirst_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, value, start, end, (void*)f))
#define AARRAY_aSortBinaryLast_BLOCK(vec, value, start, end, f) \
  (((size_t(*)(void*, uint64_t, size_t, size_t, void*)) \
    AARRAY_sortBinaryLast_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, value, start, end, (void*)f))
#endif
#if defined(__cplusplus)
AARRAY_define(size_t AARRAY_sortBinaryFirst_LAMBDA_uint8_t(
    const uint8_t array[], const uint8_t value,
    size_t start, size_t end, std::function<int(uint8_t, uint8_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_LAMBDA_uint16_t(
    const uint16_t array[], const uint16_t value,
    size_t start, size_t end, std::function<int(uint16_t, uint16_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_LAMBDA_uint32_t(
    const uint32_t array[], const uint32_t value,
    size_t start, size_t end, std::function<int(uint32_t, uint32_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryFirst_LAMBDA_uint64_t(
    const uint64_t array[], const uint64_t value,
    size_t start, size_t end, std::function<int(uint64_t, uint64_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return start;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(f(array[mid], value))  start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && f(array[start], value)) start++;
  return start; })
static void(*const AARRAY_sortBinaryFirst_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortBinaryFirst_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_sortBinaryFirst_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_sortBinaryFirst_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortBinaryFirst_LAMBDA_uint64_t };
AARRAY_define(size_t AARRAY_sortBinaryLast_LAMBDA_uint8_t(
    const uint8_t array[], const uint8_t value,
    size_t start, size_t end, std::function<int(uint8_t, uint8_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_LAMBDA_uint16_t(
    const uint16_t array[], const uint16_t value,
    size_t start, size_t end, std::function<int(uint16_t, uint16_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_LAMBDA_uint32_t(
    const uint32_t array[], const uint32_t value,
    size_t start, size_t end, std::function<int(uint32_t, uint32_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
AARRAY_define(size_t AARRAY_sortBinaryLast_LAMBDA_uint64_t(
    const uint64_t array[], const uint64_t value,
    size_t start, size_t end, std::function<int(uint64_t, uint64_t)>f), {
  size_t rend = end;
  end -= 1;
  if(start >= rend) return end;
  while(start < end) {
    size_t mid = start+(end-start)/2;
    if(!f(value, array[mid])) start = mid+1;
    else                            end = mid; }
  if(start == rend-1 && !f(value, array[start])) start++;
  return start; })
static void(*const AARRAY_sortBinaryLast_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortBinaryLast_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_sortBinaryLast_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_sortBinaryLast_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortBinaryLast_LAMBDA_uint64_t };
#define AARRAY_aSortBinaryFirst_LAMBDA(vec, value, start, end, f) \
  (((size_t(*)(void*, uint64_t, size_t, size_t, std::function<int(int,int)>)) \
    AARRAY_sortBinaryFirst_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, value, start, end, (std::function<int(int,int)>)f))
#define AARRAY_aSortBinaryLast_LAMBDA(vec, value, start, end, f) \
  (((size_t(*)(void*, uint64_t, size_t, size_t, std::function<int(int,int)>)) \
    AARRAY_sortBinaryLast_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, value, start, end, (std::function<int(int,int)>)f))
#endif

AARRAY_define(void AARRAY_sortNextRange__uint8_t(
    AARRAY_sortIt*it, size_t*start, size_t*end), {
  *start = it->decimal;
  it->decimal += it->decimal_step;
  it->numerator += it->numerator_step;
  if(it->numerator >= it->denominator) {
    it->numerator -= it->denominator;
    it->decimal++; }
  *end = it->decimal; })
AARRAY_define(void AARRAY_sortNextRange__uint16_t(
    AARRAY_sortIt*it, size_t*start, size_t*end), {
  *start = it->decimal;
  it->decimal += it->decimal_step;
  it->numerator += it->numerator_step;
  if(it->numerator >= it->denominator) {
    it->numerator -= it->denominator;
    it->decimal++; }
  *end = it->decimal; })
AARRAY_define(void AARRAY_sortNextRange__uint32_t(
    AARRAY_sortIt*it, size_t*start, size_t*end), {
  *start = it->decimal;
  it->decimal += it->decimal_step;
  it->numerator += it->numerator_step;
  if(it->numerator >= it->denominator) {
    it->numerator -= it->denominator;
    it->decimal++; }
  *end = it->decimal; })
AARRAY_define(void AARRAY_sortNextRange__uint64_t(
    AARRAY_sortIt*it, size_t*start, size_t*end), {
  *start = it->decimal;
  it->decimal += it->decimal_step;
  it->numerator += it->numerator_step;
  if(it->numerator >= it->denominator) {
    it->numerator -= it->denominator;
    it->decimal++; }
  *end = it->decimal; })
static void(*const AARRAY_sortNextRange__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortNextRange__uint8_t,
  (void(*)(void))&AARRAY_sortNextRange__uint16_t, 0,
  (void(*)(void))&AARRAY_sortNextRange__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortNextRange__uint64_t };
AARRAY_define(int AARRAY_sortNextLevel__uint8_t(AARRAY_sortIt*it), {
  it->decimal_step += it->decimal_step;
  it->numerator_step += it->numerator_step;
  if(it->numerator_step >= it->denominator) {
    it->numerator_step -= it->denominator;
    it->decimal_step++; }
  return it->decimal_step < it->size; })
AARRAY_define(int AARRAY_sortNextLevel__uint16_t(AARRAY_sortIt*it), {
  it->decimal_step += it->decimal_step;
  it->numerator_step += it->numerator_step;
  if(it->numerator_step >= it->denominator) {
    it->numerator_step -= it->denominator;
    it->decimal_step++; }
  return it->decimal_step < it->size; })
AARRAY_define(int AARRAY_sortNextLevel__uint32_t(AARRAY_sortIt*it), {
  it->decimal_step += it->decimal_step;
  it->numerator_step += it->numerator_step;
  if(it->numerator_step >= it->denominator) {
    it->numerator_step -= it->denominator;
    it->decimal_step++; }
  return it->decimal_step < it->size; })
AARRAY_define(int AARRAY_sortNextLevel__uint64_t(AARRAY_sortIt*it), {
  it->decimal_step += it->decimal_step;
  it->numerator_step += it->numerator_step;
  if(it->numerator_step >= it->denominator) {
    it->numerator_step -= it->denominator;
    it->decimal_step++; }
  return it->decimal_step < it->size; })
static void(*const AARRAY_sortNextLevel__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortNextLevel__uint8_t,
  (void(*)(void))&AARRAY_sortNextLevel__uint16_t, 0,
  (void(*)(void))&AARRAY_sortNextLevel__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortNextLevel__uint64_t };
#define AARRAY_aSortNextRange(vec, iter, start, end) \
  (((void(*)(AARRAY_sortIt*, size_t*, size_t*)) \
    AARRAY_sortNextRange__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(iter, start, end))
#define AARRAY_aSortNextLevel(vec, iter) \
  (((int (*)(AARRAY_sortIt*)) \
    AARRAY_sortNextLevel__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])(iter))
AARRAY_define(void AARRAY_sortMergeInto_FUNC_uint8_t(
    uint8_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint8_t,uint8_t), uint8_t into[]), {
  uint8_t*A_index = &from[sA], *B_index = &from[sB];
  uint8_t*A_last = &from[eA], *B_last = &from[eB];
  uint8_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_FUNC_uint16_t(
    uint16_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint16_t,uint16_t), uint16_t into[]), {
  uint16_t*A_index = &from[sA], *B_index = &from[sB];
  uint16_t*A_last = &from[eA], *B_last = &from[eB];
  uint16_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_FUNC_uint32_t(
    uint32_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint32_t,uint32_t), uint32_t into[]), {
  uint32_t*A_index = &from[sA], *B_index = &from[sB];
  uint32_t*A_last = &from[eA], *B_last = &from[eB];
  uint32_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_FUNC_uint64_t(
    uint64_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint64_t,uint64_t), uint64_t into[]), {
  uint64_t*A_index = &from[sA], *B_index = &from[sB];
  uint64_t*A_last = &from[eA], *B_last = &from[eB];
  uint64_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
static void(*const AARRAY_sortMergeInto_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInto_FUNC_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInto_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInto_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInto_FUNC_uint64_t };
AARRAY_define(void AARRAY_sortMergeExternal_FUNC_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint8_t,uint8_t), uint8_t cache[]), {
  uint8_t*A_index = &cache[0];
  uint8_t*B_index = &array[sB];
  uint8_t*insert_index = &array[sA];
  uint8_t*A_last = &cache[eA-sA];
  uint8_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_FUNC_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint16_t,uint16_t), uint16_t cache[]), {
  uint16_t*A_index = &cache[0];
  uint16_t*B_index = &array[sB];
  uint16_t*insert_index = &array[sA];
  uint16_t*A_last = &cache[eA-sA];
  uint16_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_FUNC_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint32_t,uint32_t), uint32_t cache[]), {
  uint32_t*A_index = &cache[0];
  uint32_t*B_index = &array[sB];
  uint32_t*insert_index = &array[sA];
  uint32_t*A_last = &cache[eA-sA];
  uint32_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_FUNC_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint64_t,uint64_t), uint64_t cache[]), {
  uint64_t*A_index = &cache[0];
  uint64_t*B_index = &array[sB];
  uint64_t*insert_index = &array[sA];
  uint64_t*A_last = &cache[eA-sA];
  uint64_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
static void(*const AARRAY_sortMergeExternal_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeExternal_FUNC_uint8_t,
  (void(*)(void))&AARRAY_sortMergeExternal_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeExternal_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeExternal_FUNC_uint64_t };
AARRAY_define(void AARRAY_sortMergeInternal_FUNC_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint8_t,uint8_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint8_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint8_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint8_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_FUNC_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint16_t,uint16_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint16_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint16_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint16_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_FUNC_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint32_t,uint32_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint32_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint32_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint32_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_FUNC_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint64_t,uint64_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint64_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint64_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint64_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
static void(*const AARRAY_sortMergeInternal_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInternal_FUNC_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInternal_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInternal_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInternal_FUNC_uint64_t };
AARRAY_define(void AARRAY_sortMergeInPlace_FUNC_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint8_t,uint8_t), uint8_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_FUNC(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_FUNC(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_FUNC_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint16_t,uint16_t), uint16_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_FUNC(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_FUNC(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_FUNC_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint32_t,uint32_t), uint32_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_FUNC(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_FUNC(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_FUNC_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(*f)(uint64_t,uint64_t), uint64_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_FUNC(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_FUNC(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
static void(*const AARRAY_sortMergeInPlace_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInPlace_FUNC_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInPlace_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInPlace_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInPlace_FUNC_uint64_t };
#define AARRAY_aSortMergeInto_FUNC(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void(*)(void), void*)) \
    AARRAY_sortMergeInto_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void(*)(void))f, vecb))
#define AARRAY_aSortMergeExternal_FUNC(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void(*)(void), void*)) \
    AARRAY_sortMergeExternal_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void(*)(void))f, vecb))
#define AARRAY_aSortMergeInternal_FUNC(vec, s1, s2, s3, s4, f, s5) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void(*)(void), size_t)) \
    AARRAY_sortMergeInternal_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void(*)(void))f, s5))
#define AARRAY_aSortMergeInPlace_FUNC(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void(*)(void), void*)) \
    AARRAY_sortMergeInPlace_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void(*)(void))f, vecb))

#if __has_extension(blocks)
AARRAY_define(void AARRAY_sortMergeInto_BLOCK_uint8_t(
    uint8_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint8_t,uint8_t), uint8_t into[]), {
  uint8_t*A_index = &from[sA], *B_index = &from[sB];
  uint8_t*A_last = &from[eA], *B_last = &from[eB];
  uint8_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_BLOCK_uint16_t(
    uint16_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint16_t,uint16_t), uint16_t into[]), {
  uint16_t*A_index = &from[sA], *B_index = &from[sB];
  uint16_t*A_last = &from[eA], *B_last = &from[eB];
  uint16_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_BLOCK_uint32_t(
    uint32_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint32_t,uint32_t), uint32_t into[]), {
  uint32_t*A_index = &from[sA], *B_index = &from[sB];
  uint32_t*A_last = &from[eA], *B_last = &from[eB];
  uint32_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_BLOCK_uint64_t(
    uint64_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint64_t,uint64_t), uint64_t into[]), {
  uint64_t*A_index = &from[sA], *B_index = &from[sB];
  uint64_t*A_last = &from[eA], *B_last = &from[eB];
  uint64_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
static void(*const AARRAY_sortMergeInto_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInto_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInto_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInto_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInto_BLOCK_uint64_t };
AARRAY_define(void AARRAY_sortMergeExternal_BLOCK_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint8_t,uint8_t), uint8_t cache[]), {
  uint8_t*A_index = &cache[0];
  uint8_t*B_index = &array[sB];
  uint8_t*insert_index = &array[sA];
  uint8_t*A_last = &cache[eA-sA];
  uint8_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_BLOCK_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint16_t,uint16_t), uint16_t cache[]), {
  uint16_t*A_index = &cache[0];
  uint16_t*B_index = &array[sB];
  uint16_t*insert_index = &array[sA];
  uint16_t*A_last = &cache[eA-sA];
  uint16_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_BLOCK_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint32_t,uint32_t), uint32_t cache[]), {
  uint32_t*A_index = &cache[0];
  uint32_t*B_index = &array[sB];
  uint32_t*insert_index = &array[sA];
  uint32_t*A_last = &cache[eA-sA];
  uint32_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_BLOCK_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint64_t,uint64_t), uint64_t cache[]), {
  uint64_t*A_index = &cache[0];
  uint64_t*B_index = &array[sB];
  uint64_t*insert_index = &array[sA];
  uint64_t*A_last = &cache[eA-sA];
  uint64_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
static void(*const AARRAY_sortMergeExternal_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeExternal_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_sortMergeExternal_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeExternal_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeExternal_BLOCK_uint64_t };
AARRAY_define(void AARRAY_sortMergeInternal_BLOCK_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint8_t,uint8_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint8_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint8_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint8_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_BLOCK_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint16_t,uint16_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint16_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint16_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint16_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_BLOCK_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint32_t,uint32_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint32_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint32_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint32_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_BLOCK_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint64_t,uint64_t), size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint64_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint64_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint64_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
static void(*const AARRAY_sortMergeInternal_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInternal_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInternal_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInternal_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInternal_BLOCK_uint64_t };
AARRAY_define(void AARRAY_sortMergeInPlace_BLOCK_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint8_t,uint8_t), uint8_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_BLOCK(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_BLOCK(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_BLOCK_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint16_t,uint16_t), uint16_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_BLOCK(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_BLOCK(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_BLOCK_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint32_t,uint32_t), uint32_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_BLOCK(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_BLOCK(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_BLOCK_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    int(^f)(uint64_t,uint64_t), uint64_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_BLOCK(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_BLOCK(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
static void(*const AARRAY_sortMergeInPlace_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInPlace_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInPlace_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInPlace_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInPlace_BLOCK_uint64_t };
#define AARRAY_aSortMergeInto_BLOCK(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void*, void*)) \
    AARRAY_sortMergeInto_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void*)f, vecb))
#define AARRAY_aSortMergeExternal_BLOCK(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void*, void*)) \
    AARRAY_sortMergeExternal_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void*)f, vecb))
#define AARRAY_aSortMergeInternal_BLOCK(vec, s1, s2, s3, s4, f, s5) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void*, size_t)) \
    AARRAY_sortMergeInternal_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void*)f, s5))
#define AARRAY_aSortMergeInPlace_BLOCK(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, void*, void*)) \
    AARRAY_sortMergeInPlace_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (void*)f, vecb))
#endif
#if defined(__cplusplus)
AARRAY_define(void AARRAY_sortMergeInto_LAMBDA_uint8_t(
    uint8_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint8_t, uint8_t)>f, uint8_t into[]), {
  uint8_t*A_index = &from[sA], *B_index = &from[sB];
  uint8_t*A_last = &from[eA], *B_last = &from[eB];
  uint8_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_LAMBDA_uint16_t(
    uint16_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint16_t, uint16_t)>f, uint16_t into[]), {
  uint16_t*A_index = &from[sA], *B_index = &from[sB];
  uint16_t*A_last = &from[eA], *B_last = &from[eB];
  uint16_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_LAMBDA_uint32_t(
    uint32_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint32_t, uint32_t)>f, uint32_t into[]), {
  uint32_t*A_index = &from[sA], *B_index = &from[sB];
  uint32_t*A_last = &from[eA], *B_last = &from[eB];
  uint32_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
AARRAY_define(void AARRAY_sortMergeInto_LAMBDA_uint64_t(
    uint64_t from[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint64_t, uint64_t)>f, uint64_t into[]), {
  uint64_t*A_index = &from[sA], *B_index = &from[sB];
  uint64_t*A_last = &from[eA], *B_last = &from[eB];
  uint64_t*insert_index = &into[0];
  while(1) {
    if(!f(*B_index, *A_index)) {
      *insert_index = *A_index;
      A_index++;
      insert_index++;
      if(A_index == A_last) {
        memcpy(insert_index, B_index, (size_t)(B_last-B_index)*sizeof(from[0]));
        break; } }
    else {
      *insert_index = *B_index;
      B_index++;
      insert_index++;
      if(B_index == B_last) {
        memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(from[0]));
        break; } } } })
static void(*const AARRAY_sortMergeInto_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInto_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInto_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInto_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInto_LAMBDA_uint64_t };
AARRAY_define(void AARRAY_sortMergeExternal_LAMBDA_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint8_t, uint8_t)>f, uint8_t cache[]), {
  uint8_t*A_index = &cache[0];
  uint8_t*B_index = &array[sB];
  uint8_t*insert_index = &array[sA];
  uint8_t*A_last = &cache[eA-sA];
  uint8_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_LAMBDA_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint16_t, uint16_t)>f, uint16_t cache[]), {
  uint16_t*A_index = &cache[0];
  uint16_t*B_index = &array[sB];
  uint16_t*insert_index = &array[sA];
  uint16_t*A_last = &cache[eA-sA];
  uint16_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_LAMBDA_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint32_t, uint32_t)>f, uint32_t cache[]), {
  uint32_t*A_index = &cache[0];
  uint32_t*B_index = &array[sB];
  uint32_t*insert_index = &array[sA];
  uint32_t*A_last = &cache[eA-sA];
  uint32_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
AARRAY_define(void AARRAY_sortMergeExternal_LAMBDA_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint64_t, uint64_t)>f, uint64_t cache[]), {
  uint64_t*A_index = &cache[0];
  uint64_t*B_index = &array[sB];
  uint64_t*insert_index = &array[sA];
  uint64_t*A_last = &cache[eA-sA];
  uint64_t*B_last = &array[eB];
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(*B_index, *A_index)) {
        *insert_index = *A_index;
        A_index++;
        insert_index++;
        if(A_index == A_last) break; }
      else {
        *insert_index = *B_index;
        B_index++;
        insert_index++;
        if(B_index == B_last) break; } } }
  memcpy(insert_index, A_index, (size_t)(A_last-A_index)*sizeof(array[0])); })
static void(*const AARRAY_sortMergeExternal_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeExternal_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_sortMergeExternal_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeExternal_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeExternal_LAMBDA_uint64_t };
AARRAY_define(void AARRAY_sortMergeInternal_LAMBDA_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint8_t, uint8_t)>f, size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint8_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint8_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint8_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_LAMBDA_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint16_t, uint16_t)>f, size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint16_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint16_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint16_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_LAMBDA_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint32_t, uint32_t)>f, size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint32_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint32_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint32_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
AARRAY_define(void AARRAY_sortMergeInternal_LAMBDA_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint64_t, uint64_t)>f, size_t sBuff), {
  size_t A_count = 0, B_count = 0, insert = 0;
  if(eB-sB > 0 && eA-sA > 0) {
    while(1) {
      if(!f(array[sB+B_count], array[sBuff+A_count])) {
        { uint64_t temp = array[sA+insert];
                                  array[sA+insert] = array[sBuff+A_count];
                                  array[sBuff+A_count] = temp; };
        A_count++;
        insert++;
        if(A_count >= eA-sA) break; }
      else {
        { uint64_t temp = array[sA+insert];
                                  array[sA+insert] = array[sB+B_count];
                                  array[sB+B_count] = temp; };
        B_count++;
        insert++;
        if(B_count >= eB-sB) break; } } }
    for(size_t n = 0; n < (eA-sA)-A_count; n++)
    { uint64_t temp = array[sBuff+A_count+n];
                                  array[sBuff+A_count+n] = array[sA+insert+n];
                                  array[sA+insert+n] = temp; }; })
static void(*const AARRAY_sortMergeInternal_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInternal_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInternal_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInternal_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInternal_LAMBDA_uint64_t };
AARRAY_define(void AARRAY_sortMergeInPlace_LAMBDA_uint8_t(
    uint8_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint8_t, uint8_t)>f, uint8_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_LAMBDA(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_LAMBDA_uint16_t(
    uint16_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint16_t, uint16_t)>f, uint16_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_LAMBDA(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_LAMBDA_uint32_t(
    uint32_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint32_t, uint32_t)>f, uint32_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_LAMBDA(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
AARRAY_define(void AARRAY_sortMergeInPlace_LAMBDA_uint64_t(
    uint64_t array[], size_t sA, size_t eA, size_t sB, size_t eB,
    std::function<int(uint64_t, uint64_t)>f, uint64_t cache[]), {
  if(eA-sA == 0 || eB-sB == 0) return;
  while(1) {
    size_t mid = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sA], sB, eB, f);
    size_t amount = mid-eA;
    AARRAY_aSortRotate(array, eA-sA, sA, mid, cache, AARRAY_sortCache);
    if(eB == mid) break;
    sB = mid;
    sA = sA+amount; eA = sB;
    sA = AARRAY_aSortBinaryLast_LAMBDA(array, array[sA], sA, eA, f);
    if(eA-sA == 0) break; } })
static void(*const AARRAY_sortMergeInPlace_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortMergeInPlace_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_sortMergeInPlace_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_sortMergeInPlace_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortMergeInPlace_LAMBDA_uint64_t };
#define AARRAY_aSortMergeInto_LAMBDA(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, \
             std::function<int(int,int)>, void*)) \
    AARRAY_sortMergeInto_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (std::function<int(int,int)>)f, vecb))
#define AARRAY_aSortMergeExternal_LAMBDA(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, \
             std::function<int(int,int)>, void*)) \
    AARRAY_sortMergeExternal_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (std::function<int(int,int)>)f, vecb))
#define AARRAY_aSortMergeInternal_LAMBDA(vec, s1, s2, s3, s4, f, s5) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, \
             std::function<int(int,int)>, size_t)) \
    AARRAY_sortMergeInternal_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (std::function<int(int,int)>)f, s5))
#define AARRAY_aSortMergeInPlace_LAMBDA(vec, s1, s2, s3, s4, f, vecb) \
  (((void(*)(void*, size_t, size_t, size_t, size_t, \
             std::function<int(int,int)>, void*)) \
    AARRAY_sortMergeInPlace_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, s1, s2, s3, s4, (std::function<int(int,int)>)f, vecb))
#endif

/* from www.hackersdelight.org/hdcodetxt/martin/src/sqrt/sqrt.c */
AARRAY_define(int64_t AARRAY_sqrt(int64_t x), {
	/* Logically, these are unsigned. We need the sign bit to test
	   whether (op - res - one) underflowed */
	int64_t op = x, res, one;
	res = 0;
	/* "one" starts at the highest power of four <= than the argument */
	one = ((uint64_t)1) << 62; /* second-to-top bit set */
	while (one > op) one >>= 2;
	while (one != 0) {
		if (op >= res + one) {
			op = op - (res + one);
			res = res +  2 * one; }
		res /= 2;
		one /= 4; }
	return(res); })

/* alternative slow sqrt */
/* AARRAY_define(uint64_t AARRAY_double_to_uint(double val), {
	if(val > (double)INT64_MAX) return (uint64_t)INT64_MAX;
	if(val < (double)INT64_MIN) return (uint64_t)INT64_MIN;
	return (uint64_t)(int64_t)val; })
AARRAY_define(uint64_t AARRAY_sqrt(uint64_t val), {
	*//* bounded newton-raphson *//*
	if(val<=1) return val;
	uint64_t min = 1, max = val;
	uint64_t a = min+1, b = (max>>4)+2, c;
	int64_t fa = (int64_t)(val-a*a), fb = (int64_t)(val-b*b);
	while(min < max) {
		*//* divide by non-zero? interpolate : bisect *//*
		c = (uint64_t)fb-(uint64_t)fa?
			b - AARRAY_double_to_uint((double)fb * (double)(b-a) /
			                          (double)((uint64_t)fb-(uint64_t)fa)) :
			max-((max-min)>>1);
		*//* keep the pingpong ball on the table *//*
		a = b; b = c < min ? min : c >= max ? max-1 : c;
		fa = fb; fb = (int64_t)(val-b*b);
		if(fb == 0) return b;
		if(fb >  0) min = b+1; else max = b; }
	return min-1; }) */

/* all that work, just to support this beast
   I'm glad I didn't write it myself */
AARRAY_define(int AARRAY_sortCompare__uint8_t(uint8_t a, uint8_t b), {
  return a<b; })
AARRAY_define(int AARRAY_sortCompare__uint16_t(uint16_t a, uint16_t b), {
  return a<b; })
AARRAY_define(int AARRAY_sortCompare__uint32_t(uint32_t a, uint32_t b), {
  return a<b; })
AARRAY_define(int AARRAY_sortCompare__uint64_t(uint64_t a, uint64_t b), {
  return a<b; })
static void(*const AARRAY_sortCompare__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sortCompare__uint8_t,
  (void(*)(void))&AARRAY_sortCompare__uint16_t, 0,
  (void(*)(void))&AARRAY_sortCompare__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sortCompare__uint64_t };
AARRAY_define(uint8_t*AARRAY_sort_FUNC_uint8_t(
    uint8_t array[], int(*f)(uint8_t,uint8_t)), {
  size_t size = aLength(array);
  uint8_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint8_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint8_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_FUNC(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_FUNC(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint8_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint8_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_FUNC(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint8_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint8_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint8_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint16_t*AARRAY_sort_FUNC_uint16_t(
    uint16_t array[], int(*f)(uint16_t,uint16_t)), {
  size_t size = aLength(array);
  uint16_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint16_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint16_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_FUNC(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_FUNC(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint16_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint16_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_FUNC(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint16_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint16_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint16_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint32_t*AARRAY_sort_FUNC_uint32_t(
    uint32_t array[], int(*f)(uint32_t,uint32_t)), {
  size_t size = aLength(array);
  uint32_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint32_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint32_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_FUNC(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_FUNC(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint32_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint32_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_FUNC(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint32_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint32_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint32_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint64_t*AARRAY_sort_FUNC_uint64_t(
    uint64_t array[], int(*f)(uint64_t,uint64_t)), {
  size_t size = aLength(array);
  uint64_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint64_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint64_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_FUNC(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_FUNC(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_FUNC(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_FUNC(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint64_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint64_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_FUNC(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint64_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_FUNC(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint64_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_FUNC(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint64_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_FUNC(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_FUNC(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
static void(*const AARRAY_sort_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sort_FUNC_uint8_t,
  (void(*)(void))&AARRAY_sort_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_sort_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sort_FUNC_uint64_t };
#define aSort(vec) \
  (AARRAY_typeof(vec, (uint64_t*(*)(void*,void(*)(void))) \
    AARRAY_sort_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])((void*)vec, (void(*)(void)) \
    AARRAY_sortCompare__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]))
#define aSortF_FUNC(vec, f) \
  (AARRAY_typeof(vec, (uint64_t*(*)(void*,void(*)(void))) \
    AARRAY_sort_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])((void*)vec, (void(*)(void))f))

#if __has_extension(blocks)
AARRAY_define(uint8_t*AARRAY_sort_BLOCK_uint8_t(
    uint8_t array[], int(^f)(uint8_t,uint8_t)), {
  size_t size = aLength(array);
  uint8_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint8_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint8_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_BLOCK(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint8_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint8_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_BLOCK(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint8_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint8_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint8_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint16_t*AARRAY_sort_BLOCK_uint16_t(
    uint16_t array[], int(^f)(uint16_t,uint16_t)), {
  size_t size = aLength(array);
  uint16_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint16_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint16_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_BLOCK(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint16_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint16_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_BLOCK(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint16_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint16_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint16_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint32_t*AARRAY_sort_BLOCK_uint32_t(
    uint32_t array[], int(^f)(uint32_t,uint32_t)), {
  size_t size = aLength(array);
  uint32_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint32_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint32_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_BLOCK(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint32_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint32_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_BLOCK(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint32_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint32_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint32_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint64_t*AARRAY_sort_BLOCK_uint64_t(
    uint64_t array[], int(^f)(uint64_t,uint64_t)), {
  size_t size = aLength(array);
  uint64_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint64_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint64_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_BLOCK(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_BLOCK(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_BLOCK(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint64_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint64_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_BLOCK(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint64_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_BLOCK(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint64_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_BLOCK(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint64_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_BLOCK(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_BLOCK(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
static void(*const AARRAY_sort_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sort_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_sort_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_sort_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sort_BLOCK_uint64_t };
#define aSortF_BLOCK(vec, f) \
  (AARRAY_typeof(vec, (uint64_t*(*)(void*, void*)) \
    AARRAY_sort_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])((void*)vec, (void*)f))
#endif
#if defined(__cplusplus)
AARRAY_define(uint8_t*AARRAY_sort_LAMBDA_uint8_t(
    uint8_t array[], std::function<int(uint8_t, uint8_t)>f), {
  size_t size = aLength(array);
  uint8_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint8_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint8_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint8_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint8_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint8_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint8_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint8_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint8_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint8_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint8_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint8_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_LAMBDA(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint8_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint8_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint8_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint8_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint16_t*AARRAY_sort_LAMBDA_uint16_t(
    uint16_t array[], std::function<int(uint16_t, uint16_t)>f), {
  size_t size = aLength(array);
  uint16_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint16_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint16_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint16_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint16_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint16_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint16_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint16_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint16_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint16_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint16_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint16_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_LAMBDA(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint16_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint16_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint16_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint16_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint32_t*AARRAY_sort_LAMBDA_uint32_t(
    uint32_t array[], std::function<int(uint32_t, uint32_t)>f), {
  size_t size = aLength(array);
  uint32_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint32_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint32_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint32_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint32_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint32_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint32_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint32_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint32_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint32_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint32_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint32_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_LAMBDA(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint32_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint32_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint32_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint32_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
AARRAY_define(uint64_t*AARRAY_sort_LAMBDA_uint64_t(
    uint64_t array[], std::function<int(uint64_t, uint64_t)>f), {
  size_t size = aLength(array);
  uint64_t cache[AARRAY_sortCache];
  AARRAY_sortIt it;
  if(size < 4) {
    if(size == 3) {
      if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; };
      if(f(array[2], array[1])) {
        { uint64_t temp = array[1];
                                  array[1] = array[2];
                                  array[2] = temp; };
        if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; } }
    else if(size == 2) {
      if(f(array[1], array[0])) { uint64_t temp = array[0];
                                  array[0] = array[1];
                                  array[1] = temp; }; }
    return array; }
  /* new it */
  it.size = size;
  /* floor_power_of_2(size) */
  size_t s = size;
  s = s | (s >> 1); s = s | (s >> 2); s = s | (s >> 4);
  s = s | (s >> 8); s = s | (s >> 16);
  if(sizeof(size_t)==8) s = s | (s >> 32);
  s = s-(s >> 1);
  it.power_of_two = s;
  it.denominator = it.power_of_two/4;
  it.numerator_step = it.size % it.denominator;
  it.decimal_step = it.size/it.denominator;
  it.numerator = it.decimal = 0;
  while(!(it.decimal >= it.size)) {
    uint8_t order[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    size_t sRange, eRange;
    AARRAY_aSortNextRange(array, &it, &sRange, &eRange);
    if(eRange-sRange==8) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+7], array[sRange+6]) ||
     (order[6] > order[7] && !f(array[sRange+6], array[sRange+7]))) {
    { uint64_t temp = array[sRange+6];
                                  array[sRange+6] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[6];
                                  order[6] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+5]) ||
     (order[5] > order[7] && !f(array[sRange+5], array[sRange+7]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[7];
                                  order[7] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+7], array[sRange+3]) ||
     (order[3] > order[7] && !f(array[sRange+3], array[sRange+7]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+7];
                                  array[sRange+7] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[7];
                                  order[7] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+6], array[sRange+3]) ||
     (order[3] > order[6] && !f(array[sRange+3], array[sRange+6]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[6];
                                  order[6] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; }
    else if(eRange-sRange==7) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+5]) ||
     (order[5] > order[6] && !f(array[sRange+5], array[sRange+6]))) {
    { uint64_t temp = array[sRange+5];
                                  array[sRange+5] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[5];
                                  order[5] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+4]) ||
     (order[4] > order[6] && !f(array[sRange+4], array[sRange+6]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+6], array[sRange+2]) ||
     (order[2] > order[6] && !f(array[sRange+2], array[sRange+6]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+6];
                                  array[sRange+6] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[6];
                                  order[6] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+0]) ||
     (order[0] > order[4] && !f(array[sRange+0], array[sRange+4]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+1]) ||
     (order[1] > order[5] && !f(array[sRange+1], array[sRange+5]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==6) {
      \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+5], array[sRange+4]) ||
     (order[4] > order[5] && !f(array[sRange+4], array[sRange+5]))) {
    { uint64_t temp = array[sRange+4];
                                  array[sRange+4] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[4];
                                  order[4] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+3]) ||
     (order[3] > order[5] && !f(array[sRange+3], array[sRange+5]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+5], array[sRange+2]) ||
     (order[2] > order[5] && !f(array[sRange+2], array[sRange+5]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+5];
                                  array[sRange+5] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[5];
                                  order[5] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; }
    else if(eRange-sRange==5) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+3]) ||
     (order[3] > order[4] && !f(array[sRange+3], array[sRange+4]))) {
    { uint64_t temp = array[sRange+3];
                                  array[sRange+3] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[3];
                                  order[3] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+2]) ||
     (order[2] > order[4] && !f(array[sRange+2], array[sRange+4]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[4];
                                  order[4] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+4], array[sRange+1]) ||
     (order[1] > order[4] && !f(array[sRange+1], array[sRange+4]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+4];
                                  array[sRange+4] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[4];
                                  order[4] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+0]) ||
     (order[0] > order[3] && !f(array[sRange+0], array[sRange+3]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[3];
                                  order[3] = temp; }; };
      \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; }
    else if(eRange-sRange==4) {
      \
  if(f(array[sRange+1], array[sRange+0]) ||
     (order[0] > order[1] && !f(array[sRange+0], array[sRange+1]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+1];
                                  array[sRange+1] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[1];
                                  order[1] = temp; }; }; \
  if(f(array[sRange+3], array[sRange+2]) ||
     (order[2] > order[3] && !f(array[sRange+2], array[sRange+3]))) {
    { uint64_t temp = array[sRange+2];
                                  array[sRange+2] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[2];
                                  order[2] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+0]) ||
     (order[0] > order[2] && !f(array[sRange+0], array[sRange+2]))) {
    { uint64_t temp = array[sRange+0];
                                  array[sRange+0] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[0];
                                  order[0] = order[2];
                                  order[2] = temp; }; };
      \
  if(f(array[sRange+3], array[sRange+1]) ||
     (order[1] > order[3] && !f(array[sRange+1], array[sRange+3]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+3];
                                  array[sRange+3] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[3];
                                  order[3] = temp; }; }; \
  if(f(array[sRange+2], array[sRange+1]) ||
     (order[1] > order[2] && !f(array[sRange+1], array[sRange+2]))) {
    { uint64_t temp = array[sRange+1];
                                  array[sRange+1] = array[sRange+2];
                                  array[sRange+2] = temp; };
    { uint8_t temp = order[1];
                                  order[1] = order[2];
                                  order[2] = temp; }; }; } }
  if(size < 8) return array;
  while(1) {
    if(it.decimal_step < AARRAY_sortCache) {
      if((it.decimal_step+1)*4 <= AARRAY_sortCache
         && it.decimal_step*4 <= size) {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t
            sA1, sB1, sA2, sB2, sA3, sB3,
            eA1, eB1, eA2, eB2, eA3, eB3;
          AARRAY_aSortNextRange(array, &it, &sA1, &eA1);
          AARRAY_aSortNextRange(array, &it, &sB1, &eB1);
          AARRAY_aSortNextRange(array, &it, &sA2, &eA2);
          AARRAY_aSortNextRange(array, &it, &sB2, &eB2);
          if(f(array[eB1-1], array[sA1])) {
            memcpy(&cache[eB1-sB1], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[0], &array[sB1], (eB1-sB1)*sizeof(array[0])); }
          else if(f(array[sB1], array[eA1-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA1, eA1, sB1, eB1, f, &cache[0]); }
          else {
            if(!f(array[sB2], array[eA2-1])
               && !f(array[sA2], array[eB1-1])) continue;
            memcpy(&cache[0], &array[sA1], (eA1-sA1)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)], &array[sB1],
                   (eB1-sB1)*sizeof(array[0])); }
          eA1 = eB1;
          if(f(array[eB2-1], array[sA2])) {
            memcpy(&cache[(eA1-sA1)+(eB2-sB2)], &array[sA2],
                   (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[eA1-sA1], &array[sB2], (eB2-sB2)*sizeof(array[0])); }
          else if(f(array[sB2], array[eA2-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              array, sA2, eA2, sB2, eB2, f, &cache[eA1-sA1]); }
          else {
            memcpy(&cache[eA1-sA1], &array[sA2], (eA2-sA2)*sizeof(array[0]));
            memcpy(&cache[(eA1-sA1)+(eA2-sA2)], &array[sB2],
                   (eB2-sB2)*sizeof(array[0])); }
          eA2 = eB2;
          sA3 = 0; eA3 = eA1-sA1;
          sB3 = eA1-sA1; eB3 = (eA1-sA1)+(eA2-sA2);
          if(f(cache[eB3-1], cache[sA3])) {
            memcpy(&array[sA1+(eA2-sA2)], &cache[sA3],
                   (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1], &cache[sB3], (eB3-sB3)*sizeof(array[0])); }
          else if(f(cache[sB3], cache[eA3-1])) {
            AARRAY_aSortMergeInto_LAMBDA(
              cache, sA3, eA3, sB3, eB3, f, &array[sA1]); }
          else {
            memcpy(&array[sA1], &cache[sA3], (eA3-sA3)*sizeof(array[0]));
            memcpy(&array[sA1+(eA1-sA1)], &cache[sB3],
                   (eB3-sB3)*sizeof(array[0])); } }
        AARRAY_aSortNextLevel(array, &it); }
      else {
        it.numerator = it.decimal = 0;
        while(!(it.decimal >= it.size)) {
          size_t sA, eA, sB, eB;
          AARRAY_aSortNextRange(array, &it, &sA, &eA);
          AARRAY_aSortNextRange(array, &it, &sB, &eB);
          if(f(array[eB-1], array[sA]))
            AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
          else if(f(array[sB], array[eA-1])) {
            memcpy(&cache[0], &array[sA], (eA-sA)*sizeof(array[0]));
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sA, eA, sB, eB, f, cache); } } } }
    else {
      size_t block_size = (size_t)AARRAY_sqrt((int64_t)it.decimal_step);
      size_t buffer_size = it.decimal_step/block_size+1;
      int find_separately;
      size_t sBuff1, eBuff1, sBuff2, eBuff2, sA, eA, sB, eB;
      size_t index, last, count, find, start, pull_index = 0;
      struct { size_t from, to, count, sRange, eRange; } pull[2];
      pull[0].from = pull[0].to = pull[0].count = 0;
      pull[1].from = pull[1].to = pull[1].count = 0;
      pull[0].sRange = pull[0].eRange = 0;
      pull[1].sRange = pull[1].eRange = 0;
      sBuff1 = 0; eBuff1 = 0;
      sBuff2 = 0; eBuff2 = 0;
      find_separately = 0;
      find = buffer_size+buffer_size;
      if(block_size <= AARRAY_sortCache)
        find = buffer_size;
      else if(find > it.decimal_step) {
        find = buffer_size;
        find_separately = 1; }
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
          for(last = sA, count = 1; count < find; last = index, count++) {
            if(eA-(last+1) == 0) index = (last+1);
  else {
    int indexSet = 0;
    size_t skip = (eA-(last+1))/(find-count);
    if(!skip) skip = 1;
    for(index = (last+1)+skip; !f(array[last], array[index-1]); index += skip)
      if(index >= eA-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index, eA, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[last], index-skip, index, f); }
 ;
          if(index == eA) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+buffer_size;
            sBuff2 = sA+buffer_size; eBuff2 = sA+count;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = sA; eBuff1 = sA+count;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = sA; eBuff1 = sA+count;
            break; }
          else if(find_separately) {
            sBuff1 = sA; eBuff1 = sA+count;
            find_separately = 0; }
          else {
            sBuff2 = sA; eBuff2 = sA+count;
            break; } }
        else if(pull_index == 0 && count > eBuff1-sBuff1) {
          sBuff1 = sA; eBuff1 = sA+count;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = sA;; }
        for(last = eB-1, count = 1; count < find; last = index-1, count++) {
            if(last-sB == 0) index = sB;
  else {
    int indexSet = 0;
    size_t skip = (last-sB)/(find-count);
    if(!skip) skip = 1;
    for(index = last-skip; index > sB
        && !f(array[index-1], array[last]); index -= skip)
      if(index < sB+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], sB, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[last], index, index+skip, f); }
 ;
          if(index == sB) break; }
        index = last;
        if(count >= buffer_size) {
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;;
          pull_index = 1;
          if(count == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB-buffer_size;
            sBuff2 = eB-buffer_size; eBuff2 = eB;
            break; }
          else if(find == buffer_size+buffer_size) {
            sBuff1 = eB-count; eBuff1 = eB;
            find = buffer_size; }
          else if(block_size <= AARRAY_sortCache) {
            sBuff1 = eB-count; eBuff1 = eB;
            break; }
          else if(find_separately) {
            sBuff1 = eB-count; eBuff1 = eB;
            find_separately = 0; }
          else {
            if(pull[0].sRange == sA) pull[0].eRange -= pull[1].count;
            sBuff2 = eB-count; eBuff2 = eB;
            break; } }
        else if(pull_index == 0 && count > (eBuff1-sBuff1)) {
          sBuff1 = eB-count; eBuff1 = eB;
          \
  pull[pull_index].sRange = sA; \
  pull[pull_index].eRange = eB; \
  pull[pull_index].count = count; \
  pull[pull_index].from = index; \
  pull[pull_index].to = eB;; } }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t sRange, eRange;
        size_t length = pull[pull_index].count;
        if(pull[pull_index].to < pull[pull_index].from) {
          index = pull[pull_index].from;
          for(count = 1; count < length; count++) {
            size_t index_ = index;
              if((pull[pull_index].from-(count-1))-pull[pull_index].to == 0) index = pull[pull_index].to;
  else {
    int indexSet = 0;
    size_t skip = ((pull[pull_index].from-(count-1))-pull[pull_index].to)/(length-count);
    if(!skip) skip = 1;
    for(index = (pull[pull_index].from-(count-1))-skip; index > pull[pull_index].to
        && !f(array[index-1], array[index_-1]); index -= skip)
      if(index < pull[pull_index].to+skip) {
      index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], pull[pull_index].to, index, f);
      indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryFirst_LAMBDA(array, array[index_-1], index, index+skip, f); }
 ;
            sRange = index+1; eRange = pull[pull_index].from+1;
            AARRAY_aSortRotate(array, (eRange-sRange)-count, sRange, eRange,
                          cache, AARRAY_sortCache);
            pull[pull_index].from = index+count; } }
        else if(pull[pull_index].to > pull[pull_index].from) {
          index = pull[pull_index].from+1;
          for(count = 1; count < length; count++) {
              if(pull[pull_index].to-index == 0) index = index;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].to-index)/(length-count);
    if(!skip) skip = 1;
    for(index = index+skip; !f(array[index], array[index-1]); index += skip)
      if(index >= pull[pull_index].to-skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index, pull[pull_index].to, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[index], index-skip, index, f); }
 ;
            sRange = pull[pull_index].from; eRange = index-1;
            AARRAY_aSortRotate(
              array, count, sRange, eRange, cache, AARRAY_sortCache);
            pull[pull_index].from = index-1-count; } } }
      buffer_size = eBuff1-sBuff1;
      block_size  = it.decimal_step/buffer_size+1;
      it.numerator = it.decimal = 0;
      while(!(it.decimal >= it.size)) {
        AARRAY_aSortNextRange(array, &it, &sA, &eA);
        AARRAY_aSortNextRange(array, &it, &sB, &eB);
        start = sA;
        if(start == pull[0].sRange) {
          if(pull[0].from > pull[0].to) {
            sA += pull[0].count;
            if(eA-sA == 0) continue; }
          else if(pull[0].from < pull[0].to) {
            eB -= pull[0].count;
            if(eB-sB == 0) continue; } }
        if(start == pull[1].sRange) {
          if(pull[1].from > pull[1].to) {
            sA += pull[1].count;
            if(eA-sA == 0) continue; }
          else if(pull[1].from < pull[1].to) {
            eB -= pull[1].count;
            if(eB-sB == 0) continue; ;  } }
        if(f(array[eB-1], array[sA]))
          AARRAY_aSortRotate(array, eA-sA, sA, eB, cache, AARRAY_sortCache);
        else if(f(array[eA], array[eA-1])) {
          size_t
            sBlockA, eBlockA, sFirstA, eFirstA, sLastA,
            eLastA, sLastB, eLastB, sBlockB, eBlockB;
          size_t indexA, findA;
          sBlockA = sA; eBlockA = eA;
          sFirstA = sA; eFirstA = sA+(eBlockA-sBlockA) % block_size;
          for(indexA = sBuff1, index = eFirstA; index < eBlockA;
              indexA++, index += block_size) 
            { uint64_t temp = array[indexA];
                                  array[indexA] = array[index];
                                  array[index] = temp; };
          sLastA = sFirstA;
          eLastA = eFirstA;
          sLastB = 0; eLastB = 0;
          sBlockB = sB; eBlockB = sB+(block_size < eB-sB? block_size : eB-sB);
          sBlockA += eFirstA-sFirstA;
          indexA = sBuff1;
          if(eLastA-sLastA <= AARRAY_sortCache)
            memcpy(&cache[0], &array[sLastA], (eLastA-sLastA)*sizeof(array[0]));
          else if(eBuff2-sBuff2 > 0)
              for(size_t n = 0; n < eLastA-sLastA; n++)
    { uint64_t temp = array[sLastA+n];
                                  array[sLastA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
          if(eBlockA-sBlockA > 0) {
            while(1) {
              if((eLastB-sLastB > 0 && !f(array[eLastB-1], array[indexA]))
                 || eBlockB-sBlockB == 0) {
                size_t B_split = AARRAY_aSortBinaryFirst_LAMBDA(
                    array, array[indexA], sLastB, eLastB, f);
                size_t B_remaining = eLastB-B_split;
                size_t minA = sBlockA;
                for(findA = minA+block_size; findA < eBlockA;
                    findA += block_size)
                  if(f(array[findA], array[minA])) minA = findA;
                  for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[minA+n];
                                  array[minA+n] = temp; };
                { uint64_t temp = array[sBlockA];
                                  array[sBlockA] = array[indexA];
                                  array[indexA] = temp; };
                indexA++;
                if(eLastA-sLastA <= AARRAY_sortCache)
                  AARRAY_aSortMergeExternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                else if(eBuff2-sBuff2 > 0)
                  AARRAY_aSortMergeInternal_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, sBuff2);
                else
                  AARRAY_aSortMergeInPlace_LAMBDA(
                      array, sLastA, eLastA, eLastA, B_split, f, cache);
                if(eBuff2-sBuff2 > 0 || block_size <= AARRAY_sortCache) {
                  if(block_size <= AARRAY_sortCache)
                    memcpy(&cache[0], &array[sBlockA],
                           block_size*sizeof(array[0]));
                  else   for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBuff2+n];
                                  array[sBuff2+n] = temp; };
                    for(size_t n = 0; n < B_remaining; n++)
    { uint64_t temp = array[B_split+n];
                                  array[B_split+n] = array[sBlockA+block_size-B_remaining+n];
                                  array[sBlockA+block_size-B_remaining+n] = temp; }; }
                else
                  AARRAY_aSortRotate(array, sBlockA-B_split, B_split,
                    sBlockA+block_size, cache, AARRAY_sortCache);
                sLastA = sBlockA-B_remaining; eLastA =
                  sBlockA-B_remaining+block_size;
                sLastB = eLastA; eLastB = eLastA+B_remaining;
                sBlockA += block_size;
                if(eBlockA-sBlockA == 0) break; }
              else if(eBlockB-sBlockB < block_size) {
                AARRAY_aSortRotate(
                  array, sBlockB-sBlockA, sBlockA, eBlockB, cache, 0);
                sLastB = sBlockA; eLastB = sBlockA+(eBlockB-sBlockB);
                sBlockA += eBlockB-sBlockB;
                eBlockA += eBlockB-sBlockB;
                eBlockB = sBlockB; }
              else {
                  for(size_t n = 0; n < block_size; n++)
    { uint64_t temp = array[sBlockA+n];
                                  array[sBlockA+n] = array[sBlockB+n];
                                  array[sBlockB+n] = temp; };
                sLastB = sBlockA; eLastB = sBlockA+block_size;
                sBlockA += block_size;
                eBlockA += block_size;
                sBlockB += block_size;
                if(eBlockB > eB-block_size) eBlockB = eB;
                else eBlockB += block_size; } } }
          if(eLastA-sLastA <= AARRAY_sortCache)
            AARRAY_aSortMergeExternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache);
          else if(eBuff2-sBuff2 > 0)
            AARRAY_aSortMergeInternal_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, sBuff2);
          else
            AARRAY_aSortMergeInPlace_LAMBDA(
              array, sLastA, eLastA, eLastA, eB, f, cache); } }
      /* insertion sort */
      size_t i, j;
      for(i = sBuff2+1; i < eBuff2; i++) {
        const uint64_t temp = array[i];
        for(j = i; j > sBuff2 && f(temp, array[j-1]); j--)
          array[j] = array[j-1];
        array[j] = temp; }
      for(pull_index = 0; pull_index < 2; pull_index++) {
        size_t amount, unique = pull[pull_index].count*2;
        if(pull[pull_index].from > pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].sRange,
            eBuff = pull[pull_index].sRange+pull[pull_index].count;
          while(eBuff-sBuff > 0) {
              if(pull[pull_index].eRange-eBuff == 0) index = eBuff;
  else {
    int indexSet = 0;
    size_t skip = (pull[pull_index].eRange-eBuff)/(unique);
    if(!skip) skip = 1;
    for(index = eBuff+skip; f(array[index-1], array[sBuff]); index += skip)
      if(index >= pull[pull_index].eRange-skip) {
        index = AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index, pull[pull_index].eRange, f);
        indexSet = 1; break; }
    if(!indexSet) index =
      AARRAY_aSortBinaryFirst_LAMBDA(array, array[sBuff], index-skip, index, f); }
   ;
            amount = index-eBuff;
            AARRAY_aSortRotate(array, eBuff-sBuff, sBuff, index,
                               cache, AARRAY_sortCache);
            sBuff += (amount+1);
            eBuff += amount;
            unique -= 2; } }
        else if(pull[pull_index].from < pull[pull_index].to) {
          size_t
            sBuff = pull[pull_index].eRange-pull[pull_index].count,
            eBuff = pull[pull_index].eRange;
          while(eBuff-sBuff > 0) {
              if(sBuff-pull[pull_index].sRange == 0) index = pull[pull_index].sRange;
  else {
    int indexSet = 0;
    size_t skip = (sBuff-pull[pull_index].sRange)/(unique);
    if(!skip) skip = 1;
    for(index = sBuff-skip; index > pull[pull_index].sRange
        && f(array[eBuff-1], array[index-1]); index -= skip)
      if(index < pull[pull_index].sRange+skip) {
        index = AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], pull[pull_index].sRange, index, f);
        indexSet = 1; break; }
  if(!indexSet) index =
    AARRAY_aSortBinaryLast_LAMBDA(array, array[eBuff-1], index, index+skip, f); };
            amount = sBuff-index;
            AARRAY_aSortRotate(
              array, amount, index, eBuff, cache, AARRAY_sortCache);
            sBuff -= amount;
            eBuff -= (amount+1);
            unique -= 2; } } } }
    if(!AARRAY_aSortNextLevel(array, &it)) break; }
  return array; })
static void(*const AARRAY_sort_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_sort_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_sort_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_sort_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_sort_LAMBDA_uint64_t };
#define aSortF_LAMBDA(vec, f) \
  (AARRAY_typeof(vec, (uint64_t*(*)(void*, \
                                    std::function<int(uint64_t,uint64_t)>)) \
    AARRAY_sort_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   ((void*)vec, (std::function<int(uint64_t,uint64_t)>)f))
#endif




/* (a^b) to avoid overflow into the sign bit */



/* an alternative, probably slower, but fully tested */
/* AARRAY_define(int AARRAY_binary_FNAME_TYPE(
	  TYPE*vec, size_t*index, TYPE key, FTYPE(TYPE)), {
	  size_t min = 0, width = aLength(vec);
	  size_t base = 0;
	  while(width) {
	    min = base + (width>>1);
	    int64_t cmp = f(key, vec[min]);
	    if(cmp==0) { *index = min; return 1; }
	    if(cmp>0)  { base = min+1; width--;  }
	    width >>= 1; }
	  *index = min; return 0; }) */

/* our own shmancy algorithm, to make your life more enjoyable */
/* same again but with stride -- for arrays of structs */
AARRAY_define(int64_t AARRAY_searchCompare__uint8_t(uint8_t a, uint8_t b), {
  
  return (int64_t)((uint64_t)a-(uint64_t)b); })
AARRAY_define(int64_t AARRAY_searchCompare__uint16_t(uint16_t a, uint16_t b), {
  
  return (int64_t)((uint64_t)a-(uint64_t)b); })
AARRAY_define(int64_t AARRAY_searchCompare__uint32_t(uint32_t a, uint32_t b), {
  
  return (int64_t)((uint64_t)a-(uint64_t)b); })
AARRAY_define(int64_t AARRAY_searchCompare__uint64_t(uint64_t a, uint64_t b), {
  
  return (int64_t)((uint64_t)a-(uint64_t)b); })
static void(*const AARRAY_searchCompare__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_searchCompare__uint8_t,
  (void(*)(void))&AARRAY_searchCompare__uint16_t, 0,
  (void(*)(void))&AARRAY_searchCompare__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_searchCompare__uint64_t };
AARRAY_define(int AARRAY_binary_FUNC_uint8_t(
  uint8_t*vec, size_t*index, uint8_t key, int64_t(*f)(uint8_t,uint8_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_FUNC_uint16_t(
  uint16_t*vec, size_t*index, uint16_t key, int64_t(*f)(uint16_t,uint16_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_FUNC_uint32_t(
  uint32_t*vec, size_t*index, uint32_t key, int64_t(*f)(uint32_t,uint32_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_FUNC_uint64_t(
  uint64_t*vec, size_t*index, uint64_t key, int64_t(*f)(uint64_t,uint64_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
static void(*const AARRAY_binary_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_binary_FUNC_uint8_t,
  (void(*)(void))&AARRAY_binary_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_binary_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_binary_FUNC_uint64_t };
AARRAY_define(int AARRAY_pingpong_FUNC_uint8_t(
  uint8_t*vec, size_t*index, uint8_t key, int64_t(*f)(uint8_t,uint8_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_FUNC_uint16_t(
  uint16_t*vec, size_t*index, uint16_t key, int64_t(*f)(uint16_t,uint16_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_FUNC_uint32_t(
  uint32_t*vec, size_t*index, uint32_t key, int64_t(*f)(uint32_t,uint32_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_FUNC_uint64_t(
  uint64_t*vec, size_t*index, uint64_t key, int64_t(*f)(uint64_t,uint64_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
static void(*const AARRAY_pingpong_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_pingpong_FUNC_uint8_t,
  (void(*)(void))&AARRAY_pingpong_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_pingpong_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_pingpong_FUNC_uint64_t };
AARRAY_define(int AARRAY_pingpongS_FUNC_uint8_t(
  uint8_t*vec, size_t*index, uint8_t*key, int64_t(*f)(uint8_t*,uint8_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_FUNC_uint16_t(
  uint16_t*vec, size_t*index, uint16_t*key, int64_t(*f)(uint16_t*,uint16_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_FUNC_uint32_t(
  uint32_t*vec, size_t*index, uint32_t*key, int64_t(*f)(uint32_t*,uint32_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_FUNC_uint64_t(
  uint64_t*vec, size_t*index, uint64_t*key, int64_t(*f)(uint64_t*,uint64_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
static void(*const AARRAY_pingpongS_FUNC_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_pingpongS_FUNC_uint8_t,
  (void(*)(void))&AARRAY_pingpongS_FUNC_uint16_t, 0,
  (void(*)(void))&AARRAY_pingpongS_FUNC_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_pingpongS_FUNC_uint64_t };
#define aSearch(vec, index, key) \
  ((int(*)(void*, size_t*, uint64_t, void(*)(void))) \
   AARRAY_binary_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, \
    (void(*)(void))AARRAY_searchCompare__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])
#define aSearchF_FUNC(vec, index, key, f) \
  ((int(*)(void*, size_t*, uint64_t, void(*)(void))) \
   AARRAY_binary_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, (void(*)(void))f)
#define aSearchP(vec, index, key) \
  ((int(*)(void*, size_t*, uint64_t, void(*)(void))) \
   AARRAY_pingpong_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, \
    (void(*)(void))AARRAY_searchCompare__FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)])
#define aSearchPF_FUNC(vec, index, key, f) \
  ((int(*)(void*, size_t*, uint64_t, void(*)(void))) \
   AARRAY_pingpong_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, (void(*)(void))f)
#define aSearchPS_FUNC(vec, index, key, f, stride) \
  ((int(*)(void*, size_t*, uint64_t, void(*)(void), size_t)) \
   AARRAY_pingpongS_FUNC_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, (void(*)(void))f, stride)

#if __has_extension(blocks)
AARRAY_define(int AARRAY_binary_BLOCK_uint8_t(
  uint8_t*vec, size_t*index, uint8_t key, int64_t(^f)(uint8_t,uint8_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_BLOCK_uint16_t(
  uint16_t*vec, size_t*index, uint16_t key, int64_t(^f)(uint16_t,uint16_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_BLOCK_uint32_t(
  uint32_t*vec, size_t*index, uint32_t key, int64_t(^f)(uint32_t,uint32_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_BLOCK_uint64_t(
  uint64_t*vec, size_t*index, uint64_t key, int64_t(^f)(uint64_t,uint64_t)), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
static void(*const AARRAY_binary_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_binary_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_binary_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_binary_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_binary_BLOCK_uint64_t };
AARRAY_define(int AARRAY_pingpong_BLOCK_uint8_t(
  uint8_t*vec, size_t*index, uint8_t key, int64_t(^f)(uint8_t,uint8_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_BLOCK_uint16_t(
  uint16_t*vec, size_t*index, uint16_t key, int64_t(^f)(uint16_t,uint16_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_BLOCK_uint32_t(
  uint32_t*vec, size_t*index, uint32_t key, int64_t(^f)(uint32_t,uint32_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_BLOCK_uint64_t(
  uint64_t*vec, size_t*index, uint64_t key, int64_t(^f)(uint64_t,uint64_t)), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
static void(*const AARRAY_pingpong_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_pingpong_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_pingpong_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_pingpong_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_pingpong_BLOCK_uint64_t };
AARRAY_define(int AARRAY_pingpongS_BLOCK_uint8_t(
  uint8_t*vec, size_t*index, uint8_t*key, int64_t(^f)(uint8_t*,uint8_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_BLOCK_uint16_t(
  uint16_t*vec, size_t*index, uint16_t*key, int64_t(^f)(uint16_t*,uint16_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_BLOCK_uint32_t(
  uint32_t*vec, size_t*index, uint32_t*key, int64_t(^f)(uint32_t*,uint32_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_BLOCK_uint64_t(
  uint64_t*vec, size_t*index, uint64_t*key, int64_t(^f)(uint64_t*,uint64_t*), size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
static void(*const AARRAY_pingpongS_BLOCK_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_pingpongS_BLOCK_uint8_t,
  (void(*)(void))&AARRAY_pingpongS_BLOCK_uint16_t, 0,
  (void(*)(void))&AARRAY_pingpongS_BLOCK_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_pingpongS_BLOCK_uint64_t };
#define aSearchF_BLOCK(vec, index, key, f) \
  ((int(*)(void*, size_t*, uint64_t, void*)) \
   AARRAY_binary_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, (void*)f)
#define aSearchPF_BLOCK(vec, index, key, f) \
  ((int(*)(void*, size_t*, uint64_t, void*)) \
   AARRAY_pingpong_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, (void*)f)
#define aSearchPS_BLOCK(vec, index, key, f, stride) \
  ((int(*)(void*, size_t*, uint64_t, void*, size_t)) \
   AARRAY_pingpongS_BLOCK_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, (void*)f, stride)
#endif
#if defined(__cplusplus)
AARRAY_define(int AARRAY_binary_LAMBDA_uint8_t(
  uint8_t*vec, size_t*index, uint8_t key, std::function<int64_t(uint8_t,uint8_t)>f), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_LAMBDA_uint16_t(
  uint16_t*vec, size_t*index, uint16_t key, std::function<int64_t(uint16_t,uint16_t)>f), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_LAMBDA_uint32_t(
  uint32_t*vec, size_t*index, uint32_t key, std::function<int64_t(uint32_t,uint32_t)>f), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
AARRAY_define(int AARRAY_binary_LAMBDA_uint64_t(
  uint64_t*vec, size_t*index, uint64_t key, std::function<int64_t(uint64_t,uint64_t)>f), {
  size_t min = 0, mid = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  while(min < max) {
    mid = (min + max) >> 1;
    int64_t cmp = f(key, vec[mid]);
    if(cmp == 0) { *index = mid; return 1; } 
    if(cmp < 0) max = mid;
    else min = ++mid; }
  *index = mid; return 0; })
static void(*const AARRAY_binary_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_binary_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_binary_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_binary_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_binary_LAMBDA_uint64_t };
AARRAY_define(int AARRAY_pingpong_LAMBDA_uint8_t(
  uint8_t*vec, size_t*index, uint8_t key, std::function<int64_t(uint8_t,uint8_t)>f), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_LAMBDA_uint16_t(
  uint16_t*vec, size_t*index, uint16_t key, std::function<int64_t(uint16_t,uint16_t)>f), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_LAMBDA_uint32_t(
  uint32_t*vec, size_t*index, uint32_t key, std::function<int64_t(uint32_t,uint32_t)>f), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpong_LAMBDA_uint64_t(
  uint64_t*vec, size_t*index, uint64_t key, std::function<int64_t(uint64_t,uint64_t)>f), {
  size_t min = 0, max = aLength(vec);
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec[a]), fb = f(key, vec[b]);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec[b]);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
static void(*const AARRAY_pingpong_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_pingpong_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_pingpong_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_pingpong_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_pingpong_LAMBDA_uint64_t };
AARRAY_define(int AARRAY_pingpongS_LAMBDA_uint8_t(
  uint8_t*vec, size_t*index, uint8_t*key, std::function<int64_t(uint8_t*,uint8_t*)>f, size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_LAMBDA_uint16_t(
  uint16_t*vec, size_t*index, uint16_t*key, std::function<int64_t(uint16_t*,uint16_t*)>f, size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_LAMBDA_uint32_t(
  uint32_t*vec, size_t*index, uint32_t*key, std::function<int64_t(uint32_t*,uint32_t*)>f, size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
AARRAY_define(int AARRAY_pingpongS_LAMBDA_uint64_t(
  uint64_t*vec, size_t*index, uint64_t*key, std::function<int64_t(uint64_t*,uint64_t*)>f, size_t stride), {
  size_t min = 0, max = aLength(vec)/stride;
  if(!max) { *index = 0; return 0; }
  /* start secant 1/8th of the way into the array, because why not */
  size_t a = min+(max>>3), b = max-1-(max>>3), c;
  int64_t fa = f(key, vec+a*stride),
    fb = f(key, vec+b*stride);
  while(min < max) {
    /* divide by non-zero? interpolate : bisect */
    c = fb-fa? b - (size_t)((double)fb * (double)(b-a) / (double)(fb-fa)) : max-((max-min)>>1);
    /* keep the pingpong ball on the table */
    a = b; b = c < min ? min : c >= max ? max-1 : c;
    fa = fb; fb = f(key, vec+b*stride);
    if(fb == 0) { *index = b; return 1; }
    if(fb >  0) min = b+1; else max = b; }
  *index = max; return 0; })
static void(*const AARRAY_pingpongS_LAMBDA_FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_pingpongS_LAMBDA_uint8_t,
  (void(*)(void))&AARRAY_pingpongS_LAMBDA_uint16_t, 0,
  (void(*)(void))&AARRAY_pingpongS_LAMBDA_uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_pingpongS_LAMBDA_uint64_t };
#define aSearchF_LAMBDA(vec, index, key, f) \
  ((int(*)(void*, size_t*, uint64_t, \
           std::function<int64_t(uint64_t,uint64_t)>)) \
   AARRAY_binary_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, \
           (std::function<int64_t(uint64_t,uint64_t)>)f)
#define aSearchPF_LAMBDA(vec, index, key, f) \
  ((int(*)(void*, size_t*, uint64_t, \
           std::function<int64_t(uint64_t,uint64_t)>)) \
   AARRAY_pingpong_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, \
           (std::function<int64_t(uint64_t,uint64_t)>)f)
#define aSearchPS_LAMBDA(vec, index, key, f, stride) \
  ((int(*)(void*, size_t*, uint64_t, \
           std::function<int64_t(uint64_t*,uint64_t*)>, size_t)) \
   AARRAY_pingpongS_LAMBDA_FUNCTIONS[sizeof(*vec) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*vec)]) \
   (vec, index, (uint64_t)key, \
           (std::function<int64_t(uint64_t*,uint64_t*)>)f, stride)
#endif




#if !defined(AARRAY_NOCONVENIENCE)
#if __has_extension(blocks)
  #define aMap     aMap_BLOCK
  #define aFilter  aFilter_BLOCK
  #define aFold    aFold_BLOCK
  #define aLoop    aLoop_BLOCK
  #define aSortF   aSortF_BLOCK
  #define aSearchF aSearchF_BLOCK
  #define aSearchPS aSearchPS_BLOCK
  #define aSearchPF aSearchPF_BLOCK
#elif defined(__cplusplus)
  #define aMap     aMap_LAMBDA
  #define aFilter  aFilter_LAMBDA
  #define aFold    aFold_LAMBDA
  #define aLoop    aLoop_LAMBDA
  #define aSortF   aSortF_LAMBDA
  #define aSearchF aSearchF_LAMBDA
  #define aSearchPS aSearchPS_LAMBDA
  #define aSearchPF aSearchPF_LAMBDA
#else
  #define aMap     aMap_FUNC
  #define aFilter  aFilter_FUNC
  #define aFold    aFold_FUNC
  #define aLoop    aLoop_FUNC
  #define aSortF   aSortF_FUNC
  #define aSearchF aSearchF_FUNC
  #define aSearchPS aSearchPS_FUNC
  #define aSearchPF aSearchPF_FUNC
#endif
#endif




/* api to quickly print arrays without '\0' endings */
AARRAY_define(int AARRAY_Write__uint8_t(
    char errLoc[], FILE*file, size_t vecsCount, uintptr_t vecs[]), {
  AARRAY_safety((void)errLoc, if(!file) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint8_t), *((size_t*)vecs[n]-1), file))
      return -1;
  return 0; })
AARRAY_define(int AARRAY_Write__uint16_t(
    char errLoc[], FILE*file, size_t vecsCount, uintptr_t vecs[]), {
  AARRAY_safety((void)errLoc, if(!file) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint16_t), *((size_t*)vecs[n]-1), file))
      return -1;
  return 0; })
AARRAY_define(int AARRAY_Write__uint32_t(
    char errLoc[], FILE*file, size_t vecsCount, uintptr_t vecs[]), {
  AARRAY_safety((void)errLoc, if(!file) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint32_t), *((size_t*)vecs[n]-1), file))
      return -1;
  return 0; })
AARRAY_define(int AARRAY_Write__uint64_t(
    char errLoc[], FILE*file, size_t vecsCount, uintptr_t vecs[]), {
  AARRAY_safety((void)errLoc, if(!file) AARRAY_Error_NullParameter);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint64_t), *((size_t*)vecs[n]-1), file))
      return -1;
  return 0; })
static void(*const AARRAY_Write__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_Write__uint8_t,
  (void(*)(void))&AARRAY_Write__uint16_t, 0,
  (void(*)(void))&AARRAY_Write__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_Write__uint64_t };
AARRAY_define(int AARRAY_WriteE__uint8_t(
    size_t vecsCount, uintptr_t vecs[]), {
  fflush(stdout);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint8_t), *((size_t*)vecs[n]-1), stderr))
      return -1;
  fflush(stderr);
  return 0; })
AARRAY_define(int AARRAY_WriteE__uint16_t(
    size_t vecsCount, uintptr_t vecs[]), {
  fflush(stdout);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint16_t), *((size_t*)vecs[n]-1), stderr))
      return -1;
  fflush(stderr);
  return 0; })
AARRAY_define(int AARRAY_WriteE__uint32_t(
    size_t vecsCount, uintptr_t vecs[]), {
  fflush(stdout);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint32_t), *((size_t*)vecs[n]-1), stderr))
      return -1;
  fflush(stderr);
  return 0; })
AARRAY_define(int AARRAY_WriteE__uint64_t(
    size_t vecsCount, uintptr_t vecs[]), {
  fflush(stdout);
  size_t n = (size_t)-1; while(++n < vecsCount)
    if(vecs[n] &&
       *((size_t*)vecs[n]-1) >
       fwrite((void*)vecs[n], sizeof(uint64_t), *((size_t*)vecs[n]-1), stderr))
      return -1;
  fflush(stderr);
  return 0; })
static void(*const AARRAY_WriteE__FUNCTIONS[9])(void) = {
	NULL,
  (void(*)(void))&AARRAY_WriteE__uint8_t,
  (void(*)(void))&AARRAY_WriteE__uint16_t, 0,
  (void(*)(void))&AARRAY_WriteE__uint32_t, 0, 0, 0,
  (void(*)(void))&AARRAY_WriteE__uint64_t };

#define AARRAY_ArgsHead(A, ...) A

#define aWriteF(file, ...) \
  ((int(*)(char[], FILE*, size_t, uintptr_t[])) \
   AARRAY_Write__FUNCTIONS[sizeof(*AARRAY_ArgsHead(__VA_ARGS__, NULL)) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*AARRAY_ArgsHead(__VA_ARGS__, NULL))]) \
  (AARRAY_LINE, file, /* for c++ */ \
   AARRAY_nowarn_internal_start AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end \
   AARRAY_nowarn_internal_end)
#define aWrite(...)  aWriteF(stdout, __VA_ARGS__)
#define aWriteE(...) \
  ((int(*)(size_t, uintptr_t[])) \
   AARRAY_WriteE__FUNCTIONS[sizeof(*AARRAY_ArgsHead(__VA_ARGS__, NULL)) > 8 ? AARRAY_Error_ArrayIsWide : sizeof(*AARRAY_ArgsHead(__VA_ARGS__, NULL))]) \
  (/* for c++ */ \
   AARRAY_nowarn_internal_start AARRAY_nowarn_internal_start \
  sizeof((uintptr_t[]){(uintptr_t)__VA_ARGS__}) / sizeof(uintptr_t), \
  (uintptr_t*)AARRAY_move((uintptr_t[]){(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end \
   AARRAY_nowarn_internal_end)




/* api to emulate printf for arrays
 * cppcheck-suppress ctuuninitvar
 * cppcheck-suppress uninitvar
 */
AARRAY_define(void AARRAY_percent_parse(
    char errLoc[], const char fmt[], size_t*pptr,
    char*pspecifier, int*pnum1, int*pnum2,
    size_t*pstart, size_t*pend, size_t*doublePercent), {
  /* get parser values */
  char specifier = *pspecifier; int num1 = *pnum1, num2 = *pnum2;
  size_t start = *pstart; size_t end = *pend; size_t ptr = *pptr;
  while(1) {
    if(fmt[ptr] == '\0') {
      end = ptr;
      specifier = 'e'; break; }
    else if(0==strncmp(fmt+ptr, "%v", 2)) {
      end = ptr;
      ptr = start = ptr+2;
      specifier = 'v'; break; }
    else if(0==strncmp(fmt+ptr, "%s", 2)) {
      end = ptr;
      ptr = start = ptr+2;
      specifier = 's'; break; }
    else if(0==strncmp(fmt+ptr, "%c", 2)) {
      end = ptr;
      ptr = start = ptr+2;
      specifier = 'c'; break; }
    else if(0==strncmp(fmt+ptr, "%%", 2)) {
      end = ptr;
      ptr = start = ptr+2;
      *doublePercent = ptr;
      specifier = '%'; break; }
    else if(fmt[ptr] == '%') {
      end = ptr;
      /* parse %u %i arguments == %base u|i bitwidth
         base == 2-64    bitwidth == c s i l ll 8 16 32 64 z m p */ 
      ptr++; size_t ptr_start = ptr;
      AARRAY_safety((void)errLoc;,
                    if(fmt[ptr]=='\0') AARRAY_Error_FormatStringMalformed);
      num1 = 0; num2 = 0;
      /* get base to print number in */
      char num_str[] = "10";
      while(fmt[ptr] >= '0' && fmt[ptr] <= '9') ptr++;
      AARRAY_safety(, if(ptr-ptr_start > 2) AARRAY_Error_FormatStringMalformed
      else) if(ptr != ptr_start) {
        num_str[0] = fmt[ptr_start];
        num_str[1] = (ptr != ptr_start+1 ? fmt[ptr_start+1] : '\0');
        num1 = (int)strtoul(num_str, NULL, 10); }
      else num1 = 10;
      /* get number's type */
      specifier = fmt[ptr++];
      AARRAY_safety(, if((specifier != 'i' && specifier != 'u' &&
         specifier != 'f' && specifier != 'd') || 
         (num1 < 2 || num1 > 64))
        AARRAY_Error_FormatStringMalformed);
      /* get number's bit width */
      if(specifier == 'i' || specifier == 'u') {
        if(0==strncmp(fmt+ptr, "ll", 2)) {
          num2 = sizeof(long long)*8; ptr+=2; }
        else if(0==strncmp(fmt+ptr, "16", 2)) { num2 = 16; ptr+=2; }
        else if(0==strncmp(fmt+ptr, "32", 2)) { num2 = 32; ptr+=2; }
        else if(0==strncmp(fmt+ptr, "64", 2)) { num2 = 64; ptr+=2; }
        else switch(fmt[ptr]) {
          case 'c': case '8': num2 = sizeof(char)*8; ptr++; break;
          case 's': num2 = sizeof(short)*8; ptr++; break;
          case 'i': num2 = sizeof(int)*8; ptr++; break;
          case 'l': num2 = sizeof(long)*8; ptr++; break;
          case 'z': num2 = sizeof(size_t)*8; ptr++; break;
          case 'm': num2 = sizeof(intmax_t)*8; ptr++; break;
          case 'p': num2 = sizeof(intptr_t)*8; ptr++; break;
          default:  num2 = sizeof(int)*8; } }
      else if(specifier == 'f'){ num2 = sizeof(float)*8; }
      else if(specifier == 'd'){ num2 = sizeof(double)*8; }
      else { AARRAY_safety(, AARRAY_Error_FormatStringMalformed); }
      AARRAY_safety(,if(!num2) AARRAY_Error_FormatStringMalformed);
      start = ptr;
      break; }
    ptr++; }
  /* return parser values */
  *pspecifier = specifier; *pnum1 = num1; *pnum2 = num2;
  *pend = end; *pstart = start; *pptr = ptr; })

static const char AARRAY_baseChars[] =
  "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._";
AARRAY_define(void AARRAY_u_to_str(
    uint64_t value,
    /* can contain INTMAX_MAX in binary */
    char str[(8 * sizeof(int64_t))+1], int base), {
  char*walkstr=str;
  /* validate base */
  if(base<2 || base>64) base = 10;
  /* reverse number */
  do *walkstr++ = AARRAY_baseChars[value%(unsigned int)base]; while(value/=(unsigned int)base);
  *walkstr='\0';
  char aux; while(--walkstr>str) aux=*walkstr, *walkstr=*str, *str++=aux; })
AARRAY_define(void AARRAY_i_to_str(
    int64_t value,
    char str[(8 * sizeof(int64_t))+1], int base), {
  char*walkstr=str;
  /* validate base */
  if(base<2 || base>64) base = 10;
  uint8_t sign = value < 0;
  if(sign) value = -value;
  /* reverse number */
  do *walkstr++ = AARRAY_baseChars[(uint64_t)value%(unsigned int)base];
  while((value=(int64_t)((uint64_t)value/(unsigned int)base)));
  if(sign) *walkstr++ = '-';
  else *walkstr++ = '+';
  *walkstr='\0';
  char aux; while(--walkstr>str) aux=*walkstr, *walkstr=*str, *str++=aux; })

/* cppp let Fmt be defined once, but output to both streams and arrays */
AARRAY_define(int AARRAY_Fmt_File(char errLoc[],
                                     FILE* fmtOut,
                                     size_t vecCount, uint64_t vec[]), {
  AARRAY_safety(, if(!fmtOut) AARRAY_Error_NullParameter);
  size_t ptr= 0, prevStart = 0, start = 0, end = 0, vc = 0;
  char specifier = 0; size_t doublePercent;
	/* defined to avoid static-analysis false positive */
  int num1 = 0, num2 = 0;
  if(!vecCount) return 0;
  char*fmt = (char*)vec[vc++];
  AARRAY_safety(, if(!fmt) AARRAY_Error_NullParameter);
  /* big enough for binary INTMAX_MAX */
  char buffer[(8 * sizeof(intmax_t))+1];
  while(1) {
    prevStart = start;
    AARRAY_percent_parse(errLoc, fmt, &ptr,
                         &specifier, &num1, &num2, &start, &end,
                         &doublePercent);
    if(end-prevStart < fwrite(fmt+prevStart, sizeof(char),
                                        end-prevStart, fmtOut)) return -1;
    switch(specifier) {
    case 'e': 
      AARRAY_safety(, );
      return 0;
     case '%': {
 if(EOF==fputc('%', fmtOut)) return -1;
 break; }
 case 's': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_safety(, if(!vec[vc]) AARRAY_Error_NullParameter)
 if(EOF==fputs((char*)vec[vc], fmtOut)) return -1;
 break; }
 case 'c': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 if(EOF==fputc((char)vec[vc], fmtOut)) return -1;
 break; }
 case 'u': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_u_to_str((num2==8? (uint8_t)vec[vc] :
                  (num2==16? (uint16_t)vec[vc] : (num2==32? (uint32_t)vec[vc] :(uint64_t)vec[vc]))),
                  buffer, num1);
 if(EOF==fputs(buffer, fmtOut)) return -1;
 break; }
 case 'i': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_i_to_str((num2==8? (int8_t)(uint8_t)vec[vc] :
                  (num2==16? (int16_t)(uint16_t)vec[vc] :
                   (num2==32? (int32_t)(uint32_t)vec[vc] : (int64_t)(uint64_t)vec[vc]))),
                  buffer, num1);
 if(EOF==fputs(buffer, fmtOut)) return -1;
 break; } 
 case 'f': case 'd': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 if(EOF==fprintf(fmtOut, "%g",
                           specifier=='f'?(double)*(float*)&vec[vc]:*(double*)&vec[vc])) return -1;
 break; }
    case 'v':
      AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
      prevStart = end+2;
      doublePercent = 0;
      do {
        AARRAY_percent_parse(errLoc, fmt, &ptr,
                             &specifier, &num1, &num2, &start, &end,
                             &doublePercent); }
      while(specifier == '%');
      AARRAY_safety(, if(specifier=='e' || specifier=='v')
          AARRAY_Error_FormatStringMalformed);
      size_t nn = (size_t)-1; while(++nn < aLength((uintptr_t*)(vec[vc]))) {
        if(nn) {
          /* resolving %% to % is a pain, since we now have to */
          /* loop through %v's separator again, doing the conversion */
          if(doublePercent) {
            size_t ptr2 = prevStart; while(ptr2 < doublePercent)
              switch(fmt[ptr2]) {
              case '%': ptr2+=2; fputc('%', fmtOut); break;
              default: fputc(fmt[ptr2++], fmtOut); }
            prevStart = doublePercent; }
          if(end-prevStart <fwrite(fmt+prevStart, sizeof(char),
                                             end-prevStart, fmtOut)) return -1; }
        switch(specifier) {
         case '%': {
 if(EOF==fputc('%', fmtOut)) return -1;
 break; }
 case 's': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_safety(, if(!vec[vc]) AARRAY_Error_NullParameter)
 if(EOF==fputs(((char**)vec[vc])[nn], fmtOut)) return -1;
 break; }
 case 'c': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 if(EOF==fputc(((char*)vec[vc])[nn], fmtOut)) return -1;
 break; }
 case 'u': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_u_to_str((num2==8? ((uint8_t*)vec[vc])[nn] :
                  (num2==16? ((uint16_t*)vec[vc])[nn] : (num2==32? ((uint32_t*)vec[vc])[nn] :((uint64_t*)vec[vc])[nn]))),
                  buffer, num1);
 if(EOF==fputs(buffer, fmtOut)) return -1;
 break; }
 case 'i': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_i_to_str((num2==8? (int8_t)((uint8_t*)vec[vc])[nn] :
                  (num2==16? (int16_t)((uint16_t*)vec[vc])[nn] :
                   (num2==32? (int32_t)((uint32_t*)vec[vc])[nn] : (int64_t)((uint64_t*)vec[vc])[nn]))),
                  buffer, num1);
 if(EOF==fputs(buffer, fmtOut)) return -1;
 break; } 
 case 'f': case 'd': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 if(EOF==fprintf(fmtOut, "%g",
                           specifier=='f'?(double)((float*)vec[vc])[nn]:((double*)vec[vc])[nn])) return -1;
 break; }; } }
      break; }
    vc++;
    specifier = 0; }
  return 0; })
AARRAY_define(int AARRAY_Fmt_Array(char errLoc[],
                                     char** fmtOut,
                                     size_t vecCount, uint64_t vec[]), {
  AARRAY_safety(, if(!fmtOut) AARRAY_Error_NullParameter);
  size_t ptr= 0, prevStart = 0, start = 0, end = 0, vc = 0;
  char specifier = 0; size_t doublePercent;
	/* defined to avoid static-analysis false positive */
  int num1 = 0, num2 = 0;
  if(!vecCount) return 0;
  char*fmt = (char*)vec[vc++];
  AARRAY_safety(, if(!fmt) AARRAY_Error_NullParameter);
  /* big enough for binary INTMAX_MAX */
  char buffer[(8 * sizeof(intmax_t))+1];
  while(1) {
    prevStart = start;
    AARRAY_percent_parse(errLoc, fmt, &ptr,
                         &specifier, &num1, &num2, &start, &end,
                         &doublePercent);
    (void)aAppendArray(fmtOut, end-prevStart,
                                 (uintptr_t)AARRAY_move(&fmt[prevStart]));
    switch(specifier) {
    case 'e': 
      AARRAY_safety(, (void)aAppend(fmtOut, 'X'); (void)aZLength2(*fmtOut, 1));
      return 0;
     case '%': {
 (void)aAppend(fmtOut, '%');
 break; }
 case 's': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_safety(, if(!vec[vc]) AARRAY_Error_NullParameter)
 (void)aAppendArray(fmtOut, SIZE_MAX,
                              (uintptr_t)AARRAY_move((char*)vec[vc]));
 break; }
 case 'c': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 (void)aAppend(fmtOut, (uint8_t)(char)vec[vc]);
 break; }
 case 'u': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_u_to_str((num2==8? (uint8_t)vec[vc] :
                  (num2==16? (uint16_t)vec[vc] : (num2==32? (uint32_t)vec[vc] :(uint64_t)vec[vc]))),
                  buffer, num1);
 (void)aAppendArray(fmtOut, SIZE_MAX,
                              (uintptr_t)AARRAY_move(buffer));
 break; }
 case 'i': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_i_to_str((num2==8? (int8_t)(uint8_t)vec[vc] :
                  (num2==16? (int16_t)(uint16_t)vec[vc] :
                   (num2==32? (int32_t)(uint32_t)vec[vc] : (int64_t)(uint64_t)vec[vc]))),
                  buffer, num1);
 (void)aAppendArray(fmtOut, SIZE_MAX,
                              (uintptr_t)AARRAY_move(buffer));
 break; } 
 case 'f': case 'd': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 int len = snprintf(*fmtOut, 0, "%g",
                           specifier=='f'?(double)*(float*)&vec[vc]:*(double*)&vec[vc]);
           /* safe to assume no errors from snprintf */
           size_t oldlen = aLength(*fmtOut);
           (void)aMulti(fmtOut, oldlen, 0, 0, (uintptr_t)len+1, 0);
           snprintf(&(*fmtOut)[oldlen], (size_t)len+1, "%g",
                           specifier=='f'?(double)*(float*)&vec[vc]:*(double*)&vec[vc]);
           (void)aZLength2(*fmtOut, 1);
 break; }
    case 'v':
      AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
      prevStart = end+2;
      doublePercent = 0;
      do {
        AARRAY_percent_parse(errLoc, fmt, &ptr,
                             &specifier, &num1, &num2, &start, &end,
                             &doublePercent); }
      while(specifier == '%');
      AARRAY_safety(, if(specifier=='e' || specifier=='v')
          AARRAY_Error_FormatStringMalformed);
      size_t nn = (size_t)-1; while(++nn < aLength((uintptr_t*)(vec[vc]))) {
        if(nn) {
          /* resolving %% to % is a pain, since we now have to */
          /* loop through %v's separator again, doing the conversion */
          if(doublePercent) {
            size_t ptr2 = prevStart; while(ptr2 < doublePercent)
              switch(fmt[ptr2]) {
              case '%': ptr2+=2; (void)aAppend(fmtOut, '%'); break;
              default: (void)aAppend(fmtOut, (uint8_t)fmt[ptr2++]); }
            prevStart = doublePercent; }
          (void)aAppendArray(fmtOut, end-prevStart,
                                 (uintptr_t)AARRAY_move(&fmt[prevStart])); }
        switch(specifier) {
         case '%': {
 (void)aAppend(fmtOut, '%');
 break; }
 case 's': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_safety(, if(!vec[vc]) AARRAY_Error_NullParameter)
 (void)aAppendArray(fmtOut, SIZE_MAX,
                              (uintptr_t)AARRAY_move(((char**)vec[vc])[nn]));
 break; }
 case 'c': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 (void)aAppend(fmtOut, (uint8_t)((char*)vec[vc])[nn]);
 break; }
 case 'u': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_u_to_str((num2==8? ((uint8_t*)vec[vc])[nn] :
                  (num2==16? ((uint16_t*)vec[vc])[nn] : (num2==32? ((uint32_t*)vec[vc])[nn] :((uint64_t*)vec[vc])[nn]))),
                  buffer, num1);
 (void)aAppendArray(fmtOut, SIZE_MAX,
                              (uintptr_t)AARRAY_move(buffer));
 break; }
 case 'i': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 AARRAY_i_to_str((num2==8? (int8_t)((uint8_t*)vec[vc])[nn] :
                  (num2==16? (int16_t)((uint16_t*)vec[vc])[nn] :
                   (num2==32? (int32_t)((uint32_t*)vec[vc])[nn] : (int64_t)((uint64_t*)vec[vc])[nn]))),
                  buffer, num1);
 (void)aAppendArray(fmtOut, SIZE_MAX,
                              (uintptr_t)AARRAY_move(buffer));
 break; } 
 case 'f': case 'd': {
 AARRAY_safety(, if(vc>=vecCount) AARRAY_Error_FormatStringArgs);
 int len = snprintf(*fmtOut, 0, "%g",
                           specifier=='f'?(double)((float*)vec[vc])[nn]:((double*)vec[vc])[nn]);
           /* safe to assume no errors from snprintf */
           size_t oldlen = aLength(*fmtOut);
           (void)aMulti(fmtOut, oldlen, 0, 0, (uintptr_t)len+1, 0);
           snprintf(&(*fmtOut)[oldlen], (size_t)len+1, "%g",
                           specifier=='f'?(double)((float*)vec[vc])[nn]:((double*)vec[vc])[nn]);
           (void)aZLength2(*fmtOut, 1);
 break; }; } }
      break; }
    vc++;
    specifier = 0; }
  return 0; })

AARRAY_define(int AARRAY_Fmt_Error(
    char errLoc[], size_t vecCount, uint64_t vec[]), {
  fflush(stdout);
  int returnValue = AARRAY_Fmt_File(errLoc, stderr, vecCount, vec);
  fflush(stderr);
  return returnValue; })

#define aFmt(...)         AARRAY_Fmt_File (AARRAY_LINE, stdout, \
                                           AARRAY_nowarn_internal_start \
sizeof((uint64_t[]){(uint64_t)__VA_ARGS__}) / sizeof(uint64_t), \
  (uint64_t*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end)
#define aFmtE(...)        AARRAY_Fmt_Error(AARRAY_LINE, \
                                           AARRAY_nowarn_internal_start \
sizeof((uint64_t[]){(uint64_t)__VA_ARGS__}) / sizeof(uint64_t), \
  (uint64_t*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end)
#define aFmtF(file, ...)  AARRAY_Fmt_File (AARRAY_LINE, file, \
                                           AARRAY_nowarn_internal_start \
sizeof((uint64_t[]){(uint64_t)__VA_ARGS__}) / sizeof(uint64_t), \
  (uint64_t*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end)
#define aFmtA(array, ...) AARRAY_Fmt_Array(AARRAY_LINE, array, \
                                           AARRAY_nowarn_internal_start \
sizeof((uint64_t[]){(uint64_t)__VA_ARGS__}) / sizeof(uint64_t), \
  (uint64_t*)AARRAY_move((uint64_t[]){(uint64_t)(uintptr_t)__VA_ARGS__}) \
  AARRAY_nowarn_internal_end)




#if !defined(AARRAY_NOCONVENIENCE)
  #define aA   aAppend
  #define aR   aReplace
  #define aAA  aAppendArray
  #define aRA  aReplaceArray
  #define aD   aDelete
  #define aC   aConcat

  #define aL   aLength
  #define aL2  aLength2
  #define aZL2 aZLength2

  #define aF   aFmt
  #define aFE  aFmtE
  #define aFF  aFmtF
  #define aFA  aFmtA
	#define aW   aWrite
  #define aWE  aWriteE
  #define aWF  aWriteF

  #define aStr(string) aAppendArray((char**)NULL, SIZE_MAX, (uintptr_t)string)
#endif

#if defined(__cplusplus)
  AARRAY_nowarn_pedantic_cpp_end
#endif

#endif
