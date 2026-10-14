struct HeavyLightDecomposition {
    vector<vector<int>> g;
    vector<int> head, tin, tout, depth, par, et;

    HeavyLightDecomposition(int n_): g(n_),
        head(n_), tin(n_), tout(n_), depth(n_), par(n_), et(n_) {}

    void add_edge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    int dfs_sz(int u, int p, int d) {
        depth[u] = d;
        int sz_u = 1;
        int max_sz_ch = 0;
        for (int &v : g[u]) if (v != p) {
            par[v] = u;
            int sz_v = dfs_sz(v, u, d+1);
            sz_u += sz_v;
            if (max_sz_ch < sz_v) {
                max_sz_ch = sz_v;
                swap(g[u].front(), v);
            }
        }
        return sz_u;
    }

    void dfs_hld(int u, int p, int &k) {
        et[k] = u;
        tin[u] = k++;
        for (int v : g[u]) if (v != p) {
            head[v] = (v == g[u].front() ? head[u] : v);
            dfs_hld(v, u, k);
        }
        tout[u] = k;
    }

    void build() {
        dfs_sz(0, -1, 0);
        int k = 0;
        /* head[0] = 0; */
        dfs_hld(0, -1, k);
    }

    int lca(int u, int v) const {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) v = par[head[v]];
            else u = par[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }

    int distance(int u, int v) const {
        // return depth[u] + depth[v] - 2 * depth[lca(u, v)];
        int ret = 0;
        while (head[u] != head[v]) {
            if (tin[u] > tin[v]) swap(u, v);
            ret += tin[v] - tin[head[v]] + 1;
            v = par[head[v]];
        }
        ret += abs(depth[u] - depth[v]);
        return ret;
    }

    int la(int v, int d) const {
        while (v != -1) {
            int u = head[v];
            if(tin[v] - d >= tin[u]) {
                v = et[tin[v] - d];
                break;
            }
            d -= tin[v] - tin[u] + 1;
            v = (u == 0 ? -1 : par[u]);
        }
        return v;
    }

    // u から v 方向に d 回移動
    // d > distance(u,v) なら -1
    int jump(int u, int v, int d) const {
        int u_ = u, v_ = v;
        int du = 0, dv = 0;
        while (head[u_] != head[v_]) {
            if (tin[u_] > tin[v_]) {
                du += tin[u_] - tin[head[u_]] + 1;
                u_ = head[u_];
            }
            else {
                dv += tin[v_] - tin[head[v_]] + 1;
                v_ = head[v_];
            }
        }
        if (tin[u_] > tin[v_]) du += tin[u_] - tin[v_];
        else dv += tin[v_] - tin[u_];
        if (d <= du) return la(u, d);
        d -= du;
        if (d <= dv) return la(v, dv - d);
        return -1;
    }

    /*
     * path (u to v) -> path_u, path_v (closed intervals)
     * path_u: u to lca(u,v)
     * path_v: lca(u,v) to v
     * path (u to v) = path_u ++ path_v
     * どちらも 区間 [l,r] は l < r の順 (深さ l < r)
     * path_u では euler tour の逆方向に計算する <= 非可換の時注意
     * path_v では euler tour の順方向に計算する
     * path_u では u to lca(u,v) の順に計算
     * path_v では lca(u,v) to v の順に計算 <= 非可換の時注意
     * exclude_lca = true の時 path に lca は含まれない <= 辺に重みがある時に使う
     * path_u = {lui, rui}_i, path_v = {lvi, rvi}_i
     * e() op rev[lu0, ru0] op ... op rev[lu1, ru1]
     * op [lv0, rv0] op ... op [lv1, rv1]
    */
    pair<vector<pair<int, int>>, vector<pair<int, int>>>
    get_path(int u, int v, bool exclude_lca = false) const {
        vector<pair<int, int>> path_u, path_v;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) {
                path_v.emplace_back(tin[head[v]], tin[v]);
                v = par[head[v]];
            }
            else {
                path_u.emplace_back(tin[head[u]], tin[u]);
                u = par[head[u]];
            }
        }
        if (depth[u] < depth[v]) path_v.emplace_back(tin[u] + exclude_lca, tin[v]);
        else path_u.emplace_back(tin[v] + exclude_lca, tin[u]);
        reverse(path_v.begin(), path_v.end());
        return {path_u, path_v};
    }

    /* subtree (u) -> [tin, tout) */
    pair<int, int> get_subtree(int u) const {
        return {tin[u], tout[u]};
    }
};

/*
 * 辺に重みが付く時 (ui, vi) -> wi
 * 深いほうの頂点に wi をつける
 * etov[i] := depth[ui] < depth[vi] ? vi : ui
 * query path (u, v) -> get_path(u, v, exclude_lca = true)
*/

