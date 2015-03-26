#include "ren-cxx-filesystem/path.h"

#include "common.cxx"

std::string const Mark("^^");

std::string Slash(std::string const &In)
{
	if (In == "|") return In;
	if (In == ">") return In;
	std::stringstream Out;
	Out << "'";
	for (auto const Char : In)
	{
		if (Char == '\'') Out << '\\';
		Out << Char;
	}
	Out << "'";
	return Out.str();
}

int main(int argc, char **argv)
{
	auto const Root = GetRoot();
	if (!Root->DirectoryExists())
	{
		std::cerr << *Root << " doesn't exist.  Is ptwrapd running?" << std::endl;
		return 1;
	}

	if (argc < 2)
	{
		std::cerr << "ptwrap requires a command line." << std::endl;
		std::cerr << "ptwrap replaces ^^-preceded paths with unwrapped substitutes." << std::endl;
		return 1;
	}

	std::stringstream OutArguments;
	bool UnwrapNext = false;
	for (int ArgIndex = 1; ArgIndex < argc; ++ArgIndex)
	{
		std::string FullArg(argv[ArgIndex]);
		if (FullArg == Mark)
		{
			UnwrapNext = true;
			continue;
		}

		if (UnwrapNext)
		{
			OutArguments << " " << Slash(Root->EnterRaw(Filesystem::PathT::Qualify(FullArg).Render()));
			UnwrapNext = false;
		}
		else
		{
			OutArguments << " " << Slash(FullArg);
		}
	}

	std::cout << "Running [" << OutArguments.str() << "]" << std::endl;
	return system(OutArguments.str().c_str());
}

