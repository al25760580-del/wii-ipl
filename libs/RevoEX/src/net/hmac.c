// Decompiled from the RevoEX "net" library.
//
// Keyed-hash message authentication code (HMAC) as specified by RFC 2104,
// generic over the digest interface supplied by the caller (the System Menu
// only ever passes the SHA-1 interface returned by NETGetSHA1Interface).
//
// NOTE: The exact layout of NETHMACContext (0xD4 bytes in total) is inferred
// from the stack usage of matched callers; the original .data of this unit
// contains an additional 0x58 bytes of unknown data that is not yet
// reproduced here.

#include <revolution/net/NETDigest.h>

#include <string.h>

typedef struct NETDigestInterface {
    void (*init)(void* ctx);
    void (*update)(void* ctx, const void* data, u32 len);
    void (*getDigest)(void* ctx, void* digest);
} NETDigestInterface;

typedef struct NETHMACContext {
    const NETDigestInterface* interface;  // 0x00
    u8 innerContext[0x60];                // 0x04
    u8 outerContext[0x60];                // 0x64
    u8 unk_0xC4[0x10];                    // 0xC4
} NETHMACContext;

#define NET_HMAC_BLOCK_SIZE 0x40

void NETHMACInit(NETHMACContext* context, const void* interface, const void* key, u32 keyLen) {
    u8 keyBuffer[NET_HMAC_BLOCK_SIZE];
    u8 tempContext[0x60];
    u8 pad[NET_HMAC_BLOCK_SIZE];
    u32 i;

    context->interface = (const NETDigestInterface*)interface;

    memset(keyBuffer, 0, sizeof(keyBuffer));
    if (keyLen > NET_HMAC_BLOCK_SIZE) {
        context->interface->init(tempContext);
        context->interface->update(tempContext, key, keyLen);
        context->interface->getDigest(tempContext, keyBuffer);
    } else {
        memcpy(keyBuffer, key, keyLen);
    }

    for (i = 0; i < NET_HMAC_BLOCK_SIZE; i++) {
        pad[i] = keyBuffer[i] ^ 0x36;
    }
    context->interface->init(context->innerContext);
    context->interface->update(context->innerContext, pad, NET_HMAC_BLOCK_SIZE);

    for (i = 0; i < NET_HMAC_BLOCK_SIZE; i++) {
        pad[i] = keyBuffer[i] ^ 0x5C;
    }
    context->interface->init(context->outerContext);
    context->interface->update(context->outerContext, pad, NET_HMAC_BLOCK_SIZE);
}

void NETHMACUpdate(NETHMACContext* context, const void* data, u32 len) {
    context->interface->update(context->innerContext, data, len);
}

void NETHMACGetDigest(NETHMACContext* context, void* digest) {
    u8 innerDigest[NET_SHA1_DIGEST_SIZE];

    context->interface->getDigest(context->innerContext, innerDigest);
    context->interface->update(context->outerContext, innerDigest, NET_SHA1_DIGEST_SIZE);
    context->interface->getDigest(context->outerContext, digest);
}

#undef NET_HMAC_BLOCK_SIZE
