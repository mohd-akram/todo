Todo
====

This is a simple todo app written in C.

Usage
-----

To view your todo list:

    todo

To change its name:

    todo -n Things to Do

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

Build
-----

Run `make` (or `nmake` on Windows).

If you're using Visual Studio, open the project and build Release.

Install
-------

Run `sudo make install` to install to `/usr/local`.

To install to another location, run `configure` first:

    ./configure --prefix=<install-dir>
    make install

On Windows, simply add the `Release` folder to your `PATH`.

The `TODO_DIR` environment variable controls the global `todo.md` file's
location. If it is not set, the current working directory is used.

If a `todo.md` file exists in the current directory, it will be used instead of
the global one.
