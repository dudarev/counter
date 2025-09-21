# counter

Tiny CLI counter.

- Persists a single number in a file.
- Commands: `inc`, `dec`, `reset`, `get`, `set N`.
- Default storage: `$XDG_CONFIG_HOME/counter/counter` or `~/.config/counter/counter`.
- Optional speech on macOS: `--speak` and `--voice NAME` (best-effort via /usr/bin/say).

## Build

make
make install           # installs to ~/bin/counter by default

Minimal size (manual):
clang -Os -flto -Wl,-dead_strip -o ~/bin/counter counter.c
strip -x ~/bin/counter   # optional on macOS

## Usage

counter get              # print current value
counter inc              # increment and print
counter dec              # decrement (never below 0) and print
counter reset            # set to 0 and print
counter set 42           # set to 42 and print

Options:
--file PATH              # override storage file
--speak                  # speak the number (macOS; best-effort)
--voice NAME             # choose voice for say(1)
--help | -h | help       # usage

Notes:
- Minimal POSIX C, dynamically linked.
- The storage file contains the number as ASCII.
