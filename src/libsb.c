#include <libsb.h>
#include <libgb.h>
#include <libutf.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

struct Libsb_ {
    Libgb *libgb;
};

struct LibsbBuilder_ {
    LibgbBuffer *buffer;
};

#define E libsb_handle_internal_error

static LibsbError libsb_handle_internal_error(LibsbError err) {
    switch (err) {
    case LIBSB_ERROR_OK:
        return LIBSB_ERROR_OK;
    case LIBSB_ERROR_OUT_OF_MEMORY:
        return LIBSB_ERROR_OUT_OF_MEMORY;
    case LIBSB_ERROR_BAD_ARGUMENT:
        abort();
    case LIBSB_ERROR_PRINTF:
        return LIBSB_ERROR_PRINTF;
    case LIBSB_ERROR_UTF8:
        return LIBSB_ERROR_UTF8;
    }
    abort();
}

#define EGB libgberror_to_libsberror

static LibsbError libgberror_to_libsberror(LibgbError err) {
    switch (err) {
    case LIBGB_ERROR_OK:
        return LIBSB_ERROR_OK;
    case LIBGB_ERROR_BAD_ARGUMENT:
        abort();
    case LIBGB_ERROR_OUT_OF_MEMORY:
        return LIBSB_ERROR_OUT_OF_MEMORY;
    case LIBGB_ERROR_INDEX:
        abort();
    }
    abort();
}

LibsbError libsb_start(Libsb **libsb) {
    LibsbError err = LIBSB_ERROR_OK;
    Libsb *result = NULL;
    Libgb *libgb = NULL;
    if (!libsb) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EGB(libgb_start(&libgb));
    if (err) goto end;
    result = malloc(sizeof(Libsb));
    result->libgb = libgb;
    *libsb = result;
    libgb = NULL;
    result = NULL;
end:
    EGB(libgb_finish(&libgb));
    free(result);
    return err;
}

LibsbError libsb_finish(Libsb **libsb) {
    LibsbError err = LIBSB_ERROR_OK;
    if (!libsb) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (!*libsb) {
        goto end;
    }
    EGB(libgb_finish(&(*libsb)->libgb));
    free(*libsb);
end:
    return err;
}

LibsbError libsb_create(Libsb *libsb, LibsbBuilder **builder) {
    LibsbError err = LIBSB_ERROR_OK;
    LibsbBuilder *result = NULL;
    LibgbBuffer *buffer = NULL;
    if (!libsb || !builder) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EGB(libgb_create(libsb->libgb, &buffer));
    if (err) goto end;
    result = malloc(sizeof(LibsbBuilder));
    if (!result) {
        err = LIBSB_ERROR_OUT_OF_MEMORY;
        goto end;
    }
    result->buffer = buffer;
    *builder = result;
    buffer = NULL;
    result = NULL;
end:
    EGB(libgb_destroy(libsb->libgb, &buffer));
    free(result);
    return err;
}

LibsbError libsb_destroy(Libsb *libsb, LibsbBuilder **builder) {
    char *str = NULL;
    size_t str_size = 0;
    LibsbError err = libsb_destroy_into(libsb, builder, &str, &str_size);
    free(str);
    return err;
}

LibsbError libsb_destroy_into(Libsb *libsb, LibsbBuilder **builder, char **out, size_t *size) {
    LibsbError err = LIBSB_ERROR_OK;
    if (!libsb || !builder || !out || !size) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    if (!*builder) {
        *out = NULL;
        *size = 0;
        goto end;
    }
    char null = '\0';
    err = EGB(libgb_append_buffer(libsb->libgb, (*builder)->buffer, &null, 1));
    if (LIBSB_ERROR_OK == err) {
        EGB(libgb_destroy_into(libsb->libgb, &(*builder)->buffer, out, size));
        --*size;
    } else {
        EGB(libgb_destroy(libsb->libgb, &(*builder)->buffer));
    }
    free(*builder);
    *builder = NULL;
end:
    return err;
}

LibsbError libsb_append(Libsb *libsb, LibsbBuilder *builder, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LibsbError err = libsb_append_v(libsb, builder, fmt, args);
    va_end(args);
    return err;
}

typedef struct {
    const char *fmt;
    va_list args;
    int n;
} AppendFormatData;

static void append_format_v(char *block, size_t block_size, void *userdata) {
    AppendFormatData *data = userdata;
    data->n = vsnprintf(block, block_size, data->fmt, data->args);
}

LibsbError libsb_append_v(Libsb *libsb, LibsbBuilder *builder, const char *fmt, va_list args) {
    LibsbError err = LIBSB_ERROR_OK;
    AppendFormatData data;
    data.fmt = fmt;
    data.n = -1;
    va_copy(data.args, args);
    if (!libsb || !builder || !fmt) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    int n = vsnprintf(NULL, 0, fmt, args);
    if (n < 0) {
        err = LIBSB_ERROR_PRINTF;
        goto end;
    }
    size_t size;
    err = EGB(libgb_get_size(libsb->libgb, builder->buffer, &size));
    if (err) goto end;
    err = EGB(libgb_insert_initialized(libsb->libgb, builder->buffer, size, n + 1, &data, append_format_v));
    if (err) goto end;
    if (n != data.n) {
        err = LIBSB_ERROR_PRINTF;
        goto end;
    }
    err = EGB(libgb_drop(libsb->libgb, builder->buffer, 1));
    if (err) goto end;
end:
    va_end(data.args);
    return err;
}

LibsbError libsb_replace(Libsb *libsb, LibsbBuilder *builder, const char *old, const char *new) {
    LibsbError err = LIBSB_ERROR_OK;
    char *string = NULL;
    size_t n = 0;
    if (!libsb || !builder || !old || !new) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    size_t old_length = strlen(old);
    if (!old_length) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    char null = '\0';
    err = EGB(libgb_append_buffer(libsb->libgb, builder->buffer, &null, 1));
    if (err) goto end;
    err = EGB(libgb_destroy_into(libsb->libgb, &builder->buffer, &string, &n));
    if (err) goto end;
    err = EGB(libgb_create(libsb->libgb, &builder->buffer));
    if (err) goto end;
    const char *left = string;
    char *right = strstr(left, old);
    while (right) {
        err = E(libsb_append(libsb, builder, "%.*s%s", (int) (right - left), left, new));
        if (err) goto end;
        left = right + old_length;
        right = strstr(left, old);
    }
    err = E(libsb_append(libsb, builder, "%s", left));
    if (err) goto end;
end:
    free(string);
    return err;
}

LibsbError libsb_reverse(Libsb *libsb, LibsbBuilder *builder) {
    LibsbError err = LIBSB_ERROR_OK;
    char *string = NULL;
    size_t n = 0;
    if (!libsb || !builder) {
        err = LIBSB_ERROR_BAD_ARGUMENT;
        goto end;
    }
    err = EGB(libgb_destroy_into(libsb->libgb, &builder->buffer, &string, &n));
    if (err) goto end;
    err = EGB(libgb_create(libsb->libgb, &builder->buffer));
    if (err) goto end;
    char *input = string + n - 1;
    while (input-- > string) {
        LibutfC8Type type = libutf_c8_type(*input);
        if (type == LIBUTF_UTF8_TRAILING) {
            continue;
        } else if (type < 0) {
            err = LIBSB_ERROR_UTF8;
            goto end;
        }
        assert(0 < type);
        int length = type;
        err = EGB(libgb_append_buffer(libsb->libgb, builder->buffer, input, length));
        if (err) goto end;
    }
end:
    free(string);
    return err;
}
