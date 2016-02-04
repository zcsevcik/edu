#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN             1
# define WINVER                          0x0501 //_WIN32_WINNT_WINXP
# define _WIN32_WINNT                    0x0501 //_WIN32_WINNT_WINXP
# define NTDDI_VERSION                   0x05010300 //NTDDI_WINXPSP3
# define _WIN32_IE                       0x0603 //_WIN32_IE_IE60SP2
#endif

#define _GNU_SOURCE 1

#ifdef __MINGW32__
# define WINSOCK_VERSION                MAKEWORD(2,2)
#endif

#ifndef _MSC_VER
# define _countof(a)                    (sizeof(a)/sizeof(*(a)))
#endif

#ifndef max
# define max(a,b)                       (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
# define min(a,b)                       (((a) < (b)) ? (a) : (b))
#endif

#ifdef __MINGW32__
# ifndef AI_NUMERICSERV
#  define AI_NUMERICSERV                0x00000008
# endif
  
# ifndef AI_ADDRCONFIG
#  define AI_ADDRCONFIG                 0x00000400
# endif
#endif

#if defined(_WIN32) && (_WIN32_WINNT < _WIN32_WINNT_WS03 || !defined(MSG_WAITALL))
# define MSG_WAITALL                    0x8
#endif


#if !defined(_WIN32) && defined(__GNUC__)
# define HAVE_VASPRINTF                  1
#endif

/* Define as a marker that can be attached to declarations that might not
    be used.  This helps to reduce warnings, such as from
    GCC -Wunused-parameter.  */
#if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
# define _GL_UNUSED __attribute__ ((__unused__))
#else
# define _GL_UNUSED
#endif
/* The name _UNUSED_PARAMETER_ is an earlier spelling, although the name
   is a misnomer outside of parameter lists.  */
#define _UNUSED_PARAMETER_ _GL_UNUSED

/* The __const__ attribute was added in gcc 2.95.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
# define _GL_ATTRIBUTE_CONST __attribute__ ((__const__))
#else
# define _GL_ATTRIBUTE_CONST /* empty */
#endif
