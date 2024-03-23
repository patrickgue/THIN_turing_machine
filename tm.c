#include <stdlib.h>
#include <string.h>
#include "tm.h"

int main(int argc, char **argv)
{
    struct s_language lang;
    struct s_state state;

    if (argc < 2)
        die("usage: ./tm <tm description file>");
    
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
        die("unable to read file");
    read_language(f, &lang, &state);

    print_tape(&state, &lang);

    while (getchar() != 'q')
    {
        step(&lang, &state);
        print_tape(&state, &lang);
    }
    
    fclose(f);
    return 0;
}

void step(struct s_language *lang, struct s_state *s)
{
    int i, t = read_tape(s);
    for (i = 0; i < lang->transitions_count; i++)
    {
        if (lang->transitions[i].q == s->q && lang->transitions[i].r == t)
        {
            printf("Pos: %d Q: %c -> %c   Read: %c Write: %c Go: %s\n",
                   s->position,
                   lang->q[lang->transitions[i].q],
                   lang->q[lang->transitions[i].t],
                   lang->gama[t],
                   lang->gama[lang->transitions[i].w],
                   lang->transitions[i].dir == LEFT ? "LEFT" : "RIGHT");
            write_tape(s, lang->transitions[i].w);
            s->q = lang->transitions[i].t;
            move_tape(s, lang->transitions[i].dir);
            
            return;
        }
    }
    fprintf(stderr, "No transition found (q to quit)\n");
}

void print_tape(struct s_state *s, struct s_language *l)
{
    int i;

    for (i = 0; i < 20; i++)
    {
        printf("%2i", s->position - 10 + i);
    }
    printf(" q=%c\n", l->q[s->q]);
    
    for (i = 0; i < 20; i++)
    {
        printf("|%c", l->gama[s->tape[(TAPE_SIZE/2) + s->position - 10 + i]]);
    }
    
    printf("\n ");
    for (i = 0; i < 10; i++)
    {
        printf("  ");
    }
    printf("^\n");
}

int read_tape(struct s_state *s)
{
    return s->tape[s->position + (TAPE_SIZE/2)];
}

void write_tape(struct s_state *s, int w)
{
    s->tape[s->position  + (TAPE_SIZE/2)] = w;
}

void move_tape(struct s_state *s, enum  e_tape_direction dir)
{
    if (dir == LEFT && s->position > 0)
    {
        s->position--;
    }
    else if (s->position + 1 < TAPE_SIZE)
    {
        s->position++;
    }
    else
    {
        die("Exceeding tape size");
    }
}

void read_language(FILE *f, struct s_language *lang, struct s_state *state)
{
    char buffer[64];
    int read_transitions = 0, i;
    lang->transitions_count = 0;
    
    while (fgets(buffer, BUFF_SIZE, f) != NULL)
    {
        if (read_transitions)
        {
            transition_add(buffer, lang);
        }
        else
        {
            if (strlen(buffer) == 0 || buffer[0] == '#')
            {
                continue;
            }
            if (buffer[0] == 'S')
            {
                lang->sigma_count = read_alphabet(buffer, lang->sigma);
            }
            else if (buffer[0] == 'G')
            {
                lang->gama_count = read_alphabet(buffer, lang->gama);
            }
            else if (buffer[0] == 'Q')
            {
                lang->q_count = read_alphabet(buffer, lang->q);
            }
            else if (buffer[0] == 'K')
            {
                config_tape(buffer, state, lang);
            }
            else if (buffer[0] == 'q')
            {
                state->q = str_index(lang->q, buffer[2], lang->q_count);
            }
            else if (strcmp(buffer, "--\n") == 0)
            {
                read_transitions = 1;
            }
        }
    }

    printf("S: %s\nG: %s\nq %s\n", lang->sigma, lang->gama, lang->q);
    for (i = 0; i < lang->transitions_count; i++)
    {
        printf("%c->%c: %c/%c -> %c\n",
               lang->q[lang->transitions[i].q],
               lang->q[lang->transitions[i].t],
               lang->gama[lang->transitions[i].r],
               lang->gama[lang->transitions[i].w],
               lang->transitions[i].dir == RIGHT ? 'R' : 'L');
    }
}


int read_alphabet(char *src, char *alph)
{
    int alph_count = 0, i;

    if (src[1] == '=')
        src += 2;
    else
        die("invalid alphabet definition");

    for (i = 0; i < strlen(src) && i < BUFF_SIZE; i++)
    {
        if (src[i] != ',' && src[i] >= ' ')
        {
            alph[alph_count++] = src[i];
        }
    }
    alph[alph_count] = 0;
    return alph_count;
}

void transition_add(char *buff, struct s_language *lang)
{
    int q,t,r,w;
    enum e_tape_direction d;
    if (strlen(buff) >= 7)
    {
        q = buff[0];
        t = buff[2];
        r = buff[4];
        w = buff[6];
        d = buff[8] == 'R' ? RIGHT : LEFT;

        lang->transitions[lang->transitions_count].q = str_index(lang->q, q, lang->q_count);
        lang->transitions[lang->transitions_count].t = str_index(lang->q, t, lang->q_count);
        lang->transitions[lang->transitions_count].r = str_index(lang->gama, r, lang->gama_count);
        lang->transitions[lang->transitions_count].w = str_index(lang->gama, w, lang->gama_count);
        lang->transitions[lang->transitions_count].dir = d;
        lang->transitions_count++;
    }
}

void config_tape(char *buff, struct s_state *state, struct s_language *lang)
{
    int i, pos = 0;
    buff+=2;
    for (i = 0; i < TAPE_SIZE; i++)
    {
        state->tape[i] = 0;
    }
    for (i = 0; i < strlen(buff) - 1; i++)
    {
        if (buff[i] != 'q')
        {
            state->tape[(TAPE_SIZE/2) + pos] = str_index(lang->gama, buff[i], lang->gama_count);
            pos++;
        }
        else
        {
            state->position = pos;
        }
    }
}

int str_index(char *str, int s, int max)
{
    int i;
    for (i = 0; i < max; i++)
    {
        if (str[i] == s)
            return i;
    }
    return -1;
}

void die(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
