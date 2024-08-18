#include <libsb.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

static Libsb *libsb;

#define ASSERT(x) assert_expression_is_true(x, #x, __FILE__, __LINE__)
#define E(err) ASSERT(LIBSB_ERROR_OK == (err))

static void assert_expression_is_true(bool value, const char *expression, 
                                      const char *file, int line) {
    if (value) {
        return;
    }
    fprintf(stderr, "At \"%s:%d\" assertion is false: \"%s\"", file, line, expression);
    exit(1);
}

static void test_empty(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    ASSERT(builder);
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    ASSERT(!strcmp(out, ""));
    ASSERT(size == strlen(out));
    free(out);
}

static void test_append(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    ASSERT(builder);
    E(libsb_append(libsb, builder, "Привет, %s!", "Артём"));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    ASSERT(!strcmp(out, "Привет, Артём!"));
    ASSERT(size == strlen(out));
    free(out);
}

static void test_replace(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    ASSERT(builder);
    E(libsb_append(libsb, builder, "<< one 2 >> << one 2 >>"));
    E(libsb_replace(libsb, builder, "one", "1"));
    E(libsb_replace(libsb, builder, "2", "two"));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    ASSERT(!builder);
    ASSERT(!strcmp(out, "<< 1 two >> << 1 two >>"));
    ASSERT(size == strlen(out));
    free(out);

    E(libsb_create(libsb, &builder));
    ASSERT(builder);
    E(libsb_append(libsb, builder, "hello"));
    E(libsb_replace(libsb, builder, "l", ""));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    ASSERT(!strcmp(out, "heo"));
    ASSERT(size == strlen(out));
    free(out);
}

static void test_reverse(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    ASSERT(builder);
    E(libsb_append(libsb, builder, "Привет"));
    E(libsb_reverse(libsb, builder));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    ASSERT(!strcmp(out, "тевирП"));
    ASSERT(size == strlen(out));
    free(out);
}

int main() {
    E(libsb_start(&libsb));
    test_empty();
    test_append();
    test_replace();
    test_reverse();
    E(libsb_finish(&libsb));
}
