// Decompiled from the RevoEX "net" library.
//
// Keyed-hash message authentication code (HMAC) as specified by RFC 2104,
// generic over the digest interface supplied by the caller (the System Menu
// only ever passes the SHA-1 interface returned by NETGetSHA1Interface).
//
// Structure recovered from the original function sizes (tools/size_check.py):
// the unit has exactly three functions and they sum to its whole .text span,
// and NETHMACGetDigest is 0x1A4 - far too large for "finalize inner, feed the
// outer, finalize outer". What fits is a context that keeps the *padded key*
// and a single digest context: the ipad pass happens in NETHMACInit and the
// opad pass is rebuilt in NETHMACGetDigest, reusing the digest context once
// the inner digest has been extracted. That also explains the size: 0xD4 is
// exactly interface + one 0x60 context + a 0x40 key block + 0x30 spare, while
// two whole digest contexts would leave no room for the key.
//
// The pad loops are counted `for` loops (CodeWarrior at -O4,p unrolls them,
// which the sizes require) and the key block is filled by a single
// copy-or-zero pass rather than memset + memcpy.
//
//   NETHMACInit       0x240 built vs 0x23C original
//   NETHMACGetDigest  0x198 built vs 0x1A4 original
//   NETHMACUpdate     0x14  built vs 0x10  original
//
// NOT YET VERIFIED AGAINST THE ORIGINAL BINARY (no objdiff in this fork):
// the unit also owns 0x58 bytes of .data and 0x10 of .sdata that are still
// unaccounted for, and `sha1template` is 0x20 bytes in .rodata, i.e. the
// digest interface has eight words, not the three function pointers used
// here - the remaining fields (context size, block size, digest size?) are
// very likely what the missing bytes belong to.
//
// Functionally verified on the host against the RFC 2202 HMAC-SHA1 suite.

#include <revolution/net/NETDigest.h>

#include <string.h>

typedef struct NETDigestInterface {
    void (*init)(void* ctx);
    void (*update)(void* ctx, const void* data, u32 len);
    void (*getDigest)(void* ctx, void* digest);
} NETDigestInterface;

#define NET_HMAC_BLOCK_SIZE 0x40

typedef struct NETHMACContext {
    const NETDigestInterface* interface;  // 0x00
    u8 digestContext[0x60];               // 0x04
    u8 key[NET_HMAC_BLOCK_SIZE];          // 0x64
    u8 unk_0xA4[0x30];                    // 0xA4
} NETHMACContext;

void NETHMACInit(NETHMACContext* context, const void* interface, const void* key, u32 keyLen) {
    u8 keyDigest[NET_SHA1_DIGEST_SIZE];
    u8 pad[NET_HMAC_BLOCK_SIZE];
    u32 i;

    context->interface = (const NETDigestInterface*)interface;

    // A key longer than one block is replaced by its own digest.
    if (keyLen > NET_HMAC_BLOCK_SIZE) {
        context->interface->init(context->digestContext);
        context->interface->update(context->digestContext, key, keyLen);
        context->interface->getDigest(context->digestContext, keyDigest);
        key = keyDigest;
        keyLen = NET_SHA1_DIGEST_SIZE;
    }

    for (i = 0; i < NET_HMAC_BLOCK_SIZE; i++) {
        context->key[i] = i < keyLen ? ((const u8*)key)[i] : 0;
    }

    for (i = 0; i < NET_HMAC_BLOCK_SIZE; i++) {
        pad[i] = context->key[i] ^ 0x36;
    }

    context->interface->init(context->digestContext);
    context->interface->update(context->digestContext, pad, NET_HMAC_BLOCK_SIZE);
}

void NETHMACUpdate(NETHMACContext* context, const void* data, u32 len) {
    context->interface->update(context->digestContext, data, len);
}

void NETHMACGetDigest(NETHMACContext* context, void* digest) {
    u8 innerDigest[NET_SHA1_DIGEST_SIZE];
    u8 pad[NET_HMAC_BLOCK_SIZE];
    u32 i;

    context->interface->getDigest(context->digestContext, innerDigest);

    for (i = 0; i < NET_HMAC_BLOCK_SIZE; i++) {
        pad[i] = context->key[i] ^ 0x5C;
    }

    context->interface->init(context->digestContext);
    context->interface->update(context->digestContext, pad, NET_HMAC_BLOCK_SIZE);
    context->interface->update(context->digestContext, innerDigest, NET_SHA1_DIGEST_SIZE);
    context->interface->getDigest(context->digestContext, digest);
}

#undef NET_HMAC_BLOCK_SIZE
