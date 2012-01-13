//
//  StackTrace.h
//  Project Unknown
//
//  Created by Robert Bu on 11/21/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_StackTrace_h
#define Project_Unknown_StackTrace_h

#include "UKN/Platform.h"

#ifdef UKN_OS_WINDOWS
#include <Windows.h>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")
#endif

#ifdef __GNUC__
#include <execinfo.h>
#endif

#include "UKN/Common.h"

#include <cstdlib>

namespace ukn {

#ifdef UKN_OS_WINDOWS

#define INVALID_FP_RET_ADDR_VALUE 0x0000000

    inline ukn_string DisplaySymbolDetails(DWORD dwAddress) {
        DWORD64 displacement = 0;
        ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME*sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];

        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        ukn_string info;

        if (SymFromAddr(GetCurrentProcess(), dwAddress, &displacement, pSymbol)) {
			IMAGEHLP_MODULE64 moduleinfo;
			moduleinfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
			if(SymGetModuleInfo64(GetCurrentProcess(), pSymbol->Address, &moduleinfo)) {
				info.append(moduleinfo.ModuleName);
			} else
				info.append("<Error_Module_Info>");

			if (pSymbol->MaxNameLen > 0) {
				info.append(pSymbol->Name);
			}
			else {
				info.append("<Unknown_Function>");
			}

        } else {
            info.append(" <Unable to get symbol details>");
        }
        return info;
	}

    inline ukn_string win_GetStackTrace() {
		HANDLE process = GetCurrentProcess();
		if(SymInitialize(process, NULL, TRUE)) {
            DWORD _ebp = INVALID_FP_RET_ADDR_VALUE;
            DWORD dwIPOfCurrentFunction = (DWORD)&win_GetStackTrace;

            // Get the current Frame pointer
            __asm {
                mov [_ebp], ebp
            }

            // We cannot walk the stack (yet!) without a frame pointer
            if (_ebp == INVALID_FP_RET_ADDR_VALUE)
                return false;

            //current Frame Pointe
            DWORD *pCurFP = (DWORD *)_ebp;
            BOOL fFirstFP = TRUE;

            ukn_string stackTrace("CurrFP\t\tRetAddr\t\tFunction\n");
            while (pCurFP != INVALID_FP_RET_ADDR_VALUE) {
                DWORD pRetAddrInCaller = (*((DWORD *)(pCurFP + 1)));

                stackTrace.append(format_string("%.8p%*p        ", pCurFP, 20, (DWORD *)pRetAddrInCaller));

                if (fFirstFP) {
                    fFirstFP = FALSE;
                }
                stackTrace.append(DisplaySymbolDetails(dwIPOfCurrentFunction));

                dwIPOfCurrentFunction = pRetAddrInCaller;

                stackTrace.append("\n");
                if (pRetAddrInCaller == INVALID_FP_RET_ADDR_VALUE) {
                    break;
                }
                DWORD pCallerFP = *((DWORD *)pCurFP);
                pCurFP = (DWORD *)pCallerFP;
            }

			SymCleanup(process);

			return stackTrace;
		}
		return "Unable to trace the function stack";
	}

#endif // UKN_OS_WINDOWS

    static ukn_string ukn_get_formatted_stack_trace_string() {
        ukn_string stackTraceString;

#ifdef __GNUC__
        const int len = 200;
        void* buffer[len];
        int nptrs = backtrace(buffer, len);
        char** strings = backtrace_symbols(buffer, nptrs);
        if (strings) {
            for (int i = 0; i < nptrs; ++i) {
                stackTraceString.append(strings[i]);
                stackTraceString.push_back('\n');
            }
            free(strings);
        }
#elif defined(UKN_OS_WINDOWS)
        stackTraceString = win_GetStackTrace();
#endif
        return stackTraceString;

    }

} // namespace ukn

#endif
