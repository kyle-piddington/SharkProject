#include "AssimpScene.h"
#include "GL_Logger.h"
#include <glm/gtc/type_ptr.hpp>
#include "GlmUtil.h"
#include "KeySpline.h"
#include "SharkSpineOscilator.h"
AssimpScene::AssimpScene(Context * ctx):
   CameraScene(ctx),
   light1(glm::vec3(0.05),glm::vec3(1.0),glm::vec3(1.0),50),
   light2(glm::vec3(0.05),glm::vec3(1.0),glm::vec3(1.0),50),
    mat(glm::vec3(0.4),glm::vec3(0.8),glm::vec3(0.4),16),
   plane(3,3)

   {
      model = new Model("assets/models/sharks/leopardShark.dae");
      assimpProg = createProgram("Assimp model viewer");
      camera.setPosition(glm::vec3(0,0,2));
      light1.transform.setPosition(glm::vec3(2.3f, -1.6f, -3.0f));
      light2.transform.setPosition(glm::vec3(-1.7f, 0.9f, 1.0f));
      Eigen::Quaternionf q1;
      q1 = Eigen::AngleAxisf(90.0f/180.0f*M_PI, Eigen::Vector3f(0,1,0));
      q1 =  Eigen::AngleAxisf(90.0f/180.0f*M_PI, Eigen::Vector3f(1,0,0)) * q1;
      SplineNode nodes[] = {
         SplineNode(Eigen::Vector3f(0,0,0),Eigen::Quaternionf::Identity()),
         SplineNode(Eigen::Vector3f(0,0,0),Eigen::Quaternionf::Identity()),
         SplineNode(Eigen::Vector3f(3,3,0),Eigen::Quaternionf::Identity()),
         
         SplineNode(Eigen::Vector3f(3.25,3,0),Eigen::Quaternionf::Identity()),
         SplineNode(Eigen::Vector3f(7,0,0),Eigen::Quaternionf::Identity()),
         SplineNode(Eigen::Vector3f(7,0,0),Eigen::Quaternionf::Identity()),
      };
      for(int i = 0; i < 6; i++)
      {
         keyspline.addNode(nodes[i]);
      }

      for(int i = 1; i <= 8; i++)
      {
         SharkSpineOscilator osc("Spine" + std::to_string(i));
         oscilators.push_back(osc);
      }

        //keyspline.close();

      debugProg = createProgram("Debug spline view program");
   }

AssimpScene::~AssimpScene()
{
   delete model;
}
void AssimpScene::initPrograms()
{
   assimpProg->addVertexShader("assets/shaders/skeleton/skel_vert.vs");
   assimpProg->addFragmentShader("assets/shaders/assimp_frag.fs");

   debugProg->addVertexShader("assets/shaders/debug_vert.vs");
   debugProg->addFragmentShader("assets/shaders/debug_frag.fs");
}

void AssimpScene::initialBind()
{
   assimpProg->addUniform("M");
   assimpProg->addUniform("V");
   assimpProg->addUniform("P");
   assimpProg->addUniformArray("gBones",100);
   assimpProg->addUniformArray("gBinds",100);
 
   debugProg->addUniform("M");
   debugProg->addUniform("V");
   debugProg->addUniform("P");
   debugProg->addUniform("debugColor");

   assimpProg->addUniformArray("diffuseTextures",3);

   assimpProg->addStructArray("pointLights",2,Light::getStruct());
   assimpProg->addUniform("numDiffuseTextures");
   assimpProg->addUniformStruct("material", Material::getStruct());
   assimpProg->addUniformArray("specularTextures",2);
   assimpProg->addUniform("numSpecularTextures");

   assimpProg->addUniformStruct("material",Material::getStruct());

   glClearColor(1.0,1.0,1.0,1.0);

   assimpProg->enable();
   light1.bind(assimpProg->getStructArray("pointLights")[0]);
   light2.bind(assimpProg->getStructArray("pointLights")[1]);
    GL_Logger::LogError("Any errors after lighting..", glGetError());
   assimpProg->disable();

   debugProg->enable();
   debugProg->getUniform("debugColor").bind(glm::vec3(1.0,0.0,0.0));
   debugProg->getUniform("P").bind(camera.getPerspectiveMatrix());
   debugProg->disable();
   //glEnable(GL_CULL_FACE);
   //glCullFace(GL_BACK);



}

void AssimpScene::render()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glm::mat4 M, V, P, MV;
   glm::mat3 NORM;
   //model->transform.setPosition(glm::vec3(0.0f, -1.75f, 0.0f));
   //model->transform.setScale(glm::vec3(0.2));
   if(Keyboard::isKeyToggled(GLFW_KEY_O))
   {
      P = camera.getOrthographicMatrix();
   }
   else
   {
      P = camera.getPerspectiveMatrix();
   }
   V = camera.getViewMatrix();
   M = model->transform.getMatrix();
   NORM = GlmUtil::createNormalMatrix(V, M);
   MV = V*M;
   GL_Logger::LogError("Any errors before enabling..", glGetError());
   assimpProg->enable();
   assimpProg->getUniform("P").bind(P);
   assimpProg->getUniform("M").bind(M);
   assimpProg->getUniform("V").bind(V);
   mat.bind(assimpProg->getUniformStruct("material"));
   model->render(*assimpProg);
   assimpProg->disable();

   debugProg->enable();
   debugProg->getUniform("V").bind(V);
   debugProg->getUniform("M").bind(glm::mat4(1.0));
   debugProg->getUniform("debugColor").bind(glm::vec3(0.8,0.0,0.0));
   keyspline.draw();
   debugProg->getUniform("debugColor").bind(glm::vec3(0.8,0.8,0.8));
   debugProg->getUniform("M").bind(plane.transform.getMatrix());
   plane.render();
   debugProg->disable();

}

void AssimpScene::update()
{
   CameraScene::update();
   for (std::vector<SharkSpineOscilator>::iterator osc = oscilators.begin(); osc != oscilators.end(); ++osc)
   {
      osc->update(1/30.0f);
      osc->apply(*model);
   }
   model->animate("Dothething",0.0);
   if(Keyboard::key(GLFW_KEY_L))
   {
      if(Keyboard::isKeyToggled(GLFW_KEY_L))
      {
         glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      }
      else
      {
         glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      }
   }
   float u = glfwGetTime();
   if(Keyboard::isKeyToggled(GLFW_KEY_I))
   {
      u = keyspline.sToU(u);
   }
   //model->transform = keyspline.transformAt(u);

  
}