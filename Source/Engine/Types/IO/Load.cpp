#include "Load.h"
#include "VirtualFileSystem.h"

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "SadFileFormat.h"

namespace sge {
	Buffer* loadFile(const String& filename) {
		VFSFile file = VFS::open(filename, OPEN_MODE_READ);
		if (!file.handle) return nullptr;

		Buffer* buffer = new Buffer;
		buffer->size = VFS::setPos(file, 0, FILE_POS_END);
		buffer->data = new char[buffer->size];
		
		VFS::setPos(file, 0, FILE_POS_START);
		VFS::read(file, buffer->data, static_cast<ulong>(buffer->size));
		VFS::close(file);

		return buffer;
	}

	Buffer* loadFileAligned4(const String& filename) {
		VFSFile file = VFS::open(filename, OPEN_MODE_READ);
		if (!file.handle) return nullptr;

		Buffer* buffer = new Buffer;
		buffer->size = VFS::setPos(file, 0, FILE_POS_END);
		u32 size = static_cast<u32>((buffer->size + 3) / 4);
		buffer->data = as<char*>(new u32[size]);
		
		VFS::setPos(file, 0, FILE_POS_START);
		VFS::read(file, buffer->data, static_cast<ulong>(buffer->size));
		VFS::close(file);
		return buffer;
	}

	Texture* loadTexture(const String& filename) {
		Buffer* buffer;
		if (!(buffer = loadFile(filename.data))) return nullptr;

		int channels = 0;
		Texture* tex = new Texture;
		tex->pixels = as<Color*>(stbi_load_from_memory(as<u8*>(buffer->data), static_cast<int>(buffer->size), as<int*>(&tex->width), as<int*>(&tex->height), &channels, STBI_rgb_alpha));
		delete buffer;
		return tex;
	}

	Model* loadModel(const String& filename)
	{
		Buffer* buff = loadFile(filename);
		if (!buff) return nullptr;

		if (!(buff->data[0] == 'S' && buff->data[1] == 'A' && buff->data[2] == 'D')) return nullptr;
		if (!(as<u8*>(buff->data.data)[6] == SFF_GROUP_3D) || !(as<u8*>(buff->data)[7] == SFF_3D_MESH)) return nullptr;

		u32 pos = 8;
		RawModel* model = new RawModel;
		while (as<u8*>(buff->data)[pos] != SFF_END) {
			switch (as<u8*>(buff->data)[pos]) {
			case SFF_3D_MESH_VERTICES: {
				pos++;
				u32 size = as<int*>(buff->data + pos)[0];
				pos += 4;

				char* vertices = buff->data + pos;
				pos += size * 12;
				model->vertices.resize(size);
				memcpy(model->vertices.data, vertices, size * 12);
			} break;
			case SFF_3D_MESH_INDICES: {
				pos++;
				u32 size = as<int*>(buff->data + pos)[0];
				pos += 4;

				char* indices = buff->data + pos;
				pos += size * 4;
				model->indices.resize(size);
				memcpy(model->indices.data, indices, size * 4);
			} break;
			case SFF_3D_MESH_UVLAYER: {
				pos++;
				u32 size = as<int*>(buff->data + pos)[0];
				pos += 4;

				char* uvs = buff->data + pos;
				pos += size * 8;

				model->textureCoordinates.resize(size);
				memcpy(model->textureCoordinates.data, uvs, size * 8);
			} break;
			case SFF_3D_MESH_VERTEXCOLORLAYER: {
				pos++;
				u32 size = as<int*>(buff->data + pos)[0];
				pos += 4;

				char* colors = buff->data + pos;
				pos += size * 16;

				model->vertexColors.resize(size);
				memcopy(model->vertexColors.data, colors, size, 12, 0, 4);
			} break;
			}
		}
		delete buff;

		if (!model->vertexColors.data) model->vertexColors.resize(model->indices.size, FVec3{ 1.0F, 1.0F, 1.0F });

		Model* ret = new Model{ *model };
		delete model;
		return ret;
	}
}