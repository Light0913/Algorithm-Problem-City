#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());

    int n = (int)(rng() % 2000) + 1;
    int m = (int)(rng() % 2000) + 1;
    int q = (int)(rng() % 2000) + 1;

    if (m > (long long)n * (n - 1) / 2)
        m = (int)min((long long)m, (long long)n * (n - 1) / 2);

    cout << n << ' ' << m << ' ' << q << '\n';

    unordered_set<long long> used;
    for (int i = 1; i <= m; i++) {
        int u, v;
        do {
            u = (int)(rng() % n) + 1;
            v = (int)(rng() % n) + 1;
        } while (u == v || used.count(min(u, v) * 2001LL + max(u, v)));
        used.insert(min(u, v) * 2001LL + max(u, v));

        long long w = (long long)(rng() % 2000000001LL) - 1000000000LL;
        cout << u << ' ' << v << ' ' << w << '\n';
    }

    for (int t = 1; t <= q; t++) {
        int op = (int)(rng() % 2) + 1;
        if (op == 1) {
            int e = (int)(rng() % m) + 1;
            long long nw = (long long)(rng() % 2000000001LL) - 1000000000LL;
            cout << "1 " << e << ' ' << nw << '\n';
        } else {
            int u = (int)(rng() % n) + 1;
            int v = (int)(rng() % n) + 1;
            cout << "2 " << u << ' ' << v << '\n';
        }
    }

    return 0;
}