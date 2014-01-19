#include <stdio.h>
#include <ctype.h>

#include "todo.h"

static
void write_header(Todo *list, int num)
{
	if (list->file == NULL)
		fopen_s(&list->file, list->filename, "w+");
	else
		freopen_s(&list->file, list->filename, "w+", list->file);

	fprintf(list->file,
			"Todo\n"
			"----\n\n"
			"%d task%s\n\n", num, num != 1 ? "s" : "");
}

static
long int find_digit(FILE *file)
{
	int c;

	while ((c = getc(file)) != EOF && !isdigit(c))
		;
	ungetc(c, file);

	return ftell(file);
}

static
void fprint_task(Todo *list, int task_no, char *task)
{
	fprintf(list->file, "%2d. %s\n", task_no, task);
}

static
void seek_tasks(Todo *list)
{
	fseek(list->file, list->start_pos, SEEK_SET);
}

void todo_init(Todo *list, const char *filename)
{	
	list->filename = filename;
	fopen_s(&list->file, filename, "r+");

	if (list->file == NULL) {
		write_header(list, 0);
		rewind(list->file);
	}

	/* Seek to first digit in file to get the number of tasks */
	find_digit(list->file);

	/* Update the number of tasks */
	fscanf_s(list->file, "%d", &(list->length));

	/* Find the first numbered task */
	list->start_pos = find_digit(list->file);
}

void get_tasks(Todo *list, char tasks[][MAXLINE])
{
	seek_tasks(list);
	for (int i = 0; i < list->length; i++)
		fscanf_s(list->file, "%*d. %[^\n]", tasks[i], sizeof tasks[i]);

	list->tasks = tasks;
}

void add_task(Todo *list, char *task)
{
	write_header(list, list->length + 1);

	for (int i = 0; i < list->length; i++)
		fprint_task(list, i+1, list->tasks[i]);

	/* Add new task */
	fprint_task(list, list->length + 1, task);
}

void remove_task(Todo *list, int task_no)
{
	/* Task numbering starts from 1 */
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
