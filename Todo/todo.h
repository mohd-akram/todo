#include <stdio.h>

#define MAXLINE 1024

typedef struct todo {
	int length;
	long int start_pos;
	const char *filename;
	FILE *file;
	char (*tasks)[MAXLINE];
} Todo;

void todo_init(Todo *list, const char *filename);
void get_tasks(Todo *list, char tasks[][MAXLINE]);
void add_task(Todo *list, char *task);
void remove_task(Todo *list, int task_no);
void print_tasks(Todo *list);
