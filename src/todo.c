#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "todo.h"

#define MARK_NOT_DONE ' '
#define MARK_DONE 'x'

static
void read_line(char *dst, int size, FILE *file)
{
	if (fgets(dst, size, file) == NULL)
		dst[0] = '\0';
	else {
		size_t i = strlen(dst) - 1;
		if (dst[i] == '\n')
			dst[i] = '\0';
	}
}

static
char is_task(FILE *file)
{
	char x;
	int res = fscanf(file, "- [%c]%*[ ]", &x);
	if (res == 1 && (x == MARK_NOT_DONE || x == MARK_DONE))
		return x;
	return 0;
}

static
char next_task(FILE *file, bool *space)
{
	*space = false;

	while (!feof(file)) {
		/* Check if a task is preceeded with a newline */
		if (!*space)
			*space = ungetc(getc(file), file) == '\n';

		/* Check if line contains a todo item */
		char x;
		if ((x = is_task(file)))
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
	if (file == NULL)
		return 0;

	if (done != NULL)
		*done = 0;

	int i = 0;
	bool space;
	char mark;

	rewind(file);
	while ((mark = next_task(file, &space))) {
		if (done != NULL && mark == MARK_DONE)
			++*done;
		if (tasks != NULL) {
			read_line(tasks[i].text, sizeof tasks[i].text, file);
			tasks[i].space = i == 0 || space;
			tasks[i].mark = mark;
		}
		++i;
	}

	return i;
}

static
bool write_header(Todo *list)
{
	if (list->file == NULL)
		list->file = fopen(list->filename, "w+");
	else
		list->file = freopen(list->filename, "w+", list->file);

	if (list->file == NULL)
		return false;

	fprintf(list->file, "%s\n", list->name);

	/* Print header underline */
	size_t len = strlen(list->name);
	for (size_t i = 0; i < len; i++)
		putc('=', list->file);
	fprintf(list->file, "\n");

	return true;
}

static
void write_task(Todo *list, bool space, char mark, char *text)
{
	fprintf(list->file, "%s- [%c] %s\n", space ? "\n" : "", mark, text);
}

rsize_t todo_init(Todo *list, const char *filename)
{
	list->filename = filename;
	list->file = fopen(filename, "r");

	bool has_name = true;

	/* Add a default name if one doesn't exist */
	if (list->file == NULL || is_task(list->file))
		has_name = false;
	else {
		read_line(list->name, sizeof list->name, list->file);
		if (strlen(list->name) == 0)
			has_name = false;
	}

	if (!has_name)
		strcpy(list->name, "Todo");

	return (rsize_t) find_tasks(list->file, NULL, NULL) *
		sizeof *list->tasks;
}

void load_tasks(Todo *list, void *tasks)
{
	list->tasks = tasks;
	list->length = find_tasks(list->file, list->tasks, NULL);
}

static
bool is_valid_text(char *text)
{
	size_t len = strlen(text);
	return len > 1 && len < MAXTEXT;
}

rsize_t add_task(Todo *list, char *text)
{
	if (!is_valid_text(text))
		return 0;

	if (!write_header(list))
		return RSIZE_MAX + 1;

	for (int i = 0; i < list->length; i++)
		write_task(list, list->tasks[i].space, list->tasks[i].mark,
			list->tasks[i].text);

	/* Add new task */
	write_task(list, list->length == 0, MARK_NOT_DONE, text);

	return (rsize_t) (list->length + 1) * sizeof *list->tasks;
}

rsize_t edit_task(Todo *list, int task_no, char *text)
{
	if (task_no < 1 || task_no > list->length || !is_valid_text(text))
		return 0;

	if (!write_header(list))
		return RSIZE_MAX + 1;

	for (int i = 0; i < list->length; i++)
		write_task(list, list->tasks[i].space, list->tasks[i].mark,
			i+1 == task_no ? text : list->tasks[i].text);

	return (rsize_t) list->length * sizeof *list->tasks;
}

rsize_t mark_task(Todo *list, int task_no)
{
	if (task_no < 1 || task_no > list->length)
		return 0;

	if (!write_header(list))
		return RSIZE_MAX + 1;

	for (int i = 0; i < list->length; i++) {
		char mark = list->tasks[i].mark;
		write_task(list, list->tasks[i].space,
			i+1 == task_no ? (mark == MARK_NOT_DONE ?
				MARK_DONE : MARK_NOT_DONE) : mark,
			list->tasks[i].text);
	}

	return (rsize_t) list->length * sizeof *list->tasks;
}

rsize_t move_task(Todo *list, int from, int to)
{
	if (from < 1 || from > list->length || to < 1 || to > list->length)
		return 0;

	if (!write_header(list))
		return RSIZE_MAX + 1;

	int len = list->length;
	for (int i = 0, num = 1; i < len; i++, num++) {
		if (i+1 == from) (void) ++i, ++len;
		if (num == to) (void) --i, --len;
		int idx = num == to ? from - 1 : i;
		int space_idx = idx;
		/* Space moves to the task below the one we're moving */
		if (from != to && idx+1 == from+1 && list->tasks[from-1].space)
			space_idx = from-1;
		/* If moving a task down, ignore its space */
		else if (from < to && idx+1 == from)
			space_idx = -1;
		/* If moving a task up, take the pushed down task's space */
		else if (to < from) {
			if (num == to) space_idx = to-1;
			else if (idx+1 == to) space_idx = -1;
		}
		bool space = space_idx != -1 && list->tasks[space_idx].space;
		write_task(list, space, list->tasks[idx].mark,
			list->tasks[idx].text);
	}

	return (rsize_t) list->length * sizeof *list->tasks;
}

rsize_t remove_task(Todo *list, int task_no)
{
	if (task_no < 1 || task_no > list->length)
		return 0;

	if (!write_header(list))
		return RSIZE_MAX + 1;

	for (int i = 0; i < list->length; i++)
		if (i+1 != task_no)
			write_task(list,
				(i+1 == task_no+1 && list->tasks[i-1].space) ||
					list->tasks[i].space,
				list->tasks[i].mark, list->tasks[i].text);

	return (rsize_t) (list->length - 1) * sizeof *list->tasks;
}

rsize_t space_task(Todo *list, int task_no)
{
	if (task_no < 2 || task_no > list->length)
		return 0;

	if (!write_header(list))
		return RSIZE_MAX + 1;

	for (int i = 0; i < list->length; i++)
		write_task(list,
			i+1 == task_no ? !list->tasks[i].space :
				list->tasks[i].space,
			list->tasks[i].mark, list->tasks[i].text);

	return (rsize_t) list->length * sizeof *list->tasks;
}

void print_tasks(Todo *list)
{
	/* Print the header */
	printf("%s\n", list->name);
	size_t len = strlen(list->name);
	for (size_t i = 0; i < len; i++)
		putchar('=');
	printf("\n");

	int done = 0;
	for (int i = 0; i < list->length; i++)
		if (list->tasks[i].mark == MARK_DONE)
			++done;

	/* Print the task count */
	printf("\n%d/%d %s\n", done, list->length,
		list->length == 1 ? "task" : "tasks");

	/* Print the tasks */
	for (int i = 0; i < list->length; i++)
		printf("%s%2d. [%c] %s\n", list->tasks[i].space ? "\n" : "",
			i+1, list->tasks[i].mark, list->tasks[i].text);
}
