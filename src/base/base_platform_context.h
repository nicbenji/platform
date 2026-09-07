#if !defined(BASE_PLATFORM_CONTEXT_H_)
#  define BASE_PLATFORM_CONTEXT_H_

// Compiler Macro Wiki: https://sourceforge.net/p/predef/wiki/Home/

/* Compiler */

// Clang
#if defined(__clang__)
#  define COMPILER_CLANG 1

#  if defined(_WIN32)
#    define OS_WINDOWS 1
#  elif defined(__gnu_linux__) || defined(__linux__)
#    define OS_LINUX 1
#  elif defined(__APPLE__) && defined(__MACH__)
#    define OS_MAC 1
#  else
#    error Unsupported OS + Compiler combi
#  endif

#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#     define ARCH_X64 1
#  elif defined(i386) || defined(__i386) || defined(__i386__)
#     define ARCH_X86 1
#  elif defined(__aarch64__)
#     define ARCH_ARM64 1
#  elif defined(__arm__)
#     define ARCH_ARM32 1
#  else
#     error Unsupported architecture
#  endif

// MSVC
#elif defined(_MSC_VER)
#  define COMPILER_MSVC 1

#  if defined(_WIN32)
#    define OS_WINDOWS 1
#  else
#    error Unsupported OS + Compiler combi
#  endif

#  if defined(_M_AMD64)
#    define ARCH_X64 1
#  elif defined(_M_IX86)
#    define ARCH_X86 1
#  elif defined(_M_ARM64)
#    define ARCH_ARM64 1
#  elif defined(_M_ARM)
#    define ARCH_ARM32 1
#  else
#    error Unsupported architecture
#  endif


// GCC
#elif defined(__GNUC__) || defined(__GNUG__)
#  define COMPILER_GCC 1

#  if defined(__gnu_linux__) || defined(__linux__)
#    define OS_LINUX 1
#  else
#    error Unsupported OS + Compiler combi
#  endif

#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#    define ARCH_X64 1
#  elif defined(i386) || defined(__i386) || defined(__i386__)
#    define ARCH_X86 1
#  elif defined(__aarch64__)
#    define ARCH_ARM64 1
#  elif defined(__arm__)
#    define ARCH_ARM32 1
#  else
#    error Unsupported architecture
#  endif

#else
#  error Unsupported compiler
#endif

/* Language */

#if defined(__cplusplus)
#  define LANGUAGE_CPP 1
#else
#  define LANGUAGE_C 1
#endif

/* Set everything not defined to 0 */

#if !defined(COMPILER_CLANG)
#  define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_MSVC)
#  define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
#  define COMPILER_GCC 0
#endif

#if !defined(OS_LINUX)
#  define OS_LINUX 0
#endif
#if !defined(OS_MAC)
#  define OS_MAC 0
#endif
#if !defined(OS_WINDOWS)
#  define OS_WINDOWS 0
#endif

#if !defined(ARCH_X64)
#  define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
#  define ARCH_X86 0
#endif
#if !defined(ARCH_ARM64)
#  define ARCH_ARM64 0
#endif
#if !defined(ARCH_ARM32)
#  define ARCH_ARM32 0
#endif

#if !defined(LANGUAGE_CPP)
#  define LANGUAGE_CPP 0
#endif
#if !defined(LANGUAGE_C)
#  define LANGUAGE_C 0
#endif

#endif // BASE_PLATFORM_CONTEXT_H_
