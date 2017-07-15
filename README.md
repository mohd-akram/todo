Todo
====

This is a simple todo app written in C.

Usage
-----

To create a new todo file or view an existing one:

    todo

To add a new task:

    todo buy milk
    todo "eat cookies"
    todo 1 pour milk

To edit a task:

    todo -e 2 buy more milk

To move a task:

    todo -m 3 2

To mark a task:

    todo 1

To remove a task:

    todo -r 1

To add space before a task:

    todo -s 2

Help:

    todo -h

You can change the header by editing the first line of the `todo.md` file.

Build
-----

Clone the repo (eg. to `/usr/local/src`) and in it run:

    cc -std=c11 src/main.c src/todo.c -o todo

If you're using Visual Studio, open the project and build Release.

Install
-------

To install globally (assuming the repo is in `/usr/local/src`):
    
    ln -s /usr/local/src/todo/todo /usr/local/bin/todo

On Windows, simply add the `Release` folder to your path.

The `TODO_DIR` environment variable controls the global `todo.md` file's
location. If it is not set, the current working directory is used.

If a `todo.md` file exists in the current directory, it will be used instead of
the global one.
