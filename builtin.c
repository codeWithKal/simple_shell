#include "shell.h"

#define SETOWD(V) (V = _strdup(_getenv("OLDPWD")))
/**
 * change_dir - changes directory
 * @data: a pointer to the data structure
 *
 * Return: (Success) 0 is returned
 * ------- (Fail) negative number will be returned
 */
int change_dir(sh_t *data)
{
	char *home;

	home = _getenv("HOME");
	if (data->args[1] == NULL)
	{
		SETOWD(data->oldpwd);
		if (chdir(home) < 0)
			return (FAIL);
		return (SUCCESS);
	}
	if (_strcmp(data->args[1], "-") == 0)
	{
		if (data->oldpwd == 0)
		{
			SETOWD(data->oldpwd);
			if (chdir(home) < 0)
				return (FAIL);
		}
		else
		{
			SETOWD(data->oldpwd);
			if (chdir(data->oldpwd) < 0)
				return (FAIL);
		}
	}
	else
	{
		SETOWD(data->oldpwd);
		if (chdir(data->args[1]) < 0)
			return (FAIL);
	}
	return (SUCCESS);
}

#undef GETCWD

/**
 * abort_prg - exit the program
 * @data: a pointer to the data structure
 *
 * Return: (Success) 0 is returned
 * ------- (Fail) negative number will returned
 */
int abort_prg(sh_t *data __attribute__((unused)))
{
	int code, i = 0;

	if (data->args[1] == NULL)
	{
		free_data(data);
		exit(0);
	}
	while (data->args[1][i])
	{
		if (_isalpha(data->args[1][i++]) > 0)
		{
			data->error_msg = _strdup("Illegal number\n");
			return (FAIL);
		}
	}
	code = _atoi(data->args[1]);
	free_data(data);
	exit(code);
}

/**
 * display_help - display the help menu
 * @data: a pointer to the data structure
 *
 * Return: (Success) 0 is returned
 * ------- (Fail) negative number will returned
 */
int display_help(sh_t *data)
{
	int fd, fw, rd = 1;
	char c;

	fd = open(data->args[1], O_RDONLY);
	if (fd < 0)
	{
		data->error_msg = _strdup("no help topics match\n");
		return (FAIL);
	}
	while (rd > 0)
	{
		rd = read(fd, &c, 1);
		fw = write(STDOUT_FILENO, &c, rd);
		if (fw < 0)
		{
			data->error_msg = _strdup("cannot write: permission denied\n");
			return (FAIL);
		}
	}
	PRINT("\n");
	return (SUCCESS);
}

/**
 * handle_builtin - handle and manage the builtins cmd
 * @data: a pointer to the data structure
 *
 * Return: (Success) 0 is returned
 * ------- (Fail) negative number will returned
 */
int handle_builtin(sh_t *data)
{
	blt_t blt[] = {
		{"exit", abort_prg},
		{"cd", change_dir},
		{"help", display_help},
		{NULL, NULL}
	};
	int i = 0;

	while ((blt + i)->cmd)
	{
		if (_strcmp(data->args[0], (blt + i)->cmd) == 0)
			return ((blt + i)->f(data));
		i++;
	}
	return (FAIL);
}
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

/**
 * isbuiltin - check whether a command is a builtin
 *
 * @cmd: the command to check
 *
 * Return: 1 if command is builtin, otherwise 0
 *
 */
int isbuiltin(const char *cmd)
{
	char *builtins[] = { "exit", "env", NULL };
	int i = 0;

	while (builtins[i])
	{
		if (_strcmp(builtins[i], cmd) == 0)
			return (1);
		i++;
	}
	return (0);
}

/**
 * executebuiltin - map a command with a function
 *
 * @tokens: command to execute
 *
 */
void executebuiltin(char **tokens)
{
	builtin_t builtins[] = {
		{ "exit", __exit },
		{ "env", _env },
		{ NULL, NULL }
	};
	int i = 0;

	while (builtins[i].cmd)
	{
		if (_strcmp(builtins[i].cmd, tokens[0]) == 0)
			builtins[i].execute_builtin(tokens);
		i++;
	}
}

/**
 * __exit - the exit builtin that exits the shell
 *
 * @tokens: array of character arrays consisting the exit command
 * and its arguments
 *
 * Usage: exit
 *
 */
void __exit(char **tokens)
{
	if (tokens[1])
		exit(_atoi(tokens[1]));
	exit(0);
}

/**
 * _env - the env builtin that prints the current environment
 *
 * @tokens: array of character arrays consisting the env command
 * and its arguments
 *
 */
void _env(char **tokens)
{
	unsigned int i;

	for (i = 0; tokens[0] && environ[i] != NULL; ++i)
		printf("%s\n", environ[i]);
}
