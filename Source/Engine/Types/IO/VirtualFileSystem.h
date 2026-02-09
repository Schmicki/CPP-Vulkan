#pragma once

#include "../Platforms/FileSystem.h"

namespace sge
{
	struct MountPoint;

	struct FileSystem {
		using detect_t = bool(*)(const String&);
		using open_t = File (*)(const String&, const u32, MountPoint*);
		using close_t = void(*)(File);
		using read_t = u32(*)(File,char*, const u32 size);
		using write_t = bool(*)(File, const char*, const u32);
		using setFilePos_t = u64(*)(File, i64, u32);
		using onMounted_t = void(*)(MountPoint*);

		detect_t detect;
		open_t open;
		close_t close;
		read_t read;
		write_t write;
		setFilePos_t setFilePos;
		onMounted_t onMounted;
	};

    struct MountPoint {
		String realPath;
		FileSystem* fs;
		void* data;
	};

	struct VFSFile {
		File handle;
		FileSystem* fs;
	};

	class VirtualFileSystem {
	public:
		static VirtualFileSystem* instance;

		VirtualFileSystem();
		~VirtualFileSystem();

		static void init(VirtualFileSystem* vfs = nullptr);
		static void destroy();

		Map<String, Array<MountPoint>> mountPoints;
		Array<SelfDeletingPointer<FileSystem>> fileSystems;
		u32 lastError;

		static void mount(const String& virtualPath, const String& realPath);
		static void unmount(const String& virtualPath, const String& realPath);
		static void unmountAll(const String& virtualPath);
		static void registerFileSystem(FileSystem* fs);

		static VFSFile open(const String& path, const u32 mode);
		static void close(VFSFile& file);
		static u32 read(VFSFile& file, char* buffer, u32 size);
		static bool write(VFSFile& file, const char* data, u32 size);
		static u64 setPos(VFSFile& file, i64 offset, u32 origin);
	};

	typedef VirtualFileSystem VFS;
}