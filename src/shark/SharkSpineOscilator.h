#ifndef __SHARK_SPINE_OSCILATOR_H__
#define __SHARK_SPINE_OSCILATOR_H__
#include "Model.h"
/**
 * Model 
 */
class SharkSpineOscilator
{

public:
   SharkSpineOscilator(std::string spineName);
   void apply(Model & model);
   void update(float dt);
   void setAlpha(float newAlpha);
   void setDTau(float newTau);
private:
   std::string boneName;
   float targDTau; //Ratio of dT to dTau
   float targAlpha;
   float dTau;
   float alpha;
   float tau;
   float phase, targPhase;

};
#endif