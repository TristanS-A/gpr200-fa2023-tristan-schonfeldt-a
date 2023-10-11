//
// Created by tropi on 10/11/2023.
//

#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include <ew/ewMath/ewMath.h>
#include "transfromations.h"

namespace tsa{
    struct Camera {
        ew::Vec3 position;
        ew::Vec3 target;
        float fov;
        float aspect;
        float nearPlain;
        float farPlain;
        bool orthographic;
        float orthoSize;
        ew::Mat4 ViewMatrix(){
            return LookAt(position, target, ew::Vec3(0, 1, 0)) * ew::Mat4(
                                                                                1, 0, 0, -position.x,
                                                                                0, 1, 0, -position.y,
                                                                                0, 0, 1, -position.z,
                                                                                0, 0, 0, 1
                                                                                );
        };
        ew::Mat4 ProjectionMatrix(){
            if (orthographic){
                return Orthographic(orthoSize, aspect, nearPlain, farPlain);
            }
            else {
                return Perspective(fov, aspect, nearPlain, farPlain);
            }
        };
    };
}