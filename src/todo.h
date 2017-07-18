#include <stdio.h>
#include <stdbool.h>

#define MAXLEN 255

typedef struct todo {
	const char *filename;
	FILE *file;
	char name[MAXLEN];
	int length;
	struct task {
		bool space;
		char mark;
		char text[MAXLEN];
	} *tasks;
} Todo;

size_t todo_init(Todo *list, const char *filename);
void load_tasks(Todo *list, void *tasks);
size_t add_task(Todo *list, char *task);
size_t edit_task(Todo *list, int task_no, char *task);
size_t mark_task(Todo *list, int task_no);
size_t move_task(Todo *list, int from, int to);
size_t remove_task(Todo *list, int task_no);
size_t space_task(Todo *list, int task_no);
void print_tasks(Todo *list);
