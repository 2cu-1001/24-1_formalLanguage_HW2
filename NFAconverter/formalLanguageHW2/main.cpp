#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#define x first
#define y second
#define ll long long
#define p (long long int)1e+9
#define a (long long int)37

using namespace std;

bool isEpsilon = false;
ll powArr[1010], startState, startStateIdx, adjDFA[1010][150], adjRDFA[1010][150]; 
string fileName;
vector<ll> stateSet, terminalSet, finalStateSet, adjNFA[1010][150], closure[1010];
vector<vector<ll>> stateSetsDFA, finalStateSetsDFA, stateSetsRDFA, finalStateSetsRDFA;
vector<ll>  startStateSetDFA, startStateSetRDFA, finalStateSetsDFAidx, finalStateSetsRDFAidx;
unordered_set<ll> stateSetHashDFA, stateSetHashRDFA;
unordered_map<ll, ll> stateSetHashNumDFA;
//adjNFA[i][j] : state i에서 terminal j를 보고 갈 수 있는 state 집합 vector
//'0'~'9' : 48~57, 'A'~'B' : 65~90, 'a'~'z' : 97~122, 'ε' : 52917 -> 123

bool cmp(vector<ll> v1, vector<ll> v2)
{
    return v1[0] < v2[0];
}

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
        for (int i = 0; i < stateSet.size(); i++)
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
                if (curTerm < 0) {
                    adjNFA[curState][123].push_back(nxtState);
                    isEpsilon = true;
                }
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

ll getStateSetHash(vector<ll>& tmpStateSet)
{
    ll curHash = 0;
    for (int i = 0; i < tmpStateSet.size(); i++)
        curHash = (curHash + tmpStateSet[i] * powArr[i]) % p;
    return curHash;
}

void DFAoutput()
{
    cout << "StateSet = { ";
    for (int j = 0; j < stateSetsDFA.size(); j++) {
        auto curState = stateSetsDFA[j];
        cout << "{" << numState2strState(curState[0]);
        for (int i = 1; i < curState.size(); i++)
            cout << ", " << numState2strState(curState[i]);
        cout << "}";
        if (j != stateSetsDFA.size() - 1) cout << ", ";
    }
    cout << " }\n";

    cout << "TerminalSet = { " << (char)terminalSet[0];
    for (int i = 1; i < terminalSet.size(); i++) {
        if (terminalSet[i] == 123) continue;
        else cout << ", " << (char)terminalSet[i];
    }
    cout << " }\n";

    cout << "DeltaFunctions = {\n";
    for (int i = 0; i < stateSetsDFA.size(); i++) {
        for (auto curTerm : terminalSet) {
            if (adjDFA[i][curTerm] == -1) continue;

            cout << "\t(";
            auto curStateSet = stateSetsDFA[i];
            cout << "{" << numState2strState(curStateSet[0]);
            for (int i = 1; i < curStateSet.size(); i++)
                cout << ", " << numState2strState(curStateSet[i]);
            cout << "}";

            cout << ", " << (char)curTerm << ") = ";

            auto nxtStateSet = stateSetsDFA[adjDFA[i][curTerm]];
            cout << "{" << numState2strState(nxtStateSet[0]);
            for (int i = 1; i < nxtStateSet.size(); i++)
                cout << ", " << numState2strState(nxtStateSet[i]);
            cout << "}\n";
        }
    }
    cout << "}\n";

    cout << "startState = { " << numState2strState(startStateSetDFA[0]);
    for (int i = 1; i < startStateSetDFA.size(); i++) {
        cout << ", " << numState2strState(startStateSetDFA[i]);
    }
    cout << " }\n";

    cout << "FinalStateSet = { ";
    for (int j = 0; j < finalStateSetsDFA.size(); j++) {
        auto curFinalStateSet = finalStateSetsDFA[j];
        cout << "{" << numState2strState(curFinalStateSet[0]);
        for (int i = 1; i < curFinalStateSet.size(); i++)
            cout << ", " << numState2strState(curFinalStateSet[i]);
        cout << "}";
        if (j != finalStateSetsDFA.size() - 1) cout << ", ";
    }
    cout << " }\n";
}

void RDFAoutput()
{
    cout << "StateSet = { ";
    for (int j = 0; j < stateSetsRDFA.size(); j++) {
        auto curState = stateSetsRDFA[j];
        cout << "{" << numState2strState(curState[0]);
        for (int i = 1; i < curState.size(); i++)
            cout << ", " << numState2strState(curState[i]);
        cout << "}";
        if (j != stateSetsRDFA.size() - 1) cout << ", ";
    }
    cout << " }\n";

    cout << "TerminalSet = { " << (char)terminalSet[0];
    for (int i = 1; i < terminalSet.size(); i++) cout << ", " << (char)terminalSet[i];
    cout << " }\n";

    cout << "DeltaFunctions = {\n";
    for (int i = 0; i < stateSetsRDFA.size(); i++) {
        for (auto curTerm : terminalSet) {
            if (adjRDFA[i][curTerm] == -1) continue;

            cout << "\t(";
            auto curStateSet = stateSetsRDFA[i];
            cout << "{" << numState2strState(curStateSet[0]);
            for (int i = 1; i < curStateSet.size(); i++)
                cout << ", " << numState2strState(curStateSet[i]);
            cout << "}";

            cout << ", " << (char)curTerm << ") = ";

            auto nxtStateSet = stateSetsRDFA[adjRDFA[i][curTerm]];
            cout << "{" << numState2strState(nxtStateSet[0]);
            for (int i = 1; i < nxtStateSet.size(); i++)
                cout << ", " << numState2strState(nxtStateSet[i]);
            cout << "}\n";
        }
    }
    cout << "}\n";

    cout << "startState = { " << numState2strState(startStateSetRDFA[0]);
    for (int i = 1; i < startStateSetRDFA.size(); i++) {
        cout << ", " << numState2strState(startStateSetRDFA[i]);
    }
    cout << " }\n";

    cout << "FinalStateSet = { ";
    for (int j = 0; j < finalStateSetsRDFA.size(); j++) {
        auto curFinalStateSet = finalStateSetsRDFA[j];
        cout << "{" << numState2strState(curFinalStateSet[0]);
        for (int i = 1; i < curFinalStateSet.size(); i++)
            cout << ", " << numState2strState(curFinalStateSet[i]);
        cout << "}";
        if (j != finalStateSetsRDFA.size() - 1) cout << ", ";
    }
    cout << " }\n";
}

void init()
{
    getPowArr();
    for (int i = 0; i < 1010; i++)
        for (int j = 0; j < 150; j++) adjDFA[i][j] = adjRDFA[i][j] = - 1;
}

void input()
{
    string str;
    cout <<  "파일명 입력(ex)abc.txt) : ";
    cin >> fileName;
    ifstream file(fileName);
    string NFAoutPutName = fileName.substr(0, fileName.length() - 4) + "NFA.txt";
    freopen(NFAoutPutName.c_str(), "w", stdout);
    while (getline(file, str)) {
        parsing(str);
        cout << str << "\n";
    }
    fileName = fileName.substr(0, fileName.length() - 4);
}

void NFA2DFA()
{
    //DFS를 통해 각 state의 closure 탐색
    for (ll curState : stateSet)
        getClosure(curState, curState);

    queue<pair<vector<ll>, ll>> q;
    vector<ll> startStateSet = closure[startState];
    ll stateNum = startState;

    stateSetsDFA.push_back(startStateSet);
    stateSetHashDFA.insert(getStateSetHash(startStateSet));
    stateSetHashNumDFA.insert({ getStateSetHash(startStateSet), 0 });
    q.push({ startStateSet, 0 });

    //BFS를 통해 도달 가능한 state set들만 탐색
    while (!q.empty()) {
        vector<ll> curStateSet;
        ll curStateNum;

        tie(curStateSet, curStateNum) = q.front();
        q.pop();

        for (ll curTerm : terminalSet) {
            vector<ll> nxtStateSet, nxtClosureSet;
            nxtStateSet.clear(); nxtClosureSet.clear();

            for (ll curState : curStateSet)
                nxtStateSet.insert(nxtStateSet.end(), adjNFA[curState][curTerm].begin(),
                    adjNFA[curState][curTerm].end());
            sort(nxtStateSet.begin(), nxtStateSet.end());
            nxtStateSet.erase(unique(nxtStateSet.begin(), nxtStateSet.end()), nxtStateSet.end());
            //nxtStatSet : 현재 state에서 어떤 terminal 보고 갈 수 있는 state 집합

            for (ll nxtState : nxtStateSet)
                nxtClosureSet.insert(nxtClosureSet.end(), closure[nxtState].begin(), closure[nxtState].end());
            sort(nxtClosureSet.begin(), nxtClosureSet.end());
            nxtClosureSet.erase(unique(nxtClosureSet.begin(), nxtClosureSet.end()), nxtClosureSet.end());
            //nxtClosureSet : nxtStatSet의 state들의 closure의 합집합

            if (nxtClosureSet.empty()) continue;

            ll nxtStateSetHash = getStateSetHash(nxtClosureSet);
            if (stateSetHashDFA.find(nxtStateSetHash) != stateSetHashDFA.end()) {
                adjDFA[curStateNum][curTerm] = stateSetHashNumDFA.find(nxtStateSetHash)->second;
                continue;
                //기존의 state set이 재등장
            }

            stateSetHashDFA.insert(nxtStateSetHash);
            stateSetsDFA.push_back(nxtClosureSet);

            ll nxtStateNum = stateSetsDFA.size() - 1;
            adjDFA[curStateNum][curTerm] = nxtStateNum;
            //또한 인접행렬 adjDFA에 cur state와 nxt state의 관계를 기록 (새로운 state set 정의)

            stateSetHashNumDFA.insert({ nxtStateSetHash , nxtStateNum });
            //nxtClosureSet의 해시값을 unordered_set에서 find, 이미 존재하는 state인지 검색
            //nxtClosureSet가 이전에 등장한 적 없는 state set이면 q에 삽입하고 set과 stateSetsDFA 벡터에 기록

            q.push({ nxtClosureSet, nxtStateNum });
        }
    }

    //BFS틑 통해 구한 state set에서 NFA의 finalstateset을 포함하는 state set들 탐색
    sort(finalStateSet.begin(), finalStateSet.end());

    for (int i = 0; i < stateSetsDFA.size(); i++) {
        auto curStateSet = stateSetsDFA[i];
        sort(curStateSet.begin(), curStateSet.end());
        bool isCurStateSetOk = true;

        for (ll curState : finalStateSet)
            if (find(curStateSet.begin(), curStateSet.end(), curState) == curStateSet.end()) {
                isCurStateSetOk = false; break;
            }

        if (isCurStateSetOk) {
            finalStateSetsDFA.push_back(curStateSet);
            finalStateSetsDFAidx.push_back(i);
        }
    }

    //startStateSet 
    startStateSetDFA = stateSetsDFA[0];

    string DFAoutPutNaem = fileName + "DFA.txt";
    freopen(DFAoutPutNaem.c_str(), "w", stdout);
    DFAoutput();
}

void DFA2RDFA()
{
    vector<ll> tmpFinalStateSet, tmpNonFinalStateSet;
    tmpFinalStateSet = finalStateSetsDFAidx;
    for (int i = 0; i < stateSetsDFA.size(); i++)
        if (find(tmpFinalStateSet.begin(), tmpFinalStateSet.end(), i) == tmpFinalStateSet.end())
            tmpNonFinalStateSet.push_back(i);
    //final state와 non final state set으로 분할, 
    // ex) {1, 3, 4}, {0, 2} -> NFA2DFA에서 만든 stateSet 인덱스 사용

    //split-----------------------------------
    vector<vector<ll>> newStateSets;
    newStateSets.push_back(tmpFinalStateSet);
    newStateSets.push_back(tmpNonFinalStateSet);

    bool isSplit = true;
    //분할이 더 이상 일어나지 않을 때까지 분할
    while (isSplit) {
        isSplit = false;
        vector<vector<ll>> tmpNewStateSets;
        // split 이후 변화된 stateSet 벡터 -> tmpNewStateSet에 저장 후, newStateSets 갱신

        for (auto curStateSet : newStateSets) {
            vector<pair<ll, ll>> tmpStateSetHash;
            // {stateHashValue, stateNum} 
            // stateNum번 state에서 모든 terminal을 차례로 볼 때, 나오는 다음 stateSet의 hash값 = stateHashVal

            for (auto curState : curStateSet) {
                vector<ll> tmpNxtStateSets;
                for (auto curTerm : terminalSet) {
                    auto nxtStateNum = adjDFA[curState][curTerm];

                    bool flag = false;
                    for (int i = 0; i < newStateSets.size(); i++)
                        if (find(newStateSets[i].begin(), newStateSets[i].end(), nxtStateNum) != newStateSets[i].end()) {
                            flag = true;
                            tmpNxtStateSets.push_back(i); break;
                        }
                    if (!flag) tmpNxtStateSets.push_back(-1);

                }
                tmpStateSetHash.push_back({ getStateSetHash(tmpNxtStateSets), curState });
            }

            sort(tmpStateSetHash.begin(), tmpStateSetHash.end()); 
            vector<ll> tmp;
            tmp.push_back(tmpStateSetHash[0].y);
            //필요시, curStateSet에서 split
            for (int i = 1; i < tmpStateSetHash.size(); i++) {
                if (i > 0 && tmpStateSetHash[i].x != tmpStateSetHash[i - 1].x) {
                    tmpNewStateSets.push_back(tmp); tmp.clear(); 
                    tmp.push_back(tmpStateSetHash[i].y);
                    isSplit = true;
                }
                else tmp.push_back(tmpStateSetHash[i].y);
            }
            if (!tmp.empty()) tmpNewStateSets.push_back(tmp);
        }
        newStateSets = tmpNewStateSets;
        //split 이후 stateSet으로 newStateSet 갱신
    }
 
    stateSetsRDFA = newStateSets;
    for (auto curStateSet : stateSetsRDFA) sort(curStateSet.begin(), curStateSet.end());
    sort(stateSetsRDFA.begin(), stateSetsRDFA.end(), cmp);

    //adjRDFA에 mere
    for (int i = 0; i < stateSetsRDFA.size(); i++) {
        auto curStateSetNum = stateSetsRDFA[i][0];
        for (auto curTerm : terminalSet) {
            ll curNxtStateSetNum = adjDFA[curStateSetNum][curTerm];
            if (curNxtStateSetNum == -1) continue;
            ll curNxtStateSetHash = getStateSetHash(stateSetsDFA[curNxtStateSetNum]);

            for (int j = 0; j < stateSetsRDFA.size(); j++)
                for (auto cmpStateSetNum : stateSetsRDFA[j])
                    if (getStateSetHash(stateSetsDFA[cmpStateSetNum]) == curNxtStateSetHash) {
                        adjRDFA[i][curTerm] = j; j = stateSetsRDFA.size() + 1; break;
                    }
        }
    }

    //DFA의 state 번호로 만든 stateSetsRDFA를 원래 state 번호로 변경
    for (int i = 0; i < stateSetsRDFA.size(); i++) {
        vector<ll> tmpStateSet; tmpStateSet.clear();
        for (auto stateSetNum : stateSetsRDFA[i]) {
            tmpStateSet.insert(tmpStateSet.begin(), stateSetsDFA[stateSetNum].begin(),
                stateSetsDFA[stateSetNum].end());
        }
        sort(tmpStateSet.begin(), tmpStateSet.end());
        tmpStateSet.erase(unique(tmpStateSet.begin(), tmpStateSet.end()), tmpStateSet.end());
        stateSetsRDFA[i] = tmpStateSet;
    }

    //RDFA의 final stateSet 탐색
    for (int i = 0; i < stateSetsRDFA.size(); i++) {
        auto curStateSet = stateSetsRDFA[i];
        sort(curStateSet.begin(), curStateSet.end());
        bool isCurStateSetOk = true;

        for (ll curState : finalStateSet)
            if (find(curStateSet.begin(), curStateSet.end(), curState) == curStateSet.end()) {
                isCurStateSetOk = false; break;
            }

        if (isCurStateSetOk) {
            finalStateSetsRDFA.push_back(curStateSet);
            finalStateSetsRDFAidx.push_back(i);
        }
    }

    //startStateSet 
    startStateSetRDFA = stateSetsRDFA[0];

    string RDFAoutPutNaem = fileName + "RDFA.txt";
    freopen(RDFAoutPutNaem.c_str(), "w", stdout);
    RDFAoutput();
}

int main()
{
    init();
    input();
    NFA2DFA();
    DFA2RDFA();
}
