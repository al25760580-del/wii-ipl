#ifndef VF_PF2_FOPEN_H
#define VF_PF2_FOPEN_H

#include <private/fa/PrFILE2/fatfs/pf_file.h>
#include <private/fa/PrFILE2/pf_types.h>

PF_FILE* pf2_fopen(const pf_ch8* path, const pf_ch8* mode);

#endif  // VF_PF2_FOPEN_H
