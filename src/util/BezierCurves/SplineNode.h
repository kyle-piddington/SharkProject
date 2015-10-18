#ifndef __SPLINENODE_H__
#define __SPLINENODE_H__

#define EIGEN_DONT_VECTORIZE
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT

#include <Eigen/Dense>
class SplineNode
{
public:
    SplineNode(Eigen::Vector3f position, Eigen::Quaternionf rotation):
    position(position),
    rotation(rotation){};
    ~SplineNode(){};

    const Eigen::Vector3f & getPosition() const{return position;}
    const Eigen::Quaternionf & getRotation() const{return rotation;}

private:
    Eigen::Vector3f position;
    Eigen::Quaternionf rotation;
};
#endif