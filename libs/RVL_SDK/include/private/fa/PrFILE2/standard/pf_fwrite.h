#ifndef VF_PF2_FWRITE_H
#define VF_PF2_FWRITE_H

#include <private/fa/PrFILE2/fatfs/pf_file.h>
#include <private/fa/PrFILE2/pf_types.h>

pf_u32 pf2_fwrite(void* p_buf, pf_u32 size, pf_u32 count, PF_FILE* p_file);

#endif  // VF_PF2_FWRITE_H
