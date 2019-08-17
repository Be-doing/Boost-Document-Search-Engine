#pragma once
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<boost/algorithm/string.hpp>
class FileUtil
{
	public:
		static bool Read(const std::string& file_path, std::string& content)
		{
			std::ifstream infile(file_path.c_str());
			if(!infile.is_open())
			{
				std::cout << "FileUtil::Read open error" << std::endl;
				return false;
			}
			std::string line;
			while(std::getline(infile, line))
			{
				content += line + "\n";
			}
			infile.close();
			return true;
		}

		static bool Write(const std::string& file_path, std::string& content)
		{
			std::ofstream ofile(file_path.c_str());
			if(!ofile.is_open())
			{
				 std::cout << "FileUtil::Write open error" << std::endl;
				 return false;
			}
			ofile.write(content.c_str(), content.size());
			ofile.close();
			return true;
		}
};
class StringUtil
{
	public:
		static void Split(const std::string& input, std::vector<std::string>& output,
		const std::string& splist_char)
		{
			boost::split(output, input, boost::is_any_of(splist_char), boost::token_compress_off);
		}
};
