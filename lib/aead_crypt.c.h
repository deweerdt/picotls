static size_t aead_encrypt(struct st_ptls_traffic_protection_t *ctx, void *output, const void *input, size_t inlen,
                           uint8_t content_type)
{
    uint8_t aad[5];
    size_t off = 0;

    build_aad(aad, inlen + 1 + ctx->aead->algo->tag_size);
    ptls_aead_encrypt_init(ctx->aead, ctx->seq++, aad, sizeof(aad));
    off += ptls_aead_encrypt_update(ctx->aead, ((uint8_t *)output) + off, input, inlen);
    off += ptls_aead_encrypt_update(ctx->aead, ((uint8_t *)output) + off, &content_type, 1);
    off += ptls_aead_encrypt_final(ctx->aead, ((uint8_t *)output) + off);

    return off;
}

static int aead_decrypt(struct st_ptls_traffic_protection_t *ctx, void *output, size_t *outlen, const void *input, size_t inlen)
{
    uint8_t aad[5];

    build_aad(aad, inlen);
    if ((*outlen = ptls_aead_decrypt(ctx->aead, output, input, inlen, ctx->seq, aad, sizeof(aad))) == SIZE_MAX)
        return PTLS_ALERT_BAD_RECORD_MAC;
    ++ctx->seq;
    return 0;
}
