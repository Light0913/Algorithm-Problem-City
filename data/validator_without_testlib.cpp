// check.cpp —— 娜娜莉的巡视 数据校验器
// 编译: g++ -O2 -std=c++17 check.cpp -o check
// 运行: check
// 规模: n, m, q ≤ 2e4

#include <bits/stdc++.h>
using namespace std;

struct TestInfo {
    int id;
    long long maxNMQ;
    int special;   // 0=无, 1=无修改, 2=树, 3=修改<=10
    string desc;
};

static vector<TestInfo> tests = {
    {1,  200,    0, "n,m,q<=200"},
    {2,  200,    0, "n,m,q<=200"},
    {3,  200,    0, "n,m,q<=200"},
    {4,  2000,   0, "n,m,q<=2000"},
    {5,  2000,   0, "n,m,q<=2000"},
    {6,  2000,   0, "n,m,q<=2000"},
    {7,  2000,   0, "n,m,q<=2000"},
    {8,  20000,  1, "无修改"},
    {9,  20000,  1, "无修改"},
    {10, 20000,  1, "无修改"},
    {11, 20000,  1, "无修改"},
    {12, 20000,  2, "树"},
    {13, 20000,  2, "树"},
    {14, 20000,  2, "树"},
    {15, 20000,  2, "树"},
    {16, 20000,  3, "修改<=10"},
    {17, 20000,  3, "修改<=10"},
    {18, 20000,  3, "修改<=10"},
    {19, 20000,  3, "修改<=10"},
    {20, 20000,  0, "无限制"},
    {21, 20000,  0, "无限制"},
    {22, 20000,  0, "无限制"},
    {23, 20000,  0, "无限制"},
    {24, 20000,  0, "无限制"},
    {25, 20000,  0, "无限制"},
    {26, 20000,  0, "无限制"},
    {27, 20000,  0, "无限制"},
    {28, 20000,  0, "无限制"},
    {29, 20000,  0, "无限制"},
    {30, 20000,  0, "无限制"},
    {31, 20000,  0, "无限制"},
};

static int failures = 0;
static int warnings = 0;

static void fail(int tc, const string& msg) {
    fprintf(stderr, "[FAIL] city%d.in: %s\n", tc, msg.c_str());
    failures++;
}
static void warn(int tc, const string& msg) {
    fprintf(stderr, "[WARN] city%d.in: %s\n", tc, msg.c_str());
    warnings++;
}

static const long long WMAX = 1000000000LL;

static int dsu_find(vector<int>& fa, int x) {
    while (fa[x] != x) { fa[x] = fa[fa[x]]; x = fa[x]; }
    return x;
}

static void check_one(const TestInfo& info) {
    int tc = info.id;
    char fname[32];
    sprintf(fname, "city%d.in", tc);

    FILE* f = fopen(fname, "rb");
    if (!f) { fail(tc, "文件不存在"); return; }
    fclose(f);

    ifstream fin(fname);
    if (!fin) { fail(tc, "无法打开"); return; }

    long long n, m, q;
    if (!(fin >> n >> m >> q)) { fail(tc, "无法读取 n,m,q"); return; }

    if (n < 1 || n > info.maxNMQ) { fail(tc, "n 越界"); return; }
    if (m < 1 || m > info.maxNMQ) { fail(tc, "m 越界"); return; }
    if (q < 1 || q > info.maxNMQ) { fail(tc, "q 越界"); return; }

    int N = (int)n, M = (int)m, Q = (int)q;

    vector<int> eu(M + 1), ev(M + 1);
    vector<long long> ew(M + 1);
    bool has_self = false, has_w_overflow = false;
    vector<long long> keys;
    keys.reserve(M);

    for (int i = 1; i <= M; i++) {
        if (!(fin >> eu[i] >> ev[i] >> ew[i])) { fail(tc, "读边失败"); return; }
        if (eu[i] < 1 || eu[i] > N) fail(tc, "边 u 越界");
        if (ev[i] < 1 || ev[i] > N) fail(tc, "边 v 越界");
        if (eu[i] == ev[i]) has_self = true;
        if (ew[i] > WMAX || ew[i] < -WMAX) has_w_overflow = true;
        if (eu[i] != ev[i]) {
            int a = min(eu[i], ev[i]), b = max(eu[i], ev[i]);
            // ★ 编码基数用 20001，兼容 2e4 规模
            keys.push_back((long long)a * 20001LL + b);
        }
    }
    if (has_self)       fail(tc, "有自环");
    if (has_w_overflow) fail(tc, "|w| > 1e9");

    {
        vector<long long> tmp = keys;
        sort(tmp.begin(), tmp.end());
        for (size_t i = 1; i < tmp.size(); i++)
            if (tmp[i] == tmp[i-1]) { fail(tc, "有重边"); break; }
    }

    int n_mod = 0, n_qry = 0;
    for (int t = 1; t <= Q; t++) {
        int type;
        if (!(fin >> type)) { fail(tc, "读操作失败"); return; }
        if (type == 1) {
            long long e, w;
            if (!(fin >> e >> w)) { fail(tc, "读 set 失败"); return; }
            if (e < 1 || e > M) fail(tc, "e 越界");
            if (w > WMAX || w < -WMAX) fail(tc, "|w| > 1e9");
            n_mod++;
        } else if (type == 2) {
            long long u, v;
            if (!(fin >> u >> v)) { fail(tc, "读 query 失败"); return; }
            if (u < 1 || u > N) fail(tc, "u 越界");
            if (v < 1 || v > N) fail(tc, "v 越界");
            n_qry++;
        } else {
            fail(tc, "未知操作类型");
            return;
        }
    }

    string extra;
    if (fin >> extra) warn(tc, "文件末尾有多余内容");

    if (info.special == 1) {
        if (n_mod != 0) fail(tc, "要求无修改，实际有 " + to_string(n_mod) + " 次修改");
    } else if (info.special == 2) {
        if (m != n - 1) fail(tc, "要求树，但 m != n-1");
        else {
            vector<int> fa(N + 1);
            for (int i = 0; i <= N; i++) fa[i] = i;
            bool cyc = false;
            for (int i = 1; i <= M; i++) {
                int a = dsu_find(fa, eu[i]);
                int b = dsu_find(fa, ev[i]);
                if (a == b) { cyc = true; break; }
                fa[a] = b;
            }
            if (cyc) fail(tc, "树上有环");
            else {
                int root = dsu_find(fa, 1);
                for (int i = 2; i <= N; i++)
                    if (dsu_find(fa, i) != root) { fail(tc, "树不连通"); break; }
            }
        }
    } else if (info.special == 3) {
        if (n_mod > 10) fail(tc, "要求修改<=10，实际 " + to_string(n_mod));
    }

    long long wmin = LLONG_MAX, wmax = LLONG_MIN;
    long long neg = 0, pos = 0, zero = 0;
    for (int i = 1; i <= M; i++) {
        wmin = min(wmin, ew[i]); wmax = max(wmax, ew[i]);
        if (ew[i] < 0) neg++;
        else if (ew[i] > 0) pos++;
        else zero++;
    }

    fprintf(stderr, "[ OK ] city%d.in: n=%lld m=%lld q=%lld mod=%d qry=%d w[%lld,%lld] n/z/p=%lld/%lld/%lld | %s\n",
            tc, n, m, q, n_mod, n_qry, wmin, wmax, neg, zero, pos, info.desc.c_str());
}

int main() {
    for (auto& info : tests) check_one(info);
    fprintf(stderr, "\n=========================\n");
    fprintf(stderr, "Total files: %d\n", (int)tests.size());
    fprintf(stderr, "Failures: %d\n", failures);
    fprintf(stderr, "Warnings: %d\n", warnings);
    if (failures == 0) fprintf(stderr, "All tests PASSED.\n");
    else               fprintf(stderr, "Some tests FAILED.\n");
    return failures == 0 ? 0 : 1;
}