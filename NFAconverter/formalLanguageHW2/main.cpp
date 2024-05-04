#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#define x first
#define y second
using namespace std;

int startState;
vector<int> eNFAfinalStateSet;
vector<pair<int,int>> eNFAadj[1010]; //{Terminal, State}
vector<int> closure[1010];

bool iseNFA = false;

int strState2numState(string stateStr)
{
	return (stateStr[1] - '0') * 100 + (stateStr[2] - '0') * 10 
		+ (stateStr[3] - '0'); //"q123" -> q123
}

void parsing(string str)
{
	int idx = 0;
	string tmp;
	tmp.clear();

	for (; idx < str.length(); idx++) 
		if (str[idx] != ' ') tmp += str[idx];

	if (tmp == "StateSet") { //"{ q000, q001, q002, ... }" -> 0, 1, 2
		idx += 4;
		string strState;
		strState.clear();
		for (; idx < str.length(); idx++) {
			if (str[idx] == ',' || str[idx] == ' ') {


				strState.clear();
			}
			else strState += str[idx];
		}
	}
	else if (tmp == "TerminalSet") { //"{ 0, a }" -> {'0', 'a'}

	}
	else if (tmp == "DeltaFunctions") return;
	else if (tmp == "StarteState") //"q000" -> 0
		startState = strState2numState(str.substr(idx + 3));
	else if (tmp == "FinalStateSet") { //"{ q001, q004 }" -> {0, 4}

	}
	else { //list of DeltaFunctions -> adjacency matrix

	}
}

void getClosure()
{

}

void input()
{
	string fileName, str; 
	cout << "파일명 입력 (ex)abc.txt) : ";
	cin >> fileName; 
	ifstream file(fileName);
	while (getline(file, str))
		parsing(str);
}

void NFA2DFA()
{

}

void eNFA2DFA()
{

}

void DFA2rDFA()
{

}

void output()
{

}

int main()
{
	ios_base::sync_with_stdio(0);
	cin.tie(0); cout.tie(0);
	input();
	if (!iseNFA) NFA2DFA();
	else eNFA2DFA();
	DFA2rDFA();
	output();
}