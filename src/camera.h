#ifndef PLAYER_H
#define PLAYER_H

#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define PLAY 0
#define PAUSE 1


class Camera{
public:
    void init(
        glm::vec3 _pos,
        float _posSpeed,
        glm::vec2 _angle,
        glm::vec2 _angleSpeed,
        glm::vec3 _lowerBound,
        glm::vec3 _upperBound
    );
    void update(float deltaPosF, float deltaPosS, glm::vec2 deltaAngle);

    glm::mat4 getConvMatrix() const;

private:
    glm::vec3 pos;
    float posSpeed;

    glm::vec2 angle;
    glm::vec2 angleSpeed;

    glm::vec3 lowerBound;
    glm::vec3 upperBound;

    double prevMousePosX;
    double prevMousePosY;
}; 


#endif