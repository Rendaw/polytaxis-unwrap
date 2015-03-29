# What is `ptunwrap`?
`ptunwrap` is a tool to open files with polytaxis headers in applications that don't support polytaxis.

`ptunwrap` consists of two parts - `ptunwrap` and `ptunwrapd`.  `ptunwrapd` must be running to use `ptunwrap`.

### `ptunwrap`

`ptunwrap` runs a command, substituting marked filenames with paths to pseudo-files served by `ptunwrapd`.

Example (assuming `file.txt.p` is some text file with a polytaxis header):
```bash
$ ptunwrap cat ^^ file.txt.p
this is the file contents sans polytaxis header
$ ptunwrap cat file2.txt
this file didn't have a polytaxis header but it's all the same to ptunwrap
```

### `ptunwrapd`

`ptunwrapd` provides a fuse filesystem that mirrors the root filesystem.  Any file opened in the `ptunwrapd` mount transparently hides the polytaxis header if present.

`ptunwrapd` uses a mount point in your home directory (`~/.local/share/ptunwrapd/mnt`).
