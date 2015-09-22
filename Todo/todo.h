#include <stdio.h>

#define MAXLEN 255

typedef struct todo {
	int length;
	const char *filename;
	FILE *file;
	char header[MAXLEN];
	struct task {
		char mark;
		char text[MAXLEN];
	} *tasks;
} Todo;

size_t todo_init(Todo *list, const char *filename);
void get_tasks(Todo *list, void *tasks);
void add_task(Todo *list, char *task);
void edit_task(Todo *list, int task_no, char *task);
void mark_task(Todo *list, int task_no);
void move_task(Todo *list, int from, int to);
void remove_task(Todo *list, int task_no);
void print_tasks(Todo *list);
