#include <bits/stdc++.h>
using namespace std;

using ll = long long;

int n, m, q;

vector<int> eu, ev;
vector<ll> cw_init;

struct Op {
    int type;
    int e, u, v;
    ll w;
};

vector<Op> ops;

/* ============================================================
   一般图部分：check_w + solve_segment
   ============================================================ */

bool check_w(
    int u, int v,
    ll W,
    vector<int>& fa,
    vector<int>& sz,
    const vector<ll>& cw
) {
    if (u == v)
        return true;

    iota(fa.begin(), fa.end(), 0);
    fill(sz.begin(), sz.end(), 1);

    auto find = [&](int x) {
        while (fa[x] != x) {
            fa[x] = fa[fa[x]];
            x = fa[x];
        }
        return x;
    };

    auto unite = [&](int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return;
        if (sz[a] < sz[b]) swap(a, b);
        fa[b] = a;
        sz[a] += sz[b];
    };

    // 加入所有 <= W 的轻边（排除 u-v 直连边）
    for (int i = 1; i <= m; ++i) {
        if (cw[i] > W) continue;
        if ((eu[i] == u && ev[i] == v) ||
            (eu[i] == v && ev[i] == u))
            continue;
        unite(eu[i], ev[i]);
    }

    if (find(u) == find(v))
        return true;

    int Cu = find(u);
    int Cv = find(v);

    // 允许至多一条 > W 的边连接两侧
    for (int i = 1; i <= m; ++i) {
        if (cw[i] <= W) continue;
        if ((eu[i] == u && ev[i] == v) ||
            (eu[i] == v && ev[i] == u))
            continue;
        int a = find(eu[i]);
        int b = find(ev[i]);
        if ((a == Cu && b == Cv) || (a == Cv && b == Cu))
            return true;
    }

    return false;
}

void solve_segment(
    int L, int R,
    const vector<ll>& cw,
    vector<ll>& ans
) {
    if (L > R) return;

    vector<int> qid;
    for (int i = L; i <= R; ++i)
        if (ops[i].type == 2)
            qid.push_back(i);

    if (qid.empty()) return;

    vector<int> real_q;
    for (int id : qid) {
        if (ops[id].u == ops[id].v)
            ans[id] = 0;
        else
            real_q.push_back(id);
    }

    if (real_q.empty()) return;

    vector<ll> ws;
    for (int i = 1; i <= m; ++i)
        ws.push_back(cw[i]);
    sort(ws.begin(), ws.end());
    ws.erase(unique(ws.begin(), ws.end()), ws.end());

    vector<int> fa(n + 1);
    vector<int> sz(n + 1);

    vector<int> valid;
    valid.reserve(real_q.size());

    for (int id : real_q) {
        if (check_w(ops[id].u, ops[id].v, ws.back(), fa, sz, cw))
            valid.push_back(id);
    }

    if (valid.empty()) return;

    function<void(int,int,const vector<int>&)> solve =
        [&](int l, int r, const vector<int>& ids) {
        if (ids.empty()) return;
        if (l == r) {
            for (int id : ids) ans[id] = ws[l];
            return;
        }
        int mid = (l + r) >> 1;
        vector<int> A, B;
        A.reserve(ids.size());
        B.reserve(ids.size());
        ll W = ws[mid];
        for (int id : ids) {
            if (check_w(ops[id].u, ops[id].v, W, fa, sz, cw))
                A.push_back(id);
            else
                B.push_back(id);
        }
        solve(l, mid, A);
        solve(mid + 1, r, B);
    };

    solve(0, (int)ws.size() - 1, valid);
}

/* ============================================================
   树上部分：HLD（非递归 dfs1 / dfs2，避免栈溢出）
   ============================================================ */

struct HLD {
    int N;
    vector<vector<pair<int,int>>> adj;
    vector<int> par, depth, sz, heavy;
    vector<int> head, pos, eid_at_pos;
    vector<pair<ll,ll>> seg;
    int cur_pos;

    void init(int n) {
        N = n;
        adj.assign(n + 1, {});
    }

    void add_edge(int u, int v, int eid) {
        adj[u].push_back({v, eid});
        adj[v].push_back({u, eid});
    }

    // 非递归 dfs1：迭代求 order，反向累加 sz 与 heavy
    void dfs1(int root) {
        vector<int> order;
        order.reserve(N);
        par.assign(N + 1, 0);
        depth.assign(N + 1, 0);
        par[root] = 0;
        depth[root] = 0;

        vector<int> stk = {root};
        while (!stk.empty()) {
            int u = stk.back(); stk.pop_back();
            order.push_back(u);
            for (auto [v, eid] : adj[u]) {
                if (v == par[u]) continue;
                par[v] = u;
                depth[v] = depth[u] + 1;
                stk.push_back(v);
            }
        }

        sz.assign(N + 1, 1);
        heavy.assign(N + 1, -1);
        for (int i = (int)order.size() - 1; i >= 0; --i) {
            int u = order[i];
            int best = -1, bestsz = 0;
            for (auto [v, eid] : adj[u]) {
                if (v == par[u]) continue;
                sz[u] += sz[v];
                if (sz[v] > bestsz) { bestsz = sz[v]; best = v; }
            }
            heavy[u] = best;
        }
    }

    // 非递归 dfs2：手动栈模拟
    void dfs2(int root) {
        head.assign(N + 1, 0);
        pos.assign(N + 1, 0);
        eid_at_pos.assign(N, -1);
        cur_pos = 0;

        // (u, h, parent_eid)
        vector<tuple<int,int,int>> stk;
        stk.push_back({root, root, -1});

        while (!stk.empty()) {
            auto [u, h, pe] = stk.back();
            stk.pop_back();

            head[u] = h;
            pos[u] = cur_pos++;
            eid_at_pos[pos[u]] = pe;

            // 先压轻儿子（后进先出，所以先压轻）
            for (auto [v, eid] : adj[u]) {
                if (v == par[u] || v == heavy[u]) continue;
                stk.push_back({v, v, eid});
            }
            // 再压重儿子（最后处理，即最先弹出）
            if (heavy[u] != -1) {
                int v = heavy[u];
                int eid = -1;
                for (auto [to, ieid] : adj[u]) {
                    if (to == v) { eid = ieid; break; }
                }
                stk.push_back({v, h, eid});
            }
        }
    }

    pair<ll,ll> merge_info(pair<ll,ll> A, pair<ll,ll> B) {
        ll x[4];
        int cnt = 0;
        if (A.first  >= 0) x[cnt++] = A.first;
        if (A.second >= 0) x[cnt++] = A.second;
        if (B.first  >= 0) x[cnt++] = B.first;
        if (B.second >= 0) x[cnt++] = B.second;
        if (cnt == 0) return {-1, -1};
        sort(x, x + cnt, greater<ll>());
        return {x[0], cnt >= 2 ? x[1] : -1};
    }

    void build_seg(int node, int l, int r, const vector<ll>& cw) {
        if (l == r) {
            int eid = eid_at_pos[l];
            if (eid == -1) seg[node] = {-1, -1};
            else           seg[node] = {cw[eid], -1};
            return;
        }
        int mid = (l + r) >> 1;
        build_seg(node << 1,     l,     mid, cw);
        build_seg(node << 1 | 1, mid + 1, r, cw);
        seg[node] = merge_info(seg[node << 1], seg[node << 1 | 1]);
    }

    void update(int node, int l, int r, int p, ll v) {
        if (l == r) { seg[node] = {v, -1}; return; }
        int mid = (l + r) >> 1;
        if (p <= mid) update(node << 1, l, mid, p, v);
        else          update(node << 1 | 1, mid + 1, r, p, v);
        seg[node] = merge_info(seg[node << 1], seg[node << 1 | 1]);
    }

    pair<ll,ll> query_seg(int node, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) return {-1, -1};
        if (ql <= l && r <= qr) return seg[node];
        int mid = (l + r) >> 1;
        return merge_info(
            query_seg(node << 1,     l,     mid, ql, qr),
            query_seg(node << 1 | 1, mid + 1, r, ql, qr)
        );
    }

    void build(int root, const vector<ll>& cw) {
        dfs1(root);
        dfs2(root);
        seg.assign(4 * N + 5, {-1, -1});
        build_seg(1, 0, cur_pos - 1, cw);
    }

    void update_edge(int eid, ll w,
                     const vector<int>& eu,
                     const vector<int>& ev) {
        int u = eu[eid], v = ev[eid];
        if (depth[u] < depth[v]) swap(u, v);
        update(1, 0, cur_pos - 1, pos[u], w);
    }

    // 返回 u-v 路径第二大边权；只有一条边时返回 -1
    ll query_path(int u, int v) {
        pair<ll,ll> res = {-1, -1};
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            res = merge_info(res,
                query_seg(1, 0, cur_pos - 1, pos[head[u]], pos[u]));
            u = par[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        if (pos[u] + 1 <= pos[v])
            res = merge_info(res,
                query_seg(1, 0, cur_pos - 1, pos[u] + 1, pos[v]));
        return res.second;
    }
};

/* ============================================================
   判断是否真正的树
   ============================================================ */
bool is_real_tree() {
    if (m != n - 1) return false;

    vector<int> fa(n + 1);
    iota(fa.begin(), fa.end(), 0);

    // 非递归 find
    auto find = [&](int x) {
        int r = x;
        while (fa[r] != r) r = fa[r];
        while (fa[x] != r) { int nxt = fa[x]; fa[x] = r; x = nxt; }
        return r;
    };

    for (int i = 1; i <= m; ++i) {
        int a = find(eu[i]);
        int b = find(ev[i]);
        if (a == b) return false;
        fa[a] = b;
    }

    int rt = find(1);
    for (int i = 2; i <= n; ++i)
        if (find(i) != rt) return false;

    return true;
}

/* ============================================================
   main
   ============================================================ */
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> n >> m >> q;

    eu.resize(m + 1);
    ev.resize(m + 1);
    cw_init.resize(m + 1);

    for (int i = 1; i <= m; ++i) {
        cin >> eu[i] >> ev[i] >> cw_init[i];
        cw_init[i] = llabs(cw_init[i]);
    }

    ops.resize(q);
    for (int i = 0; i < q; ++i) {
        cin >> ops[i].type;
        if (ops[i].type == 1) {
            cin >> ops[i].e >> ops[i].w;
            ops[i].w = llabs(ops[i].w);
        } else {
            cin >> ops[i].u >> ops[i].v;
        }
    }

    vector<ll> ans(q, LLONG_MAX);
    vector<ll> cw = cw_init;

    bool is_tree = is_real_tree();

    if (is_tree) {
        // ===== 树：HLD =====
        HLD hld;
        hld.init(n);
        for (int i = 1; i <= m; ++i)
            hld.add_edge(eu[i], ev[i], i);
        hld.build(1, cw);

        for (int i = 0; i < q; ++i) {
            if (ops[i].type == 1) {
                cw[ops[i].e] = ops[i].w;
                hld.update_edge(ops[i].e, ops[i].w, eu, ev);
            } else {
                int u = ops[i].u, v = ops[i].v;
                if (u == v) { ans[i] = 0; continue; }
                ll res = hld.query_path(u, v);
                if (res >= 0) ans[i] = res;
            }
        }
    } else {
        // ===== 一般图：分段 + 二分 =====
        int start = 0;
        for (int t = 0; t < q; ++t) {
            if (ops[t].type != 1) continue;
            solve_segment(start, t - 1, cw, ans);
            cw[ops[t].e] = ops[t].w;
            start = t + 1;
        }
        solve_segment(start, q - 1, cw, ans);
    }

    for (int i = 0; i < q; ++i) {
        if (ops[i].type != 2) continue;
        if (ans[i] == LLONG_MAX)
            cout << -1 << '\n';
        else
            cout << ans[i] << '\n';
    }

    return 0;
}