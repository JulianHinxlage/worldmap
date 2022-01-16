//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "TeretoryGenerator.h"
#include "core/util/StrUtil.h"

using namespace tri;

bool TeretoryGenerator::load(const std::string& file) {
    std::string content = StrUtil::readFile(file);
    if (content.size() > 0) {

        std::string d1 = "\n";
        std::string d2 = ":";
        std::string d3 = ";";
        std::string d4 = ",";

        auto l1 = StrUtil::split(content, d1, false);
        for (auto e1 : l1) {
            auto l2 = StrUtil::split(e1, d2, false);

            if (l2.size() >= 3) {
                std::string type = l2[0];
                std::string name = l2[1];
                std::string parent = l2[2];

                Teretory t;
                t.name = name;
                t.parent = parent;
                if (type == "country") {
                    t.type = TeretoryType::COUNTRY;
                }
                else if (type == "state") {
                    t.type = TeretoryType::STATE;
                }
                else if (type == "city") {
                    t.type = TeretoryType::CITY;
                }

                t.position = { 0, 0 };
                for (int i = 3; i < l2.size(); i++) {
                    auto e2 = l2[i];
                    if (type == "city") {
                        auto l3 = StrUtil::split(e2, d4, false);
                        if (l3.size() >= 2) {

                            glm::vec2 p;
                            try {
                                p = { std::stod(l3[0]), std::stod(l3[1]) };
                            }
                            catch (...) {}

                            t.position = p;
                        }
                    }
                    else {
                        auto l3 = StrUtil::split(e2, d3, false);
                        t.geometry.push_back({});

                        for (auto e3 : l3) {
                            auto l4 = StrUtil::split(e3, d4, false);
                            if (l4.size() >= 2) {

                                glm::vec2 p;
                                try {
                                    p = { std::stod(l4[0]), std::stod(l4[1]) };
                                }
                                catch (...) {}

                                t.geometry.back().push_back(p);
                            }
                        }
                    }
                }

                //calculate center point of biggest polygon
                int max = 0;
                int maxIndex = 0;
                for (int i = 0; i < t.geometry.size(); i++) {
                    if (t.geometry[i].size() > max) {
                        max = t.geometry[i].size();
                        maxIndex = i;
                    }
                }
                if (max > 0) {
                    for (int i = 0; i < t.geometry[maxIndex].size(); i++) {
                        t.position += t.geometry[maxIndex][i];
                    }
                    t.position /= t.geometry[maxIndex].size();
                }

                //add to list
                if (t.type == TeretoryType::COUNTRY) {
                    bool added = false;
                    for (int i = 0; i < teretory.size(); i++) {
                        if (teretory[i].name == t.parent) {
                            teretory[i].children.push_back(t);
                            added = true;
                            index = i;
                            break;
                        }
                    }
                    if (!added) {
                        Teretory tmp;
                        tmp.type = TeretoryType::CONTINENT;
                        tmp.name = t.parent;
                        tmp.children.push_back(t);
                        tmp.position = { 0, 0 };
                        teretory.push_back(tmp);
                        index = teretory.size() - 1;
                    }
                }
                else if (t.type == TeretoryType::STATE) {
                    if (index < teretory.size()) {
                        auto& list = teretory[index].children;
                        if (list.size() > 0) {
                            list[list.size() - 1].children.push_back(t);
                        }
                    }
                }
                else if (t.type == TeretoryType::CITY) {
                    if (index < teretory.size()) {
                        auto& list = teretory[index].children;
                        if (list.size() > 0) {
                            auto& list2 = list[list.size() - 1].children;
                            if (list2.size() > 0) {
                                list2[list2.size() - 1].children.push_back(t);
                            }
                        }
                    }
                }

            }
        }

        return true;
    }
    else {
        return false;
    }
}

void TeretoryGenerator::each(const std::function<int(Teretory& t, int id)>& step, int id) {
    for (auto& t : teretory) {
        each(step, step(t, id), t);
    }
}

void TeretoryGenerator::each(const std::function<int(Teretory& t, int id)>& step, int id, Teretory& t) {
    for (auto& i : t.children) {
        each(step, step(i, id), i);
    }
}