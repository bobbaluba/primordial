#include "mechanics.h"

Mechanics::Mechanics() : speed(24.5), camTurnSpeed(80.0),
    controlled(nullptr),
    player(nullptr)
{

}

Mechanics::~Mechanics()
{
    //dtor
}

void Mechanics::init(World &world_in, float &dt_in)
{
    world = &world_in;
    dt = &dt_in;

    // Set the input to control the camera
    //controlled = world->camera.get();
    controlled = world->freecam;

    //world->camera->pos = glm::vec3(0.0, 1.5, 0.0);
    world->freecam->pos = glm::vec3(0.0, 1.5, 0.0);

    /// set the main light (sun/moon)
    world->mainLight( glm::vec3(1.0, 1.0, 1.0),         /// From direction
                      glm::vec4(1.0, 1.0, 1.0, 1.0) );  /// Color

    /// add additional lights
//    world->addPointLight(glm::vec3(3.0, 1.0, -4.0),         /// Position
//                         glm::vec3(1.0, 0.5, 1.0));         /// Color
//
//    world->addPointLight(glm::vec3(-4.0, 1.0, 3.0),         /// Position
//                         glm::vec3(0.0, 1.0, 1.0));         /// Color

    /// Load some resources (should be moved)
    ///                          Model,         Texture,            Position
    /// WorldObjects:
    world->addDynamicObject( "sphere",
                             "nicewall",
                             glm::vec3(-2.0, 10.0, -4.0),
                             RigidBody::Sphere(1.0) );

    world->addDynamicObject( "rabbit",
                             "rabbit_d",
                             glm::vec3(2.0, 10.0, -4.0));

    world->addStaticObject( "quad",
                            "grass_equal",
                             glm::vec3(0.0, 0.0, 0.0) );

    world->addStaticObject( "terrain",
                            "grass_equal",
                             glm::vec3(0.0, 0.0, 0.0) );

    world->addDynamicObject( "axes",
                            "tricolor",
                            glm::vec3(0.0, 8.0, 0.0));

    /// Having issue with human model not in the correct
    /// place in the scene graph outputted by assimp import

    auto playerCreature = world->addCreature( "human_all_anim",
                        "tex_human_male",
                        glm::vec3(3.0, 0.0, 2.0) );

    player = (*playerCreature).get();

//    world->addCreature( "humale_old",
//                        "tex_human_male",
//                        glm::vec3(0.0, 0.0, 0.0) );

    world->addCreature( "humale_1hswing",
                        "tex_human_male",
                        glm::vec3(-3.0, 0.0, 2.0) );

    //world->addStaticObject( "mdl_human_male",
    //                "tex_human_male",
    //                glm::vec3(0.0, 0.0, 0.0) );

    /// Creatures
    world->addCreature( "anim_test",
                        "checkers",
                        glm::vec3(0.0, 2.0, -4.0) );

    world->addDynamicObject( "cube",
                             "nicewall",
                             glm::vec3(2.0, 10.0, -4.0),
                             RigidBody::Box(0.5f, 0.5f, 0.5f) );


    /// remove later
    worldobject_ptr_it = world->worldobjects.begin();

    /// Testing
}

void Mechanics::step(World &world_in, float dt_in)
{
    //world_in.step(dt_in);
    world_in.physicsStep(dt_in);
    for (shared_ptr<WorldObject> wObject : world_in.worldobjects)
    {
        wObject->updateTransformation();
    }

    // Make the chase cam chase the player

}

string Mechanics::debugInfo()
{
    std::ostringstream output;
    output  << "Debugging mechanics: \n"
            << "dt = " << *dt << "\n"
            << "speed = " << speed << "\n";
    return output.str();
}

/// move
void Mechanics::playerMoveForward()
{
    if (controlled) controlled->moveForward((*dt)*speed, *dt);
}

void Mechanics::playerMoveBackward()
{
    if (controlled) controlled->moveForward(-(*dt)*speed, *dt);
}

void Mechanics::playerMoveLeft()
{
    if (controlled) controlled->moveLeft((*dt)*speed, *dt);
}

void Mechanics::playerMoveRight()
{
    if (controlled) controlled->moveLeft(-(*dt)*speed, *dt);
}


/// rotate
void Mechanics::playerRotateUp()
{
    if (controlled) controlled->rotateUp((*dt)*camTurnSpeed, *dt);
}

void Mechanics::playerRotateDown()
{
    if (controlled) controlled->rotateUp(-(*dt)*camTurnSpeed, *dt);
}

void Mechanics::playerRotateLeft()
{
    if (controlled) controlled->rotateLeft((*dt)*camTurnSpeed, *dt);
}

void Mechanics::playerRotateRight()
{
    if (controlled) controlled->rotateLeft(-(*dt)*camTurnSpeed, *dt);
}

void Mechanics::playerRotateUpVal(float val)
{
    if (controlled) controlled->rotateUp(val, *dt);
}

void Mechanics::playerRotateLeftVal(float val)
{
    if (controlled) controlled->rotateLeft(val, *dt);
}

void Mechanics::func(int num_in)
{
    switch (num_in)
    {
    case 1:    /// ADD CUBE
        if (world->worldobjects.empty())
        {
              worldobject_ptr_it = world->addDynamicObject( "rabbit",
                                                          "rabbit_d",
                                                          world->freecam->pos + 2.f*world->freecam->getDir());
                                                          //world->camera->pos + 2.f*world->camera->getDir());
        }
        else
        {
              worldobject_ptr_it = world->addDynamicObject( "rabbit",
                                                          "rabbit_d",
                                                          world->freecam->pos + 2.f*world->freecam->getDir());
                                                          //world->camera->pos + 2.f*world->camera->getDir());
        }
        break;

    case 2:     /// ADD SPHERE
        if (world->worldobjects.empty())
        {
            worldobject_ptr_it = world->addDynamicObject( "sphere",
                                                          "nicewall",
                                                          world->freecam->pos + 2.f*world->freecam->getDir(),
                                                          /*world->camera->pos + 2.f*world->camera->getDir(),*/
                                                          RigidBody::Sphere(1.0));
        }
        else
        {
            world->addDynamicObject( "sphere",
                                     "nicewall",
                                     world->freecam->pos + 2.f*world->freecam->getDir(),
                                     /*world->camera->pos + 2.f*world->camera->getDir(),*/
                                     RigidBody::Sphere(1.0)  );
        }
        break;
    case 3:     world->delWorldObject(worldobject_ptr_it); worldobject_ptr_it = world->worldobjects.begin(); break;
    case 4:
        for (auto &creature : world->creatures)
        {
            creature->togglePauseAnim();
        }
        break;
    case 5:
        for (auto &creature : world->creatures)
        {
            if (creature->getActiveAnimIndex() < (creature->getNumAnims()-1))
            {
                creature->playAnim(creature->getActiveAnimIndex()+1);
            }
            else
            {
                creature->playAnim(0);
            }
        }
        break;
    case 6:
        if (controlled == world->freecam)
        {
            if (world->creatures.size() > 0)
                controlled = world->creatures.begin()->get();
        }
        else
        {
            controlled = world->freecam;
        }
        break;
    case 7:
        if (world->active_cam == world->freecam)
            world->active_cam = world->chasecam;
        else
            world->active_cam = world->freecam;


        break;

    default:    break;
    }
}
