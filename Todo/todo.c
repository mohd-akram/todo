#include <stdio.h>
#include <ctype.h>

#include "todo.h"

static
void write_header(Todo *list, int n)
{
	if (list->file == NULL)
		fopen_s(&list->file, list->filename, "w+");
	else
		freopen_s(&list->file, list->filename, "w+", list->file);

	fprintf(list->file,
			"Todo\n"
			"----\n\n"
			"%d task%s\n\n", n, n != 1 ? "s" : "");
}

static
int find_tasks(FILE *file, char tasks[][MAXLINE])
{
	int i = 0;

	rewind(file);
	while (!feof(file)) {
		/* Check if line contains a todo item */
		int res, x;
		res = tasks == NULL ?
			fscanf_s(file, "%d. %c", &x, &x) :
			fscanf_s(file, "%d. %[^\n]", &x,
				tasks[i], sizeof tasks[i]);
		if (res == 2)
			++i;

		/* Go to end of line */
		int c;
		while ((c = getc(file)) != EOF && c != '\n')
			;
	}

	return i;
}

static
void fprint_task(Todo *list, int task_no, char *task)
{
	fprintf(list->file, "%2d. %s\n", task_no, task);
}

size_t todo_init(Todo *list, const char *filename)
{	
	list->filename = filename;

	fopen_s(&list->file, filename, "r");

	/* Create a new file if it doesn't exist */
	if (list->file == NULL)
		write_header(list, 0);

	return find_tasks(list->file, NULL) * MAXLINE;
}

void get_tasks(Todo *list, void *tasks)
{
	list->tasks = (char (*)[MAXLINE]) tasks;
	list->length = find_tasks(list->file, list->tasks);

	/* Print formatted tasks to file */
	write_header(list, list->length);

	for (int i = 0; i < list->length; i++)
		fprint_task(list, i+1, list->tasks[i]);
}

void add_task(Todo *list, char *task)
{
	write_header(list, list->length + 1);

	for (int i = 0; i < list->length; i++)
		fprint_task(list, i+1, list->tasks[i]);

	/* Add new task */
	fprint_task(list, list->length + 1, task);
}

void edit_task(Todo *list, int task_no, char *task)
{
	if (task_no < 1 || task_no > list->length)
		return;

	write_header(list, list->length);

	for (int i = 0; i < list->length; i++)
		fprint_task(list, i+1,
			i+1 == task_no ? task : list->tasks[i]);
}

void move_task(Todo *list, int from, int to)
{
	if (from < 1 || from > list->length || to < 1 || to > list->length)
		return;

	write_header(list, list->length);

	int len = list->length;
	for (int i = 0, num = 1; i < len; i++, num++) {
		if (i == from - 1) ++i, ++len;
		if (num == to) --i, --len;
		fprint_task(list, num, list->tasks[num == to ? from - 1 : i]);
	}
}

void remove_task(Todo *list, int task_no)
{
	if (task_no < 1 || task_no > list->length)
		return;

	write_header(list, list->length - 1);

	for (int i = 0, num = 1; i < list->length; i++)
		if (i+1 != task_no)
			fprint_task(list, num++, list->tasks[i]);
}

void print_tasks(Todo *list)
{
	int c;

	rewind(list->file);
	while ((c = getc(list->file)) != EOF)
		putchar(c);
}
