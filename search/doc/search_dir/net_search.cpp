#include "searcher.h"
namespace searcher
{
/**************************************************
*索引模块
***************************************************/
	Index::Index()
	:jieba_(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
	{}
	
	const DocInfo* Index::GetDocInfo(int64_t Id)
	{
		if(Id >= forward_index_.size())
			return nullptr;
		return &forward_index_[Id];
	}
	const std::vector<Weight>*  Index::GetInvertedList(const std::string& key)
	{
		auto pos = inverted_index_.find(key);
		if(pos == inverted_index_.end())
			return nullptr;
		return &pos->second;
	}
	/*硬骨头*/
	bool Index::Build(const std::string file_path)
	{	
		std::cout << "Index Build Begin" << std::endl;
		std::ifstream file(file_path.c_str());
		if(!file.is_open())
		{
			std::cout <<  file_path << "nor open" << std::endl;
			return false;
		}
		//1、按行读取每个文档
		std::string line;
		while(std::getline(file, line))/*getline不包含结尾的'\n'*/
		{
			//2、构造DocInfo对象，更新正排索引数据，插入到vector
			const DocInfo* info = BuildForward(line);
			//3、更新倒排索引数据
			BuildInverted(info);
			if(info->id % 500 == 0)
				std::cout << "Build id" << info->id << std::endl;
		}

		std::cout << "Index Build Finish" << std::endl;
		file.close();
		return true;
	}
	
	const DocInfo* Index::BuildForward(const std::string& line)
	{
		//对这行内容进行切分，依据为'\3'
		std::vector<std::string> tokens;//存放切分结果
		StringUtil::Split(line, tokens, "\3");
		if(tokens.size() != 3)
		{
			std::cout << "tokens fail" << std::endl;
			return nullptr;
		}
		//构造一个DocInfo对象
		DocInfo info;
		info.id = forward_index_.size();
		info.title = tokens[0];
		info.url = tokens[1];
		info.content = tokens[2];
		//插入到正排索引
		forward_index_.push_back(info);
		return &forward_index_.back();
	}
	void Index::CutWord(const std::string& input, std::vector<std::string>& output)
	{
		jieba_.CutForSearch(input,output);	
	}
	void Index::BuildInverted(const DocInfo* doc_info)
	{
		//对当前的doc_info进行分词
		std::vector<std::string> title_tok;
		CutWord(doc_info->title, title_tok);
		std::vector<std::string> content_tok;
		CutWord(doc_info->content, content_tok);
		//对标题和正文进行词频统计
		struct WordCnt
		{
			int title_cnt;
			int content_cnt;
		};
		std::unordered_map<std::string, WordCnt> hashcnt;
		for(const auto& word : title_tok)
		{
			++hashcnt[word].title_cnt;
		}
		for(const auto& word : content_tok)
		{
			//boost::to_lower(word);//全部小写
			++hashcnt[word].content_cnt;
		}
		//遍历分词结果，更新倒排索引
		for(const auto& word_pair : hashcnt)
		{
			Weight weight;
			weight.doc_id = doc_info->id;
			weight.weight = EQ * word_pair.second.title_cnt + word_pair.second.content_cnt;
			weight.key = word_pair.first;
			//inverted_index_[word_pair.first].push_back(weight);
			std::vector<Weight>& inver_list = inverted_index_[word_pair.first];
			inver_list.push_back(weight);
		}
	}
/********************************************************
*搜索模块
********************************************************/
	Searcher::Searcher()
	:index_(new Index())
	{}
	Searcher::~Searcher()
	{
		delete index_;
		index_ = nullptr;
	}
	//构建索引
	bool Searcher::Init(const std::string& inputpath)
	{
		return index_->Build(inputpath);
	}
	bool Searcher::Search(const std::string& query, std::string& json_result)
	{
		//分词
		std::vector<std::string> tokens;
		index_->CutWord(query, tokens);
		//触发
		std::vector<Weight> all_token_result;
		for(auto word : tokens)
		{
			boost::to_lower(word);
			auto* inverted_list = index_->GetInvertedList(word);
			if(inverted_list == nullptr)
				continue;
			all_token_result.insert(all_token_result.end(), inverted_list->begin(), inverted_list->end());
		}
		//排序
		std::sort(all_token_result.begin(), all_token_result.end(),[](const Weight& w1, const Weight& w2)
		{
			return w1.weight > w2.weight;
		});
		//返回结果
		std::string html = "<html><head><meta http-equiv='content-type' content='text/html;charset=utf-8'>";
		
		for(const auto& weight : all_token_result)
		{
			const auto* doc_info = index_->GetDocInfo(weight.doc_id);
			if(doc_info == nullptr)
				continue;

			
			html += "<a href='";
			html += doc_info->url;
			html += "'>";
			html += "<h3>";
			html += doc_info->title;
		
			html += "</h3></a>";
			html += "<h5>";
			html += Getdesc(doc_info->content, weight.key); 
		
			html += "</h5>";
			//results.append(result);
		}

		html += "</div></body></head></html>";
		json_result = html;// os.str();
		return true;
	}

	std::string Searcher::Getdesc(const std::string& content, const std::string& key)
	{
		size_t pos = content.find(key);
		if(pos == std::string::npos)
		{
			//该词在正文中不存在，则在开头截取一段，176随机的数字
			return content.size() < 176 ? content : content.substr(0, 176) + "...";
		}
		//找到了就以该位置往前截取xxx(76)个字节，往后截取xxx(100)个字节
		size_t begin = pos < 60 ? 0 : pos - 60;
		if(begin + 176 >= content.size())
			return content.substr(begin);
		return content.substr(begin, 176) + "...";
	}
}//end searcher

