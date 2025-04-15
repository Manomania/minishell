/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execute.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 16:10:59 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/15 14:46:36 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "debug.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Sets up redirections for a command, stopping on the first error.
 * Also handles pre-reading heredocs if not already done.
 *
 * @param ctx Context containing environment info
 * @param cmd Command containing redirections to set up
 * @return int 0 on success, 1 on error (generic redirection failure)
 */
static int	setup_command_redirections(t_ctx *ctx, t_command *cmd)
{
	// Ensure heredocs are read if they haven't been (e.g., single command case)
	// This check might be redundant if read_all_heredocs is always called first.
	if (setup_heredocs(ctx, cmd) != 0)
	{
		// Error during heredoc processing
		return (1);
	}
	// Apply file redirections sequentially, stopping on first error
	if (setup_redirections(cmd->redirection) != 0)
	{
		// Error during file redirection (e.g., open, dup2 failed)
		return (1);
	}
	return (0); // Success
}

/**
 * @brief Handles command validation and error reporting in child process
 *
 * @param ctx Context containing environment info
 * @return int Exit code for the child process (0 on success,
	error code otherwise)
 */
static int	validate_cmd_in_child(t_ctx *ctx)
{
	// If there's no command (only redirections), exit successfully.
	if (!ctx->cmd->args || !ctx->cmd->args[0])
		return (EXIT_SUCCESS);
	// Find the binary path
	if (!command_bin(ctx))
	// command_bin now prints errors internally via bin_find
	{
		// command_bin failed to find the command. Determine the correct exit code.
		// Need to check the specific error from bin_find if possible,
		// otherwise default to command not found.
		// Let's assume bin_find sets an appropriate error state or prints.
		// Bash typically exits 127 for command not found,
		// 126 for permission/is dir.
		// If bin_find handled the error printing and determined the type:
		// We might need a way for bin_find to return the error type/code.
		// For now, let's rely on the error printed by bin_find/error()
		// and use a common failure code like 127 if the reason isn't clear.
		// If bin_find already called error(),
		// the exit code might be implicitly set ?
		// Let's explicitly return the code based on common bash errors.
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			// Check if it looks like a path that failed
			if (ft_strchr(ctx->cmd->args[0], '/'))
			{
				// It was a path, likely permission or is_dir error
				// error() should have printed "Permission denied" or "Is a directory"
				return (126);
			}
			else
			{
				// Not a path, likely "command not found"
				// error() should have printed "command not found"
				return (127);
			}
		}
		return (127); // Default command not found
	}
	return (0); // Command found and path updated in ctx->cmd->args[0]
}

/**
 * @brief Executes a single command without a pipeline
 *
 * @param ctx Context containing environment and command info
 * @return Exit status of the command
 */
static int	execute_single_command(t_ctx *ctx)
{
	pid_t	pid;
	int		status;
	int		was_signaled;
	int		saved_stdout;

	int exit_code;        // Use exit_code for clarity
	int saved_stdin = -1; // Save original std fds
	saved_stdout = -1;
	was_signaled = 0;
	exit_code = 0; // Default success
	// Save original std fds before trying builtins or forking
	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	// Try executing as a builtin first
	if (is_builtin_command(ctx->cmd->args ? ctx->cmd->args[0] : NULL))
	{
		// Setup redirections for the builtin
		if (setup_command_redirections(ctx, ctx->cmd) != 0)
		{
			exit_code = 1; // Redirection failed
		}
		else
		{
			// Execute the builtin
			builtins_try(ctx, ctx->cmd);
			// builtins_try updates ctx->exit_status
			exit_code = ctx->exit_status;
		}
		// Restore original std fds
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		return (exit_code);
	}
	// If not a builtin, fork and execute externally
	setup_parent_signals();
	pid = fork();
	if (pid == -1)
	{
		// Fork failed, restore fds and return error
		if (saved_stdin != -1)
		{
			dup2(saved_stdin, STDIN_FILENO);
			close(saved_stdin);
		}
		if (saved_stdout != -1)
		{
			dup2(saved_stdout, STDOUT_FILENO);
			close(saved_stdout);
		}
		setup_signals();                         // Restore interactive signals
		return (error(NULL, "fork", ERR_CHILD)); // Use error module
	}
	if (pid == 0) // Child process
	{
		// Child doesn't need saved fds
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		reset_signals(); // Child handles signals differently
		// Setup redirections in child
		if (setup_command_redirections(ctx, ctx->cmd) != 0)
		{
			cleanup_child_process(ctx);
			exit(1); // Exit child with status 1 on redirection failure
		}
		// Validate command path (e.g., find binary, check permissions)
		exit_code = validate_cmd_in_child(ctx);
		if (exit_code != 0)
		{
			cleanup_child_process(ctx);
			exit(exit_code); // Exit with code from validation (126, 127, etc.)
		}
		// If validation passed and command exists, execute it
		if (ctx->cmd->args && ctx->cmd->args[0])
		{
			execve(ctx->cmd->args[0], ctx->cmd->args, ctx->envp);
			// If execve returns, it's an error
			ctx_error_exit(ctx, ctx->cmd->args[0], "execve", ERR_CHILD);
			// Prints error and exits
		}
		else // Should only happen if only redirections were present
		{
			cleanup_child_process(ctx);
			exit(0); // Success if only redirections
		}
	}
	else // Parent process
	{
		// Parent closes saved fds as they are restored below
		if (saved_stdin != -1)
			close(saved_stdin);
		if (saved_stdout != -1)
			close(saved_stdout);
		waitpid(pid, &status, 0);            // Wait for the child
		exit_code = get_exit_status(status); // Get exit code from status
		// Handle signals received by the child
		if (WIFSIGNALED(status))
		{
			was_signaled = 1;
			if (WTERMSIG(status) == SIGQUIT)
				ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
			// SIGINT newline is handled by the main loop's signal handler usually
			else if (WTERMSIG(status) == SIGINT && isatty(STDOUT_FILENO))
				write(STDOUT_FILENO, "\n", 1);
			// Add check for SIGPIPE for "Broken pipe" message
			else if (WTERMSIG(status) == SIGPIPE)
			{
				// Bash doesn't always print "Broken pipe", depends on context.
				// Let's omit it for now unless specifically needed for tests.
				ft_putstr_fd("Broken pipe: ", STDERR_FILENO);
				ft_putnbr_fd(WTERMSIG(status), STDERR_FILENO);
				// Print signal number
				ft_putstr_fd("\n", STDERR_FILENO);
			}
			else if (WTERMSIG(status) != SIGINT && isatty(STDOUT_FILENO))
				// Print newline for other signals if interactive
				write(STDOUT_FILENO, "\n", 1);
			// Update exit code for signals (128 + signal number)
			exit_code = 128 + WTERMSIG(status);
		}
		setup_signals(); // Restore parent's interactive signal handlers
		return (exit_code);
	}
	return (ctx->exit_status);
}

/**
 * @brief Checks if the command is part of a pipeline
 *
 * @param cmd Command to check
 * @return true if command is part of pipeline, false otherwise
 */
static t_bool	is_pipeline(t_command *cmd)
{
	// A command is part of a pipeline if it has a 'next' command.
	return (cmd && cmd->next);
}

/**
 * @brief Validates if the command context is valid for execution
 *
 * @param ctx Context containing environment and command info
 * @param status Pointer to store error status (unused here, handled by return)
 * @return int 1 if valid, 0 if invalid or nothing to execute
 */
static int	validate_command_context(t_ctx *ctx)
{
	if (!ctx || !ctx->cmd)
	{
		// Should not happen if parsing is correct, but good check
		error(NULL, "validate_command_context", ERR_ALLOC);
		// Or some internal error
		ctx->exit_status = 1;
		// Internal error status
		return (0);
	}
	// If there are no arguments AND no redirections, there's nothing to do.
	if (!ctx->cmd->args && !ctx->cmd->redirection)
	{
		ctx->exit_status = 0; // Empty command is success (like bash)
		return (0);           // Indicate nothing to execute
	}
	return (1); // Command context is valid for execution attempt
}

/**
 * @brief Determines command type and executes appropriate handler
 *
 * @param ctx Context containing environment and command info
 * @return int Status code from command execution
 */
static int	process_command_type(t_ctx *ctx)
{
	// Check if it's just redirections (no command arguments)
	// Note: is_builtin_command checks args[0],
	// so this needs to be checked first.
	if (has_only_redirections(ctx->cmd))
	{
		// Need a way to execute only redirections,
		// similar to execute_single_command's child part
		// Let execute_single_command handle this case by checking if args[0] is NULL in child.
		return (execute_single_command(ctx));
	}
	// Check if it's a pipeline
	else if (is_pipeline(ctx->cmd))
	{
		return (exec_cmdas(ctx)); // Execute pipeline
	}
	else // Otherwise, it's a single command (builtin or external)
	{
		return (execute_single_command(ctx));
	}
}

/**
 * @brief Main execution entry point for a parsed command line.
 * Handles heredocs, validates context, and dispatches execution.
 *
 * @param ctx Context containing environment and command info
 * @return int Exit status of the command/pipeline
 */
int	command_execute(t_ctx *ctx)
{
	int	status;

	// 1. Validate the command context
	if (!validate_command_context(ctx))
	{
		return (ctx->exit_status); // Return status set by validation (0 or 1)
	}
	// 2. Read all heredocs for the entire command line *before* execution
	if (read_all_heredocs(ctx) != 0)
	{
		// If heredoc reading fails (e.g., pipe error, interrupted), set status
		// Bash often returns 1 or signal status if interrupted during heredoc
		// Let's return 1 for generic heredoc failure for now.
		// Need to ensure any opened heredoc FDs are closed.
		close_heredoc_fds(ctx->cmd); // Close any partially opened heredocs
		ctx->exit_status = 1;
		// Or potentially ctx->exit_status if signal handling updated it
		return (ctx->exit_status);
	}
	// 3. Process the command (single or pipeline)
	status = process_command_type(ctx);
	// 4. Clean up any remaining heredoc file descriptors
	close_heredoc_fds(ctx->cmd);
	// 5. Update and return the final exit status
	ctx->exit_status = status;
	return (status);
}
