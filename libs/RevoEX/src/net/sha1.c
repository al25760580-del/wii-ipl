// Decompiled from the RevoEX "net" library.
//
// SHA-1 as specified by FIPS 180-1 (see also RFC 3174), plus the generic
// digest interface returned by NETGetSHA1Interface which is consumed by
// NETHMACInit (hmac.c).
//
// Loop shapes were chosen with tools/size_check.py: CodeWarrior at -O4,p
// unrolls counted `for` loops but not `do/while` loops, and the original
// function sizes tell which is which. With the W schedule expanded by an
// unrolled loop and the four round groups kept as do/while loops,
// NETSHA1iProcessBlock goes from 0x67C to 0x33C against the original 0x39C
// (all-`for` gives 0x67C, all-`do/while` 0x22C), and NETSHA1GetDigest from
// 0x21C to 0x160 against 0x12C. Still not byte-exact: needs objdiff.

#include <revolution/net/NETDigest.h>

#include <string.h>

typedef struct NETDigestInterface {
    void (*init)(void* ctx);
    void (*update)(void* ctx, const void* data, u32 len);
    void (*getDigest)(void* ctx, void* digest);
} NETDigestInterface;

// padlead$2309 lives in .sdata2 (const) and padalign$2310 in .sbss2 (const,
// 8 bytes) in the original binary, so both are const here.
static const u8 padlead = 0x80;
static const u8 padalign[8];

static const NETDigestInterface sha1template = {
    (void (*)(void*))NETSHA1Init,
    (void (*)(void*, const void*, u32))NETSHA1Update,
    (void (*)(void*, void*))NETSHA1GetDigest,
};

void NETSHA1iProcessBlock(NETSHA1Context* context);

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

void NETSHA1Init(NETSHA1Context* context) {
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = 0;
    context->count[1] = 0;
}

void NETSHA1Update(NETSHA1Context* context, const void* input, u32 length) {
    u32 i;
    u32 index = (context->count[0] >> 3) & 0x3F;
    const u8* ptr = (const u8*)input;

    if ((context->count[0] + (length << 3)) < (length << 3)) {
        context->count[1]++;
    }
    context->count[0] += length << 3;
    context->count[1] += length >> 29;

    if (length >= 0x40 - index) {
        memcpy(&context->buffer[index], ptr, 0x40 - index);
        NETSHA1iProcessBlock(context);
        for (i = 0x40 - index; i + 0x3F < length; i += 0x40) {
            memcpy(context->buffer, &ptr[i], 0x40);
            NETSHA1iProcessBlock(context);
        }
        index = 0;
    } else {
        i = 0;
    }

    memcpy(&context->buffer[index], &ptr[i], length - i);
}

void NETSHA1GetDigest(NETSHA1Context* context, void* digest) {
    u8 bits[8];
    u32 i;
    u32 index = (context->count[0] >> 3) & 0x3F;
    u32 padLength = index < 0x38 ? 0x38 - index : 0x78 - index;

    // Length is encoded in bits, as two big-endian words.
    bits[0] = (u8)(context->count[1] >> 24);
    bits[1] = (u8)(context->count[1] >> 16);
    bits[2] = (u8)(context->count[1] >> 8);
    bits[3] = (u8)context->count[1];
    bits[4] = (u8)(context->count[0] >> 24);
    bits[5] = (u8)(context->count[0] >> 16);
    bits[6] = (u8)(context->count[0] >> 8);
    bits[7] = (u8)context->count[0];

    // padlead was already appended, so only the remaining zero bytes follow.
    NETSHA1Update(context, &padlead, 1);
    padLength--;
    while (padLength != 0) {
        u32 chunk = padLength >= sizeof(padalign) ? sizeof(padalign) : padLength;
        NETSHA1Update(context, padalign, chunk);
        padLength -= chunk;
    }
    NETSHA1Update(context, bits, 8);

    i = 0;
    do {
        ((u8*)digest)[i * 4] = (u8)(context->state[i] >> 24);
        ((u8*)digest)[i * 4 + 1] = (u8)(context->state[i] >> 16);
        ((u8*)digest)[i * 4 + 2] = (u8)(context->state[i] >> 8);
        ((u8*)digest)[i * 4 + 3] = (u8)context->state[i];
    } while (++i < 5);

    memset(context, 0, sizeof(NETSHA1Context));
}

void NETSHA1iProcessBlock(NETSHA1Context* context) {
    u32 w[80];
    u32 a;
    u32 b;
    u32 c;
    u32 d;
    u32 e;
    u32 i;
    u32 temp;

    for (i = 0; i < 0x10; i++) {
        w[i] = ((u32)context->buffer[i * 4] << 24) | ((u32)context->buffer[i * 4 + 1] << 16) | ((u32)context->buffer[i * 4 + 2] << 8) |
               (u32)context->buffer[i * 4 + 3];
    }
    do {
        w[i] = ROTATE_LEFT(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    } while (++i < 0x50);

    a = context->state[0];
    b = context->state[1];
    c = context->state[2];
    d = context->state[3];
    e = context->state[4];

    i = 0;
    do {
        temp = ROTATE_LEFT(a, 5) + ((b & c) | (~b & d)) + e + w[i] + 0x5A827999;
        e = d;
        d = c;
        c = ROTATE_LEFT(b, 30);
        b = a;
        a = temp;
    } while (++i < 0x14);
    i = 0x14;
    do {
        temp = ROTATE_LEFT(a, 5) + (b ^ c ^ d) + e + w[i] + 0x6ED9EBA1;
        e = d;
        d = c;
        c = ROTATE_LEFT(b, 30);
        b = a;
        a = temp;
    } while (++i < 0x28);
    i = 0x28;
    do {
        temp = ROTATE_LEFT(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8F1BBCDC;
        e = d;
        d = c;
        c = ROTATE_LEFT(b, 30);
        b = a;
        a = temp;
    } while (++i < 0x3C);
    i = 0x3C;
    do {
        temp = ROTATE_LEFT(a, 5) + (b ^ c ^ d) + e + w[i] + 0xCA62C1D6;
        e = d;
        d = c;
        c = ROTATE_LEFT(b, 30);
        b = a;
        a = temp;
    } while (++i < 0x50);

    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
}

const void* NETGetSHA1Interface(void) {
    return &sha1template;
}

#undef ROTATE_LEFT
