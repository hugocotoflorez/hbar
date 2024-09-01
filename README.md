# hbar

## About

An useless customizable bar for the terminal!

## Useless

It is designed to be implemented by other module,
by itself is useless (dont reload automatically,
printed over text ... ).

## How to

### Installation

``` sh
git clone https://github.com/hugocotoflorez/hbar
make
```

### Customization

``` c
int main() {
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
```

There are quite a lot of options, you cant found it anywhere. Just
read source code and get your conclusions. Sorry and thanks.

#### format

Main format is created by joining keys names with "|". Example: "time|battery". No
spaces are allowed anywhere.

#### formats

Each key has its own format, the value is placed in the %s in this format.
Example: "Brightness: %s\%".

#### Others

It isnt possible to change date and time format (yet).
Icons are not supported.


