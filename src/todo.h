#include <stdio.h>
#include <stdbool.h>

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

size_t todo_init(Todo *list, const char *filename);
void load_tasks(Todo *list, void *tasks);
void print_tasks(Todo *list);
int change_name(Todo *list, char *name);
int add_task(Todo *list, char *task, size_t *size);
int edit_task(Todo *list, int task_no, char *task);
int mark_task(Todo *list, int task_no);
int remove_task(Todo *list, int task_no);
int space_task(Todo *list, int task_no);
int move_task(Todo *list, int from, int to);
