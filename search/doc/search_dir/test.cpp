/*░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
▩ File Name: test.cpp
▩ Author: top-down
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░*/

#include "searcher.h"
using namespace searcher;
int main()
{
	/*验证索引模块*/
	/*searcher::Index index;
	bool ret = index.Build("../data_dir/tmp/raw_input");
	if(!ret)
	{
		std::cout << "Build error" << std::endl;
		return 1;
	}
	auto* test = index.GetInvertedList("filesystem");
	if(test == nullptr)
	{
		std::cout << "find  error" << std::endl;
	}
	for(auto w : *test)
	{
		std::cout << "id: " << w.doc_id << " weight" << w.weigth << std::endl;
	}*/
	/*验证搜索模块*/
	searcher::Searcher searcher;
	bool ret = searcher.Init("../data_dir/tmp/raw_input");
	if(!ret)
		return 1;
	std::string query = "filesystem";
	std::string result;
	searcher.Search(query, result);
	std::cout << "result: " << result << std::endl;
	return 0;
}
