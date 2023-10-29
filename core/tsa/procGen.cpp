//
// Created by tropi on 10/25/2023.
//

#include "procGen.h"

#include <iostream>

namespace tsa{
    ew::MeshData createSphere(float radius, int numSegments){
        ew::MeshData newMesh;

        float thetaStep = 2 * ew::PI / numSegments;
        float phiStep = ew::PI / numSegments;

        for (int row = 0; row <= numSegments; row++){
            float phi = row * phiStep;
            for (int col = 0; col <= numSegments; col++){
                float theta = col * thetaStep;
                ew::Vertex currVertex;
                currVertex.pos = ew::Vec3(radius * sin(theta) * sin(phi), radius * cos(phi), radius * cos(theta) * sin(phi));
                currVertex.uv = ew::Vec2((float)col / numSegments, (float)row / numSegments);
                currVertex.normal = ew::Normalize(ew::Vec3(currVertex.pos.x, currVertex.pos.y, currVertex.pos.z));
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
        ringStart = numSegments * (numSegments + 1) - numSegments;
        for (int i = 0; i < numSegments; i++){
            newMesh.indices.push_back(ringStart + i);
            newMesh.indices.push_back(poleStart + i);
            newMesh.indices.push_back(ringStart + i + 1);
        }

        return newMesh;
    }

    ew::MeshData createCylinder(float height, float radius, int numSegments){
        ew::MeshData newMesh;

        float topHeight = height / 2;
        float bottomHeight = -topHeight;

        ew::Vertex topCenter = {ew::Vec3(0, topHeight, 0), ew::Vec3(0, 1, 0), ew::Vec2(0.5, 0.5)};

        newMesh.vertices.push_back(topCenter);

        float thetaStep = ew::PI * 2 / numSegments;
        for (int i = 0; i <= numSegments; i++){
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = radius * cos(theta);
            currVertex.pos.z = radius * sin(theta);
            currVertex.pos.y = topHeight;
            currVertex.normal = ew::Vec3(0, 1, 0);
            currVertex.uv.x = (cos(theta) + 1) * 0.5;
            currVertex.uv.y = (sin(theta) + 1) * 0.5;
            newMesh.vertices.push_back(currVertex);
        }

        for (int i = 0; i <= numSegments; i++){
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = radius * cos(theta);
            currVertex.pos.z = radius * sin(theta);
            currVertex.pos.y = topHeight;
            currVertex.normal = ew::Normalize(ew::Vec3(cos(theta), 0, sin(theta)));
            currVertex.uv.x = (float)i / (numSegments + 1);
            currVertex.uv.y = 1;
            newMesh.vertices.push_back(currVertex);
        }

        for (int i = 0; i <= numSegments; i++){
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = radius * cos(theta);
            currVertex.pos.z = radius * sin(theta);
            currVertex.pos.y = bottomHeight;
            currVertex.normal = ew::Normalize(ew::Vec3(cos(theta), 0, sin(theta)));
            currVertex.uv.x = (float)i / (numSegments + 1);
            currVertex.uv.y = 0;
            newMesh.vertices.push_back(currVertex);
        }

        for (int i = 0; i <= numSegments; i++){
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = radius * cos(theta);
            currVertex.pos.z = radius * sin(theta);
            currVertex.pos.y = bottomHeight;
            currVertex.normal = ew::Vec3(0, -1, 0);
            currVertex.uv.x = (cos(theta) + 1) * 0.5;
            currVertex.uv.y = (sin(theta) + 1) * 0.5;
            newMesh.vertices.push_back(currVertex);
        }

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
            newMesh.indices.push_back(start + i);
            newMesh.indices.push_back(center);
            newMesh.indices.push_back(start + 1 + i);
        }

        return newMesh;
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
}