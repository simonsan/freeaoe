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


#include "DrsFile.h"

#include <string>

#include <resource/ResourceManager.h>

using std::string;
using sf::Uint32;

Logger& DrsFile::log = Logger::getLogger("freeaoe.DrsFile");

//------------------------------------------------------------------------------
DrsFile::DrsFile(string file_name, ResourceManager *resource_manager)
                 : FileIO(file_name), resource_manager_(resource_manager)
{
  header_loaded_ = false;
}


//------------------------------------------------------------------------------
DrsFile::~DrsFile()
{

}


//------------------------------------------------------------------------------
void DrsFile::loadHeader()
{
  if (header_loaded_)
    log.warn("Trying to load header again!"); 
  else
  {
    string copy_right = readString(40);
    
    string version = readString(4);
    
    //File type
    string file_type = readString(12);
    
    num_of_tables_ = read<Uint32>();
    header_offset_ = read<Uint32>(); 
    
    // Load table data
    for (Uint32 i = 0; i < num_of_tables_; i++)
    {
      table_types_.push_back(readString(8));
      table_num_of_files_.push_back(read<Uint32>());
    }
   
    // Load file headers
    for (Uint32 i = 0; i < num_of_tables_; i++)
    {
      for (Uint32 j = 0; j < table_num_of_files_[i]; j++)
      {
        sf::Uint32 id = read<Uint32>();
        sf::Uint32 pos = read<Uint32>();
        sf::Uint32 len = read<Uint32>();
                
        if (table_types_[i].find(" plsL") == 0)
        {
          SlpFile *slp = new SlpFile(id, pos, len, getIOStream());
          resource_manager_->addSlpFile(slp);
          
        }
        // else other 
        
      }
    }
      
    header_loaded_ = true;
  }
}


