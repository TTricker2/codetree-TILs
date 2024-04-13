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
int dy[8] = { 0,1,0,-1,1,1,-1,-1 };
int dx[8] = { 1,0,-1,0,1,-1,1,-1 };
bool visited[11][11];

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
			tower[i][j].isDead = (tower[i][j].power == 0);
		}
	}
}

Tower choiceAttacker() {
	priority_queue<Tower, vector<Tower>, Cmp>pq;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			Tower now = tower[i][j];
			if (now.isDead)continue;
			pq.push({ now });
		}
	}
	return pq.top();
}

Tower choiceDefender(Tower Attacker) {
	priority_queue<Tower, vector<Tower>, Cmp2>pq;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			Tower now = tower[i][j];
			if (now.isDead || (now.y == Attacker.y && now.x == Attacker.x))continue;
			pq.push({ now });
		}
	}
	return pq.top();
}

bool isBroken(int y, int x) {
	return tower[y][x].power == 0;
}

bool isIn(int y, int x) {
	return y >= 1 && x >= 1 && y <= N && x <= M;
}

void copyMap() {
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			tower[i][j].power = tower[i][j].power;
		}
	}
}

bool canLaserAttack(Tower Attacker, Tower Defender) {
	int Ay = Attacker.y;
	int Ax = Attacker.x;
	int Dy = Defender.y;
	int Dx = Defender.x;

	queue<vector<Tower>>q;
	vector<Tower>temp;
	temp.push_back(Attacker);
	q.push(temp);

	memset(visited, false, sizeof(visited));
	visited[Attacker.y][Attacker.x] = true;
	real_route.clear();

	while (!q.empty()) {
		for (int i = 0; i < 4; i++) {
			vector<Tower>now = q.front();
			int ny = now[now.size()-1].y + dy[i];
			int nx = now[now.size()-1].x + dx[i];
			if (ny > N) ny -= N;
			else if (ny < 1) ny += N;
			if (nx > M) nx -= M;
			else if (nx < 1) nx += M;
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
	int power = Attacker.power;
	int half_power = power / 2;
	for (int i = 1; i < real_route.size(); i++) {
		int y = real_route[i].y;
		int x = real_route[i].x;
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
		if (Dny > N) Dny -= N;
		else if (Dny < 1) Dny += N;
		if (Dnx > M) Dnx -= M;
		else if (Dnx < 1) Dnx += M;
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
			if (!tower[i][j].isDead && !tower[i][j].isActived) {
				tower[i][j].power++;
			}
		}
	}
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			tower[i][j].isActived = false;
		}
	}
}

void Attack() {
	Tower Attacker = choiceAttacker();
	Tower Defender = choiceDefender(Attacker);
	Attacker.power += (N + M);
	tower[Attacker.y][Attacker.x].power = Attacker.power;

	memset(visited, false, sizeof(visited));
	visited[Attacker.y][Attacker.x] = true;
	real_route.clear();

	if (canLaserAttack(Attacker, Defender)) {
		laserAttack(Attacker, Defender);
	}
	else {
		missileAttack(Attacker, Defender);
	}
	if (CountAlive() == 1) return;
	repairTower();
}

int Answer() {
	int ans = 0;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			if (!tower[i][j].isDead) {
				ans = max(ans, tower[i][j].power);
			}
		}
	}
	return ans;
}

int main() {
	cin >> N >> M >> K;
	input();
	for (int i = 0; i < K; i++) {
		turn++;
		Attack();
		if (CountAlive() == 1) break;
	}
	cout << Answer() << "\n";
	return 0;
}