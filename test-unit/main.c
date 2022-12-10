#include <libsb.h>
#include <string.h>

static void test_init(void);
static void test_fini(void);
#define TEST_INIT test_init()
#define TEST_FINI test_fini()
#include <acutest.h>

Libsb *libsb;

#define E(err) TEST_ASSERT(LIBSB_ERROR_OK == (err))

static void test_init(void) {
    E(libsb_start(&libsb));
}
static void test_fini(void) {
    E(libsb_finish(&libsb));
}

static void test_empty(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    TEST_ASSERT(builder);
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    TEST_ASSERT(!strcmp(out, ""));
    TEST_ASSERT(size == strlen(out));
    free(out);
}

static void test_append(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    TEST_ASSERT(builder);
    E(libsb_append(libsb, builder, "Привет, %s!", "Артём"));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    TEST_ASSERT(!strcmp(out, "Привет, Артём!"));
    TEST_ASSERT(size == strlen(out));
    free(out);
}

static void test_replace(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    TEST_ASSERT(builder);
    E(libsb_append(libsb, builder, "<< one 2 >> << one 2 >>"));
    E(libsb_replace(libsb, builder, "one", "1"));
    E(libsb_replace(libsb, builder, "2", "two"));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    TEST_ASSERT(!builder);
    TEST_ASSERT(!strcmp(out, "<< 1 two >> << 1 two >>"));
    TEST_ASSERT(size == strlen(out));
    free(out);

    E(libsb_create(libsb, &builder));
    TEST_ASSERT(builder);
    E(libsb_append(libsb, builder, "hello"));
    E(libsb_replace(libsb, builder, "l", ""));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    TEST_ASSERT(!strcmp(out, "heo"));
    TEST_ASSERT(size == strlen(out));
    free(out);
}

static void test_reverse(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    TEST_ASSERT(builder);
    E(libsb_append(libsb, builder, "Привет"));
    E(libsb_reverse(libsb, builder));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    TEST_ASSERT(!strcmp(out, "тевирП"));
    TEST_ASSERT(size == strlen(out));
    free(out);
}

TEST_LIST = {
        { "empty", test_empty },
        { "append", test_append },
        { "replace", test_replace },
        { "reverse", test_reverse },
        { NULL, NULL },
};
