#ifndef PRFILE2_VFMOD_PF_FILE_H
#define PRFILE2_VFMOD_PF_FILE_H

#include <private/fa/PrFILE2/common/pf_str.h>
#include <private/fa/PrFILE2/fatfs/pf_entry.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>
#include <private/fa/PrFILE2/pf_types.h>

void PFFILE_Cursor_MoveToEnd(PF_FILE* p_file);
void PFFILE_Cursor_MoveToClusterEnd(PF_FILE* p_file, pf_u32 size);

pf_s32 PFFILE_GetOpenedFile(PF_DIR_ENT* p_ent, PF_DIR_ENT** pp_open_ent);
pf_bool PFFILE_IsOpened(PF_DIR_ENT* p_ent);
void PFFILE_FinalizeAllFiles(PF_VOLUME* p_vol);
pf_s32 PFFILE_DoRemoveFile(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter);
pf_s32 PFFILE_RemoveFile(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter);
pf_s32 PFFILE_remove(PF_STR* p_path_str);
pf_s32 PFFILE_fopen(PF_STR* p_path_str, pf_u32 mode, PF_FILE** pp_file);
pf_s32 PFFILE_fclose(PF_FILE* p_file);
pf_s32 PFFILE_fread(pf_u8* p_buf, pf_u32 size, pf_u32 count, PF_FILE* p_file, pf_u32* p_count_read);
pf_s32 PFFILE_fwrite(pf_u8* p_buf, pf_u32 size, pf_u32 count, PF_FILE* p_file, pf_u32* p_count_written);
pf_s32 PFFILE_fseek(PF_FILE* p_file, pf_s32 lOffset, pf_s32 nOrigin);
pf_s32 PFFILE_finfo(PF_FILE* p_file, PF_INFO* p_info);

#endif  // PRFILE2_VFMOD_PF_FILE_H
