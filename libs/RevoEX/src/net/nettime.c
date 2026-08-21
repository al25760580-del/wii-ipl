// Decompiled from the RevoEX "net" library.
//
// NETGetUniversalCalendar fills an OSCalendarTime with the current UTC time.
// The two matched call sites (CHANSVm VmDateCommon and iplNwc24Manager) both
// feed the result straight into OSCalendarTimeToTicks, i.e. they expect the
// same epoch/scaling as the SDK's OSTicksToCalendarTime output.
//
// NOT YET VERIFIED AGAINST THE ORIGINAL BINARY: the original unit also holds
// an 8-byte .sbss static (possibly a cached tick value) which is left as a
// fallback to the binary, and the exact conversion (plain OSGetTime() vs. an
// applied UTC offset) needs objdiff confirmation.

#include <revolution/net/NETMisc.h>

#include <revolution/os/OSTime.h>

BOOL NETGetUniversalCalendar(OSCalendarTime* time) {
    OSTicksToCalendarTime(OSGetTime(), time);
    return TRUE;
}
