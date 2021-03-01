
#include <stdint.h>
#include "game.h"

Score scores[MAX_SCORES];

Score init_Score(char * name, int score){
    int i;
    Score s;
    for(i = 0; i < 3; i++)
        s.name[i] = name[i];
    s.name[3] = 0;
    s.score = score;
    return s;
}
void _add_Score(Score s, int i_from){
    int i;
    Score tmp;
    for(i = i_from; i < MAX_SCORES; i++){
        tmp = scores[i]; // tmp = the value to be replaced & moved
        if(s.score < tmp.score) continue;
        scores[i] = s;
        if(tmp.name[0]) // if tmp is a non-null entry, move it further down
            _add_Score(tmp, i+1);
        break;
    }
}
void add_Score(Score s){_add_Score(s,0);}
int get_scores_len(){
    int i, c = 0;
    for(i = 0; i < MAX_SCORES; i++)
        if(scores[i].name[0])
            c++;
    return c;
}
char * get_scores_page(){
    static char str[MAX_SCORES * 14];
    char * cp;
    int i, ro = 0;
    uint8_t r, p;
    Score s;
    for(i = 0; i < sizeof(str)-1; i++)
        str[i] = ' ';
    for(r = 0; r < MAX_SCORES; r++){
        s = scores[r];
        if(s.score == 0 && s.name[0] == 0)break;
        // Row format = "pp. NNN sssss\n"
        // insert scoreboard position
        p = r + 1;
        if(p < 10) str[ro+1] = p + '0';
        else insert(itoaconv(p), str, ro, 0);
        str[ro+2] = '.';
        insert(s.name, str, ro+4, 0); // insert name
        insert(itoaconv(s.score), str, ro+8, 0); // insert score
        str[ro+13] = '\n';
        ro += 14;
    }
    str[ro] = 0;
    return str;
}
