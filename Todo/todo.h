#include <stdio.h>

#define MAXLINE 1024

typedef struct todo {
	int length;
	const char *filename;
	FILE *file;
	char (*tasks)[MAXLINE];
} Todo;

size_t todo_init(Todo *list, const char *filename);
void get_tasks(Todo *list, void *tasks);
void add_task(Todo *list, char *task);
void edit_task(Todo *list, int task_no, char *task);
void remove_task(Todo *list, int task_no);
void print_tasks(Todo *list);
