#include "systemcalls.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <fcntl.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
 */
bool do_system(const char *cmd)
{

    /*
     * TODO  add your code here
     *  Call the system() function with the command set in the cmd
     *   and return a boolean true if the system() call completed with success
     *   or false() if it returned a failure
     */
    int result = system(cmd);

    if (result == 0)
    {
        // El comando se ejecutó correctamente
        return true;
    }
    else
    {
        // Ocurrió un error al ejecutar el comando
        return false;
    }
    // return true;
}

/**
 * @param count -The numbers of variables passed to the function. The variables are command to execute.
 *   followed by arguments to pass to the command
 *   Since exec() does not perform path expansion, the command to execute needs
 *   to be an absolute path.
 * @param ... - A list of 1 or more arguments after the @param count argument.
 *   The first is always the full path to the command to execute with execv()
 *   The remaining arguments are a list of arguments to pass to the command in execv()
 * @return true if the command @param ... with arguments @param arguments were executed successfully
 *   using the execv() call, false if an error occurred, either in invocation of the
 *   fork, waitpid, or execv() command, or if a non-zero return value was returned
 *   by the command issued in @param arguments with the specified arguments.
 */

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    /*
     * TODO:
     *   Execute a system command by calling fork, execv(),
     *   and wait instead of system (see LSP page 161).
     *   Use the command[0] as the full path to the command to execute
     *   (first argument to execv), and use the remaining arguments
     *   as second argument to the execv() command.
     *
     */
    pid_t pid = fork();

    if (pid == -1)
    {
        // Error al crear el proceso hijo
        perror("Fork failed");
        return false;
    }
    else if (pid == 0)
    {
        // Código del proceso hijo
        // Ejecutar el comando con execv()

        if (execv(command[0], command) == -1)
        {
            // Error al ejecutar execv()
            perror("Execv failed");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Código del proceso padre
        int status;
        // Esperar a que el proceso hijo termine
        if (waitpid(pid, &status, 0) == -1)
        {
            // Error al esperar al proceso hijo
            perror("Waitpid failed");
            return false;
        }

        // Verificar si el proceso hijo terminó correctamente
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    va_end(args);

    return true;
}

/**
 * @param outputfile - The full path to the file to write with command output.
 *   This file will be closed at completion of the function call.
 * All other parameters, see do_exec above
 */
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    /*
     * TODO
     *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
     *   redirect standard out to a file specified by outputfile.
     *   The rest of the behaviour is same as do_exec()
     *
     */

    pid_t pid = fork();

    if (pid == -1)
    {
        // Error al crear el proceso hijo
        perror("Fork failed");
        return false;
    }
    else if (pid == 0)
    {
        // Código del proceso hijo
        // Redirigir la salida estándar a un archivo especificado por outputfile

        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC);
        if (fd == -1)
        {
            // Error al abrir el archivo
            perror("Open failed");
            exit(EXIT_FAILURE);
        }

        // Redirigir stdout al archivo
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            // Error al redirigir
            perror("Dup2 failed");
            exit(EXIT_FAILURE);
        }

        // Cerrar el descriptor de archivo, ya que ya no es necesario
        close(fd);

        // Ejecutar el comando con execv()
        if (execv(command[0], command) == -1)
        {
            // Error al ejecutar execv()
            perror("Execv failed");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // Código del proceso padre
        int status;
        // Esperar a que el proceso hijo termine
        if (waitpid(pid, &status, 0) == -1)
        {
            // Error al esperar al proceso hijo
            perror("Waitpid failed");
            return false;
        }

        // Verificar si el proceso hijo terminó correctamente
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    va_end(args);

    return true;
}
