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
	bool outline = false;
};

class MapRenderer : public tri::System {
public:
	virtual void update() override;
	virtual void startup() override;
	void drawOutlines();

private:
	tri::Ref<tri::Buffer> buffer;
	tri::Ref<tri::FrameBuffer> fb1;
	tri::Ref<tri::FrameBuffer> fb2;

};
