//
// Created by tropi on 10/25/2023.
//
#pragma once
#include "../ew/mesh.h"

namespace tsa {
    ew::MeshData createSphere(float radius, int numSegments);

    ew::MeshData createCylinder(float height, float radius, int numSegments);

    void makeRingWithSideNormals(ew::MeshData& meshData, float yPos, float radius, int numSegments);

    void makeRingWithInputNormals(ew::MeshData& meshData, ew::Vec3 normlaVec, float yPos, float radius, int numSegments);

    ew::MeshData createPlane(float size, int numSegments);
}