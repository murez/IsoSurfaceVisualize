#include<cmath>
#include<cstdio>

extern "C" float **jet(int m) {
    int i = 0;
    int n = int(ceil(m / 4.0));
    int u_len = n * 3 - 1;
    auto *u = new float[u_len];
    for (i = 0; i < n; i++) {
        u[u_len - i - 1] = u[i] = float(i + 1) / n;
        u[n + i] = 1;
    }

    auto **J = new float *[m];
    for (i = 0; i < m; i++) {
        J[i] = new float[3];
        for (int j = 0; j < 3; j++) {
            J[i][j] = 0;
        }
    }

    int t = int(ceil(n / 2.0) - (m % 4 == 1));
    int g_len = m - t;
    for (i = 0; i < g_len; i++) {
        J[t + i][1] = u[i];
    }
    int r_len = m - t - n;
    for (i = 0; i < r_len; i++) {
        J[t + i + n][0] = u[i];
    }
    int b_start = n - t;
    for (i = b_start; i < u_len; i++) {
        J[t + i - n][2] = u[i];
    }
    delete[] u;
    return J;
}
