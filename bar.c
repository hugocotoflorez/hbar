#include "bar.h"
#include "termstuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MATCH(tok, str) if (!strcmp((tok), (str)))


struct
{
    char       *values[OBJSlen];
    char       *format;
    char       *formats[OBJSlen];
    int         intervals[OBJSlen];
    signed char margin_in;
    signed char margin_left;
    Vec2d       location;
    Colors      fg, bg;
    enum OBJS   objs;
    unsigned    size;
} bar_data = { 0 };

void *bar_data_ptr = &bar_data;

static char *
get_str_from_command(const char *com)
{
    FILE *pipe;
    char  buffer[VALUE_MAXLEN + 1] = { 0 };
    pipe                           = popen(com, "r");
    if (!pipe)
    {
        return strdup("Error");
    }
    if (fgets(buffer, VALUE_MAXLEN, pipe) == NULL)
    {
        pclose(pipe);
        return strdup("Error");
    }
    pclose(pipe);
    buffer[VALUE_MAXLEN]          = 0;
    buffer[strcspn(buffer, "\n")] = '\0';

    return strdup(buffer);
}

// This functions return correctly at 1 sep 2023
static char *
get_bar_time()
{
    return get_str_from_command("date '+%H:%M'");
}
static char *
get_bar_volume()
{
    return get_str_from_command(
    "amixer sget Master | grep -E '[0-9]{1,3}%\\]' -om1 | cut -d\\% -f1");
}
static char *
get_bar_brightness()
{
    return get_str_from_command(
    "brightnessctl | grep -E '([0-9]{1,3}%)' -o | cut -d\\% -f1");
}
static char *
get_bar_network()
{
    return get_str_from_command(
    "nmcli -t -f active,ssid dev wifi | grep -E '^yes' | cut -d\\: -f2");
}
static char *
get_bar_battery()
{
    return get_str_from_command(
    "cat /sys/class/power_supply/BAT1/capacity");
}

static char *
get_bar_sep()
{
    return strdup(SEP);
}

typedef char *(*Patchers)(void);
static Patchers patchers[] = {
    [BAR_TIME]       = get_bar_time,
    [BAR_VOLUME]     = get_bar_volume,
    [BAR_BRIGHTNESS] = get_bar_brightness,
    [BAR_NETWORK]    = get_bar_network,
    [BAR_BATTERY]    = get_bar_battery,
    [BAR_SEP]        = get_bar_sep,
};

// obj MUST be a single obj
static int
redux(enum OBJS obj)
{
    switch (obj)
    {
        case BAR_TIME:
            return 0;
        case BAR_VOLUME:
            return 1;
        case BAR_BRIGHTNESS:
            return 2;
        case BAR_NETWORK:
            return 3;
        case BAR_BATTERY:
            return 4;
        case BAR_SEP:
            return 5;

        default:
            exit(1);
    }
}

void
bar_reload_data(enum OBJS obj)
{
    if (obj & BAR_TIME)
    {
        bar_data.values[redux(BAR_TIME)] = patchers[BAR_TIME]();
    }
    if (obj & BAR_VOLUME)
        bar_data.values[redux(BAR_VOLUME)] = patchers[BAR_VOLUME]();
    if (obj & BAR_BRIGHTNESS)
        bar_data.values[redux(BAR_BRIGHTNESS)] = patchers[BAR_BRIGHTNESS]();
    if (obj & BAR_NETWORK)
        bar_data.values[redux(BAR_NETWORK)] = patchers[BAR_NETWORK]();
    if (obj & BAR_BATTERY)
        bar_data.values[redux(BAR_BATTERY)] = patchers[BAR_BATTERY]();
    if (obj & BAR_SEP)
        bar_data.values[redux(BAR_SEP)] = patchers[BAR_SEP]();
}

static enum OBJS
get_objs_from_format(const char *format)
{
    char     *s;
    enum OBJS objs        = BAR_NONE;
    char     *temp_format = strdup(bar_data.format);

    if ((s = strtok(temp_format, FORMAT_SEP)))
        do
        {
            MATCH("volume", s) objs |= BAR_VOLUME;
            MATCH("time", s) objs |= BAR_TIME;
            MATCH("brightness", s) objs |= BAR_BRIGHTNESS;
            MATCH("battery", s) objs |= BAR_BATTERY;
            MATCH("network", s) objs |= BAR_NETWORK;
            MATCH("sep", s) objs |= BAR_SEP;

        } while ((s = strtok(NULL, FORMAT_SEP)));

    free(temp_format);
    return objs;
}

char *
bar_get_str_data()
{
    char       *s;
    signed char is_top_left           = 1;
    char        buffer[MAX_BAR_WIDTH] = { 0 };
    char       *format                = strdup(bar_data.format);

    if ((s = strtok(format, FORMAT_SEP)))
        do
        {
            if (is_top_left)
            {
                sprintf(buffer + strlen(buffer), "%-*s", bar_data.margin_left, ""); // margin
                is_top_left = 0;
            }
            else
                sprintf(buffer + strlen(buffer), "%-*s", bar_data.margin_in, ""); // margin

            MATCH("volume", s)
            {
                sprintf(buffer + strlen(buffer), bar_data.formats[redux(BAR_VOLUME)],
                        bar_data.values[redux(BAR_VOLUME)]);
                continue;
            }
            MATCH("time", s)
            {
                sprintf(buffer + strlen(buffer), bar_data.formats[redux(BAR_TIME)],
                        bar_data.values[redux(BAR_TIME)]);
                continue;
            }
            MATCH("brightness", s)
            {
                sprintf(buffer + strlen(buffer),
                        bar_data.formats[redux(BAR_BRIGHTNESS)],
                        bar_data.values[redux(BAR_BRIGHTNESS)]);
                continue;
            }
            MATCH("battery", s)
            {
                sprintf(buffer + strlen(buffer), bar_data.formats[redux(BAR_BATTERY)],
                        bar_data.values[redux(BAR_BATTERY)]);
                continue;
            }
            MATCH("network", s)
            {
                sprintf(buffer + strlen(buffer), bar_data.formats[redux(BAR_NETWORK)],
                        bar_data.values[redux(BAR_NETWORK)]);
                continue;
            }
            MATCH("sep", s)
            {
                sprintf(buffer + strlen(buffer), bar_data.formats[redux(BAR_SEP)],
                        bar_data.values[redux(BAR_SEP)]);
                continue;
            }
            //
        } while ((s = strtok(NULL, FORMAT_SEP)));

    free(format);
    return strdup(buffer);
}

void
bar_printall()
{
    char *data = bar_get_str_data();
    term_cursor_save_current_possition();
    term_cursor_position(bar_data.location.i, bar_data.location.j);
    term_apply_color(bar_data.fg, FG);
    term_apply_color(bar_data.bg, BG);
    printf("%-*s", bar_data.size, data);
    fflush(stdout);
    free(data);
    term_cursor_restore_saved_position();
}

void
bar_render()
{
    char *temp_format = strdup(bar_data.format);
    bar_printall();
    free(temp_format);
}

void
bar_delete()
{
    free(bar_data.format);
    free(bar_data.formats[redux(BAR_VOLUME)]);
    free(bar_data.formats[redux(BAR_TIME)]);
    free(bar_data.formats[redux(BAR_BRIGHTNESS)]);
    free(bar_data.formats[redux(BAR_BATTERY)]);
    free(bar_data.formats[redux(BAR_NETWORK)]);
    free(bar_data.values[redux(BAR_VOLUME)]);
    free(bar_data.values[redux(BAR_TIME)]);
    free(bar_data.values[redux(BAR_BRIGHTNESS)]);
    free(bar_data.values[redux(BAR_BATTERY)]);
    free(bar_data.values[redux(BAR_NETWORK)]);
}

void
bar_set_formats(enum OBJS obj, const char *format)
{
    bar_data.formats[redux(obj)] = strdup(format);
}

void
bar_set_format(const char *format)
{
    bar_data.format = strdup(format);
    bar_data.objs   = get_objs_from_format(format);
}

void
bar_set_margins(int left, int in)
{
    bar_data.margin_left = left;
    bar_data.margin_in   = in;
}

void
bar_set_location(Vec2d location)
{
    bar_data.location = location;
}

void
bar_set_width(unsigned size)
{
    bar_data.size = size;
}

void
bar_set_colors(Colors bg, Colors fg)
{
    bar_data.fg = fg;
    bar_data.bg = bg;
}

void
bar_load_default_conf()
{
    bar_set_format("time|volume|battery|network");
    bar_set_formats(BAR_VOLUME, "Vol: %s%%");
    bar_set_formats(BAR_TIME, "%s");
    bar_set_formats(BAR_BRIGHTNESS, "(%s%%)");
    bar_set_formats(BAR_BATTERY, "BAT: %s%%");
    bar_set_formats(BAR_NETWORK, "Wifi: %s");
    bar_set_formats(BAR_SEP, "%s");
    bar_set_margins(0, 4);
    bar_set_location((Vec2d){ 1, 1 });
    bar_set_colors(BLACK, WHITE);
    bar_set_width(0); // as wide as text
}

void
bar_init()
{
    bar_load_default_conf();
    bar_reload_data(bar_data.objs);
    bar_render();
    bar_delete();
}
