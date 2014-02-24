#ifndef ACTOR_H
#define ACTOR_H

#include "skeleton.h"
#include "prop.h"


class Actor : public Prop
{
    public:
        Actor();
        virtual ~Actor();

        /// Add some high level animation functionality later
        /// for now use Skeleton::startAnimation(Anum enum)

        void attachSkeleton(std::weak_ptr<Skeleton> skel_ptr_in);

        /// Temporary for debugging:
        std::shared_ptr<Skeleton> shSkelPtr();

        std::shared_ptr<Skeleton> skel_ptr;

        void pose(int anim_index, float time);
        void poseRest();
        void playAnim(int anim_index);
        void updateAnim(float dt);
        void pauseAnim();
        void unPauseAnim();
        void togglePauseAnim();

        /// debug methods;
        float getActiveAnimTime();
        float getActiveAnimTimeMod();

        /// could consider getter method/private for:
        int num_pose_matrices;
        glm::mat4* pose_matrices;

    protected:
    private:
        int active_anim;
        float active_anim_time;
        bool paused;

};

#endif // ACTOR_H
