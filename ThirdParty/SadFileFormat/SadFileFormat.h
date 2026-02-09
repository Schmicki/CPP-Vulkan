#pragma once


// Sad File Format Header
// 3 Byte magic number : SAD
// 3 Byte version : MajorMinorPatch

#define SFF_MAKE_VERSION(major, minor, patch) major << 16 | minor << 8 | patch
#define SFF_VERSION MAKE_VERSION(1, 0, 0)

enum SFFBase {
	// Used to indicate end of object or file
	//
	SFF_END = 0,



	// SFF_LOCAL_REF - Reference inside file, SFF_LOCAL_REF can reference a SFF_EXTERN_REF.
	// Example: (key, SFF_LOCAL_REF)(uint64_t pos)
	//
	SFF_LOCAL_REF = 1,

	// SFF_EXTERN_REF - Reference to other file.
	// Example: (key, SFF_EXTERN_REF)(string, filename)(uint64_t pos)
	// 
	SFF_EXTERN_REF = 2,



	// SFF_OBJECT_8 - Used to store serializeable objects those GUID is 8 bytes long.
	// 
	SFF_OBJECT_8 = 3,

	// SFF_OBJECT_16 - Used to store serializeable objects those GUID is 16 bytes long.
	// 
	SFF_OBJECT_16 = 4,

	// SFF_OBJECT_32 - Used to store serializeable objects those GUID is 32 bytes long.
	// 
	SFF_OBJECT_32 = 5,



	// SFF_BUFFER_2 - The 2 indicates that the variable storing the buffer size is 2 bytes long.
	//
	SFF_BUFFER_2 = 6,

	// SFF_BUFFER_4 - The 4 indicates that the variable storing the buffer size is 4 bytes long.
	// 
	SFF_BUFFER_4 = 7,

	// SFF_BUFFER_8 - The 8 indicates that the variable storing the buffer size is 8 bytes long.
	// 
	SFF_BUFFER_8 = 8,



	// SFF_1_BYTE - Used to store any type that is one byte long.
	// 
	SFF_1_BYTE = 9,

	// SFF_2_BYTE - Used to store any type that is two bytes long.
	// 
	SFF_2_BYTE = 10,

	// SFF_4_BYTE - Used to store any type that is 4 bytes long.
	// 
	SFF_4_BYTE = 11,

	// SFF_8_BYTE - Used to store any type that is 8 bytes long.
	// 
	SFF_8_BYTE = 12,

	// SFF_16_BYTE - Used to store any type that is 16 bytes long.
	// 
	SFF_16_BYTE = 13,

	// SFF_32_BYTE - Used to store any type that is 32 bytes long.
	// 
	SFF_32_BYTE = 14,



	// Groups - Groups are used to extend the functionality of the sad file format.
	// The space from 225 - 255 is reserved for groups.
	// Example: (key, SFF_GROUP_3D)(key, SFF_3D_MESH)(data)

	// SFF_GROUP_3D
	// 
	SFF_GROUP_3D = 225, // 0xE1
};

enum SFF3D {
	SFF_3D_MESH = 1,
	SFF_3D_MESH_VERTICES = 1,
	SFF_3D_MESH_INDICES = 2,
	SFF_3D_MESH_VERTEXCOLORLAYER = 3,
	SFF_3D_MESH_UVLAYER = 4,

	SFF_3D_SCENE = 2,

	SFF_3D_SKELETON = 3,

	SFF_3D_MATERIAL = 4,
};