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

To edit a task:

    todo -e 1 buy more milk

To move a task:

    todo -m 2 1

To mark a task:

    todo 1

To remove a task:

    todo -r 1

Help:

    todo -h

You can change the header by editing the first line of the `todo.md` file.

Build
-----

    mkdir bin
    cd Todo
    clang --std=c11 main.c todo.c -o ../bin/todo

If you're using Visual Studio, open the solution and build Release.

Install
-------

To install globally, add the `cmd` folder to your path.

You can change where the global `todo.md` is stored by editing the script file
in the `cmd` folder.

If a `todo.md` file exists in the current directory, it will be used instead of
the global one.
