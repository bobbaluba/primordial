#ifndef WORLD_H
#define WORLD_H

#include <list>
#include <memory.h>

#include "worldobject.h"
#include "creature.h"
#include "resourcemanager.h"
#include "camera.h"
#include "physicsworld.h"


using std::list;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

class World : public PhysicsWorld /// Consider merging Culling functionality into World Class
{
    public:
        World();
        virtual ~World();

        list<shared_ptr<WorldObject>>::iterator addStaticObject(string mesh_key, string tex_key, vec3 pos, vec3 dir);
        list<shared_ptr<WorldObject>>::iterator addDynamicObject(string mesh_key, string tex_key, vec3 pos, vec3 dir);
        void delWorldObject(list<shared_ptr<WorldObject>>::iterator obst_it_in);

        list<shared_ptr<Creature>>::iterator addCreature(string mesh_key, string tex_key, vec3 pos, vec3 dir);
        void delCreature(list<shared_ptr<Creature>>::iterator creature_it_in);

//        /// Step
//        void step(float dt_in);


        /// "Physical" Contents (could with benefit be private?)
        list<shared_ptr<WorldObject>>    worldobjects;
        list<shared_ptr<Creature>>       creatures;
        /// list<shared_ptr<Light>>       lights; // For future
        /// Terrain                       terrain; // For future


        /// WorldObject
        /// WorldStatic
        /// WorldCreature ?


        shared_ptr<Camera> camera; /// Shared pointer here, because we do not want other shared
        /// pointers to accidentally delete the camera.

        // void forAllWorldObjects( void (*f) (WorldObject&) );

    protected:
    private:
        ResourceManager<Mesh>    mesh_manager;
        ResourceManager<Texture> tex_manager;

};

#endif // WORLD_H
