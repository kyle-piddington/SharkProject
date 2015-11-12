#include "ErrorScene.h"
#include "Window.h"

#include <easyLogging++.h>
#include "ReloadLocator.h"
#include "GLFWHandler.h"
#include <imgui/imgui.h>
#include "imgui_impl_glfw_gl3.h"

Window::Window(GLFWwindow * window):
   currentWindow(window)
{
   errorScene = new ErrorScene();
   errorScene->initPrograms();
   errorScene->compilePrograms();
   errorScene->initialBind();
}

Window::~Window()
{
   delete errorScene;
}
void Window::loadScene(Scene * scene)
{
   currentScene = scene;
   //Copy context pointer
   errorScene->setContext(currentScene->getContext());
}
void Window::run()
{
   if(currentScene != nullptr)
   {

      currentScene->initPrograms();
      currentScene->compilePrograms();
      if(currentScene->canRenderScene())
      {
         currentScene->initialBind();
         //currentScene->initGlobalUniforms();
      }
      //While not esc pressed
      while(!glfwWindowShouldClose(currentWindow))
      {
         ImGui_ImplGlfwGL3_NewFrame();
         FileSystem::ReloadLocator::getService()->processEvents();

         //Poll and update any callbacks
         glfwPollEvents();
         //Update the input handler
         GLFWHandler::update();

         if(currentScene->shouldReloadScene())
         {
            if(currentScene->compilePrograms())
            {
               currentScene->initialBind();
               //currentScene->initGlobalUniforms();
            }
         }
         if(currentScene->canRenderScene())
         {
            //currentScene->updateGlobalUniforms();
            currentScene->update();
            currentScene->render();
         }
         else
         {
            errorScene->render();
         }

  
         ImGui::Render();
     
         glfwSwapBuffers(currentWindow);
         
      }
   }
   else
   {
      LOG(ERROR) << "No scene loaded in window, exiting...";
   }


}