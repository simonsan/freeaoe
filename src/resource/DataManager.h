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


#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <global/Logger.h>
#include <geniedat/File.h>
#include <geniedat/Unit.h>

#include <map>
#include <SFML/Config.hpp>
#include <global/NonCopyable.h>

using sf::Uint32;

class GenieTerrain;

//------------------------------------------------------------------------------
/// Class for receiving and managing data files (.dat)
//
class DataManager : public NonCopyable
{

public:
  
  static DataManager & Inst();
  
  gdat::Graphic getGraphic(sf::Uint32 id);
  gdat::Unit getUnit(sf::Uint32 id);
  
private:
  DataManager();
  virtual ~DataManager();
  
  void initialize();
  
  static Logger &log;
  
  gdat::File dat_file_;
  
};

#endif // DATAMANAGER_H