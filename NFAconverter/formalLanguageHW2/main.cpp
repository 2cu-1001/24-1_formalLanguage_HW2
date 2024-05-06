#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
#include <unordered_set>
#define ll long long
#define p (long long int)1e+9
#define a (long long int)37
#define x first
#define y second

using namespace std;

ll powArr[1010], startState, startStateIdx, adjDFA[1010][150];
vector<ll> stateSet, terminalSet, finalStateSet, adjNFA[1010][150], closure[1010];
vector<vector<ll>> stateSetsDFA, finalStateSetsDFA;
unordered_set<ll> stateSetHash;
//adjNFA[i][j] : state i에서 terminal j를 보고 갈 수 있는 state 집합 vector
//'0'~'9' : 48~57, 'A'~'B' : 65~90, 'a'~'z' : 97~122, 'ε' : 52917 -> 123

/* input form ex
StateSet = { q000, q001, q002, q003, q004 }
TerminalSet = { 0, 1 }
DeltaFunctions = {
	(q000, 0) = {q001, q002}
	(q000, 1) = {q001, q003}
	(q001, 0) = {q001, q002}
	(q001, 1) = {q001, q003}
	(q002, 0) = {q004}
	(q003, 1) = {q004}
	(q004, 0) = {q004}
	(q004, ε) = {q004}
}
StartState = q000
FinalStateSet = { q004 }
*/


int strState2numState(string stateStr)
{
	return (stateStr[1] - '0') * 100 + (stateStr[2] - '0') * 10 
		+ (stateStr[3] - '0'); //"q123" -> 123
}

string numState2strState(ll stateNum)
{
	return "q" + to_string(stateNum / 100) + to_string((stateNum % 100) / 10) + to_string(stateNum % 10);
}

void getPowArr()
{
	powArr[0] = 1;
	for (int i = 1; i <= 1000; i++) powArr[i] = (powArr[i - 1] * a) % p;
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
			else if (str[idx] == 'ε')
				terminalSet.push_back(123);
		}
	}

	else if (tmp == "DeltaFunctions") return;

	else if (tmp == "StartState") { //"q000" -> 0
		startState = strState2numState(str.substr(idx + 3));
		for(int i = 0; i<stateSet.size(); i++) 
			if (stateSet[i] == startState) { startStateIdx = i; break; }
	}
		
	else { //list of DeltaFunctions -> adjacency matrix
		ll curState, nxtState, curTerm;
		string tmp; tmp.clear();

		for (; idx < str.length(); idx++) { //get curState
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
				if (curTerm == 'ε') adjNFA[curState][123].push_back(nxtState);
				else adjNFA[curState][curTerm].push_back(nxtState);
			}
		}
	}
}

void getClosure(int curState, int parState)
{
	closure[parState].push_back(curState);
	for (ll nxtState : adjNFA[curState][123]) //ε은 123에 임의로 인덱싱
		getClosure(nxtState, parState);
}

void NFAoutput()
{

}

ll getStateSetHash(vector<ll> &tmpStateSet)
{
	ll curHash = 0;
	for (int i = 0; i < tmpStateSet.size(); i++) 
		curHash = (curHash + tmpStateSet[i] * powArr[i]) % p;
	return curHash;
}

void DFAoutput()
{
	cout << "StateSet = { ";
	for (int i = 1; i < stateSetsDFA.size(); i++) {

	}
	cout << " }\n";
	
	cout << "TerminalSet = { " << (char)terminalSet[0];
	for (int i = 1; i < terminalSet.size(); i++) cout << ", " << (char)terminalSet[i];
	cout << " }\n";

	cout << "DeltaFunctions = {\n";
	for (int i = 0; i < stateSetsDFA.size(); i++) {
		for (auto curTerm : terminalSet) {
			if (adjDFA[i][curTerm] == -1) continue;

		}
	}
	cout << "}\n";

	cout << "StartState = { " << numState2strState(startState) << " }\n";

	cout << "FinalStateSet = { ";
}

void rDFAoutput()
{

}

// 1st call----------------------------------------------------------------------------------------------
void init()
{
	getPowArr();
	for (int i = 0; i < 1010; i++)
		for (int j = 0; j < 150; j++)
			adjDFA[i][j] = -1;
}

void input()
{
	string fileName, str; 
	cout << "파일명 입력 (ex)abc.txt) : ";
	cin >> fileName; 
	ifstream file(fileName);
	while (getline(file, str))
		parsing(str);
	NFAoutput();
}

void NFA2DFA()
{	
	//DFS를 통해 각 state의 closure 탐색
	for(ll curState : stateSet)
		getClosure(curState, curState);

	queue<pair<vector<ll>, ll>> q;
	vector<ll> startStateSet = { startState };
	ll stateNum = 0; 

	stateSetsDFA.push_back(startStateSet);
	stateSetHash.insert(getStateSetHash(startStateSet));
	q.push({ startStateSet, stateNum });

	//BFS를 통해 도달 가능한 state set들만 탐색
	while (!q.empty()) {
		vector<ll> curStateSet;
		ll curStateNum;

		tie(curStateSet, curStateNum) = q.front(); q.pop();

		for (ll curTerm : terminalSet) {

		}
	}

	//BFS틑 통해 구한 state set에서 NFA의 finalstateset을 포함하는 state set들 탐색
	for (auto curStateSet : stateSetsDFA) {

	}

	DFAoutput();
}

void DFA2rDFA()
{

	rDFAoutput();
}

void output()
{

}

int main()
{
	ios_base::sync_with_stdio(0);
	cin.tie(0); cout.tie(0);
	init();
	input();
	NFA2DFA();
	DFA2rDFA();
	output();
}
