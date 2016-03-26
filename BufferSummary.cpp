/*
  BufferSummary.cpp, sumarize sparse data as a sequence of location : run length pairs
  Copyright (C) 2016 David Merrell

  BufferSummary is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  BufferSummary is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <sstream>

#include "BufferSummary.hpp"

  BufferSummary::BufferSummary(vector<unsigned int > &InBuf)
  {
    Loc = Len = 0;
    Start = EndRun = InBuf.begin();
    End = InBuf.end();
  }

  bool BufferSummary::PrepareNextRunMsg(void)
  {
    int i;
    ostringstream MsgStream;

    for(StartRun=EndRun; (StartRun != End) && (*StartRun == 0); StartRun++);
    if(StartRun == End)
      return(false);

    for(EndRun = StartRun; (EndRun != End) && (*EndRun != 0); EndRun++);
    Loc = StartRun - Start;
    Len = EndRun - StartRun;
    MsgStream << "  " << Loc << " : " << Len;
    Msg = MsgStream.str();
    return(true);
  }

