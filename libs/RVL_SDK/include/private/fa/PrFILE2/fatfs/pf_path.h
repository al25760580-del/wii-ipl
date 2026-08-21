#ifndef PRFILE2_FA_PF_PATH_H
#define PRFILE2_FA_PF_PATH_H

#include <private/fa/PrFILE2/common/pf_str.h>
#include <private/fa/PrFILE2/pf_types.h>

typedef struct PF_FILE_NAME_ITER {
    const pf_s8* buf;     // 0x00
    pf_u32 dot_inserted;  // 0x04
    pf_u32 is_long_name;  // 0x08
    pf_u16 index;         // 0x0C
} PF_FILE_NAME_ITER;      // total size: 0x10

#ifdef __cplusplus
extern "C" {
#endif

pf_s32 PFPATH_DoSplitPath(struct PF_STR* p_path, struct PF_STR* p_dir_path, struct PF_STR* p_filename, pf_u32 wildcard);
pf_u16 PFPATH_GetNextCharOfPattern(struct PF_STR* p_pattern, pf_u32 is_long_name);
pf_u32 PFPATH_DoMatchFileNameWithPattern(pf_u16 c_name, struct PF_FILE_NAME_ITER* p_name, pf_u16 c_pat, struct PF_STR* p_pattern,
                                         pf_u32 is_long_name);
pf_s32 PFPATH_cmpNameImpl(const pf_s8* sName, const pf_s8* sPattern, pf_u32* p_is_end);
pf_s32 PFPATH_cmpNameUni(const pf_u16* p_name, struct PF_STR* sPattern);
pf_s32 PFPATH_cmpName(const pf_s8* sShort, struct PF_STR* p_pattern, pf_u32 is_short_search);
pf_s32 PFPATH_cmpTailSFN(const pf_s8* sfn_name, const pf_s8* pattern);
void PFPATH_InitTokenOfPath(struct PF_STR* p_str, pf_s8* path, pf_u32 code_mode);
pf_s32 PFPATH_GetNextTokenOfPath(struct PF_STR* p_str, pf_u32 wildcard);
pf_s32 PFPATH_SplitPath(struct PF_STR* p_path, struct PF_STR* p_dir_path, struct PF_STR* p_filename);
struct PF_VOLUME* PFPATH_GetVolumeFromPath(struct PF_STR* p_path);
pf_u32 PFPATH_MatchFileNameWithPattern(const pf_s8* file_name, struct PF_STR* p_pattern, pf_u32 is_long_name);
pf_s32 PFPATH_putShortName(pf_u8* pDirEntry, const pf_s8* short_name, pf_u8 attr);
pf_s32 PFPATH_getShortName(pf_s8* short_name, const pf_u8* pDirEntry, pf_u8 attr);
void PFPATH_getLongNameformShortName(pf_s8* short_name, pf_s8* long_name, pf_u8 flag);
pf_u32 PFPATH_GetLengthFromShortname(const pf_s8* sSrc);
pf_u32 PFPATH_GetLengthFromUnicode(const pf_u16* sSrc);
pf_s32 PFPATH_transformFromUnicodeToNormal(pf_s8* sDest, const pf_u16* sSrc);
pf_s32 PFPATH_transformInUnicode(pf_u16* sDestStr, const pf_s8* sSrcStr);
pf_u32 PFPATH_parseShortName(pf_s8* pDest, struct PF_STR* p_pattern);
pf_s32 PFPATH_parseShortNameNumeric(pf_s8* p_char, pf_u32 count);
pf_u32 PFPATH_CheckExtShortNameSignature(struct PF_STR* p_str);
pf_u32 PFPATH_CheckExtShortName(struct PF_STR* p_str, pf_u32 target, pf_u32 wildcard);
pf_u32 PFPATH_GetExtShortNameIndex(struct PF_STR* p_str, pf_u32* p_index);
pf_s32 PFPATH_AdjustExtShortName(pf_s8* pName, pf_u32 position);

#ifdef __cplusplus
}
#endif

#endif  // PRFILE2_FA_PF_PATH_H
