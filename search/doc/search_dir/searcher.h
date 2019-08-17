/*░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
▩ File Name: sercher.h
▩ Author: top-down
▩ Description: //构建索引模块和搜索模块
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░*/
//#pragma once
#include<string>
#include<sstream>
#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include<memory>
#include<json/json.h>
#include"../public_dir/util.hpp"
#include<cppjieba/Jieba.hpp>

#define EQ 20 /*权重公式(equal) : 标题是正文的20倍*/
namespace searcher
{
	struct DocInfo
	{
		uint64_t id;
		std::string title;
		std::string content;
		std::string url;
	};
	//weight表示某个词在某个文档出现某次
	struct Weight
	{
		uint64_t doc_id;
		int weight;//权重，为排序做准备
				   //当前用词频计算权重
		std::string key;//关键词
	};

	//通过这个类来描述索引模块
	class Index
	{
		const char* const DICT_PATH = "../dict/jieba.dict.utf8";
		const char* const HMM_PATH = "../dict/hmm_model.utf8";
		const char* const USER_DICT_PATH = "../dict/user.dict.utf8";
		const char* const IDF_PATH = "../dict/idf.utf8";
		const char* const STOP_WORD_PATH = "../dict/stop_words.utf8";
		public:
			Index();
			//读取raw_inout文件，在内存中构建索引
			bool Build(const std::string file_path);
			//正排索引，编号找文档
			const DocInfo* GetDocInfo(int64_t Id);
			//倒排索引，词找id
			const std::vector<Weight>* GetInvertedList(const std::string& key);
			void CutWord(const std::string& input,std::vector<std::string>& output);
			//{
			//	jieba_.CutForSearch(input,output);	
			//}
		private:
			const DocInfo* BuildForward(const std::string& line);
			void BuildInverted(const DocInfo* doc_info);
		private:
			//通过编号获取文档内容，下标作为编号
			std::vector<DocInfo> forward_index_;
			//通过词，来对应id列表
			std::unordered_map<std::string, std::vector<Weight>> inverted_index_;
			
			cppjieba::Jieba jieba_;
	};
	class Searcher
	{
		public:
			Searcher();
			~Searcher();
			bool Init(const std::string& inputpath);
			bool Search(const std::string& query, std::string& result);
		private:
			std::string Getdesc(const std::string& content, const std::string& key);
		private:
			Index* index_;
	};
}//end sercher

