#ifndef PRFILE2_VFMOD_PF_C_LIBRARY_IMPLEMENTATION_H
#define PRFILE2_VFMOD_PF_C_LIBRARY_IMPLEMENTATION_H

#include <private/fa/PrFILE2/pf_types.h>

pf_s32 pf_toupper(pf_s32 c);

void* pf_memcpy(void* dst, void* src, pf_u32 length);
void* pf_memset(void* dst, pf_s32 c, pf_u32 length);

pf_s32 pf_memcmp(const void* s1, const void* s2, pf_u32 n);

pf_u32 pf_strlen(const pf_s8* s);
pf_s8* pf_strcpy(pf_s8* dst, const pf_s8* src);
pf_s32 pf_strcmp(const pf_s8* s1, const pf_s8* s2);
pf_s32 pf_strncmp(const pf_s8* s1, const pf_s8* s2, pf_u32 length);

#endif  // PRFILE2_VFMOD_PF_C_LIBRARY_IMPLEMENTATION_H
