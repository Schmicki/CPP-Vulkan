#pragma once

#include "DataObjects.h"

namespace sge {
	Buffer* loadFile(const String& filename);
	Buffer* loadFileAligned4(const String& filename);
	Texture* loadTexture(const String& filename);
	Model* loadModel(const String& filename);
}