#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <rtEngine/model.hpp>
#include <rtEngine/component.hpp>

class GameObject
{
public:
    Model *model = nullptr;

    std::vector<Component *> components;
    std::vector<GameObject *> children;
    GameObject *parent = nullptr;
    void addChildrenRecursive(std::vector<GameObject *> &go_vector);

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 si)
    {
        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, si);
    }
    GameObject()
    {
    }

    glm::mat4 getGlobalModelMatrix()
    {
        glm::mat4 globalMat = modelMatrix;
        GameObject *curr = parent;
        while (curr != nullptr)
        {
            globalMat = curr->modelMatrix * globalMat;
            curr = curr->parent;
        }
        return globalMat;
    }
    glm::vec3 getGlobalPosition()
    {
        return glm::vec3(getGlobalModelMatrix()[3]);
    }
    glm::quat getGlobalRotation()
    {
        return glm::quat(glm::mat3(getGlobalModelMatrix()));
    }

    glm::vec3 getGlobalRotationEuler()
    {
        return glm::eulerAngles(getGlobalRotation());
    }

    glm::vec3 getGlobalScale()
    {
        return glm::vec3(glm::length(getGlobalModelMatrix()[0]), glm::length(getGlobalModelMatrix()[1]), glm::length(getGlobalModelMatrix()[2]));
    }
    glm::vec3 getGlobalForward()
    {
        // glm::vec4 f =  glm::vec4(0,0,1.0f,-1.0f) * glm::toMat4(glm::quat(glm::radians(rotation)));
        glm::vec4 f = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
        glm::mat4 rotmat = glm::toMat4(getGlobalRotation());
        f = rotmat * f;
        return glm::normalize(glm::vec3(f.x, f.y, f.z));
    }
    glm::vec3 getGlobalUp()
    {
        glm::vec3 front = getGlobalForward();
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 up = glm::normalize(glm::cross(right, front));
        return up;
    }
    glm::vec3 getGlobalRight()
    {
        glm::vec3 front = getGlobalForward();
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        return right;
    }

    void translateLocal(glm::vec3 translation)
    {
        modelMatrix = glm::translate(modelMatrix, translation);
    }

    void rotateLocalEuler(glm::vec3 rot)
    {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void rotateLocal(glm::quat rot)
    {
        modelMatrix *= glm::toMat4(rot);
    }

    void scaleLocal(glm::vec3 sc)
    {
        modelMatrix = glm::scale(modelMatrix, sc);
    }

    void translateGlobal(glm::vec3 translation)
    {
        glm::mat4 temp = glm::inverse(getGlobalModelMatrix()) * glm::translate(glm::mat4(1.0f), translation) * getGlobalModelMatrix();
        modelMatrix *= temp;
    }

    void rotateGlobal(glm::quat rotation)
    {
        glm::quat temp = glm::inverse(getGlobalRotation()) * rotation * getGlobalRotation();
        rotateLocal(temp);
    }

    void setLocalRotation(glm::quat rotation)
    {
        glm::mat4 temp = glm::mat4(1.0f);
        temp = glm::translate(temp, getLocalPosition());
        temp *= glm::toMat4(rotation);
        temp = glm::scale(temp, getLocalScale());
        modelMatrix = temp;
    }

    void setLocalPosition(glm::vec3 position)
    {
        modelMatrix[3][0] = position.x;
        modelMatrix[3][1] = position.y;
        modelMatrix[3][2] = position.z;
    }

    void setLocalScale(glm::vec3 scale)
    {
        glm::mat4 temp = glm::mat4(1.0f);
        temp = glm::translate(temp, getLocalPosition());
        temp *= glm::toMat4(getLocalRotation());
        temp = glm::scale(temp, scale);
        modelMatrix = temp;
    }

    glm::vec3 getLocalPosition()
    {
        return glm::vec3(modelMatrix[3]);
    }

    glm::quat getLocalRotation()
    {
        return glm::toQuat(glm::mat3(modelMatrix));
    }

    glm::vec3 getLocalScale()
    {
        return glm::vec3(
            glm::length(glm::vec3(modelMatrix[0])),
            glm::length(glm::vec3(modelMatrix[1])),
            glm::length(glm::vec3(modelMatrix[2])));
    }

    glm::vec3 getLocalRotationEuler()
    {
        glm::vec3 euler = glm::eulerAngles(getLocalRotation());
        return euler;
    }

    void setLocalRotationEuler(glm::vec3 angles)
    {
        glm::quat q = glm::quat(angles);
        setLocalRotation(q);
    }
};