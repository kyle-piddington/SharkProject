#include "SharkSpineOscilator.h"
#include <imgui/imgui.h>
SharkSpineOscilator::SharkSpineOscilator(std::string boneName):
boneName(boneName)
{
   dTau = targDTau = 1.0;
   alpha = targAlpha = 0.0;
   phase = targPhase = 0.0;
   tau = 0.0;
}

void SharkSpineOscilator::SharkSpineOscilator::apply(Model & model)
{
   Bone * const bonePtr = model.getBone(boneName);
   if(bonePtr != nullptr)
   {
      Transform t;
      t.setRotation(sin(tau + phase) * alpha, glm::vec3(1,0,0));
      bonePtr->setAnimatedTransform(bonePtr->getOffsetMatrix() * t.getMatrix());
   }
}
void SharkSpineOscilator::SharkSpineOscilator::update(float dt)
{
   ImGui::Text(boneName.c_str());
   ImGui::InputFloat("alpha", &targAlpha,0.01f,1.0f);
   ImGui::InputFloat("dTau", &targDTau,0.01f,1.0f);
   ImGui::InputFloat("phase", &targPhase,0.01f,1.0f);
   
   dTau += (targDTau - dTau) * 0.01; //Exponential interpolation
   alpha += (targAlpha - alpha) * 0.01;
   phase += (targPhase - phase) * 0.01;
   tau += dTau * dt;
}
void SharkSpineOscilator::setAlpha(float newAlpha)
{
   targAlpha = newAlpha;
}
void SharkSpineOscilator::setDTau(float dTau)
{
   targDTau = dTau;
}