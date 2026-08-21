#ifndef VF_PF2_CREATE_H
#define VF_PF2_CREATE_H

#include <private/fa/PrFILE2/fatfs/pf_file.h>
#include <private/fa/PrFILE2/pf_types.h>

PF_FILE* pf2_create(const pf_ch8* path, pf_int32 mode);

#endif  // VF_PF2_CREATE_H
