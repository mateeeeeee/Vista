#pragma once
#include <cassert>

#define _VISTA_STRINGIFY_IMPL(a) #a
#define _VISTA_CONCAT_IMPL(x, y) x##y

#define VISTA_STRINGIFY(a) _VISTA_STRINGIFY_IMPL(a)
#define VISTA_CONCAT(x, y) _VISTA_CONCAT_IMPL(x, y)

#define VISTA_TODO(...)    
#define VISTA_HACK(stmt, msg)            stmt
#define VISTA_ASSERT(expr)			    assert(expr)
#define VISTA_ASSERT_MSG(expr, msg)      assert(expr && msg)
#define VISTA_DEBUGBREAK()			    __debugbreak()
#define VISTA_UNREACHABLE()		        __assume(false)
#define VISTA_FORCEINLINE			    __forceinline
#define VISTA_NOINLINE			        __declspec(noinline)
#define VISTA_NODISCARD			        [[nodiscard]]
#define VISTA_NORETURN				    [[noreturn]]
#define VISTA_DEPRECATED			    [[deprecated]]
#define VISTA_MAYBE_UNUSED              [[maybe_unused]]
#define VISTA_DEPRECATED_MSG(msg)	    [[deprecated(#msg)]]
#define VISTA_DEBUGZONE_BEGIN            __pragma(optimize("", off))
#define VISTA_DEBUGZONE_END              __pragma(optimize("", on))

#define VISTA_NONCOPYABLE(Class)                 \
        Class(Class const&)            = delete; \
        Class& operator=(Class const&) = delete;

#define VISTA_NONMOVABLE(Class)                      \
        Class(Class&&) noexcept            = delete; \
        Class& operator=(Class&&) noexcept = delete;

#define VISTA_NONCOPYABLE_NONMOVABLE(Class) \
        VISTA_NONCOPYABLE(Class)            \
        VISTA_NONMOVABLE(Class)

#define VISTA_DEFAULT_COPYABLE(Class)             \
        Class(Class const&)            = default; \
        Class& operator=(Class const&) = default;

#define VISTA_DEFAULT_MOVABLE(Class)                  \
        Class(Class&&) noexcept            = default; \
        Class& operator=(Class&&) noexcept = default;

#define VISTA_DEFAULT_COPYABLE_MOVABLE(Class) \
        VISTA_DEFAULT_COPYABLE(Class)         \
        VISTA_DEFAULT_MOVABLE(Class)