#pragma once

#include "Types/Aliases.h"
#include "Types/Core/String.h"

namespace sge
{    
    typedef void* File;

    enum OpenMode {
    	OPEN_MODE_READ = 0x1,
    	OPEN_MODE_WRITE = 0x2,
    	OPEN_MODE_READWRITE = 0x1 | 0x2,
    	OPEN_MODE_TRUNC = 0x4,
    	OPEN_MODE_CREATE = 0x8,
    };
    
    enum FilePos {
    	FILE_POS_START = 0, FILE_POS_CURRENT = 1, FILE_POS_END = 2
    };

    File openFile(const sge::String& path, u32 mode);
    void closeFile(File handle);
    u32 readFile(const File handle, char* buffer, const u32 size);
    bool writeFile(const File handle, const char* data, const u32 size);
    u64 setFilePos(File handle, i64 off, u32 origin);

    u32 readToBuffer(const String& path, char*& buffer);
    bool writeToFile(const String& path, const char* buffer, u32 size, bool append);

    bool isDir(const sge::String& path);
    void forFilesR(const String& path, const Array<String>& endFilter, void(*callback)(const String& name, const String& path));
}