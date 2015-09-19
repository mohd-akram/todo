#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "todo.h"

#define HEADER \
	"Todo\n" \
	"----\n\n"

#define MARK_NOT_DONE ' '
#define MARK_DONE 'x'

static
char next_task(FILE *file)
{
	while (!feof(file)) {
		/* Check if line contains a todo item */
		int res;
		char x;

		res = fscanf_s(file, "- [%c] ", &x, 1);
		if (res == 1 && (x == MARK_NOT_DONE || x == MARK_DONE))
			return x;

		/* Go to end of line */
		int c;
		while ((c = getc(file)) != EOF && c != '\n')
			;
	}

	return 0;
}

static
int find_tasks(FILE *file, struct task *tasks, int *done)
{
	if (done != NULL)
		*done = 0;

	int i = 0;
	char mark;

	rewind(file);
	while (mark = next_task(file)) {
		if (done != NULL && mark == MARK_DONE)
			++*done;
		if (tasks != NULL) {
			int res = fscanf_s(file, "%[^\n]",
				tasks[i].text, sizeof tasks[i].text);
			if (res != 1)
				tasks[i].text[0] = '\0';
			tasks[i].mark = mark;
		}
		++i;
	}

	return i;
}

static
void write_header(Todo *list)
{
	if (list->file == NULL)
		fopen_s(&list->file, list->filename, "w+");
	else
		freopen_s(&list->file, list->filename, "w+", list->file);

	fprintf(list->file, HEADER);
}

static
void write_task(Todo *list, char mark, char *task)
{
	fprintf(list->file, "- [%c] %s\n", mark, task);
}

size_t todo_init(Todo *list, const char *filename)
{
	list->filename = filename;

	fopen_s(&list->file, filename, "r");

	/* Create a new file if it doesn't exist */
	if (list->file == NULL)
		write_header(list);

	return find_tasks(list->file, NULL, NULL) * sizeof *list->tasks;
}

void get_tasks(Todo *list, void *tasks)
{
	list->tasks = tasks;
	list->length = find_tasks(list->file, list->tasks, NULL);

	/* Print formatted tasks to file */
	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list, list->tasks[i].mark, list->tasks[i].text);
}

void add_task(Todo *list, char *task)
{
	if (strlen(task) >= MAXLINE)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list, list->tasks[i].mark, list->tasks[i].text);

	/* Add new task */
	write_task(list, MARK_NOT_DONE, task);
}

void edit_task(Todo *list, int task_no, char *task)
{
	if (task_no < 1 || task_no > list->length || strlen(task) >= MAXLINE)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list,
			list->tasks[i].mark,
			i+1 == task_no ? task : list->tasks[i].text);
}

void mark_task(Todo *list, int task_no)
{
	if (task_no < 1 || task_no > list->length)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++) {
		char mark = list->tasks[i].mark;
		write_task(list,
			i+1 == task_no ?
			(mark == MARK_NOT_DONE ?
				MARK_DONE : MARK_NOT_DONE) : mark,
			list->tasks[i].text);
	}
}

void move_task(Todo *list, int from, int to)
{
	if (from < 1 || from > list->length || to < 1 || to > list->length)
		return;

	write_header(list);

	int len = list->length;
	for (int i = 0, num = 1; i < len; i++, num++) {
		if (i == from - 1) ++i, ++len;
		if (num == to) --i, --len;
		int idx = num == to ? from - 1 : i;
		write_task(list, list->tasks[idx].mark, list->tasks[idx].text);
	}
}

void remove_task(Todo *list, int task_no)
{
	if (task_no < 1 || task_no > list->length)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		if (i+1 != task_no)
			write_task(list,
				list->tasks[i].mark,
				list->tasks[i].text);
}

void print_tasks(Todo *list)
{
	int done;
	int count = find_tasks(list->file, NULL, &done);

	printf(HEADER);
	printf("%d/%d task%s\n\n", done, count, count == 1 ? "" : "s");

	char mark;
	int num = 1;

	rewind(list->file);
	while (mark = next_task(list->file)) {
		printf("%2d. [%c] ", num++, mark);
		int c;
		while ((c = getc(list->file)) != EOF && c != '\n')
			putchar(c);
		putchar('\n');
	}
}
