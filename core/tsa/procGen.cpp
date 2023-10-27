//
// Created by tropi on 10/25/2023.
//

#include "procGen.h"

#include <iostream>

namespace tsa{
    ew::MeshData createSphere(float radius, int numSegments){
        ew::MeshData newMesh;

        return newMesh;
    }

    ew::MeshData createCylinder(float height, float radius, int numSegments){
        ew::MeshData newMesh;

        float topHeight = height / 2;
        float bottomHeight = -topHeight;

        ew::Vertex topCenter = {(0, topHeight, 0)};
        newMesh.vertices.push_back(topCenter);

        float thetaStep = ew::PI * 2 / numSegments;
        for (int i = 0; i <= numSegments; i++){
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = cos(theta);
            currVertex.pos.z = sin(theta);
            currVertex.pos.y = topHeight;
            newMesh.vertices.push_back(currVertex);
        }

        for (int i = 0; i <= numSegments; i++){
            float theta = i * thetaStep;
            ew::Vertex currVertex;
            currVertex.pos.x = cos(theta);
            currVertex.pos.z = sin(theta);
            currVertex.pos.y = bottomHeight;
            newMesh.vertices.push_back(currVertex);
        }

        ew::Vertex bottomCenter = {(0, bottomHeight, 0)};
        newMesh.vertices.push_back(bottomCenter);

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