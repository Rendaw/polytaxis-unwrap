DoOnce 'ren-cxx-filesystem/Tupfile.lua'

Define.Executable
{
	Name = 'polytaxis-unwrap',
	Sources = Item() + 'polytaxis-unwrap.cxx',
	Objects = Item() + FilesystemObjects,
	BuildFlags = '-D_FILE_OFFSET_BITS=64 -I/usr/include/fuse',
	LinkFlags = '-lfuse -pthread',
}

