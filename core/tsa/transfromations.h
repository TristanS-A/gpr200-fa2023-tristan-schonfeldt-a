#pragma once
#include "../ew/ewMath/mat4.h"
#include "../ew/ewMath/vec3.h"
#include <ew/ewMath/ewMath.h>

namespace tsa {

	//Identity matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Scale on x,y,z axes
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		);
	};
	//Rotation around X axis (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		return ew::Mat4(
			1, 0, 0, 0,
			0, cos(rad), -sin(rad), 0,
			0, sin(rad), cos(rad), 0,
			0, 0, 0, 1
		);
	};
	//Rotation around Y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		return ew::Mat4(
			cos(rad), 0, sin(rad), 0,
			0, 1, 0, 0,
			-sin(rad), 0, cos(rad), 0,
			0, 0, 0, 1
		);
	};
	//Rotation around Z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		return ew::Mat4(
			cos(rad), -sin(rad), 0, 0,
			sin(rad), cos(rad), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Translate x,y,z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	};

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); //Euler angles (degrees)
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const {
			return Translate(position) * RotateY(ew::Radians(rotation.y)) * RotateX(ew::Radians(rotation.x)) * RotateZ(ew::Radians(rotation.z)) * Scale(scale);
		}
	};

    inline ew::Mat4 LookAt(ew::Vec3 eyePos, ew::Vec3 target, ew::Vec3 up){
        //Epic Gram-Schmidt moment

        ew::Vec3 forward = target - eyePos;
        forward = forward / ew::Magnitude(forward);

        ew::Vec3 right = ew::Cross(forward, up);
        right = right / ew::Magnitude(right);

        ew::Vec3 left = ew::Cross(forward, right);
        left = left / ew::Magnitude(left);

        return ew::Mat4(
                right.x, left.x, forward.x, 0,
                right.y, left.y, forward.y, 0,
                right.z, left.z, forward.z, 0,
                0, 0, 0, 1
                );
    }

    inline ew::Mat4 Orthographic(float height, float aspect, float near, float far){

    }

    inline ew::Mat4 Perspective(float fov, float aspect, float near, float far){

    }
}
