#ifndef PRFILE2_VFMOD_PF_API_UTILITIES_H
#define PRFILE2_VFMOD_PF_API_UTILITIES_H

#include <private/fa/PrFILE2/pf_types.h>

pf_s32 PFAPI_ParseOpenModeString(const char* mode_str);
pf_s32 PFAPI_convertError(pf_s32 err);
pf_s32 PFAPI_convertReturnValue(pf_s32 err);
void* PFAPI_convertReturnValue2NULL(pf_s32 err, void* p_stream);
pf_s32 PFAPI_convertReturnValue4unmount(pf_s32 err);

#endif  // PRFILE2_VFMOD_PF_API_UTILITIES_H
