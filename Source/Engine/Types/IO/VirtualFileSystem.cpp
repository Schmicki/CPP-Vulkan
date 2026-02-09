#include "VirtualFileSystem.h"

#if defined(DEBUG) || defined(_DEBUG)
	#include <stdio.h>
#endif

namespace sge {

	File openNativeFS(const String& path, const u32 mode, MountPoint* mp)
	{
		if (mp) {
			String realPath = mp->realPath;
			if (realPath.data[realPath.size - 1] == '/')
				realPath.append(path.data + 1, path.size - 1);
			else
				realPath.append(path.data, path.size);

			File handle = openFile(realPath, mode);
			return handle ? handle : 0;
		}
		else {
			File handle = openFile(path, mode);
			return handle ? handle : 0;
		}
	}

	void onMountedNativeFS(MountPoint* mp)
	{

	}

	VirtualFileSystem* VirtualFileSystem::instance;

	VirtualFileSystem::VirtualFileSystem() :lastError(0) {

	}

	VirtualFileSystem::~VirtualFileSystem() {

	}

	void VirtualFileSystem::init(VirtualFileSystem* vfs) {

		if (instance) {

		}
		else if (vfs) {
			instance = vfs;
		}
		else {
			instance = new VirtualFileSystem();
			registerFileSystem(new FileSystem{isDir, openNativeFS, closeFile, readFile, writeFile, setFilePos, onMountedNativeFS});
		}
	}

	void VirtualFileSystem::destroy() {
		delete instance;
	}
	// VirtualPath can only be One Directory name ("/Name" not "/Name/Noname")
	void VirtualFileSystem::mount(const String& virtualPath, const String& realPath) {
		for (u32 i = 0; i < instance->fileSystems.size; i++) {
			if (instance->fileSystems[i].data->detect(realPath)) {
				u32 index = instance->mountPoints.addUnique(virtualPath, Array<MountPoint>());
				instance->mountPoints.getIndex(index).add({ realPath, instance->fileSystems[i], NULL });
				instance->fileSystems[i].data->onMounted(&instance->mountPoints.getIndex(index).get(instance->mountPoints.size - 1));
				break;
			}
		}
	}

	void VirtualFileSystem::unmount(const String& virtualPath, const String& realPath) {
		u32 index = 0;
		if (instance->mountPoints.findFirstIndex(virtualPath, index)) {
			Array<MountPoint>* mountPoints = &instance->mountPoints.getIndex(index);
			for (u32 i = 0; i < mountPoints->size; i++) {
				if (mountPoints->get(i).realPath == realPath) {
					mountPoints->remove(i);
					break;
				}
			}
		}
	}

	void VirtualFileSystem::unmountAll(const String& virtualPath) {
		instance->mountPoints.remove(virtualPath);
	}

	void VirtualFileSystem::registerFileSystem(FileSystem* fs) {
		instance->fileSystems.addM(mov(SelfDeletingPointer<FileSystem>(fs)));
	}

	VFSFile VirtualFileSystem::open(const String& path, const u32 mode) {

		if (path.data[0] == '/') {
			u32 splitPoint = path.find("/", 1);
			String virtualPath, remains;
			path.split(splitPoint, virtualPath, remains);

			if (instance->mountPoints.findFirstIndex(virtualPath, splitPoint)) {
				Array<MountPoint>& mountPoints = instance->mountPoints.getIndex(splitPoint);
				for (u32 i = 0, ix = mountPoints.size - 1; i < mountPoints.size; i++, ix--) {
					MountPoint* mp = &mountPoints.get(ix);
					VFSFile file = {mp->fs->open(remains, mode, mp), mp->fs};
					if (file.handle)
						return file;
				}
			}
#if DEBUG || _DEBUG
			printf("ERROR - could not open file: %s\n", path.data);
#endif
			return VFSFile{ 0, 0 };
		}
		else {
#if DEBUG || _DEBUG
			VFSFile f{ instance->fileSystems[0].data->open(path, mode, NULL), instance->fileSystems[0] };
			if (f.handle == 0) printf("ERROR - could not open file: %s\n", path.data);
			return f;
#else
			return VFSFile{ instance->fileSystems[0].data->open(path, mode, NULL), instance->fileSystems[0] };
#endif
		}
	}

	void VirtualFileSystem::close(VFSFile& file) {
		if (file.handle)
			file.fs->close(file.handle);
	}

	u32 VirtualFileSystem::read(VFSFile& file, char* buffer, u32 size) {
		return file.fs->read(file.handle, buffer, size);
	}

	bool VirtualFileSystem::write(VFSFile& file, const char* data, u32 size) {
		return file.fs->write(file.handle, data, size);
	}

	u64 VirtualFileSystem::setPos(VFSFile& file, i64 offset, u32 origin) {
		return file.fs->setFilePos(file.handle, offset, origin);
	}
}