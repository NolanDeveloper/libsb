/* libsb - LIBrary for String Building.
 *
 * Library provides utility functions for string construction.
 *
 * Example:
 *
 * >>> LibsbError err;
 * >>> LibsbBuilder *builder;
 * >>>
 * >>> err = libsb_create(&builder);
 * >>> if (err) { die(); }
 * >>>
 * >>> err = libsb_append(builder, "My name is %s. ", "Nolan");
 * >>> if (err) { die(); }
 * >>>
 * >>> err = libsb_append(builder, "I'm %d years old.", 42);
 * >>> if (err) { die(); }
 * >>>
 * >>> char *out;
 * >>> size_t out_size;
 * >>> libsb_destroy_into(builder, &out, &out_size);
 * >>>
 * >>> printf("%s\n", out);
 * >>> free(out);
 */

#ifndef LIBSB_H
#define LIBSB_H

#include <stdlib.h>
#include <stdarg.h>

typedef enum {
    LIBSB_ERROR_OK,
    LIBSB_ERROR_OUT_OF_MEMORY,
    LIBSB_ERROR_BAD_ARGUMENT,
    LIBSB_ERROR_PRINTF,
    LIBSB_ERROR_UTF8,
} LibsbError;

// The structure that user needs to pass into every function as first parameter.
typedef struct Libsb_ Libsb;

// Builder of the string. Holds growing buffer.
typedef struct LibsbBuilder_ LibsbBuilder;

// Create and initialize libsb object.
LibsbError libsb_start(Libsb **libsb);

// Free resources taken by libsb.
LibsbError libsb_finish(Libsb **libsb);

// Create string builder.
LibsbError libsb_create(Libsb *libsb, LibsbBuilder **builder);

// Free resources taken by string builder.
LibsbError libsb_destroy(Libsb *libsb, LibsbBuilder **builder);

// Put the built string pointer into out, put length of that string into
// size and free all remaining resources taken by string builder.
LibsbError libsb_destroy_into(Libsb *libsb, LibsbBuilder **builder, char **out, size_t *size);

// Append string built using sprintf.
#if defined(SUPPORTS_ATTRIBUTE_FORMAT)
__attribute__ ((format(printf, 3, 4)))
#endif
LibsbError libsb_append(Libsb *libsb, LibsbBuilder *builder, const char *fmt, ...);

// Append string built using vsprintf
LibsbError libsb_append_v(Libsb *libsb, LibsbBuilder *builder, const char *fmt, va_list args);

// Replace all occurrences of old with new. If old is empty LIBSB_ERROR_BAD_ARGUMENT is returned.
// New may be empty in which case all occurrences of old will be deleted.
LibsbError libsb_replace(Libsb *libsb, LibsbBuilder *builder, const char *old, const char *new);

// Reverse characters of string that the builder currently holds.
LibsbError libsb_reverse(Libsb *libsb, LibsbBuilder *builder);

#endif
