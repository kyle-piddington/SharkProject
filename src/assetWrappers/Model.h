#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Transform.h"
#include "Skeleton.h"
#include "SkeletonRenderer.h"
#include "SkeletalAnimation.h"
/**
 * An Assimp renderable model
 */
class Model
{
public:
   Model(std::string path);
   void render(Program & prog);
   void renderSkeleton();
   void animate(std::string animName, float time);
   Bone * const getBone(std::string boneName);
   float getWidth(){return maxBounds.x - minBounds.x;}
   float getHeight(){return maxBounds.y - minBounds.y;}
   float getLength(){return maxBounds.z - minBounds.z;};
   Transform transform;

private:
   std::vector< std::shared_ptr<Mesh> > meshes;
   std::string directory;
   glm::vec3 minBounds;
   glm::vec3 maxBounds;
   void loadModel(std::string path);
   void loadBones(aiNode * node);
   void processNode(aiNode * node, const aiScene * scene);
   std::vector<VertexBoneData> processMeshBoneData(aiMesh * mesh);
   std::shared_ptr<Mesh> processMesh(aiMesh * mesh, const aiScene * scene);
   std::vector<std::shared_ptr<Texture2D> > loadMaterialTextures(
      aiMaterial * mat, aiTextureType type,
      TextureType::type texType);

   Skeleton skeleton;
   SkeletonRenderer skelRenderer;
   //Animate the model with a loaded animation.


//Load this somewhere else so it's not duplicated per model
   std::unordered_map<std::string, SkeletalAnimation> animations;
   
};

#endif