#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#define FOR(i,s,e) for(int i=s; i<e; i++)

int L, Q;
int customer_cnt, sushi_cnt; //결과

struct CMD {
    int cmd, t, x, n;
    string name;
};

vector<CMD> v, commands;
unordered_set<string> visit_customer_name; //방문 손님 저장, 재방문 없음.
unordered_map<string, int> sitting_pos; //앉은 자리
unordered_map<string, int> visit_customer_time; //방문 시각
unordered_map<string, int> eat_num; //먹어야 할 수

void input(){
    cin >> L >> Q;
    FOR(i,0,Q){
        int cmd = -1, t=-1, x=-1, n=-1;
        string name = "";
        
        cin >> cmd;
        if(cmd == 100){
            cin >> t >> x >> name;
        }
        else if(cmd == 200){
            cin >> t >> x >> name >> n;
            visit_customer_name.insert(name);
            sitting_pos[name] = x;
            visit_customer_time[name] = t;
            eat_num[name] = n;
        }
        else if(cmd == 300){
            cin >> t;
        }
        
        CMD tmp;
        tmp.cmd = cmd;
        tmp.t = t;
        tmp.x = x;
        tmp.name = name;
        tmp.n = n;
        commands.push_back(tmp);
        
        
    }
    
}
//t 작을 수록, 명령이 작을 수록 먼저
bool cmp(CMD left, CMD right){
    if(left.t < right.t) return true;
    if(left.t > right.t) return false;
    
    if(left.cmd > right.cmd) return false;
    if(left.cmd < right.cmd) return true;
    
    return false;
}

void sol(){
    FOR(i, 0, commands.size()) { //생성된 초밥이 언제 없어질지 기록
        if(commands[i].cmd != 100) continue;
        
        CMD sushis = commands[i];
        int meet_sushi_customer_time = 0; //초밥과 손님이 만나게 할수 있는 시간
        int eat_sushi_time = 0; //초밥 먹는 시각
        
        if(sushis.t < visit_customer_time[sushis.name]){
            //초밥이 손님보다 먼저 존재
            int now_sushi_pos = sushis.x; //현재 초밥 위치
            int diff_t = visit_customer_time[sushis.name] - sushis.t; // 방문시점과 초밥이 올라온 시간 차이
            now_sushi_pos = ((now_sushi_pos + diff_t) % L);
            
            if(sitting_pos[sushis.name] >= now_sushi_pos){
                //손님이 초밥보다 시계방향 먼쪽에 있음
                meet_sushi_customer_time = sitting_pos[sushis.name] - now_sushi_pos; 
                
            } 
            else if(sitting_pos[sushis.name] < now_sushi_pos){
                //손님이 초밥보다 시계방향 가까운 쪽
                meet_sushi_customer_time = (sitting_pos[sushis.name] + L) - now_sushi_pos;
            }
            eat_sushi_time = visit_customer_time[sushis.name] + meet_sushi_customer_time;
                
        } 
        else{
            //손님 입장 후에 초밥이 올라감
            
            int now_sushi_pos = sushis.x;
            if(sitting_pos[sushis.name] >= now_sushi_pos){
                meet_sushi_customer_time = sitting_pos[sushis.name] - now_sushi_pos;
            }
            else if(sitting_pos[sushis.name] < now_sushi_pos){
                meet_sushi_customer_time = (sitting_pos[sushis.name] + L) - now_sushi_pos;
            }
            eat_sushi_time = sushis.t + meet_sushi_customer_time; 
            
        }
        
        
        CMD tmp;
        tmp.cmd = sushis.cmd + 1; //101
        tmp.t = eat_sushi_time;
        tmp.x = -1;
        tmp.name = sushis.name;
        tmp.n = -1;
        v.push_back(tmp); //스시 사라지는 시점 저장
    }
    
    
    commands.insert(commands.end(), v.begin(), v.end());
    sort(commands.begin(), commands.end(), cmp);
    
    FOR(i, 0, commands.size()){
        if(commands[i].cmd == 100){
            ++sushi_cnt;
        }
        else if(commands[i].cmd == 101){
            sushi_cnt --; 
            string now_name = commands[i].name;
            eat_num[now_name] -= 1; //먹어야 할 수를 하나 줄임
            if(eat_num[now_name] == 0) -- customer_cnt; //다 먹으면 손님 없앰.
            
        }
        else if(commands[i].cmd == 200){
            ++customer_cnt;
        }
        else if(commands[i].cmd == 300){
            cout<< customer_cnt << " " << sushi_cnt << '\n';   
        }
        
    }
    
    
}


int main(){
    input();
    sol();
    
    return 0;
}