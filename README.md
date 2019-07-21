# ldtdesktop

A set of patches for Telegram Desktop applied through `LD_PRELOAD`.

It's only possible to use these patches if Telegram Desktop is built against system shared Qt
libraries which is available on some distributions.

## Building and Installing

Run `make && make install` to build and install ldtdesktop to your system.

## Usage

You will need to modify `LD_PRELOAD` and include `libldtdesktop.so` there, for instance
`LD_PRELOAD=/usr/local/lib/libldtdesktop.so`.

It's can be done by creating a shell script named as primary telegram binary (`Telegram` or
`telegram-desktop` depending on your distribution) in `/usr/local/bin` with the following content
as example:

```sh
#!/bin/sh
p="`realpath "$0"`"
p="`basename "$p"`"
export LD_PRELOAD=/usr/local/lib/libldtdesktop.so
exec "/usr/bin/$p" "$@"
```

You can also define some environment variables there to configure the library.

### Font Changing

Use `TG_FONT_NORMAL`, `TG_FONT_NORMAL_SIZE`, and `TG_FONT_NORMAL_KERNING` to override
default Telegram normal font.

Use `TG_FONT_MONOSPACE`, `TG_FONT_MONOSPACE_SIZE`, and `TG_FONT_MONOSPACE_KERNING` to override
default Telegram monospace font.

For example:

```sh
export TG_FONT_NORMAL='Tahoma'
export TG_FONT_NORMAL_SIZE=11
export TG_FONT_NORMAL_KERNING=false
export TG_FONT_MONOSPACE='Dina'
export TG_FONT_MONOSPACE_SIZE=12
export TG_FONT_MONOSPACE_KERNING=false
```
