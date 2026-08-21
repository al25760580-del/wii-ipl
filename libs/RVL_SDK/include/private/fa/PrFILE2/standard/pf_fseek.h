#ifndef VF_PF2_FSEEK_H
#define VF_PF2_FSEEK_H

#include <private/fa/PrFILE2/fatfs/pf_file.h>
#include <private/fa/PrFILE2/pf_types.h>

int pf2_fseek(PF_FILE* pFile, pf_s32 lOffset, pf_int32 nOrigin);

#endif  // VF_PF2_FSEEK_H
