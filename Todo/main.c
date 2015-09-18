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
	else if (argc > 2)
		for (int i = 1; i < argc - 1; i++)
			if (strcmp(argv[i], "-r") == 0)
				remove_task(&list, atoi(argv[i+1]));

	/* Print tasks and cleanup */
	print_tasks(&list);
	free(tasks);
	fclose(list.file);

    return 0;
}
