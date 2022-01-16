//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"
#include <glm/glm.hpp>


class MapGenerator {
public:
	bool generate(const std::string &file, bool singleMesh = false, bool noMeshUpdate = false, bool spherical = false);
};
