#include "bar.h"
#include <sys/ioctl.h>

Vec2d
fullscreen()
{
    Vec2d          size;
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    size.i = w.ws_row;
    size.j = w.ws_col;
    return size;
}

int
main()
{
    unsigned width = fullscreen().j;
    bar_load_default_conf();
    bar_set_colors(BLUE, BLACK);
    bar_set_format("time|battery");
    bar_set_formats(BAR_TIME, "[%s]");
    bar_set_formats(BAR_BATTERY, "Battery: %s%%");
    bar_set_width(width);
    bar_reload_data(BAR_TIME | BAR_BATTERY);
    bar_render();
    return 0;
}
