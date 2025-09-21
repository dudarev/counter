# Changelog


All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## 0.1.0 — 2025-09-21

### Added

Initial implementation:

- Tiny CLI counter that persists a single number.
- Commands: `inc`, `dec`, `reset`, `get`, `set N` (default is `get`).
- Storage file: `$XDG_CONFIG_HOME/counter/counter` or `~/.config/counter/counter`.
  - Creates `~/.config` and `.../counter` directories if missing (best-effort).
- Optional speech on macOS via `/usr/bin/say` with `--speak` and `--voice NAME`.
  - Best-effort: silently skips if `say` is unavailable or errors.
- Makefile optimized for small binaries (`-Os -flto -Wl,-dead_strip`),
  installs to `~/bin` by default and strips on install.
- Added README, MIT LICENSE, VERSION, CHANGELOG.
