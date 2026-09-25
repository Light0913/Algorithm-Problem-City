// gen.cpp —— 娜娜莉的巡视 数据生成器
// 编译: g++ -O2 -std=c++17 gen.cpp -o gen.exe
// 运行: gen.exe          （无参数，当前时间作为种子）
// 输出: 只生成 city8.in ~ city31.in
//       ★ 不触碰 city1.in ~ city7.in（前两个子任务保留原样）
// 规模: 子任务 1: ≤200；子任务 2: ≤2000；子任务 3~6: ≤2e4

#include <bits/stdc++.h>
using namespace std;

static mt19937_64 rng;

inline int rint(int lo, int hi) {
    return lo + (int)(rng() % (unsigned long long)(hi - lo + 1));
}
inline long long rll(long long lo, long long hi) {
    return lo + (long long)(rng() % (unsigned long long)(hi - lo + 1));
}

int n, m, q;
vector<pair<int,int>> edges;
vector<int> ew;

void init(int nn, int mm, int qq) {
    n = nn; m = mm; q = qq;
    edges.assign(m + 1, {0, 0});
    ew.assign(m + 1, 0);
}

// ★ 编码基数用 20001，兼容 2e4 规模
inline long long enc(int u, int v) {
    if (u > v) swap(u, v);
    return (long long)u * 20001LL + v;
}

// ==================== 输出 ====================
static char outbuf[1 << 24];
static size_t outpos = 0;
static FILE* g_out = nullptr;

inline void wc(char c) {
    if (outpos >= sizeof(outbuf)) {
        fwrite(outbuf, 1, outpos, g_out);
        outpos = 0;
    }
    outbuf[outpos++] = c;
}
inline void wi(long long x) {
    if (x < 0) { wc('-'); x = -x; }
    if (x == 0) { wc('0'); return; }
    char tmp[20]; int len = 0;
    while (x) { tmp[len++] = (char)('0' + x % 10); x /= 10; }
    while (len) wc(tmp[--len]);
}
inline void wflush() {
    if (outpos) { fwrite(outbuf, 1, outpos, g_out); outpos = 0; }
}

void out_header() { wi(n); wc(' '); wi(m); wc(' '); wi(q); wc('\n'); }
void out_edges() {
    for (int i = 1; i <= m; i++) {
        wi(edges[i].first);  wc(' ');
        wi(edges[i].second); wc(' ');
        wi(ew[i]);           wc('\n');
    }
}
void op_mod(int e, long long w) { wc('1'); wc(' '); wi(e); wc(' '); wi(w); wc('\n'); }
void op_qry(int u, int v)        { wc('2'); wc(' '); wi(u); wc(' '); wi(v); wc('\n'); }

// ==================== 图生成 ====================

void gen_random_graph(long long wlo, long long whi) {
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    int cnt = 0;
    while (cnt < m) {
        int u = rint(1, n), v = rint(1, n);
        if (u == v) continue;
        long long key = enc(u, v);
        if (used.count(key)) continue;
        used.insert(key);
        cnt++;
        edges[cnt] = {u, v};
        ew[cnt] = (int)rll(wlo, whi);
    }
}

void gen_chain_plus(int mm, long long wlo, long long whi) {
    int base = min(mm, n - 1);
    unordered_set<long long> used;
    used.reserve((size_t)mm * 2);
    for (int i = 1; i <= base; i++) {
        edges[i] = {i, i + 1};
        ew[i] = (int)rll(wlo, whi);
        used.insert(enc(i, i + 1));
    }
    for (int i = base + 1; i <= mm; i++) {
        int u, v;
        do {
            u = rint(1, n);
            v = rint(1, n);
        } while (u == v || used.count(enc(u, v)));
        used.insert(enc(u, v));
        edges[i] = {u, v};
        ew[i] = (int)rll(wlo, whi);
    }
}

void gen_random_tree(long long wlo, long long whi) {
    vector<int> perm(n);
    iota(perm.begin(), perm.end(), 1);
    for (int i = n - 1; i > 0; i--) swap(perm[i], perm[rint(0, i)]);
    for (int i = 1; i < n; i++) {
        int p = perm[rint(0, i - 1)];
        edges[i] = {p, perm[i]};
        ew[i] = (int)rll(wlo, whi);
    }
}

void gen_star_plus(int mm, long long wlo, long long whi) {
    int base = min(mm, n - 1);
    unordered_set<long long> used;
    used.reserve((size_t)mm * 2);
    for (int i = 2; i <= base + 1; i++) {
        edges[i - 1] = {1, i};
        ew[i - 1] = (int)rll(wlo, whi);
        used.insert(enc(1, i));
    }
    for (int i = base + 1; i <= mm; i++) {
        int u, v;
        do {
            u = rint(1, n);
            v = rint(1, n);
        } while (u == v || used.count(enc(u, v)));
        used.insert(enc(u, v));
        edges[i] = {u, v};
        ew[i] = (int)rll(wlo, whi);
    }
}

void gen_complete_binary_tree(long long wlo, long long whi) {
    for (int i = 2; i <= n; i++) {
        edges[i - 1] = {i / 2, i};
        ew[i - 1] = (int)rll(wlo, whi);
    }
}

void gen_disconnected(int nc, long long wlo, long long whi) {
    vector<vector<int>> comps(nc);
    for (int i = 1; i <= n; i++) comps[i % nc].push_back(i);
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    int cnt = 0;
    for (auto& c : comps) {
        for (int i = 1; i < (int)c.size() && cnt < m; i++) {
            int u = c[i];
            int v = c[rint(0, i - 1)];
            long long key = enc(u, v);
            if (used.count(key)) continue;
            used.insert(key);
            cnt++;
            edges[cnt] = {u, v};
            ew[cnt] = (int)rll(wlo, whi);
        }
    }
    while (cnt < m) {
        int ci = rint(0, nc - 1);
        if (comps[ci].size() <= 1) continue;
        int u = comps[ci][rint(0, (int)comps[ci].size() - 1)];
        int v = comps[ci][rint(0, (int)comps[ci].size() - 1)];
        if (u == v) continue;
        long long key = enc(u, v);
        if (used.count(key)) continue;
        used.insert(key);
        cnt++;
        edges[cnt] = {u, v};
        ew[cnt] = (int)rll(wlo, whi);
    }
}

void gen_bipartite(long long wlo, long long whi) {
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    int cnt = 0;
    while (cnt < m) {
        int u = rint(1, n / 2), v = rint(n / 2 + 1, n);
        long long key = enc(u, v);
        if (used.count(key)) continue;
        used.insert(key);
        cnt++;
        edges[cnt] = {u, v};
        ew[cnt] = (int)rll(wlo, whi);
    }
}

// ==================== 子任务 3：n,m,q ≤ 2e4，无修改 ====================
void t3_1() {  // city8
    init(20000, 20000, 20000);
    gen_random_graph(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) op_qry(rint(1,n), rint(1,n));
}
void t3_2() {  // city9
    init(20000, 20000, 20000);
    gen_chain_plus(20000, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) op_qry(rint(1,n), rint(1,n));
}
void t3_3() {  // city10
    init(20000, 20000, 20000);
    gen_star_plus(20000, 0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) op_qry(rint(1,n), rint(1,n));
}
void t3_4() {  // city11
    init(20000, 20000, 20000);
    gen_disconnected(4, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) op_qry(rint(1,n), rint(1,n));
}

// ==================== 子任务 4：n,m,q ≤ 2e4，树 ====================
void t4_1() {  // city12
    init(20000, 19999, 20000);
    gen_random_tree(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL));
        else           op_qry(rint(1,n), rint(1,n));
    }
}
void t4_2() {  // city13
    init(20000, 19999, 20000);
    gen_chain_plus(19999, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod((rng() & 1) ? 1 : m, rll(-1000000000LL, 1000000000LL));
        else {
            int u = rint(1, n / 2), v = rint(n / 2 + 1, n);
            op_qry(u, v);
        }
    }
}
void t4_3() {  // city14
    init(20000, 19999, 20000);
    gen_star_plus(19999, 0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), rll(0, 1000000000LL));
        else           op_qry(rint(1,n), rint(1,n));
    }
}
void t4_4() {  // city15
    init(20000, 19999, 20000);
    gen_complete_binary_tree(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) {
            int leaf = rint(n / 2 + 1, n);
            op_mod(leaf - 1, rll(0, 1000000000LL));
        } else {
            int leaf = rint(n / 2 + 1, n);
            op_qry(1, leaf);
        }
    }
}

// ==================== 子任务 5：n,m,q ≤ 2e4，修改 ≤ 10 ====================
void t5_1() {  // city16
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= 10; t++) op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL));
    for (int t = 11; t <= q; t++) op_qry(rint(1,n), rint(1,n));
}
void t5_2() {  // city17
    init(20000, 20000, 20000);
    gen_chain_plus(20000, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    vector<int> mp;
    for (int i = 0; i < 10; i++) mp.push_back(rint(1, q));
    sort(mp.begin(), mp.end());
    int k = 0;
    for (int t = 1; t <= q; t++) {
        if (k < 10 && t == mp[k]) { k++; op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL)); }
        else op_qry(rint(1,n), rint(1,n));
    }
}
void t5_3() {  // city18
    init(20000, 20000, 20000);
    gen_random_graph(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= 10; t++) op_mod(rint(1,m), rll(0, 1000000000LL));
    for (int t = 11; t <= q; t++) op_qry(rint(1,n), rint(1,n));
}
void t5_4() {  // city19
    init(20000, 20000, 20000);
    gen_disconnected(4, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    vector<int> mp;
    for (int i = 0; i < 10; i++) mp.push_back(rint(1, q));
    sort(mp.begin(), mp.end());
    int k = 0;
    for (int t = 1; t <= q; t++) {
        if (k < 10 && t == mp[k]) { k++; op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL)); }
        else op_qry(rint(1,n), rint(1,n));
    }
}

// ==================== 子任务 6：n,m,q ≤ 2e4，无限制 + Hack ====================
void t6_1() {  // city20
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL));
        else           op_qry(rint(1,n), rint(1,n));
    }
}
void t6_2() {  // city21
    init(20000, 20000, 20000);
    gen_chain_plus(20000, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL));
        else           op_qry(rint(1,n), rint(1,n));
    }
}
void t6_3() {  // city22
    init(20000, 20000, 20000);
    gen_disconnected(5, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL));
        else {
            int u = rint(1, n);
            int v = (rint(0,4) == 0) ? u : rint(1, n);
            op_qry(u, v);
        }
    }
}
void t6_4() {  // city23
    init(20000, 20000, 20000);
    gen_random_graph(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), rll(0, 1000000000LL));
        else           op_qry(rint(1,n), rint(1,n));
    }
}
void t6_5() {  // city24  全负数
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, -1);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), rll(-1000000000LL, -1LL));
        else           op_qry(rint(1,n), rint(1,n));
    }
}
void t6_6() {  // city25  几乎全修改
    init(20000, 20000, 20000);
    gen_random_graph(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t < q; t++) op_mod(rint(1,m), rll(0, 1000000000LL));
    op_qry(rint(1,n), rint(1,n));
}
void t6_7() {  // city26  完全二叉树
    init(20000, 19999, 20000);
    gen_complete_binary_tree(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) {
            int leaf = rint(n / 2 + 1, n);
            op_mod(leaf - 1, rll(0, 1000000000LL));
        } else {
            int leaf = rint(n / 2 + 1, n);
            op_qry(1, leaf);
        }
    }
}
void t6_8() {  // city27  反复修改同一条边
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    int e = rint(1, m);
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(e, rll(-1000000000LL, 1000000000LL));
        else           op_qry(rint(1,n), rint(1,n));
    }
}
void t6_9() {  // city28  修改为主
    init(20000, 20000, 20000);
    gen_random_graph(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0,5) < 4) op_mod(rint(1,m), rll(0, 1000000000LL));
        else               op_qry(rint(1,n), rint(1,n));
    }
}
void t6_10() {  // city29  二分图 |w| 相同
    init(20000, 20000, 20000);
    gen_bipartite(-1000000000LL, 1000000000LL);
    for (int i = 1; i <= m; i++) {
        int sgn = (i % 2 == 0) ? 1 : -1;
        ew[i] = sgn * 1000000000;
    }
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) {
            int sgn = (rng() & 1) ? 1 : -1;
            op_mod(rint(1,m), sgn * 1000000000LL);
        } else op_qry(rint(1,n), rint(1,n));
    }
}
void t6_11() {  // city30  修改查询完全交错
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (t & 1) op_mod(rint(1,m), rll(-1000000000LL, 1000000000LL));
        else       op_qry(rint(1,n), rint(1,n));
    }
}
void t6_12() {  // city31  边界权值
    init(20000, 20000, 20000);
    gen_random_graph(0, 1000000000LL);
    auto special_w = []() -> long long {
        int r = rint(0, 2);
        if (r == 0) return 0;
        if (r == 1) return 1000000000LL;
        return -1000000000LL;
    };
    for (int i = 1; i <= m; i++) ew[i] = (int)special_w();
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1,m), special_w());
        else           op_qry(rint(1,n), rint(1,n));
    }
}

// ==================== main ====================
int main() {
    unsigned long long seed = (unsigned long long)chrono::steady_clock::now().time_since_epoch().count();
    rng.seed(seed);

    // ★ 只生成 city8 ~ city31，不触碰 city1 ~ city7
    for (int tc = 8; tc <= 31; tc++) {
        char filename[32];
        sprintf(filename, "city%d.in", tc);
        g_out = fopen(filename, "wb");
        if (!g_out) { fprintf(stderr, "cannot open %s\n", filename); return 1; }
        outpos = 0;

        switch (tc) {
            case 8:  t3_1(); break;
            case 9:  t3_2(); break;
            case 10: t3_3(); break;
            case 11: t3_4(); break;
            case 12: t4_1(); break;
            case 13: t4_2(); break;
            case 14: t4_3(); break;
            case 15: t4_4(); break;
            case 16: t5_1(); break;
            case 17: t5_2(); break;
            case 18: t5_3(); break;
            case 19: t5_4(); break;
            case 20: t6_1(); break;
            case 21: t6_2(); break;
            case 22: t6_3(); break;
            case 23: t6_4(); break;
            case 24: t6_5(); break;
            case 25: t6_6(); break;
            case 26: t6_7(); break;
            case 27: t6_8(); break;
            case 28: t6_9(); break;
            case 29: t6_10(); break;
            case 30: t6_11(); break;
            case 31: t6_12(); break;
        }

        wflush();
        fclose(g_out);
        g_out = nullptr;
    }

    fprintf(stderr, "Generated city8.in ~ city31.in (city1.in ~ city7.in untouched). Seed = %llu\n", seed);
    return 0;
}