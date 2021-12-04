#include "camera.h" 


void Camera::init(
        glm::vec3 _pos,
        float _posSpeed,
        glm::vec2 _angle,
        glm::vec2 _angleSpeed,
        glm::vec3 _lowerBound,
        glm::vec3 _upperBound
    ){
    
    pos = _pos;

    posSpeed = _posSpeed;

    angle = _angle;
    
    angleSpeed = _angleSpeed;

    lowerBound = _lowerBound;
    upperBound = _upperBound;
}


void Camera::update(float deltaPosF, float deltaPosS, glm::vec2 deltaAngle){
    glm::mat4 conv = glm::rotate(glm::mat4(1.0f), -angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
              conv = glm::rotate(conv, -angle.x, glm::vec3(1.0f, 0.0f, 0.0f));

    angle +=  angleSpeed * deltaAngle; 

    while (angle.y > 2.0f * glm::pi<float>()) {
        angle.y -= 2.0f * glm::pi<float>();
    }
    while (angle.y < 0.0f) {
        angle.y += 2.0f * glm::pi<float>();
    }
    if (angle.x > 0.5f * glm::pi<float>()) {
        angle.x = 0.5f * glm::pi<float>();
    }
    if (angle.x < -0.5f * glm::pi<float>()) {
        angle.x = -0.5f * glm::pi<float>();
    }

    // forward/backward
    glm::vec4 posOffset;
    
    posOffset = {0.0f, 0.0f, -deltaPosF, 0.0f};

    posOffset = conv * posOffset;

    pos += posSpeed * glm::vec3(posOffset);

    // left/right
    posOffset = {-deltaPosS, 0.0f, 0.0f, 0.0f};

    posOffset = conv * posOffset;

    pos += posSpeed * glm::vec3(posOffset);

    if (pos.x < lowerBound.x) {
        pos.x = lowerBound.x;
    } 
    if (pos.y < lowerBound.y) {
        pos.y = lowerBound.y;
    } 
    if (pos.z < lowerBound.z) {
        pos.z = lowerBound.z;
    } 
    if (pos.x > upperBound.x) {
        pos.x = upperBound.x;
    } 
    if (pos.y > upperBound.y) {
        pos.y = upperBound.y;
    }     
    if (pos.z > upperBound.z) {
        pos.z = upperBound.z;
    } 
}

glm::mat4 Camera::getConvMatrix() const {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
              rot = glm::rotate(rot            , angle.y, glm::vec3(0.0f, 1.0f, 0.0f));


    glm::mat4 conv = glm::translate(glm::mat4(1.0), -pos);
    conv = rot * conv;

    return conv;
}