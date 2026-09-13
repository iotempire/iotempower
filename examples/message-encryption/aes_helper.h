#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <bearssl/bearssl_block.h>
#include <bearssl/bearssl_aead.h>

static const uint8_t AES_PSK[16] = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

#define AES_GCM_IV_LEN   12
#define AES_GCM_TAG_LEN  16
#define EEPROM_MAGIC     0xAE
#define EEPROM_ADDR_MAGIC   0
#define EEPROM_ADDR_COUNTER 1
#define EEPROM_ADDR_RX      5

static void eeprom_init() {
    EEPROM.begin(16);
    if (EEPROM.read(EEPROM_ADDR_MAGIC) != EEPROM_MAGIC) {
        // First boot — initialise counters to 0
        EEPROM.write(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
        EEPROM.put(EEPROM_ADDR_COUNTER, (uint32_t)0);
        EEPROM.put(EEPROM_ADDR_RX,      (uint32_t)0);
        EEPROM.commit();
        ulog("EEPROM initialised with fresh counters");
    }
}

static uint32_t eeprom_read_tx_counter() {
    uint32_t val;
    EEPROM.get(EEPROM_ADDR_COUNTER, val);
    return val;
}

static void eeprom_write_tx_counter(uint32_t val) {
    EEPROM.put(EEPROM_ADDR_COUNTER, val);
    EEPROM.commit();
}

static uint32_t eeprom_read_rx_counter() {
    uint32_t val;
    EEPROM.get(EEPROM_ADDR_RX, val);
    return val;
}

static void eeprom_write_rx_counter(uint32_t val) {
    EEPROM.put(EEPROM_ADDR_RX, val);
    EEPROM.commit();
}

static void aes_random_bytes(uint8_t* buf, size_t len) {
    for (size_t i = 0; i < len; i++) buf[i] = (uint8_t)(RANDOM_REG32 & 0xFF);
}

static String bytes_to_hex(const uint8_t* buf, size_t len) {
    String out; out.reserve(len * 2);
    const char* h = "0123456789abcdef";
    for (size_t i = 0; i < len; i++) { out += h[(buf[i]>>4)&0xF]; out += h[buf[i]&0xF]; }
    return out;
}

static bool hex_to_bytes(const String& hex, uint8_t* out, size_t expected_len) {
    if (hex.length() != expected_len * 2) return false;
    auto fh = [](char c) -> int {
        if (c>='0'&&c<='9') return c-'0';
        if (c>='a'&&c<='f') return c-'a'+10;
        if (c>='A'&&c<='F') return c-'A'+10;
        return -1;
    };
    for (size_t i = 0; i < expected_len; i++) {
        int hi = fh(hex[i*2]), lo = fh(hex[i*2+1]);
        if (hi<0||lo<0) return false;
        out[i] = (uint8_t)((hi<<4)|lo);
    }
    return true;
}


// ENCRYPT


static String aes_publish_secure(const String& plaintext) {
    // 1. Read and increment TX counter
    uint32_t counter = eeprom_read_tx_counter() + 1;
    eeprom_write_tx_counter(counter);

    // 2. Build plaintext: 4-byte big-endian counter + message
    size_t msglen = plaintext.length();
    size_t ptlen  = 4 + msglen;
    uint8_t* pt   = (uint8_t*)malloc(ptlen);
    if (!pt) return "ERR_ALLOC";

    pt[0] = (counter >> 24) & 0xFF;
    pt[1] = (counter >> 16) & 0xFF;
    pt[2] = (counter >>  8) & 0xFF;
    pt[3] =  counter        & 0xFF;
    memcpy(pt + 4, plaintext.c_str(), msglen);

    // 3. Generate random IV
    uint8_t iv[AES_GCM_IV_LEN];
    aes_random_bytes(iv, AES_GCM_IV_LEN);

    // 4. AES-128-GCM encrypt in-place
    uint8_t tag[AES_GCM_TAG_LEN];
    br_gcm_context gcm;
    br_aes_small_ctr_keys aes_ctx;
    br_aes_small_ctr_init(&aes_ctx, AES_PSK, 16);
    br_gcm_init(&gcm, &aes_ctx.vtable, br_ghash_ctmul32);
    br_gcm_reset(&gcm, iv, AES_GCM_IV_LEN);
    br_gcm_flip(&gcm);
    br_gcm_run(&gcm, 1, pt, ptlen);
    br_gcm_get_tag(&gcm, tag);

    String result = bytes_to_hex(iv, AES_GCM_IV_LEN)
                  + bytes_to_hex(tag, AES_GCM_TAG_LEN)
                  + bytes_to_hex(pt, ptlen);
    free(pt);

    ulog("TX counter: %u", counter);
    return result;
}


// DECRYPT


static String aes_receive_secure(const String& hex_msg) {
    const size_t MIN_HEX = (AES_GCM_IV_LEN + AES_GCM_TAG_LEN + 4) * 2;
    if (hex_msg.length() < MIN_HEX || hex_msg.length() % 2 != 0) {
        ulog("AES-GCM RX: invalid length");
        return "";
    }

    size_t ct_len = (hex_msg.length() / 2) - AES_GCM_IV_LEN - AES_GCM_TAG_LEN;

    uint8_t iv[AES_GCM_IV_LEN], tag[AES_GCM_TAG_LEN];
    uint8_t* ct = (uint8_t*)malloc(ct_len);
    if (!ct) return "";

    bool ok = hex_to_bytes(hex_msg.substring(0, AES_GCM_IV_LEN * 2), iv, AES_GCM_IV_LEN)
           && hex_to_bytes(hex_msg.substring(AES_GCM_IV_LEN * 2,
                           (AES_GCM_IV_LEN + AES_GCM_TAG_LEN) * 2), tag, AES_GCM_TAG_LEN)
           && hex_to_bytes(hex_msg.substring((AES_GCM_IV_LEN + AES_GCM_TAG_LEN) * 2), ct, ct_len);

    if (!ok) { free(ct); ulog("AES-GCM RX: hex decode failed"); return ""; }

    br_gcm_context gcm;
    br_aes_small_ctr_keys aes_ctx;
    br_aes_small_ctr_init(&aes_ctx, AES_PSK, 16);
    br_gcm_init(&gcm, &aes_ctx.vtable, br_ghash_ctmul32);
    br_gcm_reset(&gcm, iv, AES_GCM_IV_LEN);
    br_gcm_flip(&gcm);
    br_gcm_run(&gcm, 0, ct, ct_len);

    if (!br_gcm_check_tag(&gcm, tag)) {
        free(ct); ulog("AES-GCM RX: auth FAILED - rejected"); return "";
    }

    // Extract and validate counter (first 4 bytes of plaintext)
    if (ct_len < 4) { free(ct); ulog("AES-GCM RX: too short for counter"); return ""; }

    uint32_t rx_counter = ((uint32_t)ct[0] << 24) | ((uint32_t)ct[1] << 16)
                        | ((uint32_t)ct[2] <<  8) |  (uint32_t)ct[3];
    uint32_t last_rx    = eeprom_read_rx_counter();

    if (rx_counter <= last_rx) {
        free(ct);
        ulog("AES-GCM RX: REPLAY DETECTED - counter %u <= last seen %u",
     rx_counter, last_rx);
        return "";
    }

    eeprom_write_rx_counter(rx_counter);

    String result;
    result.reserve(ct_len - 4);

    for (size_t i = 4; i < ct_len; i++) {
        result += (char)ct[i];
    }
    free(ct);
    ulog("AES-GCM RX: accepted, counter=%u", rx_counter);
    return result;
}