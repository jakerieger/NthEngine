/*
 *  Filename: Macros.hpp
 *  This code is part of the Nth Engine core library
 *  Copyright 2025 Jake Rieger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>

/* ============================================================================
 * GENERAL PURPOSE MACROS
 * ============================================================================ */

/* Array size calculation */
#define N_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Min/Max macros with type safety */
#define N_MIN(a, b)                                                                                                    \
    ({                                                                                                                 \
        __typeof__(a) _a = (a);                                                                                        \
        __typeof__(b) _b = (b);                                                                                        \
        _a < _b ? _a : _b;                                                                                             \
    })

#define N_MAX(a, b)                                                                                                    \
    ({                                                                                                                 \
        __typeof__(a) _a = (a);                                                                                        \
        __typeof__(b) _b = (b);                                                                                        \
        _a > _b ? _a : _b;                                                                                             \
    })

/* Clamp a value between min and max */
#define N_CLAMP(x, min, max) N_MAX((min), N_MIN((x), (max)))

/* Swap two values */
#define N_SWAP(a, b)                                                                                                   \
    do {                                                                                                               \
        __typeof__(a) _tmp = (a);                                                                                      \
        (a)                = (b);                                                                                      \
        (b)                = _tmp;                                                                                     \
    } while (0)

/* Absolute value */
#define N_ABS(x)                                                                                                       \
    ({                                                                                                                 \
        __typeof__(x) _x = (x);                                                                                        \
        _x < 0 ? -_x : _x;                                                                                             \
    })

/* Check if number is power of 2 */
#define N_IS_POW2(x) (((x) != 0) && (((x) & ((x)-1)) == 0))

/* Align value up to nearest power of 2 boundary */
#define N_ALIGN_UP(x, align) (((x) + ((align)-1)) & ~((align)-1))

/* Align value down to nearest power of 2 boundary */
#define N_ALIGN_DOWN(x, align) ((x) & ~((align)-1))

/* Get offset of a field in a struct */
#define N_OFFSET_OF(type, member) offsetof(type, member)

/* Get container struct from member pointer */
#define N_CONTAINER_OF(ptr, type, member)                                                                              \
    ({                                                                                                                 \
        const __typeof__(((type*)0)->member)* _mptr = (ptr);                                                           \
        (type*)((char*)_mptr - N_OFFSET_OF(type, member));                                                             \
    })

/* Bit manipulation */
#define N_BIT(n) (1U << (n))
#define N_BIT_SET(x, n) ((x) |= N_BIT(n))
#define N_BIT_CLEAR(x, n) ((x) &= ~N_BIT(n))
#define N_BIT_TOGGLE(x, n) ((x) ^= N_BIT(n))
#define N_BIT_CHECK(x, n) (((x) & N_BIT(n)) != 0)

/* Create bitmask */
#define N_BITMASK(nbits) ((1U << (nbits)) - 1)

/* Safe boolean conversion */
#define N_BOOL(x) (!!(x))

/* Stringify */
#define N_STRINGIFY_IMPL(x) #x
#define N_STRINGIFY(x) N_STRINGIFY_IMPL(x)

/* Concatenate */
#define N_CONCAT_IMPL(a, b) a##b
#define N_CONCAT(a, b) N_CONCAT_IMPL(a, b)

/* Unused variable/parameter marker */
#define N_UNUSED(x) ((void)(x))

/* Static assertion with message */
#define N_STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)

/* Likely/Unlikely branch prediction hints */
#if defined(__GNUC__) || defined(__clang__)
    #define N_LIKELY(x) __builtin_expect(!!(x), 1)
    #define N_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define N_LIKELY(x) (x)
    #define N_UNLIKELY(x) (x)
#endif

/* Force inline */
#if defined(__GNUC__) || defined(__clang__)
    #define N_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
    #define N_FORCE_INLINE __forceinline
#else
    #define N_FORCE_INLINE inline
#endif

/* No inline */
#if defined(__GNUC__) || defined(__clang__)
    #define N_NO_INLINE __attribute__((noinline))
#elif defined(_MSC_VER)
    #define N_NO_INLINE __declspec(noinline)
#else
    #define N_NO_INLINE
#endif

/* ============================================================================
 * MEMORY MANAGEMENT MACROS (SAFETY FOCUSED)
 * ============================================================================ */

/* Safe malloc with type */
#define N_MALLOC(type, count) ((type*)malloc(sizeof(type) * (count)))

/* Safe calloc with type (zeroed memory) */
#define N_CALLOC(type, count) ((type*)calloc((count), sizeof(type)))

/* Safe realloc with type */
#define N_REALLOC(ptr, type, count) ((type*)realloc((ptr), sizeof(type) * (count)))

/* Allocate single object */
#define N_NEW(type) N_CALLOC(type, 1)

/* Safe free that nullifies pointer */
#define N_FREE(ptr)                                                                                                    \
    do {                                                                                                               \
        free(ptr);                                                                                                     \
        (ptr) = NULL;                                                                                                  \
    } while (0)

/* Safe free with type check */
#define N_DELETE(ptr)                                                                                                  \
    do {                                                                                                               \
        if ((ptr) != NULL) {                                                                                           \
            free(ptr);                                                                                                 \
            (ptr) = NULL;                                                                                              \
        }                                                                                                              \
    } while (0)

/* Zero memory */
#define N_ZERO(ptr, type) memset((ptr), 0, sizeof(type))

/* Zero array */
#define N_ZERO_ARRAY(arr) memset((arr), 0, sizeof(arr))

/* Zero struct by pointer */
#define N_ZERO_STRUCT(ptr) memset((ptr), 0, sizeof(*(ptr)))

/* Safe memcpy with bounds checking */
#define N_MEMCPY_SAFE(dest, src, dest_size, src_size)                                                                  \
    do {                                                                                                               \
        size_t _copy_size = N_MIN((dest_size), (src_size));                                                            \
        memcpy((dest), (src), _copy_size);                                                                             \
    } while (0)

/* Duplicate memory */
#define N_MEMDUP(src, size)                                                                                            \
    ({                                                                                                                 \
        void* _new = malloc(size);                                                                                     \
        if (_new != NULL) memcpy(_new, (src), (size));                                                                 \
        _new;                                                                                                          \
    })

/* Check if pointer is aligned */
#define N_IS_ALIGNED(ptr, alignment) ((((uintptr_t)(ptr)) & ((alignment)-1)) == 0)

/* Safe pointer arithmetic */
#define N_PTR_ADD(ptr, offset) ((void*)((char*)(ptr) + (offset)))
#define N_PTR_SUB(ptr, offset) ((void*)((char*)(ptr) - (offset)))
#define N_PTR_DIFF(ptr1, ptr2) ((size_t)((char*)(ptr1) - (char*)(ptr2)))

/* ============================================================================
 * GRAPHICS & GAME SPECIFIC MACROS
 * ============================================================================ */

/* Color manipulation (RGBA) */
#define N_COLOR_RGBA(r, g, b, a)                                                                                       \
    ((uint32_t)(((uint8_t)(r)) | ((uint8_t)(g) << 8) | ((uint8_t)(b) << 16) | ((uint8_t)(a) << 24)))

#define N_COLOR_RGB(r, g, b) N_COLOR_RGBA(r, g, b, 255)

#define N_COLOR_GET_R(color) ((uint8_t)((color) & 0xFF))
#define N_COLOR_GET_G(color) ((uint8_t)(((color) >> 8) & 0xFF))
#define N_COLOR_GET_B(color) ((uint8_t)(((color) >> 16) & 0xFF))
#define N_COLOR_GET_A(color) ((uint8_t)(((color) >> 24) & 0xFF))

/* Convert float color (0.0-1.0) to byte (0-255) */
#define N_COLOR_F2B(f) ((uint8_t)N_CLAMP((f) * 255.0f, 0.0f, 255.0f))

/* Convert byte color (0-255) to float (0.0-1.0) */
#define N_COLOR_B2F(b) ((float)(b) / 255.0f)

/* Rectangle operations */
#define N_RECT_CONTAINS(rect_x, rect_y, rect_w, rect_h, px, py)                                                        \
    (((px) >= (rect_x)) && ((px) < (rect_x) + (rect_w)) && ((py) >= (rect_y)) && ((py) < (rect_y) + (rect_h)))

/* Check if two rectangles intersect */
#define N_RECT_INTERSECTS(x1, y1, w1, h1, x2, y2, w2, h2)                                                              \
    (!((x1) + (w1) <= (x2) || (x2) + (w2) <= (x1) || (y1) + (h1) <= (y2) || (y2) + (h2) <= (y1)))

/* Degrees to radians */
#define N_DEG2RAD(deg) ((deg) * 0.017453292519943295f)

/* Radians to degrees */
#define N_RAD2DEG(rad) ((rad) * 57.29577951308232f)

/* Linear interpolation */
#define N_LERP(a, b, t) ((a) + ((b) - (a)) * (t))

/* Inverse lerp (find t given a, b, and value) */
#define N_INV_LERP(a, b, v) (((v) - (a)) / ((b) - (a)))

/* Remap value from one range to another */
#define N_REMAP(val, in_min, in_max, out_min, out_max)                                                                 \
    N_LERP((out_min), (out_max), N_INV_LERP((in_min), (in_max), (val)))

/* Smoothstep interpolation */
#define N_SMOOTHSTEP(t) ((t) * (t) * (3.0f - 2.0f * (t)))

/* Check if float values are approximately equal */
#define N_FLOAT_EQ(a, b, epsilon) (N_ABS((a) - (b)) < (epsilon))

/* Default epsilon for float comparison */
#define N_EPSILON 1e-6f

/* Normalize value to 0.0-1.0 range */
#define N_NORMALIZE(val, min, max) N_CLAMP(((val) - (min)) / ((max) - (min)), 0.0f, 1.0f)

/* Convert normalized value back to range */
#define N_DENORMALIZE(val, min, max) ((val) * ((max) - (min)) + (min))

/* 2D Vector operations (requires vec2 struct with x, y) */
#define N_VEC2_DOT(v1, v2) ((v1).x * (v2).x + (v1).y * (v2).y)
#define N_VEC2_LENGTH_SQ(v) ((v).x * (v).x + (v).y * (v).y)

/* 3D Vector operations (requires vec3 struct with x, y, z) */
#define N_VEC3_DOT(v1, v2) ((v1).x * (v2).x + (v1).y * (v2).y + (v1).z * (v2).z)
#define N_VEC3_LENGTH_SQ(v) ((v).x * (v).x + (v).y * (v).y + (v).z * (v).z)

/* ============================================================================
 * ASSET & RESOURCE MANAGEMENT
 * ============================================================================ */

/* Safe string duplication */
#define N_STRDUP_SAFE(str)                                                                                             \
    ({                                                                                                                 \
        const char* _s = (str);                                                                                        \
        char* _dup     = NULL;                                                                                         \
        if (_s != NULL) {                                                                                              \
            size_t _len = strlen(_s) + 1;                                                                              \
            _dup        = (char*)malloc(_len);                                                                         \
            if (_dup != NULL) memcpy(_dup, _s, _len);                                                                  \
        }                                                                                                              \
        _dup;                                                                                                          \
    })

/* Safe string copy with null termination guarantee */
#define N_STRCPY_SAFE(dest, dest_size, src)                                                                            \
    do {                                                                                                               \
        if ((dest) != NULL && (src) != NULL && (dest_size) > 0) {                                                      \
            strncpy((dest), (src), (dest_size)-1);                                                                     \
            (dest)[(dest_size)-1] = '\0';                                                                              \
        }                                                                                                              \
    } while (0)

/* String comparison shortcuts */
#define N_STREQ(s1, s2) (strcmp((s1), (s2)) == 0)
#define N_STRNEQ(s1, s2, n) (strncmp((s1), (s2), (n)) == 0)

/* ============================================================================
 * DEBUG & LOGGING MACROS
 * ============================================================================ */

#ifndef NDEBUG
    #define N_DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

    #define N_ASSERT(cond)                                                                                             \
        do {                                                                                                           \
            if (N_UNLIKELY(!(cond))) {                                                                                 \
                fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #cond, __FILE__, __LINE__);                \
                abort();                                                                                               \
            }                                                                                                          \
        } while (0)

    #define N_ASSERT_MSG(cond, msg)                                                                                    \
        do {                                                                                                           \
            if (N_UNLIKELY(!(cond))) {                                                                                 \
                fprintf(stderr, "Assertion failed: %s (%s), file %s, line %d\n", #cond, msg, __FILE__, __LINE__);      \
                abort();                                                                                               \
            }                                                                                                          \
        } while (0)
#else
    #define N_DEBUG_PRINT(fmt, ...) ((void)0)
    #define N_ASSERT(cond) ((void)0)
    #define N_ASSERT_MSG(cond, msg) ((void)0)
#endif

#define N_PRINT_ERROR(fmt, ...) fprintf(stderr, "[ERROR] %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

/* Runtime error checking (always active) */
#define N_CHECK(cond)                                                                                                  \
    do {                                                                                                               \
        if (N_UNLIKELY(!(cond))) {                                                                                     \
            fprintf(stderr, "Check failed: %s, file %s, line %d\n", #cond, __FILE__, __LINE__);                        \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

#define N_CHECK_MSG(cond, msg)                                                                                         \
    do {                                                                                                               \
        if (N_UNLIKELY(!(cond))) {                                                                                     \
            fprintf(stderr, "Check failed: %s (%s), file %s, line %d\n", #cond, msg, __FILE__, __LINE__);              \
            abort();                                                                                                   \
        }                                                                                                              \
    } while (0)

/* Verify allocation succeeded */
#define N_CHECK_ALLOC(ptr) N_CHECK_MSG((ptr) != NULL, "Memory allocation failed")

/* ============================================================================
 * PERFORMANCE & OPTIMIZATION
 * ============================================================================ */

/* Prefetch memory (compiler-specific) */
#if defined(__GNUC__) || defined(__clang__)
    #define N_PREFETCH(addr) __builtin_prefetch(addr)
    #define N_PREFETCH_WRITE(addr) __builtin_prefetch(addr, 1)
#else
    #define N_PREFETCH(addr) ((void)0)
    #define N_PREFETCH_WRITE(addr) ((void)0)
#endif

/* Restrict pointer keyword */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define N_RESTRICT restrict
#elif defined(__GNUC__) || defined(__clang__)
    #define N_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define N_RESTRICT __restrict
#else
    #define N_RESTRICT
#endif

/* Function attributes */
#if defined(__GNUC__) || defined(__clang__)
    #define N_PURE __attribute__((pure))
    #define N_CONST __attribute__((const))
    #define N_HOT __attribute__((hot))
    #define N_COLD __attribute__((cold))
#else
    #define N_PURE
    #define N_CONST
    #define N_HOT
    #define N_COLD
#endif

/* ============================================================================
 * FOREACH LOOP HELPERS
 * ============================================================================ */

/* Iterate over array with automatic size detection */
#define N_FOREACH(type, var, array) for (type* var = (array); var < (array) + N_ARRAY_SIZE(array); ++var)

/* Iterate with index */
#define N_FOREACH_I(type, var, array, index_var)                                                                       \
    for (size_t index_var = 0, _keep = 1; _keep && index_var < N_ARRAY_SIZE(array); _keep = !_keep, ++index_var)       \
        for (type* var = &(array)[index_var]; _keep; _keep = !_keep)

/* ============================================================================
 * TIMER MACROS
 * ============================================================================ */

/* Simple frame time calculation helper */
#define N_DELTA_TIME(last_time, current_time) ((float)((current_time) - (last_time)))

/* Convert seconds to milliseconds */
#define N_SEC_TO_MS(sec) ((sec) * 1000.0f)

/* Convert milliseconds to seconds */
#define N_MS_TO_SEC(ms) ((ms) / 1000.0f)

/* FPS calculation from delta time */
#define N_FPS_FROM_DT(dt) (1.0f / (dt))

/// @brief Deletes both the move/copy assignment operator and constructor
#define N_CLASS_PREVENT_MOVES_COPIES(CLASS_NAME)                                                                       \
    CLASS_NAME(const CLASS_NAME&)            = delete;                                                                 \
    CLASS_NAME(CLASS_NAME&&)                 = delete;                                                                 \
    CLASS_NAME& operator=(const CLASS_NAME&) = delete;                                                                 \
    CLASS_NAME& operator=(CLASS_NAME&&)      = delete;

/// @brief Deletes the copy constructor and assignment operator
#define N_CLASS_PREVENT_COPIES(CLASS_NAME)                                                                             \
    CLASS_NAME(const CLASS_NAME&)            = delete;                                                                 \
    CLASS_NAME& operator=(const CLASS_NAME&) = delete;

/// @brief Deletes the move constructor and assignment operator
#define N_CLASS_PREVENT_MOVES(CLASS_NAME)                                                                              \
    CLASS_NAME(CLASS_NAME&&)            = delete;                                                                      \
    CLASS_NAME& operator=(CLASS_NAME&&) = delete;

#define N_ND [[nodiscard]]

#define N_DO_NOTHING ((void)0)