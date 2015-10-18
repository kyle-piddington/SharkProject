#include "GL_Logger.h"
#include "KeySpline.h"
#include <Eigen/Geometry>
#include "Transform.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>


//Calculate the fernet frame of a point.
glm::vec3 getNormalVector(const Eigen::Matrix4f & B, const Eigen::Vector3f points[],
    float u){
    Eigen::MatrixXf G(3,4);
    G << points[0](0), points[1](0), points[2](0), points[3](0),
         points[0](1), points[1](1), points[2](1), points[3](1),
         points[0](2), points[1](2), points[2](2), points[3](2);

    Eigen::Vector4f dUVec, d2UVec;
    dUVec << 0.0f, 1.0f, 2*u, 3*u*u;
    dUVec= dUVec.transpose();


    Eigen::Vector3f Tvec = (G*B*dUVec).normalized(); //T Vector
    return glm::vec3(Tvec(0),Tvec(1),Tvec(2));
}

KeySpline::KeySpline():
numSplines(0)
{
    setSplineType(BSPLINE);
    vao.addAttribute(0,vertexBuffer);
};
KeySpline::~KeySpline(){};

void KeySpline::setSplineType(SplineType t){
    switch (t){
        case CATMULL:
            this->B << 0.0f, -1.0f,  2.0f, -1.0f,
                       2.0f,  0.0f, -5.0f,  3.0f,
                       0.0f,  1.0f,  4.0f, -3.0f,
                       0.0f,  0.0f, -1.0f,  1.0f;
            this->B*=0.5;
            break;
        case BSPLINE:
            this->B  << 1.0f, -3.0f,  3.0f, -1.0f,
                        4.0f,  0.0f, -6.0f,  3.0f,
                        1.0f,  3.0f,  3.0f, -3.0f,
                        0.0f,  0.0f,  0.0f,  1.0f;
            this->B /= 6.0f;
            break;
    }
    if(this->type != t){
        recalculateTable();
        this->type = t;
    }
}

void KeySpline::addNode(const SplineNode & node){
    nodePos.push_back(node.getPosition());
    nodeRot.push_back(node.getRotation());
    if(nodePos.size() >= 4){
        numSplines++;
    }
    if(numSplines > 0){
        recalculateTable();
    }
    updateVBO = true;
}

//Replace with Geometry shader later
void KeySpline::draw(){
    if(updateVBO && numSplines > 0){

        Eigen::MatrixXf G(3,nodePos.size());
        Eigen::MatrixXf Gk(3,4);
        for(int i = 0; i < nodePos.size(); i++){
            G.block<3,1>(0,i) = nodePos[i];
        }
        std::vector<glm::vec3>  positions;
        positions.clear();
        for(int k = 0; k < numSplines; k++){
            Gk = G.block<3,4>(0,k);
            int n = 32;
            for(int i = 0; i < n; i++){
                float u = i/(n-1.0f);
                Eigen::Vector4f uVec(1.0,u,u*u,u*u*u);
                Eigen::Vector3f P=Gk*B*uVec;
                positions.push_back(glm::vec3(P(0),P(1),P(2)));
            }
        }
        vertexBuffer.setData(positions);
        updateVBO = false;
    }
    if (numSplines > 0)
    {
        vao.bind();
        GL_Logger::LogError("Could not bind spline vao");

        glDrawArrays(GL_LINE_STRIP,0,vertexBuffer.getNumVerts());
        GL_Logger::LogError("Could not draw spline");
        vao.unbind();

    }
    //Drawing code here
}
void KeySpline::close(){
    nodePos.push_back(nodePos[0]);
    nodeRot.push_back(nodeRot[0]);
    //n-2,n-1,n,0
    nodePos.push_back(nodePos[1]);
    nodeRot.push_back(nodeRot[1]);
    //n-1,n,0,1
    nodePos.push_back(nodePos[2]);
    nodeRot.push_back(nodeRot[2]);
    //n,0,1,2
    numSplines+=3;
}

Transform KeySpline::transformAt(float s){
    assert(numSplines > 0);
    //Convert S to U through table lookup
    float kfloat;
    float uu = s;
    float u = std::modf(uu, &kfloat);
    int k = (int)std::floor(kfloat) % numSplines;
    Eigen::MatrixXf GPos(3,4);
    Eigen::MatrixXf GRot(4,4);

    for(int i = 0; i < 4; i++){
        GPos.block<3,1>(0,i) = nodePos[k+i];
    }
    Eigen::Vector4f uVec(1.0,u,u*u,u*u*u);
    Eigen::Vector3f P= GPos * B * uVec;
    Transform t;
    t.setPosition(glm::vec3(P(0),P(1),P(2)));
    t.lookAlong(getNormalVector(B,&(nodePos[k]),u));
    return t;
}




void KeySpline::recalculateTable(){

}








