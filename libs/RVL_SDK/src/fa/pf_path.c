// Decompiled from the RVL_SDK "fa" library (File Allocation / PrFILE2 FAT).
//
// Reconstructed by porting the matching sibling implementation of the same
// pf code: the RevoEX VF library in this repo (Matching) and the Wii Sports
// VF decompilation (doldecomp/ogws, CC0), whose VFipf_* symbols were
// cross-checked against the fa symbols of this binary by size.
//
// NOT YET VERIFIED AGAINST THE ORIGINAL BINARY (no objdiff in this fork).
// Differences flagged below must be re-checked with objdiff; this unit is
// kept NonMatching until then.

#include <private/fa/PrFILE2/common/pf_clib.h>
#include <private/fa/PrFILE2/common/pf_code.h>
#include <private/fa/PrFILE2/common/pf_service.h>
#include <private/fa/PrFILE2/common/pf_w_clib.h>
#include <private/fa/PrFILE2/fatfs/pf_path.h>
#include <private/fa/PrFILE2/fatfs/pf_volume.h>

// PFPATH_DoSplitPath - 0x815DC914, 488 bytes
// NOTE: size-diff(ogws=504) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_DoSplitPath(struct PF_STR* p_path, struct PF_STR* p_dir_path, struct PF_STR* p_filename, pf_u32 wildcard) {
    pf_s8* p;
    pf_s8* p_tail_prev;
    pf_u32 code_mode;
    struct PF_STR token;
    struct PF_STR token_prev;
    pf_s32 err;

    if (p_dir_path == PF_NULL) {
        return 10;
    }

    code_mode = PFSTR_GetCodeMode(p_path);
    PFSTR_SetCodeMode(p_dir_path, code_mode);
    PFSTR_SetCodeMode(p_filename, code_mode);

    p_dir_path->p_head = PF_NULL;
    if (p_filename != PF_NULL) {
        p_filename->p_head = PF_NULL;
    }

    if (PFSTR_StrNCmp(p_path, (const pf_s8*)":", 1, 1, 1) == 0) {
        PFSTR_MoveStrPos(p_path, 2);
    }

    p = (pf_s8*)p_path->p_head;
    PFPATH_InitTokenOfPath(&token, (pf_s8*)p_path->p_head, code_mode);
    err = PFPATH_GetNextTokenOfPath(&token, wildcard);

    if (err != 0) {
        return err;
    }

    if (token.p_head == PF_NULL || (pf_u16)PFSTR_StrLen(&token) == 0) {
        return 2;
    }

    if (p_path->p_tail < token.p_tail) {
        return 2;
    }

    token_prev = token;

    while (1) {
        if (PFSTR_StrNCmp(&token, (const pf_s8*)"\\", 2, 0, 1) == 0) {
            p = (pf_s8*)token_prev.p_tail;
            break;
        }

        err = PFPATH_GetNextTokenOfPath(&token, wildcard);
        if (err != 0) {
            return err;
        }

        if ((pf_u16)PFSTR_StrLen(&token) == 0) {
            return 2;
        }

        if (p_path->p_tail < token.p_tail) {
            break;
        }

        if (token.p_head == PF_NULL) {
            break;
        }

        p_tail_prev = (pf_s8*)token_prev.p_tail;
        p = p_tail_prev;
        token_prev = token;
    }

    p_dir_path->p_head = p_path->p_head;
    p_dir_path->p_tail = p;
    if (p_filename != PF_NULL) {
        *p_filename = token_prev;
    }

    return 0;
}

// PFPATH_GetNextCharOfPattern - 0x815DCAFC, 892 bytes
// NOTE: size-diff(ogws=704) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_u16 PFPATH_GetNextCharOfPattern(struct PF_STR* p_pattern, pf_u32 is_long_name) {
    pf_u16 twc;
    pf_u16 wc;
    pf_u16 tmp_wc;
    pf_s8 pattern[3];

    if (p_pattern->code_mode == 1) {
        pattern[0] = *p_pattern->p_head++;
        pattern[1] = 0;

        if (pattern[0] == 0 || p_pattern->p_tail < p_pattern->p_head) {
            return 0;
        }

        if (pf_vol_set.codeset.is_oem_mb_char(pattern[0], 1) != 0) {
            pattern[1] = *p_pattern->p_head++;
            twc = ((pf_u8)pattern[0] << 8) + (pf_u8)pattern[1];
        } else {
            twc = pattern[0];
        }

        if (is_long_name != 0) {
            pf_vol_set.codeset.oem2unicode(pattern, &twc);
        }
    } else {
        pattern[0] = *(pf_u16*)p_pattern->p_head;
        pattern[1] = (pf_s8)((*(pf_u16*)p_pattern->p_head) >> 8);
        p_pattern->p_head += 2;
        pattern[2] = 0;

        if ((pattern[0] == 0 && pattern[1] == 0) || p_pattern->p_tail < p_pattern->p_head) {
            return 0;
        }

        twc = ((pf_u8)pattern[1] << 8) + (pf_u8)pattern[0];

        if (is_long_name == 0) {
            pf_vol_set.codeset.unicode2oem(&twc, (pf_s8*)&wc);
            if (pf_vol_set.codeset.is_oem_mb_char(wc >> 8, 1) != 0) {
                twc = wc;
            } else {
                twc = pattern[0];
            }
        }
    }

    twc = (twc >= 0x61 && twc <= 0x7A) ? twc - 0x20 : twc;

    if (is_long_name != 0) {
        if (VFiPFPATH_UNI_ConvertFWchar(twc, &tmp_wc) == 1) {
            twc = tmp_wc;
        }
    } else {
        if (VFiPFPATH_OEM_ConvertFWchar((const pf_s8*)&twc, &tmp_wc) == 1) {
            twc = tmp_wc;
        }
    }

    return twc;
}

// PFPATH_DoMatchFileNameWithPattern - 0x815DCE78, 1232 bytes
// NOTE: size-diff(ogws=1268) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_DoMatchFileNameWithPattern(pf_u16 c_name, struct PF_FILE_NAME_ITER* p_name, pf_u16 c_pat, struct PF_STR* p_pattern,
                                         pf_u32 is_long_name) {
    struct PF_FILE_NAME_ITER name;
    struct PF_STR pattern;

    while (c_pat != '\0') {
        switch (c_pat) {
            case '?': {
                if (c_name == '\0') {
                    return 0;
                }
                break;
            }
            case '*': {
                c_pat = PFPATH_GetNextCharOfPattern(p_pattern, is_long_name);
                while (c_pat == '*' || c_pat == '?') {
                    c_pat = PFPATH_GetNextCharOfPattern(p_pattern, is_long_name);
                }
                if (c_pat == '\0') {
                    return 1;
                }

                while (c_name != '\0') {
                    if (c_name == c_pat) {
                        name = *p_name;
                        pattern = *p_pattern;

                        c_name = VFiPFPATH_GetNextCharOfFileName(&name);
                        c_pat = PFPATH_GetNextCharOfPattern(&pattern, is_long_name);

                        if (PFPATH_DoMatchFileNameWithPattern(c_name, &name, c_pat, &pattern, is_long_name)) {
                            return 1;
                        }
                    }
                    c_name = VFiPFPATH_GetNextCharOfFileName(p_name);
                }
                return 0;
            }
            default: {
                if (c_name != c_pat) {
                    return 0;
                }
                break;
            }
        }
        c_name = VFiPFPATH_GetNextCharOfFileName(p_name);
        c_pat = PFPATH_GetNextCharOfPattern(p_pattern, is_long_name);
    }
    return c_name == '\0';
}

// PFPATH_cmpNameImpl - 0x815DD348, 872 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_cmpNameImpl(const pf_s8* sName, const pf_s8* sPattern, pf_u32* p_is_end) {
    pf_s32 pw;
    pf_s32 nw;
    pf_s32 ret;
    pf_u16 p;
    pf_u16 n;
    pf_u16 tp;
    pf_u16 tn;

    while (*sPattern != '\0') {
        pw = pf_vol_set.codeset.oem_char_width(sPattern);
        nw = pf_vol_set.codeset.oem_char_width(sName);

        p = (pw == 1) ? (pf_toupper(*sPattern)) : ((pf_u16)PF_GET_LE_U16((const pf_u8*)sPattern));
        n = (nw == 1) ? (pf_toupper(*sName)) : ((pf_u16)PF_GET_LE_U16((const pf_u8*)sName));

        if (pw == 2 && VFiPFPATH_OEM_ConvertFWchar(sPattern, &tp) == 1) {
            p = tp;
        }

        if (nw == 2 && VFiPFPATH_OEM_ConvertFWchar(sName, &tn) == 1) {
            n = tn;
        }

        sPattern += pw;

        switch (p) {
            case '?': {
                if (n == 0) {
                    return 1;
                }
                break;
            }
            case '*': {
                do {
                    pw = pf_vol_set.codeset.oem_char_width(sPattern);
                    p = (pw == 1) ? (pf_toupper(*sPattern)) : ((pf_u16)PF_GET_LE_U16((const pf_u8*)sPattern));

                    sPattern += pw;

                    if (p == '?') {
                        if (n == 0) {
                            return 1;
                        }
                        sName += nw;
                        nw = pf_vol_set.codeset.oem_char_width(sName);
                        n = (nw == 1) ? (pf_toupper(*sName)) : ((pf_u16)PF_GET_LE_U16((const pf_u8*)sName));
                    }
                } while (p == '?' || p == '*');

                if (p == 0) {
                    return 0;
                }

                while (n != 0) {
                    sName += nw;
                    if (n == p) {
                        ret = PFPATH_cmpNameImpl(sName, sPattern, p_is_end);
                        if (ret == 0) {
                            return 0;
                        }
                        if (*p_is_end != 0) {
                            return ret;
                        }
                    }

                    nw = pf_vol_set.codeset.oem_char_width(sName);
                    n = (nw == 1) ? (pf_toupper(*sName)) : ((pf_u16)PF_GET_LE_U16((const pf_u8*)sName));
                }

                if (*sName == '\0' || *sPattern == '\0') {
                    *p_is_end = 1;
                }
            }

            default: {
                if (n != p) {
                    return 1;
                }
                break;
            }
        }

        sName += nw;
    }

    return *sName != '\0';
}

// PFPATH_cmpNameUni - 0x815DD6B0, 44 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_cmpNameUni(const pf_u16* p_name, struct PF_STR* sPattern) {
    return PFPATH_MatchFileNameWithPattern((const pf_s8*)p_name, sPattern, 1) == 0;
}

// PFPATH_cmpName - 0x815DD6DC, 348 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_cmpName(const pf_s8* sShort, struct PF_STR* p_pattern, pf_u32 is_short_search) {
    pf_u32 is_end;
    pf_s8 tmpBuf[13];
    pf_s8* p_tmpBuf;
    const pf_s8* sPattern;

    is_end = 0;
    p_tmpBuf = tmpBuf;
    sPattern = PFSTR_GetStrPos(p_pattern, 3);
    pf_strcpy(tmpBuf, sShort);

    if (((pf_vol_set.setting & 2) == 2) && PFSTR_GetCodeMode(p_pattern) == 2 && pf_strcmp(tmpBuf, (const pf_s8*)".") != 0 &&
        pf_strcmp(tmpBuf, (const pf_s8*)"..") != 0 && PFPATH_CheckExtShortName(p_pattern, 3, 1) == 0 && is_short_search == 0) {
        return 1;
    }

    if (pf_strcmp(sPattern, (const pf_s8*)"*.*") == 0) {
        for (; *p_tmpBuf != '\0' && *p_tmpBuf != '.'; p_tmpBuf++) {
        }
        if (*p_tmpBuf == '\0') {
            *p_tmpBuf = '.';
            p_tmpBuf[1] = '\0';
        }
    } else {
        if (*sPattern == '\0') {
            return 1;
        }
        if (tmpBuf[0] == '\0') {
            return 1;
        }
    }

    return PFPATH_cmpNameImpl(tmpBuf, sPattern, &is_end);
}

// PFPATH_cmpTailSFN - 0x815DD838, 44 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_cmpTailSFN(const pf_s8* sfn_name, const pf_s8* pattern) {
    return pf_strcmp(sfn_name, pattern) != 0;
}

// PFPATH_InitTokenOfPath - 0x815DD864, 16 bytes
// from ogws (Wii Sports VF, CC0)

void PFPATH_InitTokenOfPath(struct PF_STR* p_str, pf_s8* path, pf_u32 code_mode) {
    p_str->p_head = path;
    p_str->p_tail = path;
    p_str->code_mode = code_mode;
}

// PFPATH_GetNextTokenOfPath - 0x815DD874, 900 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_GetNextTokenOfPath(struct PF_STR* p_str, pf_u32 wildcard) {
    pf_u32 extsfn_len;
    pf_u32 code_mode;

    p_str->p_head = p_str->p_tail;
    extsfn_len = 0;

    if (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", 1, 0, 1) == 0) {
        p_str->p_tail = 0;
        p_str->p_head = 0;
        return 0;
    }

    if (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", 1, 0, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)"/", 1, 0, 1) == 0) {
        PFSTR_MoveStrPos(p_str, 1);
    }

    if (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", 1, 0, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)"/", 1, 0, 1) == 0) {
        return 2;
    }

    code_mode = PFSTR_GetCodeMode(p_str);
    p_str->p_tail = p_str->p_head;

    if ((pf_vol_set.setting & 2) == 2) {
        extsfn_len = PFPATH_CheckExtShortName(p_str, 2, wildcard);
        if (extsfn_len != 0) {
            if (code_mode == 1) {
                p_str->p_tail += extsfn_len;
            } else {
                p_str->p_tail += extsfn_len * 2;
            }
        }
    }

    if (extsfn_len == 0) {
        while (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", 2, 0, 1) != 0) {
            if (code_mode == 1 && pf_vol_set.codeset.is_oem_mb_char(*p_str->p_tail, 1) != 0) {
                p_str->p_tail++;
                if (pf_vol_set.codeset.is_oem_mb_char(*p_str->p_tail, 2) == 0 || *p_str->p_tail == '\0') {
                    return 2;
                }
            } else {
                if (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", 2, 0, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)"/", 2, 0, 1) == 0) {
                    break;
                }

                if ((pf_vol_set.config & 0x10000) == 0) {
                    if (code_mode == 1 && (pf_u8)*p_str->p_tail < 0x80 &&
                        ((pf_u8)*p_str->p_tail < 0x20 || (pf_valid_fn_char[(pf_u8)*p_str->p_tail - 0x20] & 1) == 0)) {
                        goto check_wildcard;
                    } else if (code_mode == 2) {
                        pf_u16 bVar1 = (pf_u64)(((pf_u8)p_str->p_tail[0] << 8) + (pf_u8)p_str->p_tail[1]);
                        pf_u32 bVar3 = 1;

                        if (bVar1 < 0x80) {
                            pf_s32 bVar2 = 0;

                            if ((pf_u8)bVar1 >= 0x20 && ((pf_u8)bVar1 & 0xFF00) == 0) {
                                if ((pf_valid_fn_char[(pf_u8)((pf_u8)bVar1 - 0x20)] & 1) != 0) {
                                    bVar2 = 1;
                                }
                            }
                            if (bVar2 == 0) {
                                bVar3 = 0;
                            }
                        }

                        if (bVar3 == 0) {
                        check_wildcard:
                            if (wildcard == 0 ||
                                (PFSTR_StrNCmp(p_str, (const pf_s8*)"*", 2, 0, 1) != 0 && PFSTR_StrNCmp(p_str, (const pf_s8*)"?", 2, 0, 1) != 0)) {
                                return 2;
                            }
                        }
                    }
                }
            }

            if (code_mode == 1) {
                p_str->p_tail += 1;
            } else {
                p_str->p_tail += 2;
            }
        }
    }
    return 0;
}

// PFPATH_SplitPath - 0x815DDBF8, 8 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_SplitPath(struct PF_STR* p_path, struct PF_STR* p_dir_path, struct PF_STR* p_filename) {
    PFPATH_DoSplitPath(p_path, p_dir_path, p_filename, 0);
}

// PFPATH_SplitPathPattern - 0x815DDC00, 8 bytes
// from RevoEX VF (this repo, Matching)

pf_s32 PFPATH_SplitPathPattern(PF_STR* p_path, PF_STR* p_dir_path, PF_STR* p_pattern) {
    return PFPATH_DoSplitPath(p_path, p_dir_path, p_pattern, 1U);
}

// PFPATH_GetVolumeFromPath - 0x815DDC08, 128 bytes
// NOTE: size-diff(ogws=152) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

struct PF_VOLUME* PFPATH_GetVolumeFromPath(struct PF_STR* p_path) {
    struct PF_VOLUME* p_vol;
    pf_s8 drv_char[2];

    if (PFSTR_StrLen(p_path) == 0) {
        p_vol = 0;
    } else {
        if (PFSTR_StrNumChar(p_path, 1) >= 2 && PFSTR_StrNCmp(p_path, (const pf_s8*)":", 1, 1, 1) == 0) {
            PFSTR_ToUpperNStr(p_path, 1, drv_char);
            p_vol = PFVOL_GetVolumeFromDrvChar(drv_char[0]);
        } else {
            p_vol = PFVOL_GetCurrentVolume();
        }
    }
    return p_vol;
}

// PFPATH_MatchFileNameWithPattern - 0x815DDC88, 796 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_MatchFileNameWithPattern(const pf_s8* file_name, struct PF_STR* p_pattern, pf_u32 is_long_name) {
    pf_u16 c_name;
    pf_u16 c_pat;
    struct PF_FILE_NAME_ITER name;
    struct PF_STR pattern;
    pf_u32 is_match;
    pf_s8 sig[2];

    is_match = 1;
    sig[0] = *(pf_s8*)"~";
    sig[1] = *(pf_s8*)"1";

    name.buf = file_name;
    name.dot_inserted = 0;
    name.is_long_name = is_long_name;
    name.index = 0;

    pattern = *p_pattern;

    if ((pf_u32)PFSTR_GetCodeMode(p_pattern) == 1) {
        if (is_long_name == 0 && (pf_vol_set.setting & 2) == 2 && (pf_u32)PFPATH_CheckExtShortNameSignature(&pattern) == 1 &&
            pf_strncmp(file_name, sig, 2) == 0) {
            name.index += 2;
            pattern.p_head += 2;
        }
    } else {
        if ((pf_vol_set.setting & 2) == 2 && is_long_name == 0 && PFSTR_StrNCmp(p_pattern, (const pf_s8*)".", 1, 0, 1) != 0 &&
            PFSTR_StrNCmp(p_pattern, (const pf_s8*)"..", 1, 0, 2) != 0 && PFPATH_CheckExtShortName(p_pattern, 1, 0) == 0) {
            is_match = 0;
        }
    }

    if (is_match == 1) {
        c_name = VFiPFPATH_GetNextCharOfFileName(&name);
        c_pat = PFPATH_GetNextCharOfPattern(&pattern, is_long_name);
        is_match = PFPATH_DoMatchFileNameWithPattern(c_name, &name, c_pat, &pattern, is_long_name);
    }

    return is_match;
}

// PFPATH_putShortName - 0x815DDF98, 380 bytes
// NOTE: size-diff(ogws=392) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_putShortName(pf_u8* pDirEntry, const pf_s8* short_name, pf_u8 attr) {
    pf_s32 i;

    i = 0;
    while (i < 8 && *short_name != '\0' && *short_name != '.') {
        *pDirEntry++ = *short_name++;
        i++;
    }

    if (i == 0) {
        i = 0;
        while (i < 2 && *short_name != '\0') {
            *pDirEntry++ = *short_name++;
            i++;
        }
    }

    if (i > 8) {
        return 10;
    }

    if ((attr & 8) == 0) {
        while (i < 8) {
            *pDirEntry++ = ' ';
            i++;
        }
    }

    if (*short_name != '\0') {
        if ((attr & 8) == 0) {
            short_name++;
        }
        while (*short_name != '\0') {
            *pDirEntry++ = *short_name++;
            i++;
        }
    }

    while (i < 11) {
        *pDirEntry++ = ' ';
        i++;
    }

    return 0;
}

// PFPATH_getShortName - 0x815DE114, 592 bytes
// NOTE: size-diff(ogws=604) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_getShortName(pf_s8* short_name, const pf_u8* pDirEntry, pf_u8 attr) {
    pf_s32 i;
    pf_s32 nLen;

    i = 0;
    nLen = -1;
    for (i = 0; i < 8; i++) {
        short_name[i] = pDirEntry[i];
        if ((pf_s32)short_name[i] != ' ') {
            nLen = i;
        }
    }

    short_name += (nLen += 1);
    if (nLen == 0) {
        return 10;
    }

    nLen = 7;
    for (i = 8; i < 11; i++) {
        if (pDirEntry[i] != ' ') {
            nLen = i;
        }
    }

    if (nLen > 7) {
        if ((attr & 8) == 0) {
            *short_name++ = '.';
        }
        for (i = 8; i <= nLen; i++) {
            *short_name++ = pDirEntry[i];
        }
    }
    *short_name = '\0';
    return 0;
}

// PFPATH_getLongNameformShortName - 0x815DE364, 432 bytes
// from ogws (Wii Sports VF, CC0)

void PFPATH_getLongNameformShortName(pf_s8* short_name, pf_s8* long_name, pf_u8 flag) {
    pf_s32 i;
    pf_s32 j;

    for (i = 0; i < 8; i++) {
        if (short_name[i] == '\0' || short_name[i] == '.') {
            break;
        }

        if ((flag & 8) != 0 && short_name[i] >= 'A' && short_name[i] <= 'Z') {
            long_name[i] = short_name[i] + 0x20;
        } else {
            long_name[i] = short_name[i];
        }
    }

    if (short_name[i] == '.') {
        long_name[i] = '.';
        i++;
    }

    j = i + 3;
    while (i < j) {
        if (short_name[i] == '\0') {
            break;
        }

        if ((flag & 0x10) != 0 && short_name[i] >= 'A' && short_name[i] <= 'Z') {
            long_name[i] = short_name[i] + 0x20;
        } else {
            long_name[i] = short_name[i];
        }
        i++;
    }

    long_name[i] = '\0';
}

// PFPATH_GetLengthFromShortname - 0x815DE514, 104 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_GetLengthFromShortname(const pf_s8* sSrc) {
    pf_s32 i;
    pf_u32 szStr;

    for (szStr = i = 0; sSrc[i]; i++) {
        if (i == 8 && (sSrc[i] != ' ' || sSrc[i + 1] != ' ' || sSrc[i + 2] != ' ')) {
            szStr++;
        }
        if (sSrc[i] != ' ') {
            szStr++;
        }
    }
    return szStr;
}

// PFPATH_GetLengthFromUnicode - 0x815DE57C, 156 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_GetLengthFromUnicode(const pf_u16* sSrc) {
    pf_s32 i;
    pf_s32 width;
    pf_u32 szStr;
    pf_s16 oem_width;
    pf_s16 uni_width;
    pf_s8 Dest[2];

    szStr = 0;
    width = 0;
    while (sSrc[width] != 0) {
        PFCODE_Divide_Width(pf_vol_set.codeset.unicode2oem(sSrc + width, Dest), &oem_width, &uni_width);
        szStr += oem_width;
        width += uni_width >> 1;
    }
    return szStr;
}

// PFPATH_transformFromUnicodeToNormal - 0x815DE618, 300 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_transformFromUnicodeToNormal(pf_s8* sDest, const pf_u16* sSrc) {
    pf_s32 i;
    pf_s32 width;
    pf_u32 szStr;
    pf_s16 oem_width;
    pf_s16 uni_width;
    pf_u16 dot_buf[2];
    pf_u16 space_buf[2];

    if ((pf_vol_set.setting & 2) == 2) {
        dot_buf[0] = '.';
        dot_buf[1] = 0;
        space_buf[0] = ' ';
        space_buf[1] = 0;
        for (i = 0; sSrc[i] != 0; i++) {
            if (pf_w_strncmp(sSrc + i, dot_buf, 1) == 0) {
                *sDest = '.';
            } else if (pf_w_strncmp(sSrc + i, space_buf, 1) == 0) {
                *sDest = ' ';
            } else {
                *sDest = '_';
            }
            sDest++;
        }
    } else {
        for (i = 0; sSrc[i] != 0; i += uni_width >> 1) {
            PFCODE_Divide_Width(pf_vol_set.codeset.unicode2oem(sSrc + i, sDest), &oem_width, &uni_width);
            sDest += oem_width;
        }
    }
    *sDest = '\0';
    return 0;
}

// PFPATH_transformInUnicode - 0x815DE744, 152 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_transformInUnicode(pf_u16* sDestStr, const pf_s8* sSrcStr) {
    pf_s32 i;
    pf_s32 width;
    pf_s32 szStr;
    pf_s16 oem_width;
    pf_s16 uni_width;

    szStr = 0;
    width = 0;
    while (sSrcStr[width] != '\0') {
        PFCODE_Divide_Width(pf_vol_set.codeset.oem2unicode(sSrcStr + width, sDestStr), &oem_width, &uni_width);
        sDestStr += uni_width >> 1;
        width += oem_width;
        szStr++;
    }
    *sDestStr = 0;
    return szStr;
}

// PFPATH_parseShortName - 0x815DE7DC, 1864 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_parseShortName(pf_s8* pDest, struct PF_STR* p_pattern) {
    pf_s32 width;
    const pf_s8* p_cur_src;
    pf_u32 is_create_long;
    pf_u32 is_create_tail;
    pf_u16* p_name_cnt;
    pf_u16 num_base;
    pf_u16 num_ext;
    pf_u16 last_width;
    pf_u16 prev_last_width;
    pf_u16 src_dot;
    pf_u16 prev_src_dot;
    pf_u16 src_pos;
    pf_u16 dst_pos;
    pf_u16 ext_pos;
    pf_u16 move_cnt;
    pf_s16 i;
    pf_u16 wchar;
    pf_u16 t_wchar;

    is_create_long = 0;
    is_create_tail = 0;
    p_cur_src = PFSTR_GetStrPos(p_pattern, 3);

    if ((pf_vol_set.setting & 2) == 0) {
        while (*p_cur_src == ' ' || *p_cur_src == '.') {
            is_create_tail = 1;
            p_cur_src++;
        }

        src_dot = 0;
        prev_src_dot = 0;
        src_pos = 0;
        while (p_cur_src[src_pos] != '\0') {
            if (p_cur_src[src_pos] == '.') {
                if (src_dot != 0) {
                    prev_src_dot = src_dot;
                }
                src_dot = src_pos;
            }
            src_pos++;
        }

        if (src_dot != 0 && p_cur_src[src_dot + 1] == '\0') {
            if (prev_src_dot != 0) {
                src_dot = prev_src_dot;
            }
            is_create_tail = 1;
        }

        dst_pos = 0;
        src_pos = 0;
        num_base = 0;
        num_ext = 0;
        p_name_cnt = &num_base;
        last_width = 1;
        prev_last_width = 1;

        while (num_ext < 3 && p_cur_src[src_pos] != '\0') {
            if (num_base != 8 && (src_dot == 0 || src_pos != src_dot) || p_name_cnt == &num_ext) {
                if ((p_cur_src[src_pos] != ' ') && (p_cur_src[src_pos] != '.')) {
                    width = pf_vol_set.codeset.oem_char_width(p_cur_src + src_pos);

                    if (width != 1) {
                        if (src_pos < src_dot || src_dot == 0) {
                            if ((num_base + width) > 8) {
                                is_create_tail = 1;
                                goto jump;  // remove me!
                            }
                        }

                        if (src_dot != 0 && src_pos > src_dot) {
                            if ((num_ext + width) > 3) {
                                is_create_tail = 1;
                                break;
                            }
                        }

                        if (p_name_cnt == &num_base) {
                            prev_last_width = last_width;
                            last_width = width;
                        }

                        while (width != 0) {
                            wchar = ((((pf_u8)p_cur_src[src_pos]) << 8)) + ((pf_u8)p_cur_src[src_pos + 1]);
                            if (VFiPFPATH_OEM_ConvertFWchar(p_cur_src + src_pos, &t_wchar)) {
                                wchar = t_wchar;
                                is_create_long = 1;
                            }

                            pDest[dst_pos] = (pf_s8)(wchar >> 8);
                            pDest[dst_pos + 1] = (pf_s8)wchar;
                            dst_pos += 2;
                            width -= 2;
                            src_pos += 2;
                            *p_name_cnt += 2;
                        }
                        src_pos -= (pf_u8)1;
                    } else {
                        if (p_name_cnt == &num_base) {
                            prev_last_width = last_width;
                            last_width = 1;
                        }

                        if ((pf_vol_set.config & 0x10000) || (pf_u8)p_cur_src[src_pos] >= 0x80 ||
                            ((pf_u8)p_cur_src[src_pos] >= 0x20 && (pf_valid_fn_char[(pf_u8)p_cur_src[src_pos] - 0x20] & 2))) {
                            pDest[dst_pos] = pf_toupper(p_cur_src[src_pos]);
                            dst_pos++;
                            (*p_name_cnt)++;

                            if (p_cur_src[src_pos] >= 'a' && p_cur_src[src_pos] <= 'z') {
                                is_create_long = 1;
                            }
                        } else {
                            is_create_tail = 1;

                            pDest[dst_pos] = '_';
                            dst_pos++;
                            (*p_name_cnt)++;
                        }
                    }
                } else {
                    is_create_tail = 1;
                }
            } else {
                p_name_cnt = &num_ext;
                if (p_cur_src[src_pos] != '\0') {
                    if (p_cur_src[src_pos] != '.') {
                        is_create_tail = 1;
                    }
                }
                if ((src_dot)) {
                    pDest[dst_pos++] = '.';
                    src_pos = src_dot;
                } else {
                    break;
                }
            }
        jump:
            src_pos++;
        }

        if ((num_ext == 3) && (p_cur_src[src_pos] != '\0')) {
            is_create_tail = 1;
        }
        pDest[dst_pos] = '\0';

        if (is_create_tail != 0) {
            dst_pos = num_base;
            if (num_base == 8) {
                if (last_width != 1) {
                    dst_pos = 8 - last_width;
                } else {
                    dst_pos = 8 - ((prev_last_width == 1) ? 2 : (prev_last_width + 1));
                }
            } else if (num_base == 7) {
                dst_pos = dst_pos - prev_last_width;
            }

            ext_pos = dst_pos + 2;
            if (ext_pos < num_base) {
                move_cnt = num_base - ext_pos;
                for (i = -1; i < ((num_ext + 1)); i++) {
                    pDest[num_base + i] = pDest[(num_base + move_cnt) + i];
                }
            } else if (ext_pos > num_base) {
                move_cnt = ext_pos - num_base;
                for (i = num_ext + 1; i >= 0; i--) {
                    pDest[(num_base + move_cnt) + i] = pDest[num_base + i];
                }
            }

            if (num_base != 0) {
                if ((pf_vol_set.setting & 2) != 2) {
                    pDest[dst_pos++] = '~';
                    pDest[dst_pos++] = '1';
                } else {
                    pDest[(dst_pos++)] = '_';
                    pDest[(dst_pos++)] = '_';
                }
            }
            is_create_long = 1;
        }

        if ((pf_u32)PFSTR_GetCodeMode(p_pattern) == 2) {
            is_create_long = 1;
        }
    } else {
        while ((*p_cur_src) != '\0' && !is_create_tail) {
            if (*p_cur_src != '.' && *p_cur_src != ' ') {
                is_create_tail = 1;
            }
            p_cur_src++;
        }

        if (is_create_tail != 0) {
            pDest[0] = 1;
            pDest[1] = 2;
            pDest[2] = '0';
            pDest[3] = '0';
            pDest[4] = '0';
            pDest[5] = '0';
            pDest[6] = '0';
            pDest[7] = '0';
            pDest[8] = '\0';
        } else {
            pDest[0] = '\0';
        }
        is_create_long = 1;
    }

    return is_create_long;
}

// PFPATH_parseShortNameNumeric - 0x815DEF24, 476 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_parseShortNameNumeric(pf_s8* p_char, pf_u32 count) {
    pf_u32 numeric_cnt;
    pf_u32 pos_tail;
    pf_u32 pos_dot;
    pf_u32 pos_ext;
    pf_u32 pos_slide;
    pf_u32 pos_end;
    pf_s8 numeric[6];

    if (count == 0) {
        return 0;
    }

    for (pos_tail = 1; p_char[pos_tail] != '~'; pos_tail++) {
    }

    for (pos_dot = pos_tail + 1; p_char[pos_dot] != '.' && p_char[pos_dot] != '\0'; pos_dot++) {
    }

    pos_ext = pos_dot + 1;

    if (p_char[pos_dot] == '.' && p_char[pos_ext] != '\0') {
        for (pos_end = pos_ext + 1; p_char[pos_end] != '\0'; pos_end++) {
        }
    } else {
        pos_end = pos_ext;
    }

    numeric_cnt = 0;
    while (count != 0) {
        numeric[numeric_cnt] = (count % 10) + '0';
        numeric_cnt++;
        count /= 10;
    }

    if (pos_tail + numeric_cnt >= pos_ext) {
        pos_slide = pos_tail + numeric_cnt + 1;
        if (pos_slide > 8) {
            pos_slide = 8;
        }
        p_char[pos_slide + 4] = p_char[pos_end];
        p_char[pos_slide + 3] = p_char[pos_ext + 2];
        p_char[pos_slide + 2] = p_char[pos_ext + 1];
        p_char[pos_slide + 1] = p_char[pos_ext];
        p_char[pos_slide] = p_char[pos_dot];
    }

    if (pos_tail + numeric_cnt >= 8) {
        pos_tail = 7 - numeric_cnt;
    }

    p_char[pos_tail] = '~';
    pos_tail++;
    while (numeric_cnt != 0) {
        p_char[pos_tail] = numeric[numeric_cnt - 1];
        numeric_cnt--;
        pos_tail++;
    }

    return 0;
}

// PFPATH_SetSearchPattern - 0x815DF100, 276 bytes
// from RevoEX VF (this repo, Matching)

void PFPATH_SetSearchPattern(pf_s8* p_buf_local, pf_u16* p_buf_unicode, PF_STR* p_pattern) {
    pf_u16 wc[2];

    if (PFSTR_GetCodeMode(p_pattern) == 1) {
        if (pf_strcmp(p_pattern->p_head, (pf_s8*)"*.*") == 0) {
            pf_strcpy(p_buf_local, (pf_s8*)"*");
            return;
        }
        pf_strcpy(p_buf_local, p_pattern->p_head);
        return;
    }
    if (PFSTR_StrCmp(p_pattern, (pf_s8*)"*.*") == 0) {
        wc[0] = '*';
        wc[1] = 0;
        pf_w_strcpy(p_buf_unicode, wc);
    } else {
        pf_w_strcpy(p_buf_unicode, (pf_u16*)p_pattern->p_head);
    }
    if ((pf_vol_set.setting & 0x02) == 0x02) {
        pf_vol_set.setting &= ~(0x01 | 0x02);
        pf_vol_set.setting |= 0x01;
        PFPATH_transformFromUnicodeToNormal(p_buf_local, p_buf_unicode);
        pf_vol_set.setting &= ~(0x01 | 0x02);
        pf_vol_set.setting |= 0x02;
        return;
    }
    PFPATH_transformFromUnicodeToNormal(p_buf_local, p_buf_unicode);
}

// PFPATH_CheckExtShortNameSignature - 0x815DF214, 92 bytes
// NOTE: size-diff(ogws=108) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_CheckExtShortNameSignature(struct PF_STR* p_str) {
    pf_u32 result;
    pf_s8 sig[2];

    sig[0] = *(pf_s8*)"~";
    sig[1] = *(pf_s8*)"1";

    result = 0;

    if (p_str == PF_NULL) {
        return 10;
    }
    if (PFSTR_StrNCmp(p_str, sig, 1, 0, 2) == 0) {
        result = 1;
    }
    return result;
}

// PFPATH_CheckExtShortName - 0x815DF270, 788 bytes
// NOTE: size-diff(ogws=804) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_CheckExtShortName(struct PF_STR* p_str, pf_u32 target, pf_u32 wildcard) {
    pf_u32 result;
    pf_s16 i;
    pf_s16 num;
    pf_u32 is_wildcard;
    pf_s8 sig[2];
    pf_s8* p_c;
    pf_u16* p_wc;

    result = 0;
    is_wildcard = 0;
    sig[0] = *(pf_s8*)"~";
    sig[1] = *(pf_s8*)"1";

    if (p_str == PF_NULL) {
        return 10;
    }

    if (PFSTR_StrNCmp(p_str, sig, target, 0, 2) == 0 ||
        (PFSTR_StrNCmp(p_str, (const pf_s8*)".", target, 0, 1) == 0 &&
         (PFSTR_StrNCmp(p_str, (const pf_s8*)".", target, 1, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", target, 0, 1) == 0)) ||
        PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", target, 0, 1) == 0) {
        i = 2;
        goto jump;
        while (1) {
            if ((pf_u32)PFSTR_GetCodeMode(p_str) == 1) {
                p_c = (pf_s8*)PFSTR_GetStrPos(p_str, target);
                num = p_c[i] - '0';
            } else {
                p_wc = (pf_u16*)PFSTR_GetStrPos(p_str, target);
                num = p_wc[i] - '0';
            }

            if (num < 0 || num > 9) {
                if (wildcard == 0) {
                    break;
                }

                if (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", target, i, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)".", target, i, 1) == 0) {
                    if (wildcard == 1 && PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", target, i, 1) == 0) {
                        is_wildcard = 1;
                    }
                } else {
                    break;
                }
            }

            i++;
        jump:;
            if (i < 8) {
                if ((PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", target, i, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)"/", target, i, 1) == 0) ==
                        0 &&
                    (PFSTR_StrNCmp(p_str, (const pf_s8*)"\0", target, i, 1) != 0 && PFSTR_StrNCmp(p_str, (const pf_s8*)" ", target, i, 1) != 0)) {
                    continue;
                }
            }
            break;
        }

        if (i == 8 || is_wildcard == 1) {
            if (((PFSTR_StrNCmp(p_str, (const pf_s8*)"\0", target, i, 1) == 0) ||
                 (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", target, i, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)"/", target, i, 1) == 0) == 0) ||
                (PFSTR_StrNCmp(p_str, (const pf_s8*)" ", target, i, 1) == 0)) {
                result = i;
            }
        }
    }
    return result;
}

// PFPATH_GetExtShortNameIndex - 0x815DF584, 396 bytes
// NOTE: size-diff(ogws=420) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_u32 PFPATH_GetExtShortNameIndex(struct PF_STR* p_str, pf_u32* p_index) {
    pf_u32 result;
    pf_s16 i;
    pf_s16 num;
    pf_u32 index;
    pf_s8 sig[2];
    pf_s8* p_c;
    pf_u16* p_wc;

    result = 0;
    sig[0] = *(pf_s8*)"~";
    sig[1] = *(pf_s8*)"1";

    if (p_str == PF_NULL || p_index == PF_NULL) {
        return 10;
    }

    if (PFSTR_StrNCmp(p_str, sig, 1, 0, 2) == 0) {
        index = 0;
        i = 2;

        while (i < 8 || PFSTR_StrNCmp(p_str, (const pf_s8*)"", 1, i, 1) != 0) {
            if ((pf_u32)PFSTR_GetCodeMode(p_str) == 1) {
                p_c = (pf_s8*)PFSTR_GetStrPos(p_str, 1);
                num = p_c[i] - '0';
            } else {
                p_wc = (pf_u16*)PFSTR_GetStrPos(p_str, 1);
                num = p_wc[i] - '0';
            }

            if (num < 0 || num > 9) {
                break;
            }
            index = index * 10;
            index = index + num;
            i++;
        }

        if (i == 8) {
            pf_u32 is_valid = 0;
            if (PFSTR_StrNCmp(p_str, (const pf_s8*)"\\", 1, i, 1) == 0 || PFSTR_StrNCmp(p_str, (const pf_s8*)"/", 1, i, 1) == 0) {
                is_valid = 1;
            }

            if (is_valid == 0 || PFSTR_StrNCmp(p_str, 0, 1, i, 1) == 0) {
                *p_index = index;
                result = 1;
            }
        }
    }
    return result;
}

// PFPATH_AdjustExtShortName - 0x815DF710, 100 bytes
// NOTE: size-diff(ogws=116) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

pf_s32 PFPATH_AdjustExtShortName(pf_s8* pName, pf_u32 position) {
    pf_u32 num;
    pf_u32 i;
    pf_u32 div1;
    pf_u32 div2;

    pf_s8** pName_copy;

    if (pName == 0) {
        return 10;
    }

    pName += 7;
    i = 7;
    div1 = 10;
    div2 = 1;

    while ((i > 1) && (position != 0)) {
        num = position / div1;
        num = position - (num * div1);
        pName_copy = &pName;

        if (num != 0) {
            position = position - num;
            num = num / div2;
            // TODO: Fakematch
            *(*pName_copy) = (pf_s8)((*pName) + ((pf_s8)num));
        }

        div1 *= 10;
        i -= 1;
        pName -= 1;
        div2 *= 10;
    }

    return 0;
}
