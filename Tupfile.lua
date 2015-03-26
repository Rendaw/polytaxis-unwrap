DoOnce 'ren-cxx-filesystem/Tupfile.lua'

Define.Executable
{
	Name = 'ptwrap',
	Sources = Item() + 'ptwrap.cxx',
	Objects = Item() + FilesystemObjects,
}

Define.Executable
{
	Name = 'ptwrapd',
	Sources = Item() + 'ptwrapd.cxx',
	Objects = Item() + FilesystemObjects,
	BuildFlags = '-D_FILE_OFFSET_BITS=64 -I/usr/include/fuse',
	LinkFlags = '-lfuse -pthread',
}

