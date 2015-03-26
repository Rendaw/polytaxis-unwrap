#include <mutex>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <set>

#include "ren-cxx-basics/error.h"
#include "ren-cxx-filesystem/file.h"
#include "ren-cxx-fuse/fuse_wrapper.h"
#include "ren-cxx-fuse/fuse_outofband.h"

#include "common.cxx"

std::string const Mark("<<<<\n");

// Signal handling
std::vector<function<void(void)>> SignalHandlers;

void HandleSignal(int SignalNumber)
{
	std::cout << "Got signal " << SignalNumber << std::endl;
	for (auto const &Handler : SignalHandlers) Handler();
}

// Fuse data management
struct FileT
{
	int Handle;
	size_t HeaderLength;
};

FileT &GetFile(struct fuse_file_info *fi)
{
	return **reinterpret_cast<std::shared_ptr<FileT> *>(fi->fh);
}

void SetFile(struct fuse_file_info *fi, std::shared_ptr<FileT> File)
{
	fi->fh = reinterpret_cast<uint64_t>(new std::shared_ptr<FileT>(std::move(File)));
}

void ClearFile(struct fuse_file_info *fi)
{
	delete reinterpret_cast<std::shared_ptr<FileT> *>(fi->fh);
}

// polytaxis utils
struct HeaderReadFail {};

size_t GetHeaderLength(std::string const &Path)
{
	auto File = Filesystem::FileT::OpenRead(Path);
	try
	{
		int Length = 0;
		{
			std::vector<uint8_t> Header1(24);
			File.Read(Header1);
			if (Header1.size() != 24) throw HeaderReadFail();
			if (std::string((char const *)&Header1[0], 12) != "polytaxis00 ") throw HeaderReadFail();
			std::stringstream LengthIn(std::string((char const *)&Header1[13], 11));
			LengthIn >> Length;
			if (!LengthIn) throw HeaderReadFail();
		}
		if (Length < 0)
		{
			std::vector<uint8_t> Buffer;
			std::string Parts[2];
			for (int Next = 0; Buffer.resize(0), File.Read(Buffer); Next = !Next)
			{
				Parts[Next] = std::string((char const *)&Buffer[0], Buffer.size());
				auto const Joined = Parts[!Next] + Parts[Next];
				auto const FoundAt = Joined.find_first_of(Mark);
				if (FoundAt == std::string::npos) continue;
				size_t const FullLength = File.Tell() - Joined.size() + Mark.size();
				//File.Seek(FullLength);
				return FullLength;

			}
			throw HeaderReadFail();
		}
		else
		{
			auto FullLength = 24 + Length;
			//File.Seek(FullLength);
			return FullLength;
		}
	}
	catch (HeaderReadFail const &Error) 
	{
		//File.Seek(0);
		return 0;
	}
}

size_t WriteEmptyHeader(int fd)
{
	char const Text[] = "polytaxis00 00000000512\n\0";
	write(fd, Text, sizeof(Text) - 1);
	return 24 + 512;
}

// Filesystem implementation
struct FilesystemT
{
	std::set<pid_t> OutOfBandThreadIDs; // Unused
	void OperationBegin(bool const OutOfBand) {}
	void OperationEnd(bool const OutOfBand) {}

	int create(bool const OutOfBand, const char *path, mode_t mode, struct fuse_file_info *fi)
	{
		Assert(!OutOfBand);
		auto fd = ::open(path, fi->flags, mode);
		if (fd == -1)
			return -errno;
		auto Length = WriteEmptyHeader(fd);
		SetFile(fi, std::make_shared<FileT>(FileT{fd, Length}));
		return 0;
	}

	int release(bool const OutOfBand, const char *path, struct fuse_file_info *fi)
	{
		Assert(!OutOfBand);
		close(GetFile(fi).Handle);
		ClearFile(fi);
		return 0;
	}

	int open(bool const OutOfBand, const char *path, struct fuse_file_info *fi)
	{
		Assert(!OutOfBand);
		auto fd = ::open(path, fi->flags);
		if (fd == -1)
			return -errno;
		SetFile(fi, std::make_shared<FileT>(FileT{fd, GetHeaderLength(path)}));
		return 0;
	}

	int read(bool const OutOfBand, const char *path, char *out, size_t count, off_t start, struct fuse_file_info *fi)
	{
		Assert(!OutOfBand);
		auto File = GetFile(fi);
		auto res = pread(File.Handle, out, count, File.HeaderLength + start);
		if (res == -1)
			res = -errno;
		return res;
	}

	int write(bool const OutOfBand, const char *path, const char *in, size_t count, off_t start, struct fuse_file_info *fi)
	{
		Assert(!OutOfBand);
		auto File = GetFile(fi);
		auto res = pwrite(File.Handle, in, count, File.HeaderLength + start);
		if (res == -1)
			res = -errno;
		return res;
	}

	int truncate(bool const OutOfBand, const char *path, off_t size)
	{
		Assert(!OutOfBand);
		auto res = ::truncate(path, GetHeaderLength(path) + size);
		if (res == -1)
			return -errno;
		return 0;
	}

	int getattr(bool const OutOfBand, const char *path, struct stat *buf)
	{
		Assert(!OutOfBand);
		auto res = lstat(path, buf);
		if (res == -1) return -errno;
		return 0;
	}

	// ///////////////////////////////////////////////////////////
	// Passthrough methods
	int utimens(bool const OutOfBand, const char *path, const struct timespec ts[2])
	{
		Assert(!OutOfBand);
		int res;
		struct timeval tv[2];

		tv[0].tv_sec = ts[0].tv_sec;
		tv[0].tv_usec = ts[0].tv_nsec / 1000;
		tv[1].tv_sec = ts[1].tv_sec;
		tv[1].tv_usec = ts[1].tv_nsec / 1000;

		res = ::utimes(path, tv);
		if (res == -1)
			return -errno;

		return 0;
	}

	int access(bool const OutOfBand, const char *path, int amode)
	{
		Assert(!OutOfBand);
		int res;

		res = ::access(path, amode);
		if (res == -1)
			return -errno;

		return 0;
	}

	int unlink(bool const OutOfBand, const char *path)
	{
		Assert(!OutOfBand);
		int res;

		res = ::unlink(path);
		if (res == -1)
			return -errno;

		return 0;
	}

	int chmod(bool const OutOfBand, const char *path, mode_t mode)
	{
		Assert(!OutOfBand);
		auto res = ::chmod(path, mode);
		if (res == -1)
			return -errno;
		return 0;
	}

	int chown(bool const OutOfBand, const char *path, uid_t uid, gid_t gid)
	{
		Assert(!OutOfBand);
		auto res = ::lchown(path, uid, gid);
		if (res == -1)
			return -errno;
		return 0;
	}

	int rename(bool const OutOfBand, const char *from, const char *to)
	{
		Assert(!OutOfBand);
		auto res = ::rename(from, to);
		if (res == -1)
			return -errno;
		return 0;
	}
};

// Runner
int main(int argc, char **argv)
{
	try
	{
		// Make mount directory
		auto const Root = GetRoot();
		Root->CreateDirectory();
		FinallyT RootCleanup([&](void) { Root->Delete(); });

		// Set up fuse and run
		FilesystemT Filesystem;
		FuseT<FilesystemT> Fuse(*Root, Filesystem);

		{
			struct ::sigaction HandlerInfo;
			memset(&HandlerInfo, 0, sizeof(struct sigaction));
			HandlerInfo.sa_handler = HandleSignal;
			sigemptyset(&(HandlerInfo.sa_mask));
			HandlerInfo.sa_flags = 0;
			sigaction(SIGINT, &HandlerInfo, NULL);
			sigaction(SIGTERM, &HandlerInfo, NULL);
			sigaction(SIGHUP, &HandlerInfo, NULL);
		}
		SignalHandlers.push_back([&Fuse](void) { Fuse.Kill(); });
		FinallyT SignalCleanup([](void) { SignalHandlers.clear(); });

		return Fuse.Run();
	}
	catch (UserErrorT const &Error)
	{
		std::cerr << "Error: " << Error << std::endl;
		return 1;
	}
	catch (SystemErrorT const &Error)
	{
		std::cerr << "System error: " << Error << std::endl;
		return 1;
	}
	catch (ConstructionErrorT const &Error)
	{
		std::cerr << "Uncaught error: " << Error << std::endl;
		return 1;
	}
	catch (std::runtime_error const &Error)
	{
		std::cerr << "Uncaught error: " << Error.what() << std::endl;
		return 1;
	}
}
