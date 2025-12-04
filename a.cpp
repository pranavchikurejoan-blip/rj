#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;
    int N;
    cin >> N;

    vector<vector<char>> vwall(n+1, vector<char>(m, 0));
    vector<vector<char>> hwall(n, vector<char>(m+1, 0));

    for (int i = 0; i < N; i++) {
        int x1, y1, x2, y2;
        cin >> x1 >> y1 >> x2 >> y2;

        if (x1 > x2) swap(x1, x2);
        if (y1 > y2) swap(y1, y2);

        if (x1 == x2) {
            for (int y = y1; y < y2; y++) {
                if (y >= 0 && y < m && x1 > 0 && x1 < n)
                    vwall[x1][y] = 1;
            }
        } else if (y1 == y2) {
            for (int x = x1; x < x2; x++) {
                if (x >= 0 && x < n && y1 > 0 && y1 < m)
                    hwall[y1][x] = 1;
            }
        }
    }

    vector<vector<int>> comp(n, vector<int>(m, -1));
    int cid = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (comp[i][j] != -1) continue;

            queue<pair<int,int>> q;
            q.push({i, j});
            comp[i][j] = cid;

            while (!q.empty()) {
                auto p = q.front(); q.pop();
                int x = p.first, y = p.second;

                if (x+1 < n && !vwall[x+1][y] && comp[x+1][y] == -1) {
                    comp[x+1][y] = cid;
                    q.push({x+1, y});
                }
                if (x-1 >= 0 && !vwall[x][y] && comp[x-1][y] == -1) {
                    comp[x-1][y] = cid;
                    q.push({x-1, y});
                }
                if (y+1 < m && !hwall[y+1][x] && comp[x][y+1] == -1) {
                    comp[x][y+1] = cid;
                    q.push({x, y+1});
                }
                if (y-1 >= 0 && !hwall[y][x] && comp[x][y-1] == -1) {
                    comp[x][y-1] = cid;
                    q.push({x, y-1});
                }
            }
            cid++;
        }
    }

    int P = cid;
    vector<int> cnt(P, 0), minx(P, INT_MAX), maxx(P, INT_MIN),
                miny(P, INT_MAX), maxy(P, INT_MIN);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int id = comp[i][j];
            cnt[id]++;
            minx[id] = min(minx[id], i);
            maxx[id] = max(maxx[id], i);
            miny[id] = min(miny[id], j);
            maxy[id] = max(maxy[id], j);
        }
    }

    for (int id = 0; id < P; id++) {
        long long area = 1LL * (maxx[id] - minx[id] + 1) *
                         (maxy[id] - miny[id] + 1);
        if (area != cnt[id]) {
            cout << "Invalid\n";
            return 0;
        }
    }

    vector<unordered_set<int>> adj(P);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int a = comp[i][j];

            if (i+1 < n && vwall[i+1][j]) {
                int b = comp[i+1][j];
                if (a != b) {
                    adj[a].insert(b);
                    adj[b].insert(a);
                }
            }
            if (j+1 < m && hwall[j+1][i]) {
                int b = comp[i][j+1];
                if (a != b) {
                    adj[a].insert(b);
                    adj[b].insert(a);
                }
            }
        }
    }

    vector<int> side(P, -1);
    for (int id = 0; id < P; id++) {
        int sx = minx[id], sy = miny[id];
        side[id] = (sx + sy) & 1;
    }

    vector<int> lid(P, -1), rid(P, -1);
    int L = 0, R = 0;
    for (int i = 0; i < P; i++) {
        if (side[i] == 0) lid[i] = L++;
        else rid[i] = R++;
    }

    vector<vector<int>> g(L);
    for (int u = 0; u < P; u++) {
        if (side[u] != 0) continue;
        for (int v : adj[u]) {
            if (side[v] == 1) g[lid[u]].push_back(rid[v]);
        }
    }

    vector<int> dist(L), ml(L, -1), mr(R, -1);

    function<bool()> bfs = [&]() {
        queue<int> q;
        fill(dist.begin(), dist.end(), -1);

        for (int i = 0; i < L; i++) {
            if (ml[i] == -1) {
                dist[i] = 0;
                q.push(i);
            }
        }

        bool ok = false;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : g[u]) {
                int mu = mr[v];
                if (mu != -1 && dist[mu] == -1) {
                    dist[mu] = dist[u] + 1;
                    q.push(mu);
                }
                if (mu == -1) ok = true;
            }
        }
        return ok;
    };

    function<bool(int)> dfs = [&](int u) {
        for (int v : g[u]) {
            int mu = mr[v];
            if (mu == -1 ||
                (dist[mu] == dist[u] + 1 && dfs(mu))) {
                ml[u] = v;
                mr[v] = u;
                return true;
            }
        }
        dist[u] = -1;
        return false;
    };

    int match = 0;
    while (bfs()) {
        for (int i = 0; i < L; i++) {
            if (ml[i] == -1 && dfs(i)) match++;
        }
    }

    int holes = P - match;
    cout << holes << "\n";
    return 0;
}
