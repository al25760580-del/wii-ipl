// Decompiled from the RevoEX "net" library.
//
// Standard MD5 as specified by RFC 1321 (RSA Data Security, Inc.
// MD5 message-digest algorithm).
//
// The original .data of this unit holds two tables: `t$2350` (0x100 = the
// 64 sine constants) and `k$2351` (0xC0 = 48 u32). 48 is exactly the number
// of rounds whose message-word index is not simply the round number, i.e.
// rounds 16..63, so `k` is the precomputed x[] index table and the compressor
// is written as four 4x-unrolled loops rather than fully unrolled. That
// structure, together with the do/while decode and zeroize loops (a plain
// `for` gets unrolled by -O4,p and blows the function up), brings
// ProcessBlock from 0xA74 down to 0x4B8 against the original 0x4C8; the
// remaining 0x10 needs objdiff. Verified against the RFC 1321 test suite.

#include <revolution/net/NETDigest.h>

#include <string.h>

static u32 t[64] = {
    0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE, 0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501, 0x698098D8, 0x8B44F7AF, 0xFFFF5BB1,
    0x895CD7BE, 0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821, 0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA, 0xD62F105D, 0x02441453,
    0xD8A1E681, 0xE7D3FBC8, 0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED, 0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A, 0xFFFA3942,
    0x8771F681, 0x6D9D6122, 0xFDE5380C, 0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70, 0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05,
    0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665, 0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039, 0x655B59C3, 0x8F0CCC92, 0xFFEFF47D,
    0x85845DD1, 0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1, 0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391,
};

static u32 k[48] = {
    1,  6, 11, 0, 5, 10, 15, 4, 9, 14, 3,  8, 13, 2, 7,  12, 5, 8,  11, 14, 1, 4,  7, 10,
    13, 0, 3,  6, 9, 12, 15, 2, 0, 7,  14, 5, 12, 3, 10, 1,  8, 15, 6,  13, 4, 11, 2, 9,
};

static void ProcessBlock(NETMD5Context* context, const u8* block);

#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | ~(z)))

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FF(a, b, c, d, x, s, ac)                                                                                                                     \
    {                                                                                                                                                \
        (a) += F((b), (c), (d)) + (x) + ac;                                                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                                                 \
        (a) += (b);                                                                                                                                  \
    }
#define GG(a, b, c, d, x, s, ac)                                                                                                                     \
    {                                                                                                                                                \
        (a) += G((b), (c), (d)) + (x) + ac;                                                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                                                 \
        (a) += (b);                                                                                                                                  \
    }
#define HH(a, b, c, d, x, s, ac)                                                                                                                     \
    {                                                                                                                                                \
        (a) += H((b), (c), (d)) + (x) + ac;                                                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                                                 \
        (a) += (b);                                                                                                                                  \
    }
#define II(a, b, c, d, x, s, ac)                                                                                                                     \
    {                                                                                                                                                \
        (a) += I((b), (c), (d)) + (x) + ac;                                                                                                          \
        (a) = ROTATE_LEFT((a), (s));                                                                                                                 \
        (a) += (b);                                                                                                                                  \
    }

static void Encode(u8* output, const u32* input, u32 length) {
    u32 i = 0;
    do {
        *output++ = (u8)(*input & 0xFF);
        *output++ = (u8)((*input >> 8) & 0xFF);
        *output++ = (u8)((*input >> 16) & 0xFF);
        *output++ = (u8)((*input >> 24) & 0xFF);
        input++;
    } while (++i < length);
}

static void Decode(u32* output, const u8* input, u32 length) {
    u32 i;
    for (i = 0; i < length; i++) {
        *output++ = ((u32)*input) | ((u32)input[1] << 8) | ((u32)input[2] << 16) | ((u32)input[3] << 24);
        input += 4;
    }
}

void NETMD5Init(NETMD5Context* context) {
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->length = 0;
}

void NETMD5Update(NETMD5Context* context, const void* input, u32 length) {
    u32 index = (u32)((context->length >> 3) & 0x3F);
    const u8* ptr = (const u8*)input;
    u32 partLength;

    context->length += (u64)length << 3;

    partLength = 0x40 - index;
    if (length >= partLength) {
        memcpy(&context->buffer8[index], ptr, partLength);
        ProcessBlock(context, context->buffer8);
        for (; length - partLength >= 0x40; partLength += 0x40) {
            ProcessBlock(context, &ptr[partLength]);
        }
        index = 0;
    } else {
        partLength = 0;
    }

    memcpy(&context->buffer8[index], &ptr[partLength], length - partLength);
}

void NETMD5GetDigest(NETMD5Context* context, void* digest) {
    u8 bits[8];
    u8 padding[0x40];
    u32 lengthWords[2];
    u32 index = (u32)((context->length >> 3) & 0x3F);
    u32 padLength = index < 0x38 ? 0x38 - index : 0x78 - index;

    lengthWords[0] = (u32)context->length;
    lengthWords[1] = (u32)(context->length >> 32);
    Encode(bits, lengthWords, 2);

    memset(padding, 0, sizeof(padding));
    padding[0] = 0x80;
    NETMD5Update(context, padding, padLength);

    NETMD5Update(context, bits, 8);
    Encode((u8*)digest, context->state, 4);

    memset(context, 0, sizeof(NETMD5Context));
}

static void ProcessBlock(NETMD5Context* context, const u8* block) {
    u32 a = context->state[0];
    u32 b = context->state[1];
    u32 c = context->state[2];
    u32 d = context->state[3];
    u32 x[16];
    int i;
    u32* xp;
    const u8* bp;

    i = 0;
    bp = block;
    xp = x;
    do {
        xp[i] = (u32)bp[0] | ((u32)bp[1] << 8) | ((u32)bp[2] << 16) | ((u32)bp[3] << 24);
        bp += 4;
    } while (++i < 16);

    for (i = 0; i < 4; i++) {
        FF(a, b, c, d, x[i * 4 + 0], 7, t[i * 4 + 0]);
        FF(d, a, b, c, x[i * 4 + 1], 12, t[i * 4 + 1]);
        FF(c, d, a, b, x[i * 4 + 2], 17, t[i * 4 + 2]);
        FF(b, c, d, a, x[i * 4 + 3], 22, t[i * 4 + 3]);
    }
    for (i = 0; i < 4; i++) {
        GG(a, b, c, d, x[k[i * 4 + 0]], 5, t[16 + i * 4 + 0]);
        GG(d, a, b, c, x[k[i * 4 + 1]], 9, t[16 + i * 4 + 1]);
        GG(c, d, a, b, x[k[i * 4 + 2]], 14, t[16 + i * 4 + 2]);
        GG(b, c, d, a, x[k[i * 4 + 3]], 20, t[16 + i * 4 + 3]);
    }
    for (i = 4; i < 8; i++) {
        HH(a, b, c, d, x[k[i * 4 + 0]], 4, t[16 + i * 4 + 0]);
        HH(d, a, b, c, x[k[i * 4 + 1]], 11, t[16 + i * 4 + 1]);
        HH(c, d, a, b, x[k[i * 4 + 2]], 16, t[16 + i * 4 + 2]);
        HH(b, c, d, a, x[k[i * 4 + 3]], 23, t[16 + i * 4 + 3]);
    }
    for (i = 8; i < 12; i++) {
        II(a, b, c, d, x[k[i * 4 + 0]], 6, t[16 + i * 4 + 0]);
        II(d, a, b, c, x[k[i * 4 + 1]], 10, t[16 + i * 4 + 1]);
        II(c, d, a, b, x[k[i * 4 + 2]], 15, t[16 + i * 4 + 2]);
        II(b, c, d, a, x[k[i * 4 + 3]], 21, t[16 + i * 4 + 3]);
    }

    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;

    xp = x;
    do {
        *xp++ = 0;
    } while (xp != &x[16]);
}
