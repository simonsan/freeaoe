/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "DrsGraphics.h"

#include "IOHelper.h"

//Debug
#include <iostream>

using std::ios_base;
using std::streampos;

DrsGraphics::DrsGraphics()
{

}

DrsGraphics::DrsGraphics(const DrsGraphics& other)
{

}

DrsGraphics::~DrsGraphics()
{

}

//------------------------------------------------------------------------------
void DrsGraphics::load(std::istream& istr)
{
    DrsFile::load(istr);
    
    readHeader();
}

//------------------------------------------------------------------------------
void DrsGraphics::readHeader()
{
  //dunno 4 bytes
  istr_->seekg(4, ios_base::cur);
  
  long header_len = IOHelper::readLong(*istr_);
  start_of_slp_ = streampos(header_len);
  
  //dunno 8 bytes
  istr_->seekg(8, ios_base::cur);
  
  num_of_slp_ = IOHelper::readLong(*istr_);
}



