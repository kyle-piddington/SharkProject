#include "AssimpScene.h"
#include "GL_Logger.h"
#include <glm/gtc/type_ptr.hpp>
#include "GlmUtil.h"
#include "KeySpline.h"
#include "SharkSpineOscilator.h"
#include <imgui/imgui.h>

//Max speed is 1 m/sec
float getMaxSpeed()
{
   return 1;
}
float getMinSpeed()
{
   return 0.2;
}

float getFrequency(float length, float speed)
{
   float minHz;
   float maxHz;
   if(length < 60)
   {
      minHz = 1.25;
      maxHz = 2.25;
   }
   else if(length < 90)
   {
      minHz = 1.0;
      maxHz = 1.75;
   }
   else
   {
      minHz = 0.75;
      maxHz = 1.20;
   }
   return minHz + (maxHz - minHz) * (speed - getMinSpeed()) / (getMaxSpeed() - getMinSpeed());

}
float getAmplitude(float length, float speed)
{
   float minHz;
   float maxHz;
   if(length < 60)
   {
      minHz = 1.25;
      maxHz = 2.25;
   }
   else if(length < 90)
   {
      minHz = 1.0;
      maxHz = 1.75;
   }
   else
   {
      minHz = 0.75;
      maxHz = 1.20;
   }
   //Aimplitude of oscilation is based off of full spine, not just one section
   float maxA = length/(5*2);
   return maxA * (getFrequency(length, speed) - minHz)/(maxHz - minHz);

}

AssimpScene::AssimpScene(Context * ctx):
   CameraScene(ctx),
   light1(glm::vec3(0.05),glm::vec3(1.0),glm::vec3(1.0),50),
   light2(glm::vec3(0.05),glm::vec3(1.0),glm::vec3(1.0),50),
    mat(glm::vec3(0.4),glm::vec3(0.8),glm::vec3(0.4),16),
   plane(1,1),
   gridTexture("assets/textures/Grid.png")

   {
      model = new Model("assets/models/sharks/leopardShark2.dae");


      assimpProg = createProgram("Assimp model viewer");
      camera.setPosition(glm::vec3(0,0,2));
      light1.transform.setPosition(glm::vec3(2.3f, -1.6f, -3.0f));
      light2.transform.setPosition(glm::vec3(-1.7f, 0.9f, 1.0f));
      Eigen::Quaternionf q1;
      q1 = Eigen::AngleAxisf(90.0f/180.0f*M_PI, Eigen::Vector3f(0,1,0));
      q1 =  Eigen::AngleAxisf(90.0f/180.0f*M_PI, Eigen::Vector3f(1,0,0)) * q1;

      for(int i = 1; i <= 9; i++)
      {
         SharkSpineOscilator osc("Spine" + std::to_string(i),i);
         osc.setPhase(-M_PI/9*i);
         osc.setAlpha(0.15/9. * (i < 3 ? 1.5 : i)); //Model Subcarangiform motion by osiclating the last 2/3 more than the first
         if(i > 5)
         {
            //osc.setHarmonic(i-5); //Set harmonics for spine
         }
         oscilators.push_back(osc);

      }
      //Fake Spine0 to move head of shark
      SharkSpineOscilator osc0("Spine0",0);
      osc0.setPhase(0);
      osc0.setAlpha(0.07);
      oscilators.push_back(osc0);
      plane.transform.setScale(glm::vec3(30));
      plane.transform.translate(glm::vec3(0,-0.2,0));
        //keyspline.close();
      texProg = createProgram("Texture program");
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

   texProg->addVertexShader("assets/shaders/tex_vert.vs");
   texProg->addFragmentShader("assets/shaders/tex_frag.fs");

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

   texProg->addUniform("M");
   texProg->addUniform("V");
   texProg->addUniform("P");
   texProg->addUniform("T");
   texProg->addUniform("tex");

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

   texProg->enable();
   texProg->getUniform("T").bind(glm::mat3(30.0));
   texProg->getUniform("P").bind(camera.getPerspectiveMatrix());
   texProg->disable();
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

   texProg->enable();
   gridTexture.enable(texProg->getUniform("tex").getID());
   texProg->getUniform("V").bind(V);
   texProg->getUniform("M").bind(plane.transform.getMatrix());
   plane.render();
   texProg->disable();

   debugProg->enable();
   debugProg->getUniform("V").bind(V);
   debugProg->getUniform("M").bind(glm::mat4(1.0));
   debugProg->getUniform("debugColor").bind(glm::vec3(0.8,0.0,0.0));
   keyspline.draw();

   debugProg->disable();

}

void AssimpScene::update()
{
   static float length = 100;
   float newT = glfwGetTime();
   float dt = newT - oldT;
   oldT = newT;
   CameraScene::update();

   ImGui::Begin("Shark");
   ImGui::SliderFloat("length (cm)", &length,  30,  121, "%.3f",  1.0f);

   if(ImGui::Button("Sync"))
   {
      for (std::vector<SharkSpineOscilator>::iterator i = oscilators.begin(); i != oscilators.end(); ++i)
      {
         i->setTau(0);
      }
   }
   ImGui::End();
   //
   model->transform.setScale(glm::vec3(length/100.f/model->getWidth()));
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

   ImGui::Begin("Movement");
   {
      static float targSpeed = 1.0;
      static float speed = 1.0;
      ImGui::InputFloat("Speed", &targSpeed,0.01f,1.0f);
      float maxSpeed = getMaxSpeed();
         //Clamp the target speed value
      targSpeed = min(getMaxSpeed(),targSpeed);
      targSpeed = max(getMinSpeed(),targSpeed);

      float accel = (targSpeed - speed) * dt;

      speed+= accel;
         //Set Frequency and Amplitude from speed
         //Calculate velocity Shark is appx 120 cm
      float freq = getFrequency(length, speed);
      float ampl = getAmplitude(length, speed)/100.f;

      for (std::vector<SharkSpineOscilator>::iterator osc = oscilators.begin(); osc != oscilators.end(); ++osc)
      {
            osc->setHz(freq);//Oscilating frequency function of Shark len and speed
            osc->setAlpha(ampl * (osc->getID() < 4 ? 0.7f :  1)); //Model Subcarangiform motion by osiclating the last 2/3 more than the first
         }
         //Set "Spine0" alpha back to base
         oscilators.back().setAlpha(ampl);
         u += dt * speed;


   }
   ImGui::End();
   if(keyspline.getNumNodes() >= 3)
   {

      float stepU = u;
      stepU = keyspline.sToU(stepU);
      Transform tns = keyspline.transformAt(stepU);
      model->transform.setPosition(tns.getPosition());
      model->transform.setRotation(tns.getRotation());
   }

   for (std::vector<SharkSpineOscilator>::iterator osc = oscilators.begin(); osc != oscilators.end(); ++osc)
   {
      osc->handleGUI();
      osc->update(dt);
      osc->apply(*model);
   }


   if(Mouse::clicked(GLFW_MOUSE_BUTTON_LEFT) && Keyboard::isKeyDown(GLFW_KEY_LEFT_SHIFT))
   {
      //Unproject ray
      glm::vec3 origin = glm::unProject(glm::vec3(Mouse::getX(), getContext()->getWindowHeight()-Mouse::getY(),0.0),
                                        camera.getViewMatrix(),
                                        camera.getPerspectiveMatrix(),
                                        glm::vec4(0,0,getContext()->getWindowWidth(),getContext()->getWindowHeight()));

      glm::vec3 direction = glm::normalize(
                                        glm::unProject(glm::vec3(Mouse::getX(), getContext()->getWindowHeight()-Mouse::getY(),1.0),
                                        camera.getViewMatrix(),
                                        camera.getPerspectiveMatrix(),
                                        glm::vec4(0,0,getContext()->getWindowWidth(),getContext()->getWindowHeight())) - origin);
      //Intersect with XZ plane
      glm::vec3 normal(0,1,0);
      std::cout << "Cast direction:"  << direction.x <<"," << direction.y << "," << direction.z<< std::endl;

      float t = glm::dot(-origin, normal) / glm::dot(direction, normal);
      if(t > 0.001)
      {
         glm::vec3 hit_point = origin + t * direction;
         std::cout << "Hit plane at"  << hit_point.x <<"," << hit_point.y << "," << hit_point.z<< std::endl;
         if(keyspline.getNumNodes() == 0)
         {
            keyspline.addNode(SplineNode(hit_point,glm::quat()));
         }
         keyspline.addNode(SplineNode(hit_point,glm::quat()));

      }



   }
   //model->transform = keyspline.transformAt(u);


}