#ifndef __KEYSPLINE__H__
#define __KEYSPLINE__H__

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
#include <Eigen/Dense>
#include "SplineNode.h"
#include <vector>
#include "glm/glm.hpp"
#include "SplineType.h"
#include "Transform.h"
#include "VertexArrayObject.h"

using namespace std;

class KeySpline
{

public:

    KeySpline();
    ~KeySpline();
    void setSplineType(SplineType type);
    void addNode(const SplineNode & node);
    void draw();
    void close(); //Close the spline.

    Transform transformAt(float s);
    glm::vec3 normalAt(float s);
private:
    std::vector<Eigen::Vector3f> nodePos;
    std::vector<Eigen::Quaternionf> nodeRot;
    Eigen::Matrix4f B;
    std::vector< pair<float,float> > usTable;
    SplineType type;
    VertexArrayObject vao;
    VertexBuffer vertexBuffer;
    
    bool updateVBO = true;
    int numSplines;

    void recalculateTable();
    float sToU(float s);

};

#endif