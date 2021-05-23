#include <assert.h>
#include <libsb.h>
#include <string.h>

Libsb *libsb;

#define E handle_error
static void handle_error(LibsbError err) {
    if (LIBSB_ERROR_OK == err) {
        return;
    }
    abort();
}

static void test_empty(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    assert(builder);
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    assert(!strcmp(out, ""));
    assert(size == strlen(out));
    free(out);
}

static void test_append(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    assert(builder);
    E(libsb_append(libsb, builder, "Привет, %s!", "Артём"));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    assert(!strcmp(out, "Привет, Артём!"));
    assert(size == strlen(out));
    free(out);
}

static void test_replace(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    assert(builder);
    E(libsb_append(libsb, builder, "<< one 2 >> << one 2 >>"));
    E(libsb_replace(libsb, builder, "one", "1"));
    E(libsb_replace(libsb, builder, "2", "two"));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    assert(!builder);
    assert(!strcmp(out, "<< 1 two >> << 1 two >>"));
    assert(size == strlen(out));
    free(out);

    E(libsb_create(libsb, &builder));
    assert(builder);
    E(libsb_append(libsb, builder, "hello"));
    E(libsb_replace(libsb, builder, "l", ""));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    assert(!strcmp(out, "heo"));
    assert(size == strlen(out));
    free(out);
}

static void test_reverse(void) {
    LibsbBuilder *builder = NULL;
    char *out;
    size_t size;
    E(libsb_create(libsb, &builder));
    assert(builder);
    E(libsb_append(libsb, builder, "Привет"));
    E(libsb_reverse(libsb, builder));
    E(libsb_destroy_into(libsb, &builder, &out, &size));
    assert(!strcmp(out, "тевирП"));
    assert(size == strlen(out));
    free(out);
}

int main() {
    E(libsb_start(&libsb));
    test_empty();
    test_append();
    test_replace();
    test_reverse();
    E(libsb_finish(&libsb));
    return 0;
}
