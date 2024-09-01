#ifndef _BAR_H
#define _BAR_H

#include "termstuff.h"

typedef struct
{
    unsigned i, j;
} Vec2d;

enum OBJS
{
    BAR_NONE       = 1 << 0,
    BAR_TIME       = 1 << 1,
    BAR_VOLUME     = 1 << 2,
    BAR_BRIGHTNESS = 1 << 3,
    BAR_NETWORK    = 1 << 4,
    BAR_BATTERY    = 1 << 5,
    BAR_SEP        = 1 << 6,
    BAR_ALL        = ~(BAR_NONE),
};

#define OBJSlen      (6)
#define VALUE_MAXLEN 20
#define MAX_BAR_WIDTH 100

#define SEP "|"

#define FORMAT_SEP "|"

extern char* bar_get_str_data();
extern void bar_reload_data(enum OBJS obj);
extern void bar_render();
extern void bar_delete();
extern void bar_init();
extern void bar_load_default_conf();
extern void bar_set_format(const char *format);
extern void bar_set_formats(enum OBJS obj, const char *format);
extern void bar_set_margins(int left, int in);
extern void bar_set_location(Vec2d location);
extern void bar_set_width(unsigned size);
extern void bar_set_colors(Colors bg, Colors fg);
/* How to use:
 * 1. load default to avoid no-set options
 * 2. modify formats / options
 * 3. reload data
 * 4. render bar
 *
 * bar_init load default bar and render it
 *
 * Note: bar dont reload manually.
 */


#endif /* _BAR_H */
