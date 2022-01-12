//
// Copyright (c) 2021 Julian Hinxlage. All rights reserved.
//

#include "core/core.h"

namespace tri{

    class WorldMapModule : public Module {
    public:
        virtual void startup() override {
            
        }

        virtual void update() override {

        }

    };

    TRI_REGISTER_MODULE(WorldMapModule);

}