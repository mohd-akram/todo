#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "todo.h"

#define MARK_NOT_DONE ' '
#define MARK_DONE 'x'

static
void read_line(char *dst, int size, FILE *file)
{
	if (fgets(dst, size, file) == NULL)
		dst[0] = '\0';
	else {
		int i = strlen(dst) - 1;
		if (dst[i] == '\n')
			dst[i] = '\0';
	}
}

static
void print_line(FILE *file)
{
	int c;
	while ((c = getc(file)) != EOF) {
		putchar(c);
		if (c == '\n')
			break;
	}
}

static
char is_task(FILE *file)
{
	char x;
#ifdef _MSC_VER
	int res = fscanf_s(file, "- [%c]%*[ ]", &x, 1);
#else
	int res = fscanf(file, "- [%c]%*[ ]", &x);
#endif
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
			tasks[i].space = space;
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
#ifdef _MSC_VER
		fopen_s(&list->file, list->filename, "w+");
#else
		list->file = fopen(list->filename, "w+");
#endif
	else
#ifdef _MSC_VER
		freopen_s(&list->file, list->filename, "w+", list->file);
#else
		list->file = freopen(list->filename, "w+", list->file);
#endif

	fprintf(list->file, "%s\n", list->header);

	/* Print header underline */
	int len = strlen(list->header);
	for (int i = 0; i < len; i++)
		putc('-', list->file);
	fprintf(list->file, "\n");
}

static
void write_task(Todo *list, bool space, char mark, char *text)
{
	fprintf(list->file, "%s- [%c] %s\n", space ? "\n" : "", mark, text);
}

size_t todo_init(Todo *list, const char *filename)
{
	list->filename = filename;
#ifdef _MSC_VER
	fopen_s(&list->file, filename, "r");
#else
	list->file = fopen(filename, "r");
#endif

	bool has_header = true;

	/* Add a default header if one doesn't exist */
	if (list->file == NULL || is_task(list->file))
		has_header = false;
	else {
		read_line(list->header, sizeof list->header, list->file);
		if (strlen(list->header) == 0)
			has_header = false;
	}

	if (!has_header)
#ifdef _MSC_VER
		strcpy_s(list->header, sizeof list->header, "Todo");
#else
		strcpy(list->header, "Todo");
#endif

	/* Create a new file if it doesn't exist */
	if (list->file == NULL)
		write_header(list);

	return find_tasks(list->file, NULL, NULL) * sizeof *list->tasks;
}

void load_tasks(Todo *list, void *tasks)
{
	list->tasks = tasks;
	list->length = find_tasks(list->file, list->tasks, NULL);

	/* Print formatted tasks to file */
	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list, i == 0 || list->tasks[i].space,
			list->tasks[i].mark, list->tasks[i].text);
}

static
bool is_valid_text(char *text)
{
	int len = strlen(text);
	return len > 1 && len < MAXLEN;
}

size_t add_task(Todo *list, char *text)
{
	if (!is_valid_text(text))
		return 0;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list, list->tasks[i].space, list->tasks[i].mark,
			list->tasks[i].text);

	/* Add new task */
	write_task(list, list->length == 0, MARK_NOT_DONE, text);

	return (list->length + 1) * sizeof *list->tasks;
}

void edit_task(Todo *list, int task_no, char *text)
{
	if (task_no < 1 || task_no > list->length || !is_valid_text(text))
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list, list->tasks[i].space, list->tasks[i].mark,
			i+1 == task_no ? text : list->tasks[i].text);
}

void mark_task(Todo *list, int task_no)
{
	if (task_no < 1 || task_no > list->length)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++) {
		char mark = list->tasks[i].mark;
		write_task(list, list->tasks[i].space,
			i+1 == task_no ? (mark == MARK_NOT_DONE ?
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
		if (i+1 == from) ++i, ++len;
		if (num == to) --i, --len;
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
}

void remove_task(Todo *list, int task_no)
{
	if (task_no < 1 || task_no > list->length)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		if (i+1 != task_no)
			write_task(list,
				(i+1 == task_no+1 && list->tasks[i-1].space) ||
					list->tasks[i].space,
				list->tasks[i].mark, list->tasks[i].text);
}

void space_task(Todo *list, int task_no)
{
	if (task_no < 2 || task_no > list->length)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list,
			i+1 == task_no ? !list->tasks[i].space :
				list->tasks[i].space,
			list->tasks[i].mark, list->tasks[i].text);
}

void print_tasks(Todo *list)
{
	int done;
	int count = find_tasks(list->file, NULL, &done);

	rewind(list->file);

	/* Print the header (first two lines) */
	for (int i = 0; i < 2; i++)
		print_line(list->file);

	printf("\n%d/%d task%s\n", done, count, count == 1 ? "" : "s");

	bool space;
	char mark;
	int num = 1;

	while ((mark = next_task(list->file, &space))) {
		printf("%s%2d. [%c] ", space ? "\n" : "", num++, mark);
		print_line(list->file);
	}
}
