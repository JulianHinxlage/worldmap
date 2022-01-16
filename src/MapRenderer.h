//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"
#include "render/Renderer.h"
#include "core/System.h"
#include <glm/glm.hpp>

class MapComponent {
public:
	tri::Ref<tri::Mesh> mesh;
};

enum TeretoryType {
	NONE,
	CONTINENT,
	COUNTRY,
	STATE,
	CITY,
};

class TeretoryComponent {
public:
	std::string parent;
	int teretoryId;
	tri::Color color;
	TeretoryType type;
};

class MapRenderer : public tri::System {
public:
	virtual void update() override;
	virtual void startup() override;

private:
	tri::Ref<tri::Buffer> buffer;
};
