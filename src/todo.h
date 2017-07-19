#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#define MAXTEXT 254

typedef struct todo {
	const char *filename;
	FILE *file;
	char name[60];
	int length;
	struct task {
		bool space;
		char mark;
		char text[MAXTEXT];
	} *tasks;
} Todo;

rsize_t todo_init(Todo *list, const char *filename);
void load_tasks(Todo *list, void *tasks);
rsize_t add_task(Todo *list, char *task);
rsize_t edit_task(Todo *list, int task_no, char *task);
rsize_t mark_task(Todo *list, int task_no);
rsize_t move_task(Todo *list, int from, int to);
rsize_t remove_task(Todo *list, int task_no);
rsize_t space_task(Todo *list, int task_no);
void print_tasks(Todo *list);
