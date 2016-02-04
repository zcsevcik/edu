#ifndef _MSC_VER
# define _countof(a)                    (sizeof(a)/sizeof((a)[0]))
#endif

#define HAVE_VASPRINTF 1

#define HAVE_BASENAME_GNU 1

/* oprava pro nefunkcni std::to_string na bsd */
#define __GXX_EXPERIMENTAL_CXX0X__ 1
#define _GLIBCXX_USE_C99 1
#undef _GLIBCXX_HAVE_BROKEN_VSWPRINTF
