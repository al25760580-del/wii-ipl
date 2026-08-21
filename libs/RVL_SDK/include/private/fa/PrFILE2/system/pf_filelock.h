#ifndef PRFILE2_VFMOD_PF_FILE_LOCK_H
#define PRFILE2_VFMOD_PF_FILE_LOCK_H

#include <private/fa/PrFILE2/fatfs/pf_file.h>
#include <private/fa/PrFILE2/pf_types.h>

void PF_InitLockFile();
long PF_UnLockFile(PF_FILE* p_file);

#endif  // PRFILE2_VFMOD_PF_FILE_LOCK_H
