/*░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
▩ File Name: parser.cpp
▩ Author: top-down
▩ Description:	数据处理模块
			把boost文档中设计到的html文档处理
				1、去标签
				2、把文件合并
					把文档中设计的N个html的内容合并成一个行文本文件，把换行符换成其他符号
					(生成的结果是一个大文件，里面包含很多行，每一行对应boost文档的中的一个html，方便索引模块处理)
				3、对文档的结构进行分析，提取文档的标题，正文，目标url
					一个搜索结果应该包括
						3.1 标题	 ----标题
						3.2 摘要/描述----正文
						3.3 栈是url	 ----url
						3.4 跳转url（本项目中，展示url和跳转url设定为同一个
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░*/

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<boost/filesystem.hpp>
#include "../public_dir/util.hpp"
const std::string INPUT_PATH = "../data_dir/input/";//文档的输入路径
const std::string OUTPUT_PUTH = "../data_dir/tmp/raw_input";//处理完后文档的输出目录

namespace bf = boost::filesystem;//定义别名
//doc文档，带搜索的html
struct DocInfo
{
	std::string title;//标题
	std::string content;//正文
	std::string url;//URL
};
bool ParseTitle(const std::string& html, std::string& title)
{
	size_t beg = html.find("<title>");
	if(beg == std::string::npos)
	{
		std::cout << "title not find" << std::endl;
		return false;
	}
	size_t end = html.find("</title>");
	if(end == std::string::npos)
	{
		std::cout << "</title> not find" << std::endl;
		return false;
	}
	beg += std::string("<title>").size();
	if(beg > end)
	{
		std::cout << " beg end error" << std::endl;
		return false;
	}
	title = html.substr(beg, end - beg);
	return true;
}
bool ParseContent(const std::string& html, std::string& content)
{
	bool is_content = true;
	for(auto c : html)
	{
		if(is_content)
		{
			//当前为正文状态
			if(c == '<')
			{//进入标签
				is_content = false;
			}
			else
			{//当前字符是普通的正文字符，加入到结果当中
				if(c == '\n')//换行替换为空格，方便保存
				{
					c = ' ';
				}
				content.push_back(c);
			}
		}
		else
		{//当前是标签状态
			if(c == '>')
			{
				is_content = true;
			}
		}
	}
	return true;
}
bool ParseUrl(const std::string& file_path, std::string& url)
{
	std::string prefix = "https://www.boost.org/doc/libs/1_70_0/doc/";
	std::string tail = file_path.substr(INPUT_PATH.size());
	url = prefix + tail;
	return true;
}
bool ParseFile(const std::string& file_path, DocInfo& info)
{
	std::string html;
	bool ret = FileUtil::Read(file_path, html);
	if(!ret)
	{
		std::cout << file_path << " read erroe" << std::endl;
	}
	//std::cout << "标题解析" << std::endl;
	ret = ParseTitle(html, info.title);
	if(!ret)
	{
		std::cout << file_path << "parse titlt error" << std::endl;
		return false;
	}
	//std::cout << "正文解析" << std::endl;
	ret = ParseContent(html, info.content);
	if(!ret)
	{
		std::cout << file_path << "parse content error" << std::endl;
		return false;
	}
	//std::cout << "url解析" << std::endl;
	ret = ParseUrl(file_path, info.url);
	if(!ret)
	{
		std::cout << file_path << "parse url error" << std::endl;
		return false;
	}
	return true;
}
void WriteOutput(const DocInfo& info, std::ofstream& file)
{
	//std::cout << "info 写入文件" << std::endl;
	std::string line = info.title + "\3" + info.url + "\3" + info.content + "\n";
	//std::cout << line << std::endl;
	file.write(line.c_str(), line.size());
}
bool EnumFile(const std::string& input_path, std::vector<std::string>& file_list)
{
	bf::path root_path(input_path);
	if(!bf::exists(root_path))
	{
		std::cout << input_path << "not exost" << std::endl;
		return false;
	}
	bf::recursive_directory_iterator end_dir;
	for(bf::recursive_directory_iterator iter(root_path); iter != end_dir; ++iter)
	{
		//去除目录
			//是否是普通文件
	//	if(bf::is_regular(*iter))
	//		continue;
		//只保留html
		if(iter->path().extension() != ".html")
			continue;
		file_list.push_back(iter->path().string());
	}
	return true;
}
	//1、枚举出输入路径中所有的html文档的路径
	//2、依次处理每个枚举出的路径，对文档进行分析，分析出标题/正文/url，并且去标签
	//3、把分析的结果按照一行的形式写入到输出文件中
int main()
{
	//1、枚举出输入路径中所有的html文档的路径
	//每个元素就是一个文件的路径
	std::vector<std::string> file_list;
	bool ret = EnumFile(INPUT_PATH, file_list);
	if(!ret)
	{
		std::cout << "EnumFile "<< INPUT_PATH << " error" << std::endl;
		return 1;
	}
	/*for(const auto& e : file_list)
	{
		std::cout << e << std::endl;
	}*/
	//2、依次处理每个枚举出的路径，对文档进行分析，分析出标题/正文/url，并且去标签
	std::ofstream outfile(OUTPUT_PUTH.c_str());
	if(!outfile.is_open())
	{
		std::cout << "file " << OUTPUT_PUTH << " open error" << std::endl;
		return 1;
	}
	for(const auto& file_path : file_list)
	{
		DocInfo info;
		//解析文件
		ret = ParseFile(file_path, info);
		if(!ret)
		{
			std::cout << "ParseFile " << file_path << "error" << std::endl;
			//return 1;
			continue;
		}
	//3、把分析的结果按照一行的形式写入到输出文件中
	WriteOutput(info,outfile);
	}
	outfile.close();
	return 0;
}
