#include "ren-cxx-filesystem/path.h"

OptionalT<Filesystem::PathT> GetRoot(void)
{
	if (!getenv("HOME"))
	{
		std::cerr << "HOME isn't set, can't create mount directory." << std::endl;
		return {};
	}
	return Filesystem::PathT::Absolute(std::string(getenv("HOME")) + "/.local/share/ptwrapd/mount");
}
