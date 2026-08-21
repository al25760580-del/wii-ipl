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

#include <private/fa/PrFILE2/common/pf_w_clib.h>
#include <private/fa/PrFILE2/pf_types.h>

// pf_toupper - 0x815C5F9C, 24 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 pf_toupper(pf_s32 c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }

    return c;
}

// pf_memcpy - 0x815C5FB4, 296 bytes
// from ogws (Wii Sports VF, CC0)

void* pf_memcpy(void* dst, void* src, pf_u32 length) {
    pf_s8* d;
    const pf_s8* s;
    pf_u32* ld;
    const pf_u32* ls;

    ld = dst;
    ls = src;
    if (((pf_u32)ld & 3) == 0 && ((pf_u32)ls & 3) == 0) {
        while (length > 3) {
            *ld++ = *ls++;
            length -= 4;
        }
    }

    d = (pf_s8*)ld;
    s = (const pf_s8*)ls;
    while (length != 0) {
        *d++ = *s++;
        length--;
    }

    return dst;
}

// pf_memset - 0x815C60DC, 240 bytes
// NOTE: size-diff(ogws=248) - verify against objdiff
// from ogws (Wii Sports VF, CC0)

void* pf_memset(void* dst, pf_s32 c, pf_u32 length) {
    pf_s8* d;
    pf_u32* ld;
    pf_u32 lc;
    for (d = dst; ((((pf_u32)d) & 3) != 0) && length; ++d) {
        --length;
        *d = c;
    }

    ld = (pf_u32*)d;
    while (length > 3) {
        *(ld++) = (c | (c << 8)) | ((c << 24) | (c << 16));
        length -= 4;
    }

    for (lc = 0; (length--) != 0; ++lc) {
        *((pf_s8*)ld + lc) = c;
    }

    return dst;
}

// pf_memcmp - 0x815C61CC, 120 bytes
pf_s32 pf_memcmp(const void* s1, const void* s2, pf_u32 n) {
    const pf_u8* p1;
    const pf_u8* p2;

    p1 = s1;
    p2 = s2;
    for (; n != 0; n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

// pf_strlen - 0x815C6244, 32 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 pf_strlen(const pf_s8* s) {
    const pf_s8* t;
    t = s;
    while (*t) {
        t++;
    }
    return t - s;
}

// pf_strcpy - 0x815C6264, 36 bytes
// from ogws (Wii Sports VF, CC0)

pf_s8* pf_strcpy(pf_s8* dst, const pf_s8* src) {
    pf_s8* d;  // Present in DWARF but unused here.
    goto idk;
    while ((*src) != '\0') {
        src++;
        dst++;
    idk:
        *dst = *src;
    }
}

// pf_strcmp - 0x815C6288, 56 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 pf_strcmp(const pf_s8* s1, const pf_s8* s2) {
    const pf_u8* p1;
    const pf_u8* p2;

    p1 = (const pf_u8*)s1;
    p2 = (const pf_u8*)s2;
    while (*p1 && *p2 && *p1 == *p2) {
        ++p1;
        ++p2;
    }
    return *p1 - *p2;
}

// pf_strncmp - 0x815C62C0, 76 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 pf_strncmp(const pf_s8* s1, const pf_s8* s2, pf_u32 length) {
    const pf_u8* p1;
    const pf_u8* p2;

    p1 = (const pf_u8*)s1;
    p2 = (const pf_u8*)s2;
    while (length-- != 0) {
        if (!*p1 || !*p2 || *p1 != *p2) {
            return *p1 - *p2;
        }
        ++p1;
        ++p2;
    }
    return 0;
}

// pf_w_strlen - 0x815C630C, 36 bytes
// from ogws (Wii Sports VF, CC0)

pf_u32 pf_w_strlen(const pf_u16* s) {
    const pf_u16* t;

    t = s;
    while (*t != L'\0') {
        t++;
    }

    return ((pf_s32)t - (pf_s32)s) >> 1;
}

// pf_w_strcpy - 0x815C6330, 36 bytes
// from ogws (Wii Sports VF, CC0)

pf_u16* pf_w_strcpy(pf_u16* dst, const pf_u16* src) {
    pf_u16* d;  // Present in DWARF but unused here.

    goto jump;
    while (*src != L'\0') {
        src++;
        dst++;
    jump:
        *dst = *src;
    }
}

// pf_w_strcmp - 0x815C6354, 56 bytes
pf_s32 pf_w_strcmp(const pf_u16* s1, const pf_u16* s2) {
    for (; *s1 != 0 && *s1 == *s2; s1++, s2++) {
        ;
    }
    return *s1 - *s2;
}

// pf_w_strncmp - 0x815C638C, 76 bytes
// from ogws (Wii Sports VF, CC0)

pf_s32 pf_w_strncmp(const pf_u16* s1, const pf_u16* s2, pf_u32 length) {
    const pf_u16* p1;  // Present in DWARF but unused here.
    const pf_u16* p2;  // Present in DWARF but unused here.

    while (length != 0) {
        if (*s1 == L'\0' || *s2 == L'\0' || *s1 != *s2) {
            return (int)(*s1 - *s2);
        }

        s1++;
        s2++;
        length--;
    }
    return 0;
}
