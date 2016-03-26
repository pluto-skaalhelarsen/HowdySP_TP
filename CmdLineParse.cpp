/*
  CmdLineParse, retrieve definitions from the command line in a form convenient for a program
  Copyright (C) 2016 David Merrell

  BufferSummary is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CmdLineParse is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include "CmdLineParse.hpp"

  CmdLineParse::CmdLineParse(int argc, const char * argv[])
  {
    int i;
    string CurArg;
    string VarName;
    string VarVal;
    int SepLoc;
    InitTables();

    for(i=0; i < argc; i++)
    {
      CurArg = argv[i];
      if(CurArg.substr(0, 2) == "--")
      {
        if(CmdFlags.find(CurArg) != CmdFlags.end())
          CmdFlags[CurArg] = 1;
        if(((SepLoc=CurArg.find("=")) != string::npos) && (SepLoc > 2))
        {
          VarName = CurArg.substr(2, SepLoc-2);
          VarVal = CurArg.substr(SepLoc+1, CurArg.size());

          if(CmdVars.find(VarName) != CmdVars.end())
            CmdVars[VarName] = atoi(VarVal.c_str());
        }
      }
    }
  }

  void CmdLineParse::InitTables(void)
  {
    CmdFlags["--alternate_reverse"] = 0;
    CmdFlags["--dump_final"] = 0;
    CmdFlags["--sp_summary"] = 0;
    CmdFlags["--tp_summary"] = 0;
    CmdVars["SpVerbosity"] = 0;
    CmdVars["TpVerbosity"] = 0;
    CmdVars["Epochs"] = 0;
    CmdVars["RngSeed"] = 0;
  }

  int CmdLineParse::Flags(string QString)
  {
    map<string, int>::iterator QVal;
 
    if((QVal = CmdFlags.find(QString)) == CmdFlags.end())
     return(0);

    return(QVal->second);
  }

  int CmdLineParse::Vars(string QString)
  {
    map<string, int>::iterator QVal;
 
    if((QVal = CmdVars.find(QString)) == CmdVars.end())
     return(0);

    return(QVal->second);
  }


