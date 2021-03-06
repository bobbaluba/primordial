#include "culler.h"

Culler::Culler()
{
    // ctor
}

Culler::~Culler()
{
    // dtor
}

void Culler::stage(Scene &scene_in, World &world_in)
{
//    scene_in.camera = world_in.camera;
    scene_in.camera = world_in.active_cam;
    scene_in.main_light = world_in.main_light;

    scene_in.props.clear(); /// rebuild the render list
    scene_in.actors.clear(); /// rebuild the render list

    for (auto worldobject : world_in.worldobjects)
    {
        scene_in.props.push_back(worldobject);
    }

    for (auto creature : world_in.creatures)
    {
        scene_in.actors.push_back(creature);
    }
}
