#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
//#include "transform.h"
// #include "quaternion.h"

//using glm::vec3;
//using glm::vec4;
//using glm::mat4;

#include "signals.h"

class Object3d : virtual public MoveSignalReceiver
{
public:
    Object3d();
    virtual ~Object3d();

    glm::vec3 getDir();
    void setDir(glm::vec3 u);

    glm::mat4 getTransformMatrix() const;

    // Object3d implementation of MoveSignalReceiver
    void moveForward(float meters);
    void moveLeft(float meters);
    void rotateUp(float degrees);
    void rotateLeft(float degrees);

public:
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scale;

protected:
private:
};

#endif // OBJECT3D_H
