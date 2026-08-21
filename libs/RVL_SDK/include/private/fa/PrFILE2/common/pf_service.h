#ifndef PRFILE2_VFMOD_PF_SERVICE_H
#define PRFILE2_VFMOD_PF_SERVICE_H

#include <private/fa/PrFILE2/pf_types.h>

pf_u16 PF_GET_LE_U16(const pf_u8* buf);
void PF_LE16_TO_U16_STR(pf_u8* sSrc, pf_u32 num);

#endif  // PRFILE2_VFMOD_PF_SERVICE_H
