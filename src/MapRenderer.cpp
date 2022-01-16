//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//


#include "pch.h"
#include "MapRenderer.h"
#include "engine/AssetManager.h"
#include "entity/Scene.h"
#include "engine/Transform.h"
#include "engine/Camera.h"

using namespace tri;

TRI_REGISTER_COMPONENT(MapComponent)
TRI_REGISTER_MEMBER(MapComponent, mesh)

TRI_REGISTER_TYPE(TeretoryType)
TRI_REGISTER_CONSTANT(TeretoryType, NONE)
TRI_REGISTER_CONSTANT(TeretoryType, CONTINENT)
TRI_REGISTER_CONSTANT(TeretoryType, COUNTRY)
TRI_REGISTER_CONSTANT(TeretoryType, STATE)
TRI_REGISTER_CONSTANT(TeretoryType, CITY)

TRI_REGISTER_COMPONENT(TeretoryComponent)
TRI_REGISTER_MEMBER(TeretoryComponent, parent)
TRI_REGISTER_MEMBER(TeretoryComponent, teretoryId)
TRI_REGISTER_MEMBER(TeretoryComponent, color)
TRI_REGISTER_MEMBER(TeretoryComponent, type)

TRI_REGISTER_SYSTEM(MapRenderer)

class TeretoryData {
public:
	glm::vec4 color;
	glm::vec4 entityId;
};

void MapRenderer::startup() {
	env->signals->update.callbackOrder({ "MapRenderer", "MeshComponent" });
}

void MapRenderer::update() {
	Ref<Shader> shader = env->assets->get<Shader>("map.glsl");
	Ref<Texture> texture = env->assets->get<Texture>("map.jpg");


	std::vector<TeretoryData> teretoryColors;
	env->scene->view<TeretoryComponent>().each([&](EntityId id, TeretoryComponent& t) {
		if (teretoryColors.size() <= t.teretoryId) {
			teretoryColors.resize(t.teretoryId + 1);
		}
		teretoryColors[t.teretoryId].color = t.color.vec();
		teretoryColors[t.teretoryId].entityId = Color(id).vec();
	});

	if (!buffer) {
		buffer = Ref<Buffer>::make();
		buffer->init(teretoryColors.data(), teretoryColors.size() * sizeof(teretoryColors[0]), sizeof(teretoryColors[0]), tri::UNIFORM_BUFFER, true);
	}
	else {
		buffer->setData(teretoryColors.data(), teretoryColors.size() * sizeof(teretoryColors[0]));
	}


	env->scene->view<Camera, Transform>().each([&](Camera& cam, Transform &camTransform) {
		if (cam.active) {
			env->scene->view<MapComponent, Transform>().each([&](MapComponent& map, Transform& t) {
				if (map.mesh) {
					shader->bind();

					if (cam.output) {
						cam.output->bind();
					}
					else {
						FrameBuffer::unbind();
					}

					shader->set("iTransform", t.getMatrix());
					shader->set("projection", cam.projection);
					shader->set("uTeretories", buffer.get());

					shader->set("mapTexture", 0);
					texture->bind(0);

					map.mesh->vertexArray.submit();
				}
			});
		}
	});

}
