#include <iostream>
#include <cstdlib>
#include <math.h>
using namespace std;

struct Vertex {
    int x;
    int y;
};

Vertex getVertex(Vertex origin, float angle, float raio);

int main() {

    Vertex origin;
    origin.x = 0;
    origin.y = 0;

    float numberOfVertex = 16;
    float angleIncrement = 360/numberOfVertex;
    float raio = 5;

    Vertex circleVertexArray [16];

    for(double currentAngle = 0.0; currentAngle < 360.0; currentAngle+=angleIncrement) {
        cout << currentAngle << endl;
    }


    return 0;
}

// WIP
Vertex getVertex(Vertex origin, float angle, float raio) {
    Vertex temp;
    temp.x = 0;
    temp.y = 0;

    return temp;
}