//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"
#include "MapRenderer.h"
#include <glm/glm.hpp>

class TeretoryGenerator {
public:
    class Teretory {
    public:
        std::string name;
        std::string parent;
        TeretoryType type;
        std::vector<Teretory> children;
        glm::vec2 position;
        std::vector<std::vector<glm::vec2>> geometry;
    };
    std::vector<Teretory> teretory;

    bool load(const std::string& file);
    void each(const std::function<int(Teretory& t, int id)>& step, int id);

private:
    int index = 0;

    void each(const std::function<int(Teretory& t, int id)>& step, int id, Teretory& t);
};
