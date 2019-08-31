/*░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
▩ File Name: server.cpp
▩ Author: top-down
▩ Description: 
▩ Created Time: 2019年08月20日 星期二 22时44分19秒
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░*/

#include<iostream>
#include<fstream>
#include "searcher.h"
#include"httplib.h"
using namespace std;
namespace hb = httplib;

const char* BASE_PATH = "./root";
void GetFile(const hb::Request& req, hb::Response& res)                                                          
{
	string body;
	string tmp;
	ifstream file("./html/search.html");
	while(getline(file,tmp))
	{
		body += tmp +  "\n";
	}
	res.set_content(body.c_str(),"text/html");
}


int main()
{
	daemon(1,0);
	hb::Server srv;
	
	searcher::Searcher search;
	bool ret = search.Init("../data_dir/tmp/raw_input");
	if(!ret)
		return 1;
	srv.set_base_dir(BASE_PATH);
	srv.Get("/cgi-bin/cpp_get.cgi",[&search](const hb::Request& req, hb::Response& res)
				{
				std::string query = req.get_param_value("query");
				std::string body;
				search.Search(query,body);
				res.set_content(body.c_str(),"text/html");
				});
	
	srv.Get("/",GetFile);
	srv.listen("172.17.0.7",9527);
	return 0;
}

