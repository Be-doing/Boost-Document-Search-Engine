/*░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
▩ File Name: server.cpp
▩ Author: top-down
▩ Description: 
░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░*/

#include<iostream>
#include<fstream>
#include "searcher.h"
#include"httplib.h"
using namespace std;
namespace hb = httplib;

const char* BASE_PATH = "./";
void GetFile(const hb::Request& req, hb::Response& res)                                                          
{
	string body;
	string tmp;
	ifstream file("./root/test.html");
	while(getline(file,tmp))
	{
		body += tmp +  "\n";
	}
	res.set_content(body.c_str(),"text/html");
}

int main()
{
	hb::Server srv;
	
	searcher::Searcher search;
	bool ret = search.Init("../data_dir/tmp/raw_input");
	if(!ret)
		return 1;
	srv.set_base_dir(BASE_PATH);
	srv.Get("/cgi-bin/cpp_get.cgi",[&search](const hb::Request& req, hb::Response& res)
				{
				std::string query = req.get_param_value("first_name");
				std::string body;
				search.Search(query,body);
				res.set_content(body.c_str(),"text/html");
				});
	
	srv.Get("/",GetFile);
	if(!srv.listen("***.***.***.***",***))//私网IP和端口
	{
		std::cout << "listen error" << std::endl;	
	}
	return 0;
}

