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
vector<ll> stateSet, terminalSet, finalStateSet, adjNFA[1010][150], adjrDFA[1010][150];
vector<vector<ll>> stateSetsDFA;
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

void getPowArr()
{
	powArr[0] = 1;
	for (int i = 1; i <= 1000; i++) powArr[i] = (powArr[i - 1] * a) % p;
}

//3rd call----------------------------------------------------------------------------------------------
int strState2numState(string stateStr)
{
	return (stateStr[1] - '0') * 100 + (stateStr[2] - '0') * 10 
		+ (stateStr[3] - '0'); //"q123" -> 123
}

//2nd call----------------------------------------------------------------------------------------------
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
				if (curTerm == 'ε') adjNFA[curState][123].push_back(nxtState);
				else adjNFA[curState][curTerm].push_back(nxtState);
			}
		}
	}
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

}

// 1st call----------------------------------------------------------------------------------------------
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
			vector<ll> tmpNxtStatSet;
			for (ll curState : curStateSet)
				tmpNxtStatSet.insert(tmpNxtStatSet.begin(), adjNFA[curState][curTerm].begin(), 
					adjNFA[curState][curTerm].end());

			unique(tmpNxtStatSet.begin(), tmpNxtStatSet.end());
			sort(tmpNxtStatSet.begin(), tmpNxtStatSet.end());

			ll nxtStateSetHash = getStateSetHash(tmpNxtStatSet);
			if (stateSetHash.find(nxtStateSetHash) == stateSetHash.end()) continue;
			stateSetHash.insert(nxtStateSetHash);
			stateSetsDFA.push_back(tmpNxtStatSet);
			//현재 state의 해시값을 unordered_set에서 find, 이미 존재하는 state인지 검색
			//이전에 등장한 적 없는 state set이면 q에 삽입하고 set과 stateSetsDFA 벡터에 기록

			ll nxtStateNum = stateSetsDFA.size() - 1;
			adjDFA[curStateNum][curTerm] = nxtStateNum;
			//또한 인접행렬 adjDFA에 cur state와 nxt state의 관계를 기록
			
			q.push({ tmpNxtStatSet, nxtStateNum});
		}
	}
	DFAoutput();
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
	getPowArr();
	input();
	NFA2DFA();
	DFA2rDFA();
	output();
}
