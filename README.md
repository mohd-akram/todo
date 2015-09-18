Todo
====

This is a simple todo app written in C.

Usage
-----

To create a new todo file or view an existing one:

    todo

To add a new task:

    todo "Buy milk"

To edit a task:

    todo -e 1 "Buy more milk"

To move a task:

    todo "Eat cookies"
    todo -m 2 1

To remove a task:

    todo -r 1
