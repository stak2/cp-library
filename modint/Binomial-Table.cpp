mint com[MX][MX];

void cominit() {
    com[0][0] = 1;
    for (int i = 1; i < MX; i++) {
        com[i][0] = 1;
        for (int j = 1; j < MX; j++) com[i][j] = com[i-1][j-1] + com[i-1][j];
    }
}
