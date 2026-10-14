int r = 0;
for(int l = 0; l < N; l++){
    while(r < N and /* r を加えてもok */true){
        /* r を加える */
        r++;
    }

    /* 現在の左端 l に対して r は条件を満たす最大 */
    /* 答えへの寄与を計算 */

    if(l == r) r++;
    else{
        /* l を除く */
        /* l++; はしない */
    }
}

