# What is `polytaxis-unwrap`?

`polytaxis-unwrap` is a tool to open files with polytaxis headers in applications that don't support polytaxis.

`polytaxis-unwrap` does this by exposing a fuse filesystem that mirrors the root filesystem.  Any file opened in the fuse filesystem transparently hides the polytaxis header if present.

This is probably most useful with a path translator like `unpt` in [polytaxis-utils](https://github.com/Rendaw/polytaxis-utils) or `polytaxis-unwrap`-aware applications like `ptq` (also in polytaxis-utils).

### Installation

Installation requires fuse and tup.

After cloning the repository:
```bash
git submodule update --init
tup init
cp tup.template.config tup.config
tup upd
```

Copy `polytaxis-unwrap` to somewhere on your path.

### Usage

`polytaxis-unwrap` takes no command line arguments.  Start it and leave it running until you no longer need to unwrap files.

