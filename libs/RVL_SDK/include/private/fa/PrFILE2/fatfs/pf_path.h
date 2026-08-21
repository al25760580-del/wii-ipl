#ifndef PRFILE2_VFMOD_PF_PATH_H
#define PRFILE2_VFMOD_PF_PATH_H

#include <private/fa/PrFILE2/common/pf_str.h>
#include <private/fa/PrFILE2/pf_types.h>

typedef struct PF_FILE_NAME_ITER {
    const pf_s8* buf;  // 0x00
    pf_u16 kind;       // 0x04
    pf_u16 index;      // 0x06
} PF_FILE_NAME_ITER;

#define PF_IS_PATH_SEPERATOR(s, t, i) ((PFSTR_StrNCmp(s, (pf_s8*)"\\", t, i, 1) == 0 || PFSTR_StrNCmp(s, (pf_s8*)"/", t, i, 1) == 0))

#define PF_IS_PATH_NULL(s, t, i) (PFSTR_StrNCmp(s, (pf_s8*)"\0", t, i, 1) == 0)
#define PF_IS_PATH_NOT_NULL(s, t, i) (PFSTR_StrNCmp(s, (pf_s8*)"\0", t, i, 1) != 0)

pf_s32 PFPATH_cmpNameUni(const pf_u16* p_name, PF_STR* sPattern);
pf_s32 PFPATH_cmpName(const pf_s8* sShort, PF_STR* p_pattern, pf_bool is_short_search);
pf_s32 PFPATH_cmpTailSFN(const pf_s8* sfn_name, const pf_s8* pattern);
void PFPATH_InitTokenOfPath(PF_STR* p_str, pf_s8* path, pf_u32 code_mode);
pf_s32 PFPATH_GetNextTokenOfPath(PF_STR* p_str, pf_bool wildcard);
pf_s32 PFPATH_SplitPath(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_filename);
pf_s32 PFPATH_SplitPathPattern(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_pattern);
PF_VOLUME* PFPATH_GetVolumeFromPath(PF_STR* p_path);
pf_bool PFPATH_MatchFileNameWithPattern(const pf_s8* file_name, PF_STR* p_pattern, pf_bool is_short_search, pf_bool is_long_name);
pf_s32 PFPATH_putShortName(pf_u8* pDirEntry, const pf_s8* short_name, pf_u8 attr);
pf_s32 PFPATH_getShortName(pf_s8* short_name, const pf_u8* pDirEntry, pf_u8 attr);
void PFPATH_getLongNameformShortName(pf_s8* short_name, pf_s8* long_name, pf_u8 flag);
pf_u32 PFPATH_GetLengthFromShortname(const pf_s8* sSrc);
pf_u32 PFPATH_GetLengthFromUnicode(const pf_u16* sSrc);
pf_s32 PFPATH_transformFromUnicodeToNormal(pf_s8* sDest, const pf_u16* sSrc);
pf_s32 PFPATH_transformInUnicode(pf_u16* sDestStr, const pf_s8* sSrcStr);
pf_u32 PFPATH_parseShortName(pf_s8* pDest, PF_STR* p_pattern);
pf_s32 PFPATH_parseShortNameNumeric(pf_s8* p_char, pf_u32 count);
void PFPATH_SetSearchPattern(pf_s8* p_buf_local, pf_u16* p_buf_unicode, PF_STR* p_pattern);
pf_u32 PFPATH_CheckExtShortNameSignature(PF_STR* p_str);
pf_u32 PFPATH_CheckExtShortName(PF_STR* p_str, pf_u32 target, pf_bool wildcard);
pf_bool PFPATH_GetExtShortNameIndex(PF_STR* p_str, pf_u32* p_index);
pf_s32 PFPATH_AdjustExtShortName(pf_s8* pName, pf_u32 position);

#endif  // PRFILE2_VFMOD_PF_PATH_H
