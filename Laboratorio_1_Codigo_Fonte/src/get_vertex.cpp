#include <cstdlib>
#include <math.h>
#include <vector>
#include "get_vertex.h"

#define PI 3.14159265

std::vector<Vertex> getCircleVertexArray(int numberOfVertex, int raio);
Vertex getVertex(Vertex origin, float angle, float raio);

std::vector<Vertex> getCircleVertexArray(int numberOfVertex, int raio) {

    Vertex origin;
    origin.x = 0;
    origin.y = 0;

    float numberOfVertex = 16;
    float angleIncrement = 360/numberOfVertex;
    float raio = 5;

    std::vector<Vertex> circleVertexArray;

    for(double currentAngle = 0.0; currentAngle < 360.0; currentAngle+=angleIncrement) {
        circleVertexArray.push_back(getVertex(origin, currentAngle, raio));
    }
    
    return  circleVertexArray
}

// WIP
Vertex getVertex(Vertex origin, float angle, float raio) {
    Vertex temp;

    temp.x = cos(angle * PI / 180) * 10;
    temp.y = sin(angle * PI / 180) * 10;

    return temp;
}