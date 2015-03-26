Filesystem::PathT GetRoot(void)
{
	if (!getenv("HOME"))
	{
		std::cerr << "HOME isn't set, can't create mount directory." << std::endl;
		return 1;
	}
	return RootPath = Filesystem::PathT::Absolute(getenv("HOME") + "/.share/ptwrapd/mount");
}
