#include <stdio.h>
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
char next_task(FILE *file)
{
	while (!feof(file)) {
		/* Check if line contains a todo item */
		int res;
		char x;

#ifdef _MSC_VER
		res = fscanf_s(file, "- [%c] ", &x, 1);
#else
		res = fscanf(file, "- [%c] ", &x);
#endif
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
	while ((mark = next_task(file))) {
		if (done != NULL && mark == MARK_DONE)
			++*done;
		if (tasks != NULL) {
			read_line(tasks[i].text, sizeof tasks[i].text, file);
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
		fputc('-', list->file);
	fputc('\n', list->file);
}

static
void write_task(Todo *list, char mark, char *task)
{
	fprintf(list->file, "- [%c] %s\n", mark, task);
}

size_t todo_init(Todo *list, const char *filename)
{
	list->filename = filename;
#ifdef _MSC_VER
	fopen_s(&list->file, filename, "r");
#else
	list->file = fopen(filename, "r");
#endif

	/* Create a new file if it doesn't exist */
	if (list->file == NULL) {
#ifdef _MSC_VER
		strcpy_s(list->header, sizeof list->header, "Todo");
#else
		strcpy(list->header, "Todo");
#endif
		write_header(list);
	}
	else
		read_line(list->header, sizeof list->header, list->file);

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
	if (strlen(task) >= MAXLEN)
		return;

	write_header(list);

	for (int i = 0; i < list->length; i++)
		write_task(list, list->tasks[i].mark, list->tasks[i].text);

	/* Add new task */
	write_task(list, MARK_NOT_DONE, task);
}

void edit_task(Todo *list, int task_no, char *task)
{
	if (task_no < 1 || task_no > list->length || strlen(task) >= MAXLEN)
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

	rewind(list->file);

	/* Print the header (first two lines) */
	for (int i = 0; i < 2; i++)
		print_line(list->file);
	printf("%d/%d task%s\n\n", done, count, count == 1 ? "" : "s");

	char mark;
	int num = 1;
	
	while ((mark = next_task(list->file))) {
		printf("%2d. [%c] ", num++, mark);
		print_line(list->file);
	}
}
