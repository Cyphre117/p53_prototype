#include <map>
#include <string>

class ConfigFile {
	public:
		ConfigFile() {}
		~ConfigFile() {}

		// Sets the path to configuration files
		// relative to the executable
		void setRelativePath( std::string path );

		// Loads the specified configuration file
		bool load( std::string filename );

		bool getFloat( std::string key, float* value );

		bool getInt( std::string key, int* value );

	private:

	// The base path to search for config files
	std::string basePath_;

	std::map<std::string, std::string> data_;
};