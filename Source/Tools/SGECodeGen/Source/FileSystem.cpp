#include "FileSystem.h"

#if defined(D_SYSTEM_WINDOWS)

#include "Types/Core/Pointers.h"
#include <Windows.h>

namespace sge
{
	File openFile(const String& path, const u32 mode) {
		DWORD access = (mode & OPEN_MODE_READ) ? GENERIC_READ : 0;
		access |= (mode & OPEN_MODE_WRITE) ? GENERIC_WRITE : 0;
		DWORD creation = (mode & OPEN_MODE_CREATE) ? CREATE_NEW : 0;
		creation = (!creation && mode & OPEN_MODE_TRUNC) ? TRUNCATE_EXISTING : creation;
		creation = (!creation) ? OPEN_EXISTING : creation;
		DWORD attributes = FILE_ATTRIBUTE_NORMAL;
		attributes |= (mode & OPEN_MODE_WRITE) ? 0 : FILE_ATTRIBUTE_READONLY;

		void* ret = CreateFileW(SelfDeletingPointer(path.toWString()), access, FILE_SHARE_READ, NULL, creation, attributes, NULL);
		return (ret == INVALID_HANDLE_VALUE) ? NULL : ret;
	}

	void closeFile(File handle) {
		CloseHandle(handle);
	}

	u32 readFile(File handle, char* buffer, u32 size) {
		DWORD bytesRead = 0;
		ReadFile(handle, buffer, size, &bytesRead, NULL);
		return bytesRead;
	}
	
	bool writeFile(File handle, const char* data, u32 size) {
		ulong bytesWritten = 0;
		return WriteFile(handle, data, size, &bytesWritten, NULL);
	}

	u64 setFilePos(File handle, i64 offset, u32 origin) {
		LARGE_INTEGER a;
		a.QuadPart = offset;
		LARGE_INTEGER b;
		SetFilePointerEx(handle, a, &b, origin);
		return b.QuadPart;
	}

    bool isDir(const String& path) {
		return GetFileAttributesW(SelfDeletingPointer(path.toWString())) & FILE_ATTRIBUTE_DIRECTORY;
	}

    void forFilesR(const String& path, const Array<String>& endFilter, void(*callback)(const String& name, const String& path))
    {
        WIN32_FIND_DATAW data;

        String spath = path;
        spath.append("/*");
        wchar_t* p = spath.toWString();
        HANDLE search = FindFirstFileW(p, &data);
        delete p;

        String _path = path;
        if (_path[_path.size -  1] != '/')
        {
            _path.append("/", 1);
        }
        String _name;

        do {
            if (search == INVALID_HANDLE_VALUE)
                break;

            _name = data.cFileName;

            if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
                for (uint32_t i = 0; i < _name.size; i++) {
                    if (_name[i] != '.') {
                        String filepath = _path;
                        filepath.append(_name);
                        forFilesR(filepath, endFilter, callback);
                        break;
                    }
                }
            }
            else {
                if (endFilter.size == 0)
                {
                    callback(_name, _path);
                }
                else
                {
                    for (u32 i = 0; i < endFilter.size; i++) {
                        if (_name.endsWith(endFilter.get(i).data)) {
                            callback(_name, _path);
                            break;
                        }   
                    }   
                }
            }

        } while (FindNextFileW(search, &data));

        if (search)
        {
            FindClose(search);
        }
    }
}

#elif defined(D_SYSTEM_LINUX)

#define _FILE_OFFSET_BITS 64
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

// I am using the void* handle directly as 'int' should this not function I will need to implement the void* as ptr to handle or typedef the handle type to int!
static_assert(sizeof(int) <= sizeof(void*));

namespace sge
{
    File openFile(const String& path, u32 mode)
    {
        i32 flags = 0;
        if (mode & OPEN_MODE_READWRITE)
        {
            flags |= O_RDWR;
        }
        else if (mode & OPEN_MODE_READ)
        {
            flags |= O_RDONLY;
        }
        else if (mode & OPEN_MODE_WRITE)
        {
            flags |= O_WRONLY;
        }
    
        if (mode & OPEN_MODE_TRUNC)
        {
            flags |= O_TRUNC;
        }
    
        File handle;
        if (mode & OPEN_MODE_CREATE)
        {
            flags |= O_CREAT;
            mode_t creatFlags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
            *(int*)&handle = open(path.data, flags, creatFlags);
        }
        else
        {
            *(int*)&handle = open(path.data, flags);
        }

        return *(int*)&handle != -1 ? handle : NULL;
    }
    
    void closeFile(File handle)
    {
        close(*(int*)&handle);
    }
    
    u32 readFile(const File handle, char* buffer, const u32 size)
    {
        i64 count = read(*(int*)&handle, buffer, size);
        if (count)
        {
            return (u32)count;
        }
        else{
            return 0;
        }
    }
    
    bool writeFile(const File handle, const char* data, const u32 size)
    {
        return write(*(int*)&handle, data, size);
    }

    u64 setFilePos(File handle, i64 off, u32 origin)
    {
        i64 count = lseek(*(int*)&handle, off, *(int*)&origin);
        if (count)
        {
            return count;
        }
        else
        {
            return 0;
        }
    }
    
    bool isDir(const sge::String& path)
    {
        struct stat stat{};
        if(1 + lstat(path.data, &stat))
        {
            return S_ISDIR(stat.st_mode);
        }
        else
        {
            return false;
        }
    }

    void forFilesR(const String& path, const Array<String>& endFilter, void(*callback)(const String& name, const String& path))
    {
        dirent *dir = NULL;
        DIR* search = opendir(path);
        if (search == NULL)
        {
            return;
        }

        String _path = path;
        if (_path[_path.size -  1] != '/')
        {
            _path.append("/", 1);
        }
        String _name;
        while (dir = readdir(search))
        {

            _name = dir->d_name;

            String filepath = _path;
            filepath.append(_name);

            if (isDir(filepath))
            {
                for (uint32_t i = 0; i < _name.size; i++) {
                    if (_name[i] != '.') {
                        forFilesR(filepath, endFilter, callback);
                        break;
                    }
                }
            }
            else
            {
                if (endFilter.size == 0)
                {
                    callback(_name, _path);
                }
                else
                {
                    for (uint32_t i = 0; i < endFilter.size; i++) {
                        if (_name.endsWith(endFilter.get(i).data)) {
                            callback(_name, _path);
                            break;
                        }
                    }
                }
            }
        }
        
        closedir(search);
    }
}

#endif

#include <iostream>

namespace sge {
    u32 readToBuffer(const String& path, char*& buffer)
    {
        File f = openFile(path, OPEN_MODE_READ);
        if (!f)
        {
            return 0;
        }

        u32 end = (u32)setFilePos(f, 0, FILE_POS_END);
        setFilePos(f, 0, FILE_POS_START);

        buffer = new char[end];
        readFile(f, buffer, end);

        closeFile(f);
        return end;
    }

    bool writeToFile(const String& path, const char* buffer, u32 size, bool append)
    {
        u32 mode = OPEN_MODE_WRITE | OPEN_MODE_CREATE;
        if (!append)
        {
            mode |= OPEN_MODE_TRUNC;
        }

        File f = openFile(path, mode);
        if (!f)
        {
            return false;
        }

        writeFile(f, buffer, size);
        closeFile(f);
        return true;
    }
}
