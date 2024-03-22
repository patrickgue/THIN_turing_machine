#ifndef tm_h
#define tm_h

#include <stdio.h>

#define BUFF_SIZE 64
#define MAX_ALPH_SIZE 64
#define MAX_TRANSITION_COUNT 64
#define TAPE_SIZE 2048

enum e_tape_direction
{
    LEFT,
    RIGHT
};

struct s_state_transition
{
    int q, t, r, w;
    enum e_tape_direction dir;
};


struct s_language
{
    char sigma[MAX_ALPH_SIZE];
    int sigma_count;
    char gama[MAX_ALPH_SIZE];
    int gama_count;
    char q[MAX_ALPH_SIZE];
    int q_count;
    int q0;
    struct s_state_transition transitions[MAX_TRANSITION_COUNT];
    int transitions_count;
};

struct s_state {
    int tape[TAPE_SIZE],
        position,
        q;
};

int read_tape(struct s_state *);
void write_tape(struct s_state *, int);

void read_language(FILE *, struct s_language*, struct s_state *);
int read_alphabet(char *, char *);
void transition_add(char *, struct s_language *);
void config_tape(char *, struct s_state*, struct s_language *);
int str_index(char*,int,int);
void move_tape(struct s_state *, enum  e_tape_direction);

void step(struct s_language *, struct s_state *);
void print_tape(struct s_state *, struct s_language *);

void die(char *);

#endif /* tm_h */
