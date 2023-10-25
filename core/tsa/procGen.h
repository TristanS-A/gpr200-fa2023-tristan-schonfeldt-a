//
// Created by tropi on 10/25/2023.
//
#pragma once
#include "../ew/mesh.h"

namespace tsa {
    ew::MeshData createSphere(float radius, int numSegments);

    ew::MeshData createCylinder(float height, float radius, int numSegments);

    ew::MeshData createPlane(float size, int numSegments);
}