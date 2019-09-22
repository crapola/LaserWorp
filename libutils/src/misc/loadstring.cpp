#include <fstream>
#include <stdexcept>
std::string LoadString(const char* p_path)
{
	std::ifstream file(p_path,std::ios::in|std::ios::binary);
	if (file)
	{
		std::string contents;
		file.seekg(0,std::ios::end);
		contents.resize(file.tellg());
		file.seekg(0,std::ios::beg);
		file.read(&contents[0],contents.size());
		file.close();
		return contents;
	}
	throw std::runtime_error(std::string("File not found: ")+p_path);
}