/***************************************************/
/*     created by TheWebServerTeam 2/17/23         */
/***************************************************/

#ifndef WEB_SERVER_SYNTAXTREE_H
#define WEB_SERVER_SYNTAXTREE_H

#include "Libraries.h"

class SyntaxTree{
public:
	std::string token;
	SyntaxTree* parent;
	std::vector<SyntaxTree*> list;

public:

	SyntaxTree(const std::string &token = "", SyntaxTree *parent = NULL): token(token), parent(parent), list(){}

	SyntaxTree *add(const std::string &t){
		SyntaxTree *newItem = new SyntaxTree(t, this);
		list.push_back(newItem);
		return (newItem);
	}

	SyntaxTree *getParent(){
		return (parent);
	}

	std::string &getToken(){
		return (token);
	}

	void display(int level = 0){
		std::cout << token << std::endl;
		for (std::vector<SyntaxTree*>::iterator iter = list.begin(); iter != list.end(); ++iter){
			int i = level;
			while (i-- >= 0)
				std::cout << "\t";
			(*iter)->display(level + 1);
		}
	}

	static void test(){
		SyntaxTree a = SyntaxTree("server");
		SyntaxTree *listen = a.add("listen");
		listen->add("localhost:3031");

		SyntaxTree *server_name = a.add("server_name");
		server_name->add("server_2");

		SyntaxTree *max_client_body_size = a.add("max_client_body_size");
		max_client_body_size->add("42949672");

		SyntaxTree *location = a.add("location");
		location->add("/planet");
		SyntaxTree *redirect = location->add("redirect");
		redirect->add("https://www.facebook.com/");

		a.display();
	}

};


#endif //WEB_SERVER_SYNTAXTREE_H
