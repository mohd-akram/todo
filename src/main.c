#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "todo.h"

#define TODO_ENV "TODO_DIR"
#define TODO_FILE "todo.md"

bool is_option(char *arg);
char get_option(char *arg);
bool get_task_no(char *arg, int *task_no);

int main(int argc, char *argv[])
{
	/* Check if a todo file exists in the current directory */
	FILE *file = fopen(TODO_FILE, "r");
	if (file != NULL) fclose(file);

	/* Get todo filename */
	char *dir = getenv(TODO_ENV);
	size_t len = dir == NULL ? 0 : strlen(dir) + 1;
	len += strlen(TODO_FILE) + 1;
	char *filename = malloc(len * sizeof *filename);
	filename[0] = '\0';
	if (file == NULL && dir != NULL)
		(void) strcat(filename, dir), strcat(filename, "/");
	strcat(filename, TODO_FILE);

	/* Create todo list */
	Todo list;
	rsize_t size = todo_init(&list, filename);

	/* Load existing tasks */
	void *tasks = malloc(size);
	load_tasks(&list, tasks);
	int count = list.length;

	/* Parse and process arguments */
	bool error = false;
	bool help = false;
	bool show = false;

	int task_no;

	bool is_add = argc > 1 && !is_option(argv[1]);
	bool is_edit = argc > 3 && get_option(argv[1]) == 'e';

	if (argc == 1)
		show = true;
	else if (argc == 2 && get_task_no(argv[1], &task_no))
		size = mark_task(&list, task_no);
	else if (argc == 3 && get_option(argv[1]) == 'r')
		size = remove_task(&list, atoi(argv[2]));
	else if (argc == 3 && get_option(argv[1]) == 's')
		size = space_task(&list, atoi(argv[2]));
	else if (argc == 4 && get_option(argv[1]) == 'm')
		size = move_task(&list, atoi(argv[2]), atoi(argv[3]));
	else if (is_add || is_edit) {
		/* Concatenate arguments when adding or editing a task */
		bool has_num = get_task_no(argv[1], &task_no);
		int start = is_edit ? 3 : has_num ? 2 : 1;

		size_t total_len = 0;
		for (int i = start; i < argc; i++)
			total_len += strlen(argv[i]) + 1;

		char *task = malloc(total_len);
		task[0] = '\0';
		for (int i = start; i < argc; i++) {
			strcat(task, argv[i]);
			if (i < argc - 1)
				strcat(task, " ");
		}

		if (is_edit)
			size = edit_task(&list, atoi(argv[2]), task);
		else if ((size = add_task(&list, task)) && size <= RSIZE_MAX) {
			tasks = realloc(tasks, size);
			if (has_num) {
				load_tasks(&list, tasks);
				size = move_task(&list, list.length, task_no);
				if (size == 0)
					remove_task(&list, list.length);
			}
		}
		free(task);
	} else {
		help = true;
		error = !(argc == 2 && get_option(argv[1]) == 'h');
	}

	/* Reload tasks after changes */
	load_tasks(&list, tasks);

	/* Print output */
	if (size == 0 && count == list.length && !show) {
		fprintf(stderr, "%s: invalid arguments\n", argv[0]);
		error = true;
	} else if (size > RSIZE_MAX) {
		fprintf(stderr, "%s: failed to open file %s\n", argv[0],
			filename);
		error = true;
	} else if (help)
		fprintf(error ? stderr : stdout,
			"usage: %s [-h]\n"
			"       %s [[-e] num] task ...\n"
			"       %s [-r|-s|-m pos] num\n",
			argv[0], argv[0], argv[0]);
	else
		print_tasks(&list);

	/* Cleanup */
	free(tasks);
	if (list.file != NULL)
		fclose(list.file);
	free(filename);

	return error ? EXIT_FAILURE : EXIT_SUCCESS;
}

bool is_option(char *arg)
{
	return strlen(arg) == 2 && arg[0] == '-' && isalpha(arg[1]);
}

char get_option(char *arg)
{
	return is_option(arg) ? arg[1] : 0;
}

bool get_task_no(char *arg, int *task_no)
{
	char *end;
	long num = strtol(arg, &end, 10);
	if (*end == '\0')
		return (void) (*task_no = (int) num), true;
	return false;
}
