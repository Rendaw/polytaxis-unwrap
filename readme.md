# What is `ptwrap`?
`ptwrap` is a tool to open files with polytaxis headers in applications that don't support polytaxis.

`ptwrap` consists of two parts - `ptwrap` and `ptwrapd`.  `ptwrapd` must be running to use `ptwrap`.

### `ptwrap`

`ptwrap` runs a command, substituting marked filenames with paths to pseudo-files served by `ptwrapd`.

Example (assuming `file.txt.p` is some text file with a polytaxis header):
```bash
$ ptwrap cat ^^ file.txt.p
this is the file contents sans polytaxis header
$ ptwrap cat file2.txt
this file didn't have a polytaxis header but it's all the same to ptwrap
```

### `ptwrapd`

`ptwrapd` provides a fuse filesystem that mirrors the root filesystem.  Any file opened in the `ptwrapd` mount transparently hides the polytaxis header if present.

`ptwrapd` uses a mount point in your home directory (`~/.local/share/ptwrapd/mnt`).
