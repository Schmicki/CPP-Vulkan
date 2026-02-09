#pragma once

#include "../Core.h"
#include "../Math/Math.h"

namespace sge {

	struct Buffer {
		SelfDeletingPointer<char> data;
		u32 size;
	};

	struct Vertex {
		FVec3 pos;
		FVec3 col;
		FVec2 uv;

		bool operator < (const Vertex& other) const {
			if (pos.x != other.pos.x) return pos.x < other.pos.x;
			else if (pos.y != other.pos.y) return pos.y < other.pos.y;
			else if (pos.z != other.pos.z) return pos.z < other.pos.z;
			else if (uv.x != other.uv.x) return uv.x < other.uv.x;
			else if (uv.y != other.uv.y) return uv.y < other.uv.y;
			return false;
		}
	};

	struct Texture {
		SelfDeletingPointer<Color> pixels;
		u32 width, height;
	};

	struct RawModel {
		Array<FVec3> vertices;
		Array<FVec3> vertexColors;
		Array<FVec3> vertexNormals;
		Array<FVec2> textureCoordinates;
		Array<u32> indices;
	};

	struct Model {

		Model(const RawModel& data);
		Model(std::initializer_list<Vertex> _vertices, std::initializer_list<u32> _indices);

		Array<Vertex> vertices;
		Array<u32> indices;
	};
}