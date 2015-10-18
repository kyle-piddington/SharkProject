#ifndef __ASSIMP_SCENE__
#define __ASSIMP_SCENE__
#include "Model.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Camera.h"
#include "CameraScene.h"
#include "Light.h"
#include "Cube.h"
#include "Material.h"
#include "Plane.h"
#include "KeySpline.h"
class AssimpScene : public CameraScene
{
public:
   AssimpScene(Context * ctx);

   virtual ~AssimpScene();
   virtual void initPrograms();
   
   virtual void initialBind();

   virtual void render();

   virtual void update();

private:
   Light light1, light2;
   Model * model;
   Program * assimpProg;
   Program * debugProg;
   Plane plane;
   Cube cube;
   glm::mat3 createNormalMatrix(const glm::mat4 & view, const glm::mat4 & model);
   Material mat;
   Transform slidingTransform;
   KeySpline keyspline;

};
#endif