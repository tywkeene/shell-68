#ifndef _SH_DEBUG_H
#define _SH_DEBUG_H

#include <stdint.h>

void sh_debug_stacktrace(uint32_t count);

#if defined(_SH_DEBUG_ALL)
#pragma message("_SH_DEBUG set")
#define _SH_DEBUG_MALLOC 1
#define _SH_DEBUG_TASK 1
#define _SH_DEBUG_STATUS_LED 1
#define _SH_DEBUG_CMDLINE 1
#define _SH_DEBUG_MUTEX 1
#define _SH_DEBUG_CHAN 1
#endif

#if defined(_SH_DEBUG_ALL) || defined(_SH_DEBUG_MALLOC) ||                     \
    defined(_SH_DEBUG_TASK) || defined(_SH_DEBUG_STATUS_LED) ||                \
    defined(_SH_DEBUG_CMDLINE) || defined(_SH_DEBUG_MUTEX) ||                  \
    defined(_SH_DEBUG_CHAN)
#define _SH_ANY_DEBUG 1
#pragma message("SH_ANY_DEBUG set")
#endif

#if defined(_SH_DEBUG_MALLOC)
#pragma once
#pragma message("_SH_DEBUG_MALLOC set")
#endif

#if defined(_SH_DEBUG_TASK)
#pragma once
#pragma message("_SH_DEBUG_TASK set")
#endif

#if defined(_SH_DEBUG_STATUS_LED)
#pragma once
#pragma message("_SH_DEBUG_STATUS_LED set")
#endif

#if defined(_SH_DEBUG_CMDLINE)
#pragma once
#pragma message("_SH_DEBUG_CMDLINE set")
#endif

#if defined(_SH_DEBUG_MUTEX)
#pragma once
#pragma message("_SH_DEBUG_MUTEX set")
#endif

#if defined(_SH_DEBUG_CHAN)
#pragma once
#pragma message("_SH_DEBUG_CHAN set")
#endif

#endif
