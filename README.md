# Smash Shell Version 1.2

Welcome to the Smash Shell! Below you will find a comprehensive guide on how to use the internal commands supported by the shell.
- Note: All standard Linux commands are also supported alongside these built-in commands. 

## Built-in Commands

### `cd [DIRECTORY]`
Change the current working directory.
- **DIRECTORY**: The path to the desired directory. If no argument is provided, it will print an error.

### `exit`
Exit the shell.

### `help`
Displays a list of all built-in commands and a short description about the shell.

### `cwd`
Print the current working directory.

### `clear`
Clear the terminal screen.

### `echo [ARGUMENTS...]`
Print the arguments to the terminal.
- **ARGUMENTS**: A list of arguments you want to echo. Each argument is printed on a new line.

### `showvar [VARIABLE_NAME]`
Display the value of shell variables.
- **VARIABLE_NAME**: Optional. If specified, it will display the value of the given variable, otherwise, it will list all the shell variables with their values.

### `showenv [ENVIRONMENT_VARIABLES...]`
Display the value of environment variables.
- **ENVIRONMENT_VARIABLES**: Optional. If not provided, it will print all the environment variables. If specific environment variables are provided, it will print their values.

### `export VARIABLE_NAME`
Export the value of a shell variable to the environment.

### `unset VARIABLE_NAME`
Unset a shell variable and also remove it from the environment.

### `alias [-n ALIAS_NAME ALIAS_VALUE] [-r ALIAS_NAME]`
Manage command aliases.
- `-n`: Create a new alias. Requires both ALIAS_NAME and ALIAS_VALUE.
- `-r`: Remove an existing alias. Requires ALIAS_NAME.

## Other Features

- **Variable Expansion**: If a variable is denoted with a `$`, the shell will expand it to its value, unless it's enclosed in double quotes.
- **Variable Reassignment**: If you assign a value to a variable using `=`, it will update the shell variable's value or create a new one if it doesn't exist.
- **Pipeline Execution**: Use `|` to pipe the output of one command to another.

## Example

```bash
$ echo "Hello, World!"
Hello, World!

$ showenv PATH
/usr/local/bin:/usr/bin:/bin

$ alias -n ls "ls -al"
$ ls
(total files... in detailed list format...)
