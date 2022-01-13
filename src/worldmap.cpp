//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "core/core.h"
#include "engine/AssetManager.h"
#include "engine/Input.h"
#include "MapGenerator.h"

namespace tri {

    class WorldMapModule : public Module {
    public:
        virtual void startup() override {

        }

        virtual void update() override {
            if (env->input->down(Input::Key::KEY_LEFT_CONTROL) && env->input->pressed(Input::Key::KEY_G)) {
                Clock clock;
                MapGenerator generator;
                std::string file = env->assets->searchFile("worldmap.txt");
                if (!generator.generate(file, true)) {
                    env->console->info("faild to load file ", file);
                }
                env->console->info("generating world map took ", clock.elapsed(), "s");
            }
        }

    };

    TRI_REGISTER_MODULE(WorldMapModule);

}
