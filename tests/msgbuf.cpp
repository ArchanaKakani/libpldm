#include <endian.h>

#include <cfloat>

#include <gtest/gtest.h>

/* We're exercising the implementation so disable the asserts for now */
#ifndef NDEBUG
#define NDEBUG 1
#endif

#include "msgbuf.h"

TEST(msgbuf, init_bad_ctx)
{
    EXPECT_NE(pldm_msgbuf_init_cc(NULL, 0, NULL, 0), PLDM_SUCCESS);
}

TEST(msgbuf, init_bad_minsize)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};

    EXPECT_NE(pldm_msgbuf_init_cc(ctx, sizeof(buf) + 1U, buf, sizeof(buf)),
              PLDM_SUCCESS);
}

TEST(msgbuf, init_bad_buf)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    EXPECT_NE(pldm_msgbuf_init_cc(ctx, 0, NULL, 0), PLDM_SUCCESS);
}

TEST(msgbuf, init_bad_len)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};

    EXPECT_NE(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, SIZE_MAX),
              PLDM_SUCCESS);
}

TEST(msgbuf, init_overflow)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    void* buf = (void*)UINTPTR_MAX;

    EXPECT_NE(pldm_msgbuf_init_cc(ctx, 0, buf, 2), PLDM_SUCCESS);
}

TEST(msgbuf, init_success)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};

    EXPECT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
}

TEST(msgbuf, destroy_none)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, destroy_exact)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {0xa5};
    uint8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, 0xa5);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, destroy_over)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {0xa5};
    uint8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    ASSERT_EQ(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    ASSERT_EQ(val, 0xa5);
    EXPECT_NE(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, destroy_under)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[2] = {0x5a, 0xa5};
    uint8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, 0x5a);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_one_uint8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {0xa5};
    uint8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, 0xa5);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_over_uint8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};
    uint8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_under_uint8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint8_t buf[1] = {};
    uint8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN;
    EXPECT_NE(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_one_int8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int8_t buf[1] = {-1};
    int8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_int8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, -1);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_over_int8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int8_t buf[1] = {};
    int8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_int8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_under_int8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint8_t buf[1] = {};
    int8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN;
    EXPECT_NE(pldm_msgbuf_extract_int8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_one_uint16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint16_t buf[1] = {htole16(0x5aa5)};
    uint16_t val = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint16(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, 0x5aa5);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_under_uint16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint16_t buf[1] = {};
    uint16_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_extract_uint16(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_over_uint16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint16_t buf[1] = {};
    uint16_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_uint16(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_one_int16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int16_t buf[1] = {(int16_t)(htole16((uint16_t)INT16_MIN))};
    int16_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_int16(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, INT16_MIN);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_over_int16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int16_t buf[1] = {};
    int16_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_int16(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_under_int16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    int16_t buf[1] = {};
    int16_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_extract_int16(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_one_uint32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint32_t buf[1] = {htole32(0x5a00ffa5)};
    uint32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, 0x5a00ffa5);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_over_uint32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint32_t buf[1] = {};
    uint32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_uint32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_under_uint32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint32_t buf[1] = {};
    uint32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_extract_uint32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_one_int32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int32_t buf[1] = {(int32_t)(htole32((uint32_t)(INT32_MIN)))};
    int32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_int32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, INT32_MIN);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_over_int32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int32_t buf[1] = {};
    int32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_int32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_under_int32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    int32_t buf[1] = {};
    int32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_extract_int32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_one_real32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint32_t buf[1] = {};
    uint32_t xform;
    real32_t val;

    val = FLT_MAX;
    memcpy(&xform, &val, sizeof(val));
    buf[0] = htole32(xform);
    val = 0;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_real32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(val, FLT_MAX);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_over_real32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    real32_t buf[1] = {};
    real32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_real32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_under_real32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    real32_t buf[1] = {};
    real32_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_extract_real32(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_array_uint8_buf0_req0)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};
    uint8_t arr[1];

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_array_uint8(ctx, arr, 0), PLDM_SUCCESS);
    ASSERT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_array_uint8_buf1_req1)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};
    uint8_t arr[1];

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_array_uint8(ctx, arr, sizeof(arr)),
              PLDM_SUCCESS);
    EXPECT_EQ(arr[0], 0);
    ASSERT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, extract_array_uint8_buf1_req2)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};
    uint8_t arr[2];

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_array_uint8(ctx, arr, sizeof(arr)),
              PLDM_SUCCESS);
    ASSERT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, extract_under_array_uint8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};
    uint8_t arr[1];

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN;
    EXPECT_NE(pldm_msgbuf_extract_array_uint8(ctx, arr, 1), PLDM_SUCCESS);
    ASSERT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, consumed_under)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy_consumed(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, consumed_exact)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};
    uint8_t val;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint8(ctx, &val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy_consumed(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, consumed_over)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t buf[1] = {};
    uint8_t val[2];

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint8(ctx, &val[0]), PLDM_SUCCESS);
    EXPECT_NE(pldm_msgbuf_extract_uint8(ctx, &val[1]), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy_consumed(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_insert_int32_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int32_t src = -12345;
    int32_t checkVal = 0;
    uint8_t buf[sizeof(int32_t)] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_int32(ctx, src), PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_int32(ctxExtract, &checkVal), PLDM_SUCCESS);

    EXPECT_EQ(src, checkVal);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, insert_under_int32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    int32_t buf[1] = {};
    int32_t val = 0;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_insert_int32(ctx, val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_insert_uint32_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint32_t src = 0xf1223344;
    uint32_t checkVal = 0;
    uint8_t buf[sizeof(uint32_t)] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_uint32(ctx, src), PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint32(ctxExtract, &checkVal), PLDM_SUCCESS);

    EXPECT_EQ(src, checkVal);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, insert_under_uint32)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint32_t buf[1] = {};
    uint32_t val = 0;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_insert_uint32(ctx, val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_insert_uint16_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint16_t src = 0xf344;
    uint16_t checkVal = 0;
    uint8_t buf[sizeof(uint16_t)] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(uint16_t)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_uint16(ctx, src), PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint16(ctxExtract, &checkVal), PLDM_SUCCESS);

    EXPECT_EQ(src, checkVal);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, insert_under_uint16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint16_t buf[1] = {};
    uint16_t val = 0;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_insert_uint16(ctx, val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_insert_int16_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int16_t src = -12;
    int16_t checkVal = 0;
    uint8_t buf[sizeof(int16_t)] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(uint16_t)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_int16(ctx, src), PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_int16(ctxExtract, &checkVal), PLDM_SUCCESS);

    EXPECT_EQ(src, checkVal);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, insert_under_int16)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    int16_t buf[1] = {};
    int16_t val = 0;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_insert_int16(ctx, val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_insert_uint8_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t src = 0xf4;
    uint8_t checkVal = 0;
    uint8_t buf[sizeof(uint8_t)] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_uint8(ctx, src), PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint8(ctxExtract, &checkVal), PLDM_SUCCESS);

    EXPECT_EQ(src, checkVal);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, insert_under_uint8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint8_t buf[1] = {};
    uint8_t val = 0;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_insert_uint8(ctx, val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_insert_int8_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    int8_t src = -4;
    int8_t checkVal = 0;
    uint8_t buf[sizeof(int8_t)] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_int8(ctx, src), PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_int8(ctxExtract, &checkVal), PLDM_SUCCESS);

    EXPECT_EQ(src, checkVal);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, insert_under_int8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    int8_t buf[1] = {};
    int8_t val = 0;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_insert_int8(ctx, val), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_insert_array_uint8_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t src[6] = {0x11, 0x22, 0x44, 0x55, 0x66, 0x77};
    uint8_t buf[6] = {};
    uint8_t retBuff[6] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_array_uint8(ctx, src, sizeof(src)),
              PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(
        pldm_msgbuf_extract_array_uint8(ctxExtract, retBuff, sizeof(retBuff)),
        PLDM_SUCCESS);

    EXPECT_EQ(memcmp(src, retBuff, sizeof(retBuff)), 0);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, pldm_msgbuf_insert_array_uint8_bad)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t src[6] = {0x11, 0x22, 0x44, 0x55, 0x66, 0x77};
    uint8_t buf[6] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_array_uint8(ctx, NULL, sizeof(src)),
              PLDM_ERROR_INVALID_DATA);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, insert_under_array_uint8)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint8_t buf[1] = {};
    uint8_t val[1] = {0};

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN + sizeof(val) - 1;
    EXPECT_NE(pldm_msgbuf_insert_array_uint8(ctx, val, sizeof(val)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_span_required_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t src[6] = {0x11, 0x22, 0x44, 0x55, 0x66, 0x77};
    uint8_t buf[6] = {0};
    const size_t required = 4;
    uint8_t expectData[required] = {0x44, 0x55, 0x66, 0x77};
    uint16_t testVal;
    uint8_t* retBuff = NULL;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_array_uint8(ctx, src, sizeof(src)),
              PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint16(ctxExtract, &testVal), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_span_required(ctxExtract, required, (void**)&retBuff),
              PLDM_SUCCESS);

    EXPECT_EQ(memcmp(expectData, retBuff, required), 0);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, pldm_msgbuf_span_required_bad)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t src[6] = {0x11, 0x22, 0x44, 0x55, 0x66, 0x77};
    uint8_t buf[6] = {0};
    const size_t required = 4;
    uint16_t testVal;
    [[maybe_unused]] uint8_t* retBuff = NULL;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_array_uint8(ctx, src, sizeof(src)),
              PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint16(ctxExtract, &testVal), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_span_required(ctxExtract, required, NULL),
              PLDM_ERROR_INVALID_DATA);

    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, span_required_under)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;

    uint8_t buf[1] = {};
    void* cursor = nullptr;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, 0), PLDM_SUCCESS);
    ctx->remaining = INTMAX_MIN;
    EXPECT_NE(pldm_msgbuf_span_required(ctx, 1, &cursor), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_ERROR_INVALID_LENGTH);
}

TEST(msgbuf, pldm_msgbuf_span_remaining_good)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t src[8] = {0x11, 0x22, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    uint8_t buf[8] = {0};
    uint16_t testVal;
    uint8_t expectData[6] = {0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    size_t remaining;
    uint8_t* retBuff = NULL;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_array_uint8(ctx, src, sizeof(src)),
              PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint16(ctxExtract, &testVal), PLDM_SUCCESS);
    EXPECT_EQ(
        pldm_msgbuf_span_remaining(ctxExtract, (void**)&retBuff, &remaining),
        PLDM_SUCCESS);

    EXPECT_EQ(remaining, sizeof(expectData));
    EXPECT_EQ(memcmp(expectData, retBuff, remaining), 0);
    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, pldm_msgbuf_span_remaining_bad)
{
    struct pldm_msgbuf _ctx;
    struct pldm_msgbuf* ctx = &_ctx;
    uint8_t src[8] = {0x11, 0x22, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
    uint8_t buf[8] = {0};
    uint16_t testVal;
    size_t remaining;
    uint8_t* retBuff = NULL;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctx, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_insert_array_uint8(ctx, src, sizeof(src)),
              PLDM_SUCCESS);

    struct pldm_msgbuf _ctxExtract;
    struct pldm_msgbuf* ctxExtract = &_ctxExtract;

    ASSERT_EQ(pldm_msgbuf_init_cc(ctxExtract, 0, buf, sizeof(buf)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint16(ctxExtract, &testVal), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_span_remaining(ctxExtract, NULL, &remaining),
              PLDM_ERROR_INVALID_DATA);
    EXPECT_EQ(pldm_msgbuf_span_remaining(ctxExtract, (void**)&retBuff, NULL),
              PLDM_ERROR_INVALID_DATA);

    EXPECT_EQ(pldm_msgbuf_destroy(ctxExtract), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(ctx), PLDM_SUCCESS);
}

TEST(msgbuf, pldm_msgbuf_copy_good)
{
    struct pldm_msgbuf _src;
    struct pldm_msgbuf* src = &_src;
    uint16_t buf[1] = {htole16(0x5aa5)};

    ASSERT_EQ(pldm_msgbuf_init_cc(src, sizeof(buf), buf, sizeof(buf)),
              PLDM_SUCCESS);

    struct pldm_msgbuf _dst;
    struct pldm_msgbuf* dst = &_dst;
    uint16_t checkVal = 0;
    uint8_t buf1[sizeof(buf)] = {};

    ASSERT_EQ(pldm_msgbuf_init_cc(dst, sizeof(buf1), buf1, sizeof(buf1)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_copy(dst, src, buf[0], name), PLDM_SUCCESS);

    ASSERT_EQ(pldm_msgbuf_init_cc(dst, sizeof(buf1), buf1, sizeof(buf1)),
              PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_extract_uint16(dst, &checkVal), PLDM_SUCCESS);

    EXPECT_EQ(buf[0], checkVal);
    EXPECT_EQ(pldm_msgbuf_destroy(src), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_destroy(dst), PLDM_SUCCESS);
}

TEST(msgbuf, pldm_msgbuf_copy_bad)
{
    struct pldm_msgbuf _src;
    struct pldm_msgbuf* src = &_src;
    struct pldm_msgbuf _dst;
    struct pldm_msgbuf* dst = &_dst;
    uint8_t buf[1] = {sizeof(uint8_t)};
    uint8_t buf1[1] = {sizeof(uint16_t)};
    uint16_t value = 8;

    ASSERT_EQ(pldm_msgbuf_init_cc(src, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    ASSERT_EQ(pldm_msgbuf_init_cc(dst, 0, buf1, sizeof(buf1)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_copy(dst, src, value, name),
              PLDM_ERROR_INVALID_LENGTH);

    ASSERT_EQ(pldm_msgbuf_init_cc(src, 0, buf1, sizeof(buf1)), PLDM_SUCCESS);
    ASSERT_EQ(pldm_msgbuf_init_cc(dst, 0, buf, sizeof(buf)), PLDM_SUCCESS);
    EXPECT_EQ(pldm_msgbuf_copy(dst, src, value, name),
              PLDM_ERROR_INVALID_LENGTH);
}
