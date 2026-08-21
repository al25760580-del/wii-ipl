// Decompiled from the RevoEX "net" library.
//
// Standard MD5 as specified by RFC 1321 (RSA Data Security, Inc.
// MD5 message-digest algorithm).
//
// NOTE: The original .data section of this unit additionally contains a
// 0xC0-byte table of 48 u32s (`k$2351` in symbols.txt) whose purpose is
// not yet known, so this unit is expected to be only partially matched
// until that table is identified.

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
    u32 i;
    for (i = 0; i < length; i++) {
        *output++ = (u8)(*input & 0xFF);
        *output++ = (u8)((*input >> 8) & 0xFF);
        *output++ = (u8)((*input >> 16) & 0xFF);
        *output++ = (u8)((*input >> 24) & 0xFF);
        input++;
    }
}

static void Decode(u32* output, const u8* input, u32 length) {
    u32 i;
    for (i = 0; i < length; i++) {
        *output++ = ((u32)*input) | ((u32)input[1] << 8) | ((u32)input[2] << 16) | ((u32)input[3] << 24);
        input += 4;
    }
}

static void MD5_memcpy(u8* output, const u8* input, u32 length) {
    u32 i;
    for (i = 0; i < length; i++) {
        output[i] = input[i];
    }
}

static void MD5_memset(u8* output, int value, u32 length) {
    u32 i;
    for (i = 0; i < length; i++) {
        output[i] = (u8)value;
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
        MD5_memcpy(&context->buffer8[index], ptr, partLength);
        ProcessBlock(context, context->buffer8);
        for (; length - partLength >= 0x40; partLength += 0x40) {
            ProcessBlock(context, &ptr[partLength]);
        }
        index = 0;
    } else {
        partLength = 0;
    }

    MD5_memcpy(&context->buffer8[index], &ptr[partLength], length - partLength);
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

    MD5_memset(padding, 0, sizeof(padding));
    padding[0] = 0x80;
    NETMD5Update(context, padding, padLength);

    NETMD5Update(context, bits, 8);
    Encode((u8*)digest, context->state, 4);

    MD5_memset((u8*)context, 0, sizeof(NETMD5Context));
}

static void ProcessBlock(NETMD5Context* context, const u8* block) {
    u32 a = context->state[0];
    u32 b = context->state[1];
    u32 c = context->state[2];
    u32 d = context->state[3];
    u32 x[16];

    Decode(x, block, 16);

    // Round 1
    FF(a, b, c, d, x[0], 7, t[0]);
    FF(d, a, b, c, x[1], 12, t[1]);
    FF(c, d, a, b, x[2], 17, t[2]);
    FF(b, c, d, a, x[3], 22, t[3]);
    FF(a, b, c, d, x[4], 7, t[4]);
    FF(d, a, b, c, x[5], 12, t[5]);
    FF(c, d, a, b, x[6], 17, t[6]);
    FF(b, c, d, a, x[7], 22, t[7]);
    FF(a, b, c, d, x[8], 7, t[8]);
    FF(d, a, b, c, x[9], 12, t[9]);
    FF(c, d, a, b, x[10], 17, t[10]);
    FF(b, c, d, a, x[11], 22, t[11]);
    FF(a, b, c, d, x[12], 7, t[12]);
    FF(d, a, b, c, x[13], 12, t[13]);
    FF(c, d, a, b, x[14], 17, t[14]);
    FF(b, c, d, a, x[15], 22, t[15]);

    // Round 2
    GG(a, b, c, d, x[1], 5, t[16]);
    GG(d, a, b, c, x[6], 9, t[17]);
    GG(c, d, a, b, x[11], 14, t[18]);
    GG(b, c, d, a, x[0], 20, t[19]);
    GG(a, b, c, d, x[5], 5, t[20]);
    GG(d, a, b, c, x[10], 9, t[21]);
    GG(c, d, a, b, x[15], 14, t[22]);
    GG(b, c, d, a, x[4], 20, t[23]);
    GG(a, b, c, d, x[9], 5, t[24]);
    GG(d, a, b, c, x[14], 9, t[25]);
    GG(c, d, a, b, x[3], 14, t[26]);
    GG(b, c, d, a, x[8], 20, t[27]);
    GG(a, b, c, d, x[13], 5, t[28]);
    GG(d, a, b, c, x[2], 9, t[29]);
    GG(c, d, a, b, x[7], 14, t[30]);
    GG(b, c, d, a, x[12], 20, t[31]);

    // Round 3
    HH(a, b, c, d, x[5], 4, t[32]);
    HH(d, a, b, c, x[8], 11, t[33]);
    HH(c, d, a, b, x[11], 16, t[34]);
    HH(b, c, d, a, x[14], 23, t[35]);
    HH(a, b, c, d, x[1], 4, t[36]);
    HH(d, a, b, c, x[4], 11, t[37]);
    HH(c, d, a, b, x[7], 16, t[38]);
    HH(b, c, d, a, x[10], 23, t[39]);
    HH(a, b, c, d, x[13], 4, t[40]);
    HH(d, a, b, c, x[0], 11, t[41]);
    HH(c, d, a, b, x[3], 16, t[42]);
    HH(b, c, d, a, x[6], 23, t[43]);
    HH(a, b, c, d, x[9], 4, t[44]);
    HH(d, a, b, c, x[12], 11, t[45]);
    HH(c, d, a, b, x[15], 16, t[46]);
    HH(b, c, d, a, x[2], 23, t[47]);

    // Round 4
    II(a, b, c, d, x[0], 6, t[48]);
    II(d, a, b, c, x[7], 10, t[49]);
    II(c, d, a, b, x[14], 15, t[50]);
    II(b, c, d, a, x[5], 21, t[51]);
    II(a, b, c, d, x[12], 6, t[52]);
    II(d, a, b, c, x[3], 10, t[53]);
    II(c, d, a, b, x[10], 15, t[54]);
    II(b, c, d, a, x[1], 21, t[55]);
    II(a, b, c, d, x[8], 6, t[56]);
    II(d, a, b, c, x[15], 10, t[57]);
    II(c, d, a, b, x[6], 15, t[58]);
    II(b, c, d, a, x[13], 21, t[59]);
    II(a, b, c, d, x[4], 6, t[60]);
    II(d, a, b, c, x[11], 10, t[61]);
    II(c, d, a, b, x[2], 15, t[62]);
    II(b, c, d, a, x[9], 21, t[63]);

    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
}

#undef F
#undef G
#undef H
#undef I
#undef ROTATE_LEFT
#undef FF
#undef GG
#undef HH
#undef II
