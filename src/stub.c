#include <stdint.h>

#define MS_SUCCEEDED 0x0
#define MS_CATCHED 0x1

#define TG_ARCH_X86 1
#define TG_ARCH_X64 2
#define TG_ARCH_ARM64 3
#define TG_ARCH_UNKNOWN 4

#if defined(_M_IX86)
#define TG_ARCH TG_ARCH_X86
#elif defined(_M_X64) || defined(_M_AMD64)
#define TG_ARCH TG_ARCH_X64
#elif defined(_M_ARM64)
#define TG_ARCH TG_ARCH_ARM64
#else
#define TG_ARCH TG_ARCH_UNKNOWN
#endif

#define EXCEPTION_EXECUTE_HANDLER      1
#define GetExceptionCode            _exception_code
#if defined(_MSC_VER)
    #define TG_CDECL __cdecl
    #define TG_STDCALL __stdcall
#else
    #define TG_CDECL __attribute__((cdecl))
    #define TG_STDCALL __attribute__((stdcall))
#endif

unsigned long TG_CDECL _exception_code(void);

typedef void (TG_STDCALL *PPROC_EXECUTOR)(void* Proc);

typedef struct _EXCEPTION
{
    uint32_t Code;
} EXCEPTION, *PEXCEPTION;

uint32_t __microseh_HandlerStub(
    PPROC_EXECUTOR ProcExecutor,
    void* Proc,
    PEXCEPTION Exception
) {
    uint32_t Result = MS_SUCCEEDED;
    uint32_t Code = 0;

    __try
    {
        ProcExecutor(Proc);
    }
    __except (Code = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER)
    {
        Result = MS_CATCHED;
        if (Exception != NULL)
        {
            // Use GetExceptionCode() instead of Record->ExceptionCode as it is more reliable.
            Exception->Code = Code;
        }
    }

    return Result;
}
