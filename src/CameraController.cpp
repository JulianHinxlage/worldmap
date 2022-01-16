//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "CameraController.h"
#include "core/SignalManager.h"
#include "engine/Input.h"
#include "entity/Scene.h"
#include "engine/Camera.h"
#include "engine/Transform.h"
#include "engine/Time.h"

using namespace tri;

TRI_REGISTER_COMPONENT(CameraController)
TRI_REGISTER_MEMBER(CameraController, minBounds)
TRI_REGISTER_MEMBER(CameraController, maxBounds)
TRI_REGISTER_MEMBER(CameraController, speed)
TRI_REGISTER_MEMBER(CameraController, zoom)
TRI_REGISTER_MEMBER(CameraController, minZoom)
TRI_REGISTER_MEMBER(CameraController, maxZoom)

TRI_UPDATE_CALLBACK("CameraController") {
	env->scene->view<CameraController, Camera, Transform>().each([](CameraController& cc, Camera& c, Transform& t) {
		//move
		glm::vec3 axis = { env->input->down('D') - env->input->down('A'), 0, env->input->down('S') - env->input->down('W') };
		t.position += axis * cc.speed * cc.zoom * env->time->deltaTime;

		//zoom
		cc.zoom *= 1.0f - env->input->getMouseWheelDelta() * 0.1f;
		cc.zoom = std::min(cc.maxZoom, cc.zoom);
		cc.zoom = std::max(cc.minZoom, cc.zoom);
		t.position.y = cc.zoom;
		float fZoom = (cc.zoom - cc.minZoom) / (cc.maxZoom - cc.minZoom);
		fZoom = std::sqrt(fZoom);
		t.rotation.x = glm::radians(std::max(-90.0f, -45.0f + -60.0f * fZoom));

		//bounds
		t.position.x = std::max(cc.minBounds.x, t.position.x);
		t.position.z = std::max(cc.minBounds.y, t.position.z);
		t.position.x = std::min(cc.maxBounds.x, t.position.x);
		t.position.z = std::min(cc.maxBounds.y, t.position.z);
	});
}

TRI_REGISTER_CALLBACK("CameraController") {
	env->signals->update.setActiveInEditModeCallback("CameraController", false);
}

