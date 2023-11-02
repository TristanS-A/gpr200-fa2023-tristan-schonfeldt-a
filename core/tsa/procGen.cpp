//
// Created by tropi on 10/25/2023.
//

#include "procGen.h"

#include <iostream>

namespace tsa{
    ew::MeshData createTorus(float innerRadius, float outerRadius, int numSegments){
        ew::MeshData newMesh;

        float thetaStep = 2 * ew::PI / numSegments;
        float phiStep = 2 * ew::PI / numSegments;

        for (int i = 0; i <= numSegments; i++){
            float theta = i * thetaStep;
            for (int j = 0; j <= numSegments; j++){
                float phi = j * phiStep;
                ew::Vertex currVertex;
                currVertex.pos.x = cos(theta) * (outerRadius + cos(phi) * innerRadius);
                currVertex.pos.y = sin(theta) * (outerRadius + cos(phi) * innerRadius);
                currVertex.pos.z = sin(phi) * innerRadius;
                currVertex.uv = ew::Vec2((float)j / numSegments, (float)i / numSegments);
                currVertex.normal = ew::Normalize(ew::Vec3(cos(theta) * (cos(phi)), sin(theta) * (cos(phi)), sin(phi)));
                newMesh.vertices.push_back(currVertex);
            }
        }

        for (int i = 0; i < numSegments; i++){
            for (int j = 0; j <= numSegments; j++) {
                newMesh.indices.push_back(i + (j * numSegments));
                newMesh.indices.push_back(i + ((j + 1) * numSegments));
                newMesh.indices.push_back(i + 1 + ((j + 1) * numSegments));
                newMesh.indices.push_back(i + (j * numSegments));
                newMesh.indices.push_back(i + + 1 + ((j + 1) * numSegments));
                newMesh.indices.push_back(i + 1 + (j * numSegments));
            }
        }

        return newMesh;
    }

    ew::MeshData createSphere(float radius, int numSegments){
        ew::MeshData newMesh;

        float thetaStep = 2 * ew::PI / numSegments;
        float phiStep = ew::PI / numSegments;

        for (int row = 0; row <= numSegments; row++){
            float phi = row * phiStep;
            for (int col = 0; col <= numSegments; col++){
                float theta = col * thetaStep;
                ew::Vertex currVertex;
                currVertex.pos = ew::Vec3(radius * cos(theta) * sin(phi), radius * cos(phi), radius * sin(theta) * sin(phi));
                currVertex.uv = ew::Vec2((float)col / numSegments, abs((float)row / numSegments - 1));
                currVertex.normal = ew::Normalize(ew::Vec3(currVertex.pos));
                newMesh.vertices.push_back(currVertex);
            }
        }

        int poleStart = 0;
        int ringStart = numSegments + 1;
        for (int i = 0; i < numSegments; i++){
            newMesh.indices.push_back(ringStart + i);
            newMesh.indices.push_back(poleStart + i);
            newMesh.indices.push_back(ringStart + i + 1);
        }

        float columns = numSegments + 1;
        for (int row = 1; row < numSegments - 1; row++){
            for (int col = 0; col < numSegments; col++){
                float start = row * columns + col;
                newMesh.indices.push_back(start);
                newMesh.indices.push_back(start + 1);
                newMesh.indices.push_back(start + columns);
                newMesh.indices.push_back(start + columns);
                newMesh.indices.push_back(start + 1);
                newMesh.indices.push_back(start + columns + 1);
            }
        }

        poleStart = numSegments * (numSegments + 1) + 1;
        ringStart = numSegments * (numSegments + 1) - numSegments - 1;
        for (int i = 0; i < numSegments; i++){
            newMesh.indices.push_back(ringStart + i + 1);
            newMesh.indices.push_back(poleStart + i);
            newMesh.indices.push_back(ringStart + i);
        }

        return newMesh;
    }

    ew::MeshData createCylinder(float height, float radius, int numSegments){
        ew::MeshData newMesh;

        float topHeight = height / 2;
        float bottomHeight = -topHeight;

        ew::Vertex topCenter = {ew::Vec3(0, topHeight, 0), ew::Vec3(0, 1, 0), ew::Vec2(0.5, 0.5)};

        newMesh.vertices.push_back(topCenter);

        makeRingWithInputNormals(newMesh, ew::Vec3(0, 1, 0), topHeight, radius, numSegments);

        makeRingWithSideNormals(newMesh, topHeight, radius, numSegments, 1);

        makeRingWithSideNormals(newMesh, bottomHeight, radius, numSegments, 0);

        makeRingWithInputNormals(newMesh, ew::Vec3(0, -1, 0), bottomHeight, radius, numSegments);

        ew::Vertex bottomCenter = {ew::Vec3(0, bottomHeight, 0), ew::Vec3(0, -1, 0), ew::Vec2(0.5, 0.5)};
        newMesh.vertices.push_back(bottomCenter);

        int start = 1;
        int center = 0;
        for (int i = 0; i <= numSegments; i++){
            newMesh.indices.push_back(start + i);
            newMesh.indices.push_back(center);
            newMesh.indices.push_back(start + 1 + i);
        }

        int sideStart = 1 + numSegments;
        int columns = numSegments + 1;
        for (int i = 0; i < columns; i++){
            start = sideStart + i;
            newMesh.indices.push_back(start);
            newMesh.indices.push_back(start + 1);
            newMesh.indices.push_back(start + columns);
            newMesh.indices.push_back(start + columns);
            newMesh.indices.push_back(start + 1);
            newMesh.indices.push_back(start + columns + 1);
        }

        start = numSegments * 3 + 4;
        center = numSegments * 4 + 5;
        for (int i = 0; i <= numSegments; i++){
            newMesh.indices.push_back(start + i + 1);
            newMesh.indices.push_back(center);
            newMesh.indices.push_back(start + i);
        }

        return newMesh;
    }

    void makeRingWithSideNormals(ew::MeshData& meshData, float yPos, float radius, int numSegments, float yUV) {
        float thetaStep = ew::PI * 2 / numSegments;
        for (int i = 0; i <= numSegments; i++) {
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = radius * cos(theta);
            currVertex.pos.z = radius * sin(theta);
            currVertex.pos.y = yPos;
            currVertex.normal = ew::Normalize(ew::Vec3(cos(theta), 0, sin(theta)));
            currVertex.uv.x = (cos(theta) + 1) * 0.5;
            currVertex.uv.y = yUV;
            meshData.vertices.push_back(currVertex);
        }
    }

    void makeRingWithInputNormals(ew::MeshData& meshData, ew::Vec3 normalVec, float yPos, float radius, int numSegments) {
        float thetaStep = ew::PI * 2 / numSegments;
        for (int i = 0; i <= numSegments; i++) {
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = radius * cos(theta);
            currVertex.pos.z = radius * sin(theta);
            currVertex.pos.y = yPos;
            currVertex.normal = normalVec;
            currVertex.uv.x = (cos(theta) + 1) * 0.5;
            currVertex.uv.y = (sin(theta) + 1) * 0.5;
            meshData.vertices.push_back(currVertex);
        }
    }

    ew::MeshData createPlane(float size, int numSegments){
        ew::MeshData newMesh;

        for (int row = 0; row <= numSegments; row++){
            for (int col = 0; col <= numSegments; col++){
                ew::Vertex currVertex;
                float newFloatSegments = static_cast<float>(numSegments);
                currVertex.pos.x = size * (col / newFloatSegments);
                currVertex.pos.z = -size * (row / newFloatSegments);
                currVertex.uv.x = col / newFloatSegments;
                currVertex.uv.y = row / newFloatSegments;
                currVertex.normal = ew::Vec3(0, 1, 0);
                newMesh.vertices.push_back(currVertex);
            }
        }

        int columns = numSegments + 1;
        for (int row = 0; row < numSegments; row++) {
            for (int col = 0; col < numSegments; col++) {
                int start = row * columns + col; 
                newMesh.indices.push_back(start);
                newMesh.indices.push_back(start + 1);
                newMesh.indices.push_back(start + columns + 1);
                newMesh.indices.push_back(start + columns + 1);
                newMesh.indices.push_back(start + columns);
                newMesh.indices.push_back(start);
            }
        }

        return newMesh;
    }

    ew::MeshData createCone(float height, float radius, int numSegments){
        ew::MeshData newMesh;

        float newHeight = height / 2;
        float thetaStep = ew::PI * 2 / numSegments;
        for (int i = 0; i <= numSegments; i++){
            float slice = (float)i / numSegments;
            for (int j = 0; j <= numSegments; j++){
                float theta = j * thetaStep;
                ew::Vertex currVertex;
                currVertex.pos.x = (radius * slice) * cos(theta);
                currVertex.pos.z = (radius * slice) * sin(theta);
                currVertex.pos.y = newHeight - 2 * (newHeight * (radius * slice / radius));
                currVertex.normal = ew::Vec3(cos(theta), -radius / height, sin(theta));
                currVertex.uv = ew::Vec2((float)j / numSegments, (float)i / numSegments);
                newMesh.vertices.push_back(currVertex);
            }
        }

        makeRingWithInputNormals(newMesh, ew::Vec3(0, -1, 0), -newHeight, radius, numSegments);

        ew::Vertex bottomCenter = {ew::Vec3(0, -newHeight, 0), ew::Vec3(0, -1, 0), ew::Vec2(0.5, 0.5)};
        newMesh.vertices.push_back(bottomCenter);

        int poleStart = 0;
        int ringStart = numSegments + 1;
        for (int i = 0; i < numSegments; i++){
            newMesh.indices.push_back(ringStart + i);
            newMesh.indices.push_back(poleStart + i);
            newMesh.indices.push_back(ringStart + i + 1);
        }

        int columns = numSegments + 1;
        for (int row = 1; row < numSegments; row++){
            for (int col = 0; col < numSegments; col++){
                int start = row * columns + col;
                newMesh.indices.push_back(start);
                newMesh.indices.push_back(start + 1);
                newMesh.indices.push_back(start + columns + 1);
                newMesh.indices.push_back(start + columns + 1);
                newMesh.indices.push_back(start + columns);
                newMesh.indices.push_back(start);
            }
        }

        int start = (numSegments + 1) * (numSegments + 1);
        int center = (numSegments + 1) * (numSegments + 1) + numSegments + 1;
        for (int i = 0; i <= numSegments; i++){
            newMesh.indices.push_back(start + i + 1);
            newMesh.indices.push_back(center);
            newMesh.indices.push_back(start + i);
        }

        return newMesh;
    }
}