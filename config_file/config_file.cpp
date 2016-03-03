#include "config_file.h"
#include <fstream>
#include <iostream>

void ConfigFile::setRelativePath( std::string path )
{

}

bool ConfigFile::load( std::string filename )
{
	std::ifstream file( filename );

	// Return false if the file is not valid
	if( !file ) return false;

	while( file )
	{
		// Grab the key, deliminated by a colon ':' char
		std::string line_start;
		std::getline( file, line_start, ':' );

		// Ignore empty lines and lines that start with '#'
		if( line_start.empty() ) continue;
		if( line_start[0] == '#' ) continue;

		// store the rest of the line in line_end
		std::string line_end;
		std::getline( file, line_end );

		// pair line stard and line end together
		// the start is the key, the end is the value
		data_[line_start] = line_end;
	}

	std::cout << "Read from " << filename << "...\n";
	for( auto it = data_.begin(); it != data_.end(); ++it )
	{
		std::cout << it->first << " " << it->second << std::endl;
	}

	return true;
}

bool ConfigFile::getFloat( std::string key, float* value )
{
	auto it = data_.find(key);

	// if the iterator points to the end, the key was not found
	if( it == data_.end() ) return false;

	*value = stof(it->second);

	return true;
}

bool ConfigFile::getInt( std::string key, int* value )
{
	auto it = data_.find(key);

	// If the iterator points to the end, the key was not found
	if( it == data_.end() ) return false;

	*value = stoi(it->second);

	return true;
}