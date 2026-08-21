#ifndef PRFILE2_VFMOD_PF_STRING_H
#define PRFILE2_VFMOD_PF_STRING_H

#include <private/fa/PrFILE2/common/pf_code.h>
#include <private/fa/PrFILE2/pf_types.h>

#define PF_STR_CODEMODE_NONE 0
#define PF_STR_CODEMODE_OEM 1
#define PF_STR_CODEMODE_UNICODE 2
#define PF_STR_CODEMODE_LOCAL 3

typedef struct PF_STR {
    const pf_s8* p_head;   // 0x00
    const pf_s8* p_tail;   // 0x04
    pf_u32 code_mode;      // 0x08
    const pf_s8* p_local;  // 0x0C
} PF_STR;

void PFSTR_SetCodeMode(PF_STR* p_str, pf_u32 code_mode);
void PFSTR_SetLocalStr(PF_STR* p_str, pf_s8* p_local);
pf_u32 PFSTR_GetCodeMode(PF_STR* p_str);

pf_s8* PFSTR_GetStrPos(PF_STR* p_str, pf_u32 target);
void PFSTR_MoveStrPos(PF_STR* p_str, pf_s16 num_char);

pf_s32 PFSTR_InitStr(PF_STR* p_str, const pf_s8* s, pf_u32 code_mode);

pf_u16 PFSTR_StrLen(PF_STR* p_str);
pf_u16 PFSTR_StrNumChar(PF_STR* p_str, pf_u32 target);

pf_s32 PFSTR_StrCmp(const PF_STR* p_str, const pf_s8* s);
pf_s32 PFSTR_StrNCmp(PF_STR* p_str, const pf_s8* s, pf_u32 target, pf_s16 offset, pf_u16 num);

void PFSTR_ToUpperNStr(PF_STR* p_str, pf_u16 num, pf_s8* dest);

#endif  // PRFILE2_VFMOD_PF_STRING_H
