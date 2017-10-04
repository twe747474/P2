#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

struct stones
{
	string ip;
	string port; 
};


class steppingStones
{
	int ssSize ;
	vector<stones> steps;
public:
	void setSteps(int size , vector<stones> s)
	{
		ssSize = size; 
		steps = s;
	}
	vector<stones> getSteps()
	{
		return steps; 
	}

};

vector<stones> parseFile(string file, int& ss)
{
	vector<stones> parsedFile; 
	ifstream inFile; 
	inFile.open(file);
	
	if(!inFile)
	{
		cout<<"File opened Fail!"<< endl; 
		exit(1); 
	}
	string inputData; 
	int flag = 0;
	while(getline(inFile, inputData))
	{
		if(flag == 0)
		{
			ss = stoi(inputData);
			flag = 1;
		}
		else
		{
			stones temp; 
			int location = inputData.find_first_of(",");
			temp.ip = inputData.substr(0, location-1);
			temp.port = inputData.substr(location + 1 ,inputData.size());	
			parsedFile.push_back(temp);
		}
	}

	return parsedFile; 
}

int main (int argc, char *argv[]) 
{ 
	string wgetURL;
	steppingStones Stones; 
	switch(argc)
	{
		case 3:
			{
			wgetURL = argv[1];
			int ss; 
			string fileName = argv[2]; 
			std::vector<stones> temp = parseFile(fileName , ss);
			Stones.setSteps(ss,temp); 
			for(stones s : Stones.getSteps())
			{
				cout<<"IP " << s.ip<<endl;
				cout<<"Port "<< s.port<<endl;
			}
			break;
		   }  
		case 2: 
			cout<<"To do";
			break; 
		default: 
			cout<<"Please provide a URL" << endl;
			exit(1);
			break;
	}





















}