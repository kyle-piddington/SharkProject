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
#include "SharkSpineOscilator.h"
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
   Program * texProg;
   Program * debugProg;

   Plane plane;
   Cube cube;
   Texture2D gridTexture;
   glm::mat3 createNormalMatrix(const glm::mat4 & view, const glm::mat4 & model);
   Material mat;
   Transform slidingTransform;
   KeySpline keyspline;
   std::vector<SharkSpineOscilator> oscilators;

   float u = 0;
   float oldT = 0, newT = 0;

};
#endif