#include "Program.h"
#include "../logger/GL_Logger.h"
#include "../io/TextLoader.h"
#include <easyLogging++.h>
#include "ShaderManager.h"
Program::Program(std::string name):
   name(name),
   vertShader(nullptr),
   fragShader(nullptr),
   geomShader(nullptr),
   tessalationShader(nullptr),
   created(false)
{
   //No initialization yet for program
}

int Program::addVertexShader(std::string shaderSrc)
{
   return addShader(vertShader, shaderSrc, GL_VERTEX_SHADER);
}
int Program::addFragmentShader(std::string shaderSrc)
{
   return addShader(fragShader, shaderSrc, GL_FRAGMENT_SHADER);
}

int Program::addShader(std::shared_ptr<Shader> & shader, std::string shaderName, GLenum shaderType)
{
   shader = ShaderManager::getShader(shaderName,shaderType);
   if(shader->compile() == 0)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}

int Program::create()
{
   int err = 0;
   if(created)
   {
      LOG(WARNING) << "Program has already been created!";
      return 0;
   }
   if(vertShader!= nullptr && vertShader->isCompiled() && fragShader!=nullptr && fragShader->isCompiled())
   {
      shaderProgram = glCreateProgram();
      err |= GL_Logger::LogError("Could not create program" + name, glGetError());
      glAttachShader(shaderProgram,vertShader->getID());
      err |= GL_Logger::LogError("Could not attatch vertex shader to program" + name, glGetError());
      glAttachShader(shaderProgram,fragShader->getID());
      err |= GL_Logger::LogError("Could not attatch fragment shader to program" + name, glGetError());
      if(geomShader!= nullptr && geomShader->isCompiled())
      {
         glAttachShader(shaderProgram,geomShader->getID());
         err |= GL_Logger::LogError("Could not attatch geometry shader to program" + name, glGetError());

      }
      if(tessalationShader != nullptr && tessalationShader->isCompiled())
      {
         glAttachShader(shaderProgram, tessalationShader->getID());
         err |= GL_Logger::LogError("Could not attatch fragment shader to program" + name, glGetError());

      }
      //If nothing has caused an error yet, link the program.
      if(!err)
      {
         glLinkProgram(shaderProgram);
         err |= GL_Logger::CheckProgram("Linking program " +name, shaderProgram);
         if(err == 0)
         {
            created = true;
            return 0;
         }
         else
         {
            return -1;
         }

      }
      else
      {
         return -1;
      }
   }
   else
   {
      LOG(ERROR) << "Program "+name+" does not have a valid vertex and fragment stage.";
      return -1;
   }
}

int Program::addAttribute(std::string attribName)
{
   if(!created)
   {
      LOG(ERROR) << "Program " + name + " has not been created. Call .create()";
      return -1;
   }
   GLint atrbId = glGetAttribLocation(shaderProgram, attribName.c_str());
   GL_Logger::LogError("Could not search program " + name, glGetError());
   if(atrbId == -1)
   {
      LOG(WARNING) << "Program " + name +  " Could not bind attribute " + attribName + " (It may not exist, or has been optimized away)";
      return -1;
   }
   else
   {
      attributes[attribName] = atrbId;
   }
   return 0;
}

int Program::addUniform(std::string uniformName)
{
   if(!created)
   {
      LOG(ERROR) << "Program " + name + " has not been created. Call .create()";
      return -1;
   }
   GLint unifId = glGetUniformLocation(shaderProgram, uniformName.c_str());
   GL_Logger::LogError("Could not search program " + name, glGetError());
   if(unifId == -1)
   {
      LOG(WARNING) << "Program " + name +  " Could not bind uniform " + uniformName + " (It may not exist, or has been optimized away)";
      return -1;
   }
   else
   {
      uniforms[uniformName] = unifId;
   }
   return 0;
}

GLint Program::getAttribute(std::string attribName)
{
   if(!created)
   {
      LOG(ERROR) << "Program " + name + " has not been created. Call .create()";
      return -1;
   }
   auto attributeId = attributes.find(attribName);
   if(attributeId == attributes.end())
   {
      LOG(WARNING) << "Program " + name + " has no attribute named " + attribName + " (Did you forget to add it to the program?)";
      return -1;
   }
   else
   {
      boundAttributes[attribName] = true;
      return attributeId->second;
   }

}

GLint Program::getUniform(std::string unifName)
{
   if(!created)
   {
      LOG(ERROR) << "Program" + name + " has not been created. Call .create()";
      return -1;
   }
   auto unifId = uniforms.find(unifName);
   if(unifId == uniforms.end())
   {
      LOG(WARNING) << "Program " + name + " has no attribute named " + unifName + " (Did you forget to add it to the program?)";
      return -1;
   }
   else
   {
      boundUniforms[unifName] = true;
      return unifId->second;
   }
}

GLint Program::hasUniform(std::string unifName)
{
   if(!created)
   {
      LOG(ERROR) << "Program " + name + " has not been created. Call .create()";
      return -1;
   }
   GLint unifId = glGetUniformLocation(shaderProgram, unifName.c_str());
   GL_Logger::LogError("Could not search program " + name, glGetError());
   if(unifId == -1)
   {
      LOG(WARNING) << "Program " + name +  " Could not find uniform " + unifName + " (It may not exist, or has been optimized away)";
   }
   return unifId;


}
bool Program::checkBoundVariables()
{
   if(!created)
   {
      LOG(ERROR) << "Program" + name + " has not been created. Call .create()";
      return -1;
   }
   bool allBound = true;
   for (auto i = attributes.begin(); i != attributes.end(); ++i)
   {
      allBound &= (boundAttributes.find((*i).first) != boundAttributes.end());
   }
   for (auto i = uniforms.begin(); i != uniforms.end(); ++i)
   {
      allBound &= (boundUniforms.find((*i).first) != boundUniforms.end());
   }
   return allBound;
}
int Program::addUniformStruct(std::string name, GL_Structure &  glStruct)
{
   std::vector<std::string> uniformNames = glStruct.getUniformNames();
   int canAddUniform = 0;
   for (std::vector<std::string>::iterator i = uniformNames.begin(); i != uniformNames.end(); ++i)
   {
      canAddUniform |= addUniform(name + "." +*i);
   }
   //If all uniforms were successfully added
   if(canAddUniform == 0)
   {
      for (std::vector<std::string>::iterator i = uniformNames.begin(); i != uniformNames.end(); ++i)
      {
         glStruct.setUniformLocation(*i,getUniform(name + "." +*i));
      }
      return 0;
   }
   else
   {
      LOG(ERROR) << "Struct " + name + " Could not find all attributes!";
      return -1;
   }
}

int Program::addUniformArray(std::string name, int len)
{
   bool canAddArray = true;
   std::vector<GLint> locs;
   for(int i = 0; i < len; i++)
   {
      GLint loc = hasUniform(name + "[" + std::to_string(i) + "]");
      canAddArray &= (loc != -1);
      locs.push_back(loc);
   }

   if(!canAddArray || len <= 0)
   {
      LOG(ERROR) << "Could not add array " + name + ", name wrong or length wrong";
      return -1;
   }
   else
   {
      UniformArrayInfo info;
      info.baseName = name;
      info.locations = locs;
      arrays[name] = info;
      return 0;
   }

}

int Program::addStructArray(std::string name, int len, GL_Structure & template_struct)
{
   if(len > 0)
   {
      bool canAddStruct = true;
      std::vector<std::string> uniformNames = template_struct.getUniformNames();
      int canAddUniform = 0;
      UniformStructArrayInfo info;
      info.baseName = name;
      for(int i = 0; i < len; i++)
      {
         GL_Structure glStruct(template_struct);
         for (std::vector<std::string>::iterator atrb = uniformNames.begin(); atrb != uniformNames.end(); ++atrb)
         {
            GLint unifLoc = hasUniform(name + "[" + std::to_string(i) + "]" + "." +*atrb);
            canAddStruct &= (unifLoc != -1);
            canAddStruct &= (glStruct.setUniformLocation(*atrb,unifLoc) == 0);
         }
         info.structs.push_back(glStruct);
      }
      if(!canAddStruct)
      {
         LOG(ERROR) << "Could not bind struct array";
         return -1;
      }
      else
      {
         structArrays[name] = info;
         return 0;
      }
   }
   else
   {
      return -1;
   }
}

const Program::UniformArrayInfo & Program::getArray(std::string name)
{
  return arrays[name];
}

const Program::UniformStructArrayInfo & Program::getStructArray(std::string name)
{
  return structArrays[name];
}

void Program::enable()
{
   glUseProgram(shaderProgram);
   GL_Logger::LogError("Could not enable program " + name, glGetError());
}
void Program::disable()
{
   glUseProgram(0);
}

