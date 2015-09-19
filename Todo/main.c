#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "todo.h"

bool is_option(char *arg)
{
	return strlen(arg) == 2 && arg[0] == '-';
}

char get_option(char *arg)
{
	if (is_option(arg))
		switch (arg[1]) {
		case 'e':
		case 'm':
		case 'r':
			return arg[1];
		}
	return 0;
}

int main(int argc, char *argv[])
{
	/* Create todo list */
	Todo list;
	size_t size = todo_init(&list, "todo.txt");

	/* Load existing tasks */
	void *tasks = malloc(size);
	get_tasks(&list, tasks);

	/* Parse and process arguments */
	bool processed = false;

	if (argc == 2 && !is_option(argv[1])) {
		if (argv[1][0] != '0') {
			int task_no = atoi(argv[1]);
			if (task_no)
				mark_task(&list, task_no),
				processed = true;
			else
				add_task(&list, argv[1]),
				processed = true;
		}
	}
	else if (argc == 3) {
		if (get_option(argv[1]) == 'r')
			remove_task(&list, atoi(argv[2])),
			processed = true;
	}
	else if (argc == 4) {
		if (get_option(argv[1]) == 'e')
			edit_task(&list, atoi(argv[2]), argv[3]),
			processed = true;
		else if (get_option(argv[1]) == 'm')
			move_task(&list, atoi(argv[2]), atoi(argv[3])),
			processed = true;
	}

	/* Concatenate arguments when adding or editing a task */
	bool is_add = argc > 2 && !is_option(argv[1]);
	bool is_edit = argc > 4 && get_option(argv[1]) == 'e';
	if (!processed && (is_add || is_edit)) {
		int start = is_edit ? 3 : 1;

		int total_len = 0;
		for (int i = start; i < argc; i++)
			total_len += strlen(argv[i]);

		if (total_len < MAXLINE) {
			char *task = malloc(MAXLINE);
			char *p = task;
			for (int i = start; i < argc; i++) {
				int len = strlen(argv[i]);
				strcpy_s(p, len + 1, argv[i]);
				p += len;
				if (i < argc - 1)
					*p = ' ';
				++p;
			}
			if (is_edit)
				edit_task(&list, atoi(argv[2]), task);
			else
				add_task(&list, task);
		}
	}

	/* Print tasks and cleanup */
	print_tasks(&list);
	free(tasks);
	fclose(list.file);

    return 0;
}
