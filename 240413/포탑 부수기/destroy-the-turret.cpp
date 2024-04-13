#include<iostream>
#include<algorithm>
#include<climits>
#include<queue>
#include<cstring>
#include<vector>

#define MAX INT_MAX
using namespace std;

int N, M, K;
int turn;
// 우 하 좌 상 우하, 좌하, 우상, 좌상
int dy[8] = { 0,1,0,-1,1,1,-1,-1 };
int dx[8] = { 1,0,-1,0,1,-1,1,-1 };

int map[51][51];
bool visited[51][51];

int prev_depth;

struct Tower {
	int y, x, power, last_attack_time;
	bool isDead, isActived;
};
vector<Tower>real_route;

struct Cmp {
	bool operator()(Tower a, Tower b) {
		if (a.power != b.power) {
			return a.power > b.power;
		}
		else if (a.last_attack_time != b.last_attack_time) {
			return a.last_attack_time < b.last_attack_time;
		}
		else if ((a.y + a.x) != (b.y + b.x)) {
			return (a.y + a.x) < (b.y + b.x);
		}
		else {
			return a.x < b.x;
		}
	}
};

struct Cmp2 {
	bool operator()(Tower a, Tower b) {
		if (a.power != b.power) {
			return a.power < b.power;
		}
		else if (a.last_attack_time != b.last_attack_time) {
			return a.last_attack_time > b.last_attack_time;
		}
		else if ((a.y + a.x) != (b.y + b.x)) {
			return (a.y + a.x) > (b.y + b.x);
		}
		else {
			return a.x > b.x;
		}
	}
};

Tower tower[11][11];

int CountAlive() {
	int cnt = 0;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			if (!tower[i][j].isDead)cnt++;
		}
	}
	return cnt;
}

void input() {
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			cin >> tower[i][j].power;
			tower[i][j].y = i;
			tower[i][j].x = j;
			tower[i][j].last_attack_time = 0;
			tower[i][j].isActived = false;
			if (tower[i][j].power == 0) {
				tower[i][j].isDead = true;
			}
			else {
				tower[i][j].isDead = false;
			}
		}
	}
}

// 1. 공격자 선정
Tower choiceAttacker() {

	priority_queue<Tower, vector<Tower>, Cmp>pq;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			Tower now = tower[i][j];
			// 사망 포탑은 공격자로 선정 불가
			if (now.isDead)continue;
			pq.push({ now });
		}
	}
	return pq.top();
}

// 2. 피해자 선정
Tower choiceDefender(Tower Attacker) {

	priority_queue<Tower, vector<Tower>, Cmp2>pq;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			Tower now = tower[i][j];
			// 사망 포탑은 피해자로 선정 불가
			if (now.isDead)continue;
			// 공격 포탑은 피해자로 선정 불가
			if (now.y == Attacker.y && now.x == Attacker.x)continue;
			pq.push({ now });
		}
	}
	return pq.top();
}


bool isBroken(int y, int x) {
	if (map[y][x] == 0)return true;
	return false;
}

bool isIn(int y, int x) {
	return y >= 1 && x >= 1 && y <= N && x <= M;
}

void copyMap() {
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			map[i][j] = tower[i][j].power;
		}
	}
}

// 레이저 공격 가능 확인
bool canLaserAttack(Tower Attacker, Tower Defender) {

	int Ay = Attacker.y;
	int Ax = Attacker.x;
	int Dy = Defender.y;
	int Dx = Defender.x;

	queue<vector<Tower>>q;
	vector<Tower>temp;
	temp.push_back(Attacker);
	q.push(temp);

	while (!q.empty()) {
		for (int i = 0; i < 4; i++) {
			vector<Tower>now = q.front();
			// 가장 최근에 이동한 지점 now[now.size()-1].y,x
			int ny = now[now.size()-1].y + dy[i];
			int nx = now[now.size()-1].x + dx[i];
			if (ny > N) {
				ny -= N;
			}
			else if (ny < 1) {
				ny += N;
			}
			if (nx > M) {
				nx -= M;
			}
			else if (nx < 1) {
				nx += M;
			}
			if (!visited[ny][nx] && !isBroken(ny, nx) && isIn(ny, nx)) {
				now.push_back(tower[ny][nx]);
				visited[ny][nx] = true;
				q.push(now);
				if (ny == Defender.y && nx == Defender.x) {
					real_route = now;
					return true;
				}
			}
		}
		q.pop();
	}
	return false;
}

void laserAttack(Tower Attacker, Tower Defender) {
	tower[Attacker.y][Attacker.x].last_attack_time = turn;
	tower[Attacker.y][Attacker.x].isActived = true;
	// real_route에 피해 포탑 좌표가 저장되어 있음
	int power = Attacker.power;
	int half_power = power / 2;
	//cout << "레이저 시작 : " << Attacker.y << " " << Attacker.x << "\n";
	//cout << "목표 지점 : " << Defender.y << " " << Defender.x << "\n";
	for (int i = 1; i < real_route.size(); i++) {
		int y = real_route[i].y;
		int x = real_route[i].x;
		//cout << "레이저 이동 좌표 " << y << " " << x << "\n";
		tower[y][x].isActived = true;
		if (y == Defender.y && x == Defender.x) {
			tower[y][x].power -= power;
		}
		else {
			tower[y][x].power -= half_power;
		}
		if (tower[y][x].power <= 0) {
			tower[y][x].isDead = true;
			tower[y][x].power = 0;

		}
	}
}

void missileAttack(Tower Attacker, Tower Defender) {
	tower[Attacker.y][Attacker.x].last_attack_time = turn;
	tower[Attacker.y][Attacker.x].isActived = true;
	int Dy = Defender.y;
	int Dx = Defender.x;
	int power = Attacker.power;
	//cout << " 포탑 공격 실시! 공격자의 좌표는 : " << Attacker.y << " " << Attacker.x << "\n";
	//cout << "현재 공격자의 파워 : " << power << "\n";
	//cout << "피해자의 좌표는 : " << Dy << " " << Dx << "\n";
	int half_power = power / 2;
	tower[Dy][Dx].power -= power;
	tower[Dy][Dx].isActived = true;
	if (tower[Dy][Dx].power <= 0) {
		tower[Dy][Dx].power = 0;
		tower[Dy][Dx].isDead = true;
	}
	for (int i = 0; i < 8; i++) {
		int Dny = Dy + dy[i];
		int Dnx = Dx + dx[i];
		if (Dny > N) {
			Dny -= N;
		}
		else if (Dny < 1) {
			Dny += N;
		}
		if (Dnx > M) {
			Dnx -= M;
		}
		else if (Dnx < 1) {
			Dnx += M;
		}
		// 공격자인 경우 영향 x
		if (Dny == Attacker.y && Dnx == Attacker.x)continue;
		tower[Dny][Dnx].power -= half_power;
		tower[Dny][Dnx].isActived = true;
		if (tower[Dny][Dnx].power <= 0) {
			tower[Dny][Dnx].power = 0;
			tower[Dny][Dnx].isDead = true;
		}
	}
}

void repairTower() {
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			if (tower[i][j].isDead || tower[i][j].isActived)continue;
			tower[i][j].power++;
		}
	}
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			tower[i][j].isActived = false;
		}
	}
}

// 3. 공격 시도
void Attack() {
	// 공격, 수비 타워 생성
	Tower Attacker = choiceAttacker();
	Tower Defender = choiceDefender(Attacker);
	Attacker.power += (N + M);
	tower[Attacker.y][Attacker.x].power = Attacker.power;

	// 레이저 공격 가능한지 확인
	// 그 전에, 전역 변수 초기화
	memset(visited, false, sizeof(visited));
	visited[Attacker.y][Attacker.x] = true;
	real_route.clear();
	prev_depth = MAX;

	copyMap();

	if (canLaserAttack(Attacker, Defender)) {
		// 가능 하면 레이저 공격 진행
		laserAttack(Attacker, Defender);
		//if (CountAlive() == 1)return;
	}
	// 불가능하면 포탄 공격 실시
	else {
		missileAttack(Attacker, Defender);
		//if (CountAlive() == 1)return;
	}
	if (CountAlive() == 1)return;
	// 포탑 정비
	repairTower();
}
int Answer() {
	int ans = 0;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			if (tower[i][j].isDead)continue;
			ans = max(ans, tower[i][j].power);
		}
	}
	return ans;
}

void PrintAllTower() {
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			cout << tower[i][j].power << " ";
		}
		cout << "\n";
	}
}

int main() {
	cin >> N >> M >> K;
	input();
	for (int i = 0; i < K; i++) {
		//cout << "\n";
		turn++;
		Attack();
		if (CountAlive() == 1)break;
		//PrintAllTower();
		
	}
	cout << Answer() << "\n";
	return 0;
}