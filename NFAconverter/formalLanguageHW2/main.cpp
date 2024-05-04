#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#define x first
#define y second
using namespace std;

int startState;
vector<int> stateSet, terminalSet, finalStateSet;
vector<pair<int,int>> NFAadj[1010]; //{Terminal, State}
//'0'~'9' : 48~57, 'A'~'B' : 65~90, 'a'~'z' : 97~122

int strState2numState(string stateStr)
{
	return (stateStr[1] - '0') * 100 + (stateStr[2] - '0') * 10 
		+ (stateStr[3] - '0'); //"q123" -> q123
}

void parsing(string str)
{
	int idx = 0;
	string tmp; tmp.clear();

	for (; idx < str.length(); idx++) {
		if (str[idx] == '}') return;
		else if (str[idx] != ' ' && str[idx] != '\t') tmp += str[idx];
		else break;
	}

	if (tmp == "StateSet" || tmp == "FinalStateSet") { //"{ q000, q001, q002, ... }" -> { 0, 1, 2, ... }
		idx += 4;
		string strState; strState.clear();
		for (; idx < str.length(); idx++) {
			if ((str[idx] == ',' || str[idx] == ' ') && !strState.empty()) {
				int curState = strState2numState(strState);
				if (tmp == "StateSet") stateSet.push_back(curState);
				else if (tmp == "FinalStateSet") finalStateSet.push_back(curState);
				strState.clear();
			}
			else if (!(str[idx] == ',' || str[idx] == ' ')) strState += str[idx];
		}
	}

	else if (tmp == "TerminalSet") { //"{ 0, a }" 
		idx += 4;
		for (; idx < str.length(); idx++) {
			if ((str[idx] >= '0' && str[idx] <= '9') || (str[idx] >= 'a' && str[idx] <= 'z')
				|| (str[idx] >= 'A' && str[idx] <= 'Z'))
				terminalSet.push_back(str[idx]);
		}
	}

	else if (tmp == "DeltaFunctions") return;

	else if (tmp == "StartState") //"q000" -> 0
		startState = strState2numState(str.substr(idx + 3));

	else { //list of DeltaFunctions -> adjacency matrix
		int curState, nxtState, curTerm;
		string tmp; tmp.clear();

		for (; idx < str.length(); idx++) { //get curStte
			if (str[idx] == 'q' || (str[idx] >= '0' && str[idx] <= '9')) tmp += str[idx];
			else if (str[idx] == ',') break;
		}
		curState = strState2numState(tmp); tmp.clear();

		idx += 2;
		curTerm = str[idx++];

		for (; idx < str.length(); idx++) { //get nxtStates
			if (str[idx] == 'q' || (str[idx] >= '0' && str[idx] <= '9')) tmp += str[idx];
			else if (str[idx] == ',' || str[idx] == '}') {
				nxtState = strState2numState(tmp); tmp.clear();
				NFAadj[curState].push_back({ curTerm, nxtState });
			}
		}
	}
}

int getStateSetHash()
{
	return 0;
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
	NFA2DFA();
	DFA2rDFA();
	output();
}
