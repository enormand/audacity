/**********************************************************************

   Audacity - A Digital Audio Editor
   Copyright 1999-2009 Audacity Team
   License: wxwidgets

   Dan Horgan

******************************************************************//**

\file HelpCommand
\brief Declarations of HelpCommand and HelpCommandType classes

\class HelpCommand
\brief Command which returns information about the given command

*//*******************************************************************/

#ifndef __HELPCOMMAND__
#define __HELPCOMMAND__

#include "CommandType.h"
#include "Command.h"

class HelpCommandType final : public CommandType
{
public:
   wxString BuildName() override;
   void BuildSignature(CommandSignature &signature) override;
   Command *Create(CommandOutputTarget *target) override;
};

class HelpCommand final : public CommandImplementation
{
public:
   HelpCommand(HelpCommandType &type, CommandOutputTarget *target)
      : CommandImplementation(type, target) { }
   bool Apply(CommandExecutionContext context) override;
};

#endif /* End of include guard: __HELPCOMMAND__ */
