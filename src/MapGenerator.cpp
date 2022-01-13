//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "MapGenerator.h"
#include "TeretoryGenerator.h"
#include "core/Environment.h"
#include "entity/Scene.h"
#include "engine/EntityInfo.h"
#include "engine/Transform.h"
#include "engine/MeshComponent.h"
#include "engine/AssetManager.h"

using namespace tri;

class Vertex {
public:
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

float sign(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pointInTriangle(glm::vec2 p, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
    float d1, d2, d3;
    bool neg, pos;

    d1 = sign(p, v1, v2);
    d2 = sign(p, v2, v3);
    d3 = sign(p, v3, v1);

    neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(neg && pos);
}

void triangulation(std::vector<std::vector<glm::vec2>>& geometry, glm::vec2 center, std::vector<Vertex>& vs, std::vector<int>& is, glm::vec2 uv = { 0, 0 }) {
    int index = vs.size();
    for (auto& geom : geometry) {

        std::vector<glm::vec2> polygon = geom;

        int triCount = polygon.size() - 2;
        for (int k = 0; k < triCount; k++) {
            for (int i = 0; i < polygon.size() - 2; i++) {
                if (sign(polygon[i], polygon[i + 1], polygon[i + 2]) > 0) {
                    continue;
                }
                bool hit = false;
                for (int j = 0; j < polygon.size() - 2; j++) {
                    if (j != i && j != i + 1 && j != i + 2) {
                        if (pointInTriangle(polygon[j], polygon[i], polygon[i + 1], polygon[i + 2])) {
                            hit = true;
                            break;
                        }
                    }
                }
                if (!hit) {
                    vs.push_back({ glm::vec3(polygon[i + 0].x - center.x, 0, -polygon[i + 0].y + center.y), glm::vec3(0, 1, 0), uv });
                    vs.push_back({ glm::vec3(polygon[i + 2].x - center.x, 0, -polygon[i + 2].y + center.y), glm::vec3(0, 1, 0), uv });
                    vs.push_back({ glm::vec3(polygon[i + 1].x - center.x, 0, -polygon[i + 1].y + center.y), glm::vec3(0, 1, 0), uv });
                    is.push_back(index + 0);
                    is.push_back(index + 1);
                    is.push_back(index + 2);
                    index += 3;

                    polygon.erase(polygon.begin() + i + 1);
                    break;
                }
            }
        }
    }
}

Ref<Mesh> triangulation(std::vector<std::vector<glm::vec2>>& geometry, glm::vec2 center) {
    if (geometry.size() > 0) {
        Ref<Mesh> m = Ref<Mesh>::make();
        std::vector<Vertex> vs;
        std::vector<int> is;
        triangulation(geometry, center, vs, is);
        m->create((float*)vs.data(), vs.size() * sizeof(Vertex) / sizeof(float), is.data(), is.size(), { {tri::FLOAT,3},{tri::FLOAT,3},{tri::FLOAT,2} }, true);
        return m;
    }
    else {
        return nullptr;
    }
}

bool MapGenerator::generate(const std::string& file, bool singleMesh){
	TeretoryGenerator teretoryGenerator;
    int index = 0;
	if (teretoryGenerator.load(file)) {
		EntityId id = env->scene->addEntity(EntityInfo("worldmap"), Transform(), NoHierarchyUpdate());

        if (singleMesh) {
            std::vector<Vertex> vs;
            std::vector<int> is;

		    teretoryGenerator.each([&](TeretoryGenerator::Teretory& t, int id) {
                Transform transform;
                transform.parent = id;
                transform.position = glm::vec3(t.position.x, 0, -t.position.y);
                if (t.type == TeretoryGenerator::Teretory::COUNTRY) {
                    if (t.geometry.size() > 0) {
                        triangulation(t.geometry, { 0, 0 }, vs, is, { index, index });
                        index++;
                    }
                }
                return (int)env->scene->addEntity(EntityInfo(t.name), transform, NoHierarchyUpdate());
		    }, id);

            env->console->info("world map mesh: ", vs.size(), " vertices");

            Ref<Mesh> mesh = Ref<Mesh>::make();
            mesh->create((float*)vs.data(), vs.size() * sizeof(Vertex) / sizeof(float), is.data(), is.size(), { {tri::FLOAT,3},{tri::FLOAT,3},{tri::FLOAT,2} }, true);
            if (!std::filesystem::exists("map")) {
                std::filesystem::create_directory("map");
            }
            mesh->save("map/map.obj");
            mesh = env->assets->get<Mesh>("map/map.obj");

            Ref<Material> mat = Ref<Material>::make();
            mat->shader = env->assets->get<Shader>("map.glsl");
            mat->save("map/map.mat");
            mat = env->assets->get<Material>("map/map.mat");

            env->scene->addComponent<MeshComponent>(id, MeshComponent(mesh, mat));

        }
        else {
            teretoryGenerator.each([&](TeretoryGenerator::Teretory& t, int id) {
                Transform transform;
                transform.parent = id;
                transform.position = glm::vec3(t.position.x, 0, -t.position.y);
                if (t.type == TeretoryGenerator::Teretory::COUNTRY) {
                    if (t.geometry.size() > 0) {
                        Ref<Mesh> mesh = triangulation(t.geometry, t.position);
                        if (!std::filesystem::exists("map")) {
                            std::filesystem::create_directory("map");
                        }
                        std::string file = "map/" + std::to_string(index) + ".obj";
                        mesh->save(file);
                        mesh = env->assets->get<Mesh>(file);
                        index++;
                        return (int)env->scene->addEntity(EntityInfo(t.name), transform, NoHierarchyUpdate(), MeshComponent(mesh));
                    }
                }
                return (int)env->scene->addEntity(EntityInfo(t.name), transform, NoHierarchyUpdate());
            }, id);
        }

        return true;
    }
    else {
        return false;
    }
}


