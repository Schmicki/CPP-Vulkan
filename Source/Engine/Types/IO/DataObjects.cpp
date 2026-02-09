#include "DataObjects.h"

namespace sge {

	Model::Model(const RawModel& data) {
		Map<Vertex, u32> vertexMap;
		for (u32 i = 0; i < data.indices.size; i++) {
			u32 idx = vertices.size;

			f32 uv_x = data.textureCoordinates[i].x, uv_y = 1.0F - data.textureCoordinates[i].y;

			Vertex vert{ data.vertices[data.indices[i]], data.vertexColors[i], {uv_x, uv_y} };
			u32 v;

			if (vertexMap.findFirstIndex(vert, v)) {
				indices.add(vertexMap.getIndex(v));
			}
			else {
				vertexMap._insert(v, vert, idx);
				vertices.add(vert);
				indices.add(idx);
			}
		}
	}

	Model::Model(std::initializer_list<Vertex> _vertices, std::initializer_list<u32> _indices) {
		vertices = _vertices;
		indices = _indices;
	}
}