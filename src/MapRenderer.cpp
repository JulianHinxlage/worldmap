//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//


#include "pch.h"
#include "MapRenderer.h"
#include "engine/AssetManager.h"
#include "entity/Scene.h"
#include "engine/Transform.h"
#include "engine/Camera.h"
#include "render/RenderContext.h"
#include "render/Window.h"
#include <glm/gtc/matrix_transform.hpp>

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
TRI_REGISTER_MEMBER(TeretoryComponent, outline)

TRI_REGISTER_SYSTEM(MapRenderer)

class TeretoryData {
public:
	glm::vec4 color;
	glm::vec4 entityId;
};

TRI_UPDATE_CALLBACK("MapRenderer/outlines") {
	if (auto* map = env->systems->getSystem<MapRenderer>()) {
		map->drawOutlines();
	}
}

void MapRenderer::startup() {
	env->signals->update.callbackOrder({ "MapRenderer", "Skybox", "MeshComponent", "MapRenderer/outlines" });
}


void drawTexture(Ref<FrameBuffer> target, Ref<FrameBuffer> source, bool blur, bool horizontal) {
	Ref<Shader> blurShader = env->assets->get<Shader>("blur.glsl");
	Ref<Mesh> plane = env->assets->get<Mesh>("models/plane.obj");

	Transform t;
	blurShader->bind();

	if (target) {
		target->bind();
	}
	else {
		FrameBuffer::unbind();
	}

	t.rotation.x = glm::radians(90.0f);
	t.scale = { 2, 2, 2 };
	blurShader->set("iTransform", t.calculateLocalMatrix());
	blurShader->set("projection", glm::mat4(1));
	if (blur) {
		if (horizontal) {
			blurShader->set("spread", glm::vec2(0, 0.001));
		}
		else {
			blurShader->set("spread", glm::vec2(0.001, 0));
		}
		blurShader->set("steps", 2);
	}
	else {
		blurShader->set("steps", 0);
	}
	blurShader->set("blurTexture", 0);
	source->getAttachment(COLOR)->bind(0);
	plane->vertexArray.submit();
}

void drawMap(Ref<FrameBuffer> target, Ref<Mesh> mesh, Ref<Buffer> buffer, glm::mat4 transform, glm::mat4 projection, std::vector<TeretoryData> &data) {
	Ref<Shader> shader = env->assets->get<Shader>("map.glsl");
	Ref<Texture> texture = env->assets->get<Texture>("map.jpg");

	buffer->setData(data.data(), data.size() * sizeof(data[0]));
	shader->bind();

	if (target) {
		target->bind();
	}
	else {
		FrameBuffer::unbind();
	}

	shader->set("iTransform", transform);
	shader->set("projection", projection);
	shader->set("uTeretories", buffer.get());

	shader->set("mapTexture", 0);
	texture->bind(0);

	mesh->vertexArray.submit();
}

void MapRenderer::update() {
	Ref<Shader> shader = env->assets->get<Shader>("map.glsl");
	Ref<Texture> texture = env->assets->get<Texture>("map.jpg");

	Ref<Shader> blurShader = env->assets->get<Shader>("blur.glsl");
	Ref<Mesh> plane = env->assets->get<Mesh>("models/plane.obj");
	
	if (!fb1) {
		fb1 = Ref<FrameBuffer>::make();
		fb1->setAttachment({ COLOR, Color(0, 0, 0, 0) });
		fb1->setAttachment({ DEPTH, Color(-1) });
		fb2 = Ref<FrameBuffer>::make();
		fb2->setAttachment({ COLOR, Color(0, 0, 0, 0) });
		fb2->setAttachment({ DEPTH, Color(-1) });
	}
	Material mat;

	std::vector<TeretoryData> teretoryColors;
	std::vector<TeretoryData> teretoryOutlineColors;
	std::vector<TeretoryData> teretoryCutoutColors;
	env->scene->view<TeretoryComponent>().each([&](EntityId id, TeretoryComponent& t) {
		if (teretoryColors.size() <= t.teretoryId) {
			teretoryColors.resize(t.teretoryId + 1);
			teretoryOutlineColors.resize(t.teretoryId + 1);
			teretoryCutoutColors.resize(t.teretoryId + 1);
		}
		teretoryColors[t.teretoryId].color = t.color.vec();
		teretoryColors[t.teretoryId].entityId = Color(id).vec();

		teretoryOutlineColors[t.teretoryId].color = t.outline ? Color::red.vec() : Color::transparent.vec();
		teretoryOutlineColors[t.teretoryId].entityId = Color(id).vec();

		teretoryCutoutColors[t.teretoryId].color = t.outline ? t.color.vec() : Color::transparent.vec();
		teretoryCutoutColors[t.teretoryId].entityId = Color(id).vec();
	});

	if (!buffer) {
		buffer = Ref<Buffer>::make();
		buffer->init(teretoryColors.data(), teretoryColors.size() * sizeof(teretoryColors[0]), sizeof(teretoryColors[0]), tri::UNIFORM_BUFFER, true);
	}

	env->scene->view<Camera, Transform>().each([&](Camera& cam, Transform &camTransform) {
		if (cam.active) {
			env->scene->view<MapComponent, Transform>().each([&](MapComponent& map, Transform& t) {

				if (map.mesh && fb1 && fb2) {
					glm::vec2 size = { 0, 0 };
					if (cam.output) {
						size = cam.output->getSize();
					}
					else {
						size = env->window->getSize();
					}
					if (fb1->getSize() != size) {
						fb1->resize(size.x, size.y);
						fb2->resize(size.x, size.y);
					}
					fb1->clear();
					fb2->clear();

					drawMap(cam.output, map.mesh, buffer, t.getMatrix(), cam.projection, teretoryColors);
	
					RenderContext::setDepth(false);
					drawMap(fb1, map.mesh, buffer, t.getMatrix(), cam.projection, teretoryOutlineColors);
					drawTexture(fb2, fb1, true, true);
					fb1->clear();
					drawTexture(fb1, fb2, true, false);
					drawMap(fb1, map.mesh, buffer, t.getMatrix(), cam.projection, teretoryCutoutColors);
					RenderContext::setDepth(true);
					
				}
				
			});
		}
	});

}

void MapRenderer::drawOutlines() {
	env->scene->view<Camera, Transform>().each([&](Camera& cam, Transform& camTransform) {
		if (cam.active) {
			env->scene->view<MapComponent, Transform>().each([&](MapComponent& map, Transform& t) {

				if (fb1) {
					RenderContext::setDepth(false);
					drawTexture(cam.output, fb1, false, false);
					RenderContext::setDepth(true);
				}

			});
		}
	});
}