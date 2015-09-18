#include <stdlib.h>
#include <string.h>

#include "todo.h"

int main(int argc, char *argv[])
{	
	/* Create todo list */
	Todo list;
	size_t size = todo_init(&list, "todo.txt");
	
	/* Load existing tasks */
	void *tasks = malloc(size);
	get_tasks(&list, tasks);

	/* Parse and process arguments */
	if (argc == 2)
		add_task(&list, argv[1]);
	else if (argc == 3) {
		if (strcmp(argv[1], "-r") == 0)
			remove_task(&list, atoi(argv[2]));
	}
	else if (argc == 4)
		if (strcmp(argv[1], "-e") == 0)
			edit_task(&list, atoi(argv[2]), argv[3]);
		else if (strcmp(argv[1], "-m") == 0)
			move_task(&list, atoi(argv[2]), atoi(argv[3]));

	/* Print tasks and cleanup */
	print_tasks(&list);
	free(tasks);
	fclose(list.file);

    return 0;
}
