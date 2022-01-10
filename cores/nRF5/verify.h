#ifndef VERIFY_H
#define VERIFY_H

#include <stdint.h>
#include <stddef.h>

// Verify stuff:

#define VERIFY_STATIC(const_expr) _Static_assert(const_expr, "Assert failed")

// namespace and templates must be outside the `extern "C"` declaration...
namespace ADAFRUIT_DETAIL
{
    template <typename T, size_t N>
    constexpr size_t arrcount_fails_if_not_array(T const (&)[N]) noexcept
    {
        return N;
    }
}
#define arrcount(arr) ADAFRUIT_DETAIL::arrcount_fails_if_not_array(arr)
#define ADA_STATIC_ASSERT(const_expr, message)   static_assert(const_expr, message)

#define VERIFY_MESS(_status, _functstr) VERIFY_MESS_impl(_status, _functstr, __PRETTY_FUNCTION__, __LINE__)


#define VERIFY_ERR_DEF(_status, _ret, _funcstr) \
    if ( 0 != _status ) {                       \
      VERIFY_MESS(_status, _funcstr);           \
      return _ret;                              \
    }

// Helper to implement optional parameter for VERIFY Macro family
#define _GET_3RD_ARG(arg1, arg2, arg3, ...)  arg3

/*------------------------------------------------------------------*/
/* VERIFY STATUS
 * - VERIFY_ERR_1ARGS : return status of condition if failed
 * - VERIFY_ERR_2ARGS : return provided status code if failed
 *------------------------------------------------------------------*/
#define VERIFY_ERR_1ARGS(_exp, _funcstr)          \
    do {                                          \
      int32_t _status = (int32_t)(_exp);          \
      VERIFY_ERR_DEF(_status, _status, _funcstr); \
    } while(0)

#define VERIFY_ERR_2ARGS(_exp, _ret, _funcstr)    \
    do {                                          \
      int32_t _status = (int32_t)(_exp);          \
      VERIFY_ERR_DEF(_status, _ret, _funcstr);    \
    } while(0)


#define PRINT_STATUS(_exp) do                            \
{                                                        \
  int32_t _status = (int32_t) _exp;                      \
  if ( 0 != _status ) VERIFY_MESS(_status, dbg_err_str); \
} while(0)                                               \

#define VERIFY_ERROR(...)       _GET_3RD_ARG(__VA_ARGS__, VERIFY_ERR_2ARGS, VERIFY_ERR_1ARGS)(__VA_ARGS__, NULL)

/*------------------------------------------------------------------*/
/* VERIFY
 * - VERIFY_1ARGS : return condition if failed (false)
 * - VERIFY_2ARGS : return provided value if failed
 *------------------------------------------------------------------*/
#define VERIFY_1ARGS(cond)            if (!(cond)) return false;
#define VERIFY_2ARGS(cond, _error)    if (!(cond)) return _error;

/**
 * Check if condition is success (true), otherwise return
 * - false value if called with 1 parameter e.g VERIFY(condition)
 * - 2 parameter if called with 2 parameters e.g VERIFY(condition, errorcode)
 */
#define VERIFY(...)  _GET_3RD_ARG(__VA_ARGS__, VERIFY_2ARGS, VERIFY_1ARGS)(__VA_ARGS__)

void VERIFY_MESS_impl(int32_t _status, const char* (*_fstr)(int32_t), const char* func_name, int line_number);

#endif
