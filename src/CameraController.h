//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"
#include "core/core.h"
#include <glm/glm.hpp>

class CameraController {
public:
	glm::vec2 minBounds;
	glm::vec2 maxBounds;
	float speed;
	float zoom;
	float minZoom;
	float maxZoom;
};
