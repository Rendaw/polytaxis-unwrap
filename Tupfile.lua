DoOnce 'ren-cxx-filesystem/Tupfile.lua'

Define.Executable
{
	Name = 'ptunwrap',
	Sources = Item() + 'ptunwrap.cxx',
	Objects = Item() + FilesystemObjects,
}

Define.Executable
{
	Name = 'ptunwrapd',
	Sources = Item() + 'ptunwrapd.cxx',
	Objects = Item() + FilesystemObjects,
	BuildFlags = '-D_FILE_OFFSET_BITS=64 -I/usr/include/fuse',
	LinkFlags = '-lfuse -pthread',
}

