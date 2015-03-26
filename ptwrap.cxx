#include "ren-cxx-filesystem/path.h"

std::string const Mark("^^");

int main(int argc, char **argv)
{
	auto const Root = GetRoot();
	if (!Root.DirectoryExists())
	{
		std::cerr << Root << " doesn't exist.  Is ptwrapd running?" << std::endl;
		return 1;
	}

	if (argc < 2)
	{
		std::cerr << "ptwrap requires a command line." << std::endl;
		std::cerr << "ptwrap replaces ^^-prefixed paths with unwrapped substitutes." << std::endl;
		return 1;
	}

	std::stringstream OutArguments;
	for (int ArgIndex = 1; ArgIndex < argc; ++ArgIndex)
	{
		std::string FullArg(argv[ArgIndex]);
		if (
			(FullArg.length() > Mark.length()) &&
			(FullArg.substr(Mark.length()) == Mark))
		{
			OutArguments << " " << 
				Slash(Root.EnterRaw(
					Filesystem::PathT::Qualify(
						FullArg.substr(Mark.length(), std::npos)).Render()));
		}
		else
		{
			OutArguments << " " << Slash(FullArg);
		}
	}

	return system(OutArguments.str().c_str());
}

