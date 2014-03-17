/*============================================================================
  CMake - Cross Platform Makefile Generator
  Copyright 2000-2009 Kitware, Inc., Insight Software Consortium

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file Copyright.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#include "cmGetTargetPropertyCommand.h"

// cmSetTargetPropertyCommand
bool cmGetTargetPropertyCommand
::InitialPass(std::vector<std::string> const& args, cmExecutionStatus &)
{
  if(args.size() != 3 )
    {
    this->SetError("called with incorrect number of arguments");
    return false;
    }
  std::string var = args[0];
  const std::string& targetName = args[1];
  std::string prop;

  if(args[2] == "ALIASED_TARGET")
    {
    if(this->Makefile->IsAlias(targetName))
      {
      if(cmTarget* target =
                          this->Makefile->FindTargetToUse(targetName))
        {
        prop = target->GetName();
        }
      }
    }
  else if(cmTarget* tgt = this->Makefile->FindTargetToUse(targetName))
    {
    cmTarget& target = *tgt;
    const char* prop_cstr = target.GetProperty(args[2]);
    if(prop_cstr)
      {
      prop = prop_cstr;
      }
    }
  else
    {
    bool issueMessage = false;
    cmOStringStream e;
    cmake::MessageType messageType = cmake::AUTHOR_WARNING;
    switch(this->Makefile->GetPolicyStatus(cmPolicies::CMP0045))
      {
      case cmPolicies::WARN:
        issueMessage = true;
        e << this->Makefile->GetPolicies()
                          ->GetPolicyWarning(cmPolicies::CMP0045) << "\n";
      case cmPolicies::OLD:
        break;
      case cmPolicies::REQUIRED_IF_USED:
      case cmPolicies::REQUIRED_ALWAYS:
      case cmPolicies::NEW:
        issueMessage = true;
        messageType = cmake::FATAL_ERROR;
      }
    if (issueMessage)
      {
      e << "get_target_property() called with non-existent target \""
        << targetName <<  "\".";
      this->Makefile->IssueMessage(messageType, e.str());
      if (messageType == cmake::FATAL_ERROR)
        {
        return false;
        }
      }
    }
  if (!prop.empty())
    {
    this->Makefile->AddDefinition(var, prop.c_str());
    return true;
    }
  this->Makefile->AddDefinition(var, (var+"-NOTFOUND").c_str());
  return true;
}

