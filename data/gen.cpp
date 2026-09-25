// gen.cpp —— 娜娜莉的巡视（第二大值版） 数据生成器
// 编译: g++ -O2 -std=c++17 gen.cpp -o gen.exe
// 运行: gen.exe
// 输出: 只生成 city8.in ~ city31.in（city1.in ~ city7.in 保持原样）
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
vector<long long> ew;

void init(int nn, int mm, int qq) {
    n = nn; m = mm; q = qq;
    edges.assign(m + 1, {0, 0});
    ew.assign(m + 1, 0);
}

inline long long enc(int u, int v) {
    if (u > v) swap(u, v);
    return (long long)u * 20001LL + v;
}

// ==================== 输出 ====================
static char outbuf[1 << 24];
static size_t outpos = 0;
static FILE* g_out = nullptr;

inline void wc(char c) {
    if (outpos >= sizeof(outbuf)) { fwrite(outbuf, 1, outpos, g_out); outpos = 0; }
    outbuf[outpos++] = c;
}
inline void wi(long long x) {
    if (x < 0) { wc('-'); x = -x; }
    if (x == 0) { wc('0'); return; }
    char tmp[20]; int len = 0;
    while (x) { tmp[len++] = (char)('0' + x % 10); x /= 10; }
    while (len) wc(tmp[--len]);
}
inline void wflush() { if (outpos) { fwrite(outbuf, 1, outpos, g_out); outpos = 0; } }

void out_header() { wi(n); wc(' '); wi(m); wc(' '); wi(q); wc('\n'); }
void out_edges() {
    for (int i = 1; i <= m; i++) {
        wi(edges[i].first); wc(' ');
        wi(edges[i].second); wc(' ');
        wi(ew[i]); wc('\n');
    }
}
void op_mod(int e, long long w) { wc('1'); wc(' '); wi(e); wc(' '); wi(w); wc('\n'); }
void op_qry(int u, int v)       { wc('2'); wc(' '); wi(u); wc(' '); wi(v); wc('\n'); }

// ==================== 通用图生成 ====================

// 填充剩余边（保留前 fixed 条边），随机加边去重
void fill_remaining(int fixed, long long wlo, long long whi) {
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    for (int i = 1; i <= fixed; i++) {
        int a = edges[i].first, b = edges[i].second;
        if (a != b) used.insert(enc(a, b));
    }
    for (int i = fixed + 1; i <= m; i++) {
        int u, v;
        do { u = rint(1, n); v = rint(1, n); }
        while (u == v || used.count(enc(u, v)));
        used.insert(enc(u, v));
        edges[i] = {u, v};
        ew[i] = rll(wlo, whi);
    }
}

void gen_random_graph(long long wlo, long long whi) {
    fill_remaining(0, wlo, whi);
}

// 链 + 高权值加边（卡单边路径）
void gen_chain_high_extra(long long chain_w, long long extra_w) {
    for (int i = 1; i <= n - 1; i++) {
        edges[i] = {i, i + 1};
        ew[i] = (i % 2 == 0) ? -chain_w : chain_w;
    }
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    for (int i = 1; i <= n - 1; i++) used.insert(enc(i, i + 1));
    for (int i = n; i <= m; i++) {
        int u, v;
        do { u = rint(1, n); v = rint(1, n); }
        while (u == v || used.count(enc(u, v)));
        used.insert(enc(u, v));
        edges[i] = {u, v};
        ew[i] = (i % 2 == 0) ? -extra_w : extra_w;
    }
}

// 星形 + 随机加边
void gen_star_plus(long long star_lo, long long star_hi,
                   long long extra_lo, long long extra_hi) {
    int base = min(m, n - 1);
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    for (int i = 2; i <= base + 1; i++) {
        edges[i - 1] = {1, i};
        ew[i - 1] = rll(star_lo, star_hi);
        used.insert(enc(1, i));
    }
    for (int i = base + 1; i <= m; i++) {
        int u, v;
        do { u = rint(1, n); v = rint(1, n); }
        while (u == v || used.count(enc(u, v)));
        used.insert(enc(u, v));
        edges[i] = {u, v};
        ew[i] = rll(extra_lo, extra_hi);
    }
}

// 随机树（Prüfer 式）
void gen_random_tree(long long wlo, long long whi) {
    vector<int> perm(n);
    iota(perm.begin(), perm.end(), 1);
    for (int i = n - 1; i > 0; i--) swap(perm[i], perm[rint(0, i)]);
    for (int i = 1; i < n; i++) {
        int p = perm[rint(0, i - 1)];
        edges[i] = {p, perm[i]};
        ew[i] = rll(wlo, whi);
    }
}

// 链树
void gen_chain_tree(long long w) {
    for (int i = 1; i <= n - 1; i++) {
        edges[i] = {i, i + 1};
        ew[i] = (i % 2 == 0) ? -w : w;
    }
}

// 星形树
void gen_star_tree(long long wlo, long long whi) {
    for (int i = 2; i <= n; i++) {
        edges[i - 1] = {1, i};
        ew[i - 1] = rll(wlo, whi);
    }
}

// 完全二叉树
void gen_complete_binary_tree(long long wlo, long long whi) {
    for (int i = 2; i <= n; i++) {
        edges[i - 1] = {i / 2, i};
        ew[i - 1] = rll(wlo, whi);
    }
}

// 多连通块
void gen_disconnected(int nc, long long wlo, long long whi) {
    vector<vector<int>> comps(nc);
    for (int i = 1; i <= n; i++) comps[i % nc].push_back(i);
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    int cnt = 0;
    for (auto& c : comps) {
        for (int i = 1; i < (int)c.size() && cnt < m; i++) {
            int u = c[i], v = c[rint(0, i - 1)];
            long long key = enc(u, v);
            if (used.count(key)) continue;
            used.insert(key); cnt++;
            edges[cnt] = {u, v};
            ew[cnt] = rll(wlo, whi);
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
        used.insert(key); cnt++;
        edges[cnt] = {u, v};
        ew[cnt] = rll(wlo, whi);
    }
}

// 稠密图（约 50% 密度）
void gen_dense_graph(int density_pct, long long wlo, long long whi) {
    // n=2e4, 50% 密度无法真的放 m/2 条边（会超 m），
    // 所以用 "随机生成 m 条边，但挑短距离" 模拟稠密
    unordered_set<long long> used;
    used.reserve((size_t)m * 2);
    for (int i = 1; i <= m; i++) {
        int u, v;
        do {
            // 限制距离近的点对，制造稠密效果
            u = rint(1, n);
            int d = rint(1, max(1, density_pct));
            v = u + d;
            if (v > n) v = u - d;
            if (v < 1) v = 1;
        } while (u == v || used.count(enc(u, v)));
        used.insert(enc(u, v));
        edges[i] = {u, v};
        ew[i] = rll(wlo, whi);
    }
}

// ==================== 子任务 3：n,m,q ≤ 2e4，无修改 ====================

void t3_1() {  // city8: 随机图
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) op_qry(rint(1, n), rint(1, n));
}

void t3_2() {  // city9: 链 + 高权值加边，60% 相邻点
    init(20000, 20000, 20000);
    gen_chain_high_extra(100, 1000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 6) {
            int u = rint(1, n - 1);
            op_qry(u, u + 1);
        } else {
            op_qry(rint(1, n), rint(1, n));
        }
    }
}

void t3_3() {  // city10: 星形 + 随机加边，60% 中心↔叶子
    init(20000, 20000, 20000);
    gen_star_plus(1, 100, 1000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 6) {
            int leaf = rint(2, n);
            op_qry(1, leaf);
        } else {
            op_qry(rint(1, n), rint(1, n));
        }
    }
}

void t3_4() {  // city11: 4 连通块
    init(20000, 20000, 20000);
    gen_disconnected(4, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) op_qry(rint(1, n), rint(1, n));
}

// ==================== 子任务 4：n,m,q ≤ 2e4，树 ====================

void t4_1() {  // city12: 随机树
    init(20000, 19999, 20000);
    gen_random_tree(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        else op_qry(rint(1, n), rint(1, n));
    }
}

void t4_2() {  // city13: 链树，50% 相邻 + 50% 距离 2
    init(20000, 19999, 20000);
    gen_chain_tree(100);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) {
            op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        } else {
            if (rint(0, 1) == 0) {
                int u = rint(1, n - 1);
                op_qry(u, u + 1);
            } else {
                int u = rint(1, n - 2);
                op_qry(u, u + 2);
            }
        }
    }
}

void t4_3() {  // city14: 星形树，60% 中心↔叶子
    init(20000, 19999, 20000);
    gen_star_tree(0, 1000);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1, m), rll(0, 1000000000LL));
        else {
            if (rint(0, 10) < 6) {
                int leaf = rint(2, n);
                op_qry(1, leaf);
            } else {
                op_qry(rint(1, n), rint(1, n));
            }
        }
    }
}

void t4_4() {  // city15: 完全二叉树，50% 兄弟对
    init(20000, 19999, 20000);
    gen_complete_binary_tree(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) {
            int leaf = rint(n / 2 + 1, n);
            op_mod(leaf - 1, rll(0, 1000000000LL));
        } else {
            if (rint(0, 1) == 0) {
                int p = rint(1, n / 2);
                if (2 * p + 1 <= n) op_qry(2 * p, 2 * p + 1);
                else op_qry(1, n);
            } else {
                op_qry(rint(1, n), rint(1, n));
            }
        }
    }
}

// ==================== 子任务 5：n,m,q ≤ 2e4，修改 ≤ 10 ====================

void t5_1() {  // city16: 随机图
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= 10; t++) op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
    for (int t = 11; t <= q; t++) op_qry(rint(1, n), rint(1, n));
}

void t5_2() {  // city17: 链 + 高权加边，60% 相邻点
    init(20000, 20000, 20000);
    gen_chain_high_extra(100, 1000000LL);
    out_header(); out_edges();
    vector<int> mp;
    for (int i = 0; i < 10; i++) mp.push_back(rint(1, q));
    sort(mp.begin(), mp.end());
    int k = 0;
    for (int t = 1; t <= q; t++) {
        if (k < 10 && t == mp[k]) {
            k++;
            op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        } else {
            if (rint(0, 10) < 6) {
                int u = rint(1, n - 1);
                op_qry(u, u + 1);
            } else {
                op_qry(rint(1, n), rint(1, n));
            }
        }
    }
}

void t5_3() {  // city18: 稠密图
    init(20000, 20000, 20000);
    gen_dense_graph(100, 0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= 10; t++) op_mod(rint(1, m), rll(0, 1000000000LL));
    for (int t = 11; t <= q; t++) op_qry(rint(1, n), rint(1, n));
}

void t5_4() {  // city19: 4 连通块
    init(20000, 20000, 20000);
    gen_disconnected(4, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    vector<int> mp;
    for (int i = 0; i < 10; i++) mp.push_back(rint(1, q));
    sort(mp.begin(), mp.end());
    int k = 0;
    for (int t = 1; t <= q; t++) {
        if (k < 10 && t == mp[k]) {
            k++;
            op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        } else {
            op_qry(rint(1, n), rint(1, n));
        }
    }
}

// ==================== 子任务 6：n,m,q ≤ 2e4，Hack ====================

void t6_1() {  // city20: 随机图，基础正确性
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        else op_qry(rint(1, n), rint(1, n));
    }
}

void t6_2() {  // city21: 链 + 随机加边，含负数
    init(20000, 20000, 20000);
    gen_chain_high_extra(100, 1000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        else {
            if (rint(0, 10) < 6) {
                int u = rint(1, n - 1);
                op_qry(u, u + 1);
            } else {
                op_qry(rint(1, n), rint(1, n));
            }
        }
    }
}

void t6_3() {  // city22: 5 连通块 + u=v + 不连通
    init(20000, 20000, 20000);
    gen_disconnected(5, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        else {
            int u = rint(1, n);
            int v = (rint(0, 4) == 0) ? u : rint(1, n);
            op_qry(u, v);
        }
    }
}

void t6_4() {  // city23: 非负随机图
    init(20000, 20000, 20000);
    gen_random_graph(0, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rng() & 1) op_mod(rint(1, m), rll(0, 1000000000LL));
        else op_qry(rint(1, n), rint(1, n));
    }
}

void t6_5() {  // city24: 单边 + 绕路
    init(20000, 20000, 20000);
    // 前 4 条边：单边捷径 + 绕路
    edges[1] = {1, 4};   ew[1] = 1;
    edges[2] = {1, 2};   ew[2] = 100;
    edges[3] = {2, 3};   ew[3] = 100;
    edges[4] = {3, 4};   ew[4] = 100;
    fill_remaining(4, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 3) {
            op_mod(rint(5, m), rll(-1000000000LL, 1000000000LL));
        } else {
            if (rint(0, 10) < 7) op_qry(1, 4);
            else op_qry(rint(1, n), rint(1, n));
        }
    }
}

void t6_6() {  // city25: 全相同权值 + 单边捷径
    init(20000, 20000, 20000);
    // 前 6 条边
    edges[1] = {1, 2}; ew[1] = 5;
    edges[2] = {2, 3}; ew[2] = 5;
    edges[3] = {3, 4}; ew[3] = 5;
    edges[4] = {4, 5}; ew[4] = 5;
    edges[5] = {2, 4}; ew[5] = 5;
    edges[6] = {1, 5}; ew[6] = 1;
    // 剩余边权值全为 5
    fill_remaining(6, 5, 5);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 3) {
            op_mod(rint(7, m), 5);  // 修改也只改到 5
        } else {
            if (rint(0, 10) < 7) op_qry(1, 5);
            else op_qry(rint(1, n), rint(1, n));
        }
    }
}

void t6_7() {  // city26: 单边 + 绕路并存（更大规模）
    init(20000, 20000, 20000);
    // 前 7 条边
    edges[1] = {1, 6};  ew[1] = 1;
    edges[2] = {1, 2};  ew[2] = 100;
    edges[3] = {2, 3};  ew[3] = 100;
    edges[4] = {3, 4};  ew[4] = 100;
    edges[5] = {4, 5};  ew[5] = 100;
    edges[6] = {5, 6};  ew[6] = 100;
    edges[7] = {2, 5};  ew[7] = 100;
    fill_remaining(7, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 3) {
            op_mod(rint(8, m), rll(-1000000000LL, 1000000000LL));
        } else {
            if (rint(0, 10) < 7) op_qry(1, 6);
            else op_qry(rint(1, n), rint(1, n));
        }
    }
}

void t6_8() {  // city27: 反复修改同一条边
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    int e = rint(1, m);
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 8) op_mod(e, rll(-1000000000LL, 1000000000LL));
        else op_qry(rint(1, n), rint(1, n));
    }
}

void t6_9() {  // city28: 分量距离 = 1
    init(20000, 20000, 20000);
    // 前 5 条边
    edges[1] = {1, 2}; ew[1] = 5;
    edges[2] = {2, 3}; ew[2] = 5;
    edges[3] = {4, 5}; ew[3] = 5;
    edges[4] = {5, 6}; ew[4] = 5;
    edges[5] = {3, 4}; ew[5] = 100;
    fill_remaining(5, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 3) {
            op_mod(rint(6, m), rll(-1000000000LL, 1000000000LL));
        } else {
            if (rint(0, 10) < 7) op_qry(1, 6);
            else op_qry(rint(1, n), rint(1, n));
        }
    }
}

void t6_10() {  // city29: 分量距离 = 2
    init(20000, 20000, 20000);
    // 前 7 条边
    edges[1] = {1, 2}; ew[1] = 5;
    edges[2] = {3, 4}; ew[2] = 5;
    edges[3] = {5, 6}; ew[3] = 5;
    edges[4] = {7, 8}; ew[4] = 5;
    edges[5] = {2, 3}; ew[5] = 100;
    edges[6] = {4, 5}; ew[6] = 100;
    edges[7] = {6, 7}; ew[7] = 100;
    fill_remaining(7, -1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 3) {
            op_mod(rint(8, m), rll(-1000000000LL, 1000000000LL));
        } else {
            if (rint(0, 10) < 7) op_qry(1, 8);
            else op_qry(rint(1, n), rint(1, n));
        }
    }
}

void t6_11() {  // city30: 修改查询完全交错
    init(20000, 20000, 20000);
    gen_random_graph(-1000000000LL, 1000000000LL);
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (t & 1) op_mod(rint(1, m), rll(-1000000000LL, 1000000000LL));
        else       op_qry(rint(1, n), rint(1, n));
    }
}

void t6_12() {  // city31: 多档权值 + 单边捷径
    init(20000, 20000, 20000);
    // 前 4 条边：单边捷径 + 绕路，含极端权值
    edges[1] = {1, 2}; ew[1] = 1;
    edges[2] = {2, 3}; ew[2] = 1000000000LL;
    edges[3] = {3, 4}; ew[3] = 1000000000LL;
    edges[4] = {1, 4}; ew[4] = 5;
    // 剩余边从多档权值中取
    {
        unordered_set<long long> used;
        used.reserve((size_t)m * 2);
        for (int i = 1; i <= 4; i++) {
            int a = edges[i].first, b = edges[i].second;
            if (a != b) used.insert(enc(a, b));
        }
        long long vals[] = {0, 1, -1, 5, -5, 1000000000LL, -1000000000LL};
        for (int i = 5; i <= m; i++) {
            int u, v;
            do { u = rint(1, n); v = rint(1, n); }
            while (u == v || used.count(enc(u, v)));
            used.insert(enc(u, v));
            edges[i] = {u, v};
            ew[i] = vals[rint(0, 6)];
        }
    }
    out_header(); out_edges();
    for (int t = 1; t <= q; t++) {
        if (rint(0, 10) < 3) {
            long long vals[] = {0, 1, -1, 5, -5, 1000000000LL, -1000000000LL};
            op_mod(rint(5, m), vals[rint(0, 6)]);
        } else {
            if (rint(0, 10) < 7) op_qry(1, 4);
            else op_qry(rint(1, n), rint(1, n));
        }
    }
}

// ==================== main ====================
int main() {
    unsigned long long seed =
        (unsigned long long)chrono::steady_clock::now().time_since_epoch().count();
    rng.seed(seed);

    // 只生成 city8 ~ city31，不触碰 city1 ~ city7
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