#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>

#define endl '\n'
#define MAX 51

using namespace std;

struct Santa
{
	int num; int x; int y; int score; bool isOut; int isStun;
};

int N, M, P, C, D, curTime = 1;
int MAP[MAX][MAX];
vector<Santa> santa;
int rx, ry;

int dx[] = { -1,0,1,0,-1,1,1,-1 };
int dy[] = { 0,1,0,-1,1,1,-1,-1 };

bool compare(pair<pair<int, int>, pair<int, int>> p1, pair<pair<int, int>, pair<int, int>> p2) {
	if (p1.first.first == p2.first.first) {
		if (p1.second.first == p2.second.first) {
			return p1.second.second > p2.second.second;
		}
		return p1.second.first > p2.second.first;
	}
	return p1.first.first < p2.first.first;
}
bool compareSanta(Santa s1, Santa s2) {
	return s1.num < s2.num;
}

int getDist(int x1, int y1, int x2, int y2) {
	return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}

int getRDir(int x1, int y1, int x2, int y2) {
	if (x1 > x2) {
		if (y1 > y2) return 7;
		else if (y1 == y2) return 0;
		else return 4;
	}
	else if (x1 < x2) {
		if (y1 > y2) return 6;
		else if (y1 == y2) return 2;
		else return 5;
	}
	else {
		if (y1 > y2) return 3;
		else if (y1 < y2) return 1;
	}
    return 0;
}
int getSDir(int x, int y) {
	int ret = -1;
	int curDist = getDist(x, y, rx, ry);
	for (int i = 0; i < 4; ++i) {
		int nx = x + dx[i];
		int ny = y + dy[i];
		if (nx >= 1 && ny >= 1 && nx <= N && ny <= N && MAP[nx][ny] == 0) {
			int nDist = getDist(nx, ny, rx, ry);
			if (curDist > nDist) {
				curDist = nDist;
				ret = i;
			}
		}
	}

	return ret;
}
int reverseDir(int d) {
	if (d == 0) return 2;
	if (d == 1) return 3;
	if (d == 2) return 0;
	if (d == 3) return 1;
    return 0;
}


void addPointOne() {
	for (int i = 0; i < santa.size(); ++i) {
		if (!santa[i].isOut) santa[i].score++;
	}
}

void pushSanta(int targetNum, int x, int y, int dir) {

	if (MAP[x][y] == 0) {
		santa[targetNum].x = x;
		santa[targetNum].y = y;
		MAP[x][y] = targetNum;
	}
	else {
		int nextNum = MAP[x][y];
		int nx = x + dx[dir];
		int ny = y + dy[dir];

		santa[targetNum].x = x;
		santa[targetNum].y = y;
		MAP[x][y] = targetNum;
		if (nx > N || ny > N || nx < 1 || ny < 1) {
			santa[nextNum].isOut = true;
			return;
		}

		pushSanta(nextNum, nx, ny, dir);

	}
}

void moveRudolph() {
	vector<pair<pair<int, int>, pair<int, int>>> v;

	for (auto s : santa) {
		if (!s.isOut)
			v.push_back({ {getDist(rx,ry,s.x,s.y),s.num},{s.x,s.y} });
	}

	sort(v.begin(), v.end(), compare);

	pair<pair<int, int>, pair<int, int>> target = v[0];

	int dir = getRDir(rx, ry, target.second.first, target.second.second);

	rx += dx[dir];
	ry += dy[dir];

	if (rx == target.second.first && ry == target.second.second) {
		santa[target.first.second].score += C;

		int nx = rx + dx[dir] * C;
		int ny = ry + dy[dir] * C;
		MAP[rx][ry] = 0;

		if (nx > N || ny > N || nx < 1 || ny < 1) {
			santa[target.first.second].isOut = true;
		}

		else {
			santa[target.first.second].isStun = curTime;
			pushSanta(target.first.second, nx, ny, dir);
		}
	}
}

void moveSanta() {

	for (int i = 1; i < santa.size(); ++i) {
		if (santa[i].isOut || curTime - santa[i].isStun < 2) continue;

		int x = santa[i].x;
		int y = santa[i].y;

		int dir = getSDir(x, y);
		if (dir == -1) continue;

		x += dx[dir];
		y += dy[dir];


		if (x == rx && y == ry) {
			santa[i].score += D;
			int ndir = reverseDir(dir);

			int nx = x + dx[ndir] * D;
			int ny = y + dy[ndir] * D;

			MAP[santa[i].x][santa[i].y] = 0;

			if (nx > N || ny > N || nx < 1 || ny < 1) {
				santa[i].isOut = true;
			}
			else {
				santa[i].isStun = curTime;
				pushSanta(i, nx, ny, ndir);
			}
		}
		else {
			MAP[santa[i].x][santa[i].y] = 0;
			MAP[x][y] = i;
			santa[i].x = x;
			santa[i].y = y;
		}
	}
}

bool checkSanta() {
	for (int i = 1; i < santa.size(); ++i) {
		if (!santa[i].isOut) return false;
	}
	return true;
}



int main(void) {
	ios_base::sync_with_stdio(0); cin.tie(0);

	cin >> N >> M >> P >> C >> D;
	cin >> rx >> ry;
	santa.push_back({ -1,-1,-1,-1,true,-1 });

	for (int i = 0; i < P; ++i) {
		int num, x, y; cin >> num >> x >> y;
		santa.push_back({ num,x,y,0,false,-1 });
		MAP[x][y] = num;
	}

	sort(santa.begin(), santa.end(), compareSanta);

	for (int i = 1; i <= M; ++i) {
		moveRudolph();
		moveSanta();
		if (checkSanta()) {
			break;
		}
		addPointOne();
		++curTime;
	}

	for (int i = 1; i < santa.size(); ++i) {
		cout << santa[i].score << ' ';
	}
	cout << endl;
}