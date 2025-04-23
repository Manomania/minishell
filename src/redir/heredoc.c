/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/17 15:30:10 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:57:15 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error.h"
#include "minishell.h"

/**
 * @brief Signal handler for heredoc mode
 * @param sig Signal number
 *
 * Gestionnaire amélioré qui force une interruption complète sur SIGINT
 * Les fonctions readline sont utilisées pour nettoyer l'état du terminal
 */
static void	sig_heredoc_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_status = 130;
		write(STDOUT_FILENO, "^C", 2);
		rl_done = 1;
		rl_replace_line("", 0);
		rl_free_line_state();
		rl_cleanup_after_signal();
	}
}

/**
 * @brief Sets up signal handlers for heredoc mode
 *
 * Configure les gestionnaires pour SIGINT et SIGQUIT en mode heredoc
 * SIGINT est capturé avec notre gestionnaire personnalisé
 * SIGQUIT est ignoré
 */
static void	setup_heredoc_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sigemptyset(&sa_int.sa_mask);
	sigemptyset(&sa_quit.sa_mask);
	sa_int.sa_flags = 0;
	sa_quit.sa_flags = 0;
	sa_int.sa_handler = sig_heredoc_handler;
	sa_quit.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa_int, NULL);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/**
 * @brief Creates a heredoc pipe
 * @return Pipe file descriptors array, or NULL on error
 *
 * Crée un pipe pour transmettre le contenu du heredoc
 */
static int	*create_heredoc_pipe(void)
{
	int	*pipe_fds;

	pipe_fds = malloc(sizeof(int) * 2);
	if (!pipe_fds)
		return (NULL);
	if (pipe(pipe_fds) == -1)
	{
		free(pipe_fds);
		return (NULL);
	}
	return (pipe_fds);
}

/**
 * @brief Vérifie si une interruption par SIGINT est en cours
 * @return true si interrompu, false sinon
 *
 * Fonction utilitaire pour centraliser la vérification de l'état
 * du signal, utilisant uniquement la variable globale existante
 */
static t_bool	is_heredoc_interrupted(void)
{
	return (g_signal_status == 130);
}

/**
 * @brief Vérifie les interruptions durant les opérations readline
 * @return 1 si interrompu, 0 sinon
 *
 * Fonction callback pour readline qui force l'arrêt si un signal
 * a été reçu entre les vérifications
 */
static int	interrupt_check_hook(void)
{
	if (is_heredoc_interrupted())
	{
		rl_done = 1;
		return (1);
	}
	return (0);
}

/**
 * @brief Handles reading heredoc input and writing to pipe
 * @param pipe_fds Pipe file descriptors
 * @param delimiter String marking end of heredoc
 * @param ctx Shell context
 * @return 0 on success, -1 on error or interrupt
 *
 * Lit les lignes du heredoc jusqu'au délimiteur ou interruption
 * Utilise un hook readline pour vérifier les interruptions
 */
static int	read_heredoc_content(int *pipe_fds, char *delimiter, t_ctx *ctx)
{
	char	*line;
	char	*expanded_line;
	int		status;

	rl_event_hook = interrupt_check_hook;
	while (1)
	{
		if (is_heredoc_interrupted())
			return (-1);
		status = read_heredoc_line(delimiter, &line);
		if (is_heredoc_interrupted())
		{
			if (line)
				free(line);
			return (-1);
		}
		if (status != 0)
		{
			if (status < 0)
				return (-1);
			return (0);
		}
		expanded_line = expand_variables_in_line(ctx, line);
		free(line);
		if (!expanded_line)
			return (-1);
		write(pipe_fds[1], expanded_line, ft_strlen(expanded_line));
		write(pipe_fds[1], "\n", 1);
		free(expanded_line);
	}
	return (0);
}

/**
 * @brief Restaure l'état du shell en mode interactif
 *
 * Réinitialise les signaux et le terminal pour le mode interactif
 * Assure une transition propre après un heredoc
 */
static void	restore_shell_state(void)
{
	setup_signals();
	rl_event_hook = NULL;
	rl_clear_signals();
	rl_on_new_line();
	rl_replace_line("", 0);
}

/**
 * @brief Creates a heredoc without using fork
 * @param ctx Shell context
 * @param delimiter String marking end of heredoc
 * @return File descriptor to read from, or -1 on error
 *
 * Fonction principale de gestion des heredocs avec support
 * pour l'interruption immédiate sur Ctrl+C
 */
int	create_heredoc(t_ctx *ctx, char *delimiter)
{
	int		*pipe_fds;
	int		read_fd;
	int		result;

	pipe_fds = create_heredoc_pipe();
	if (!pipe_fds)
		return (error(NULL, "heredoc", ERR_PIPE));
	setup_heredoc_signals();
	result = read_heredoc_content(pipe_fds, delimiter, ctx);
	close(pipe_fds[1]);
	if (result == -1 || is_heredoc_interrupted())
	{
		close(pipe_fds[0]);
		free(pipe_fds);
		if (is_heredoc_interrupted())
		{
			ctx->exit_status = 130;
			g_signal_status = 0;
		}
		restore_shell_state();
		return (-1);
	}
	read_fd = pipe_fds[0];
	free(pipe_fds);
	restore_shell_state();
	return (read_fd);
}

/**
 * @brief Handles here_doc redirections for a command
 * @param ctx Context containing environment information
 * @param cmd Command containing redirections
 * @return 0 on success, non-zero on error
 *
 * Configure les redirections de type heredoc pour la commande
 */
int	setup_heredocs(t_ctx *ctx, t_command *cmd)
{
	t_redirection	*redir;
	int				dup_result;

	(void)ctx;
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM && redir->fd >= 0)
		{
			dup_result = dup2(redir->fd, STDIN_FILENO);
			close(redir->fd);
			redir->fd = -1;
			if (dup_result == -1)
			{
				perror("dup2");
				return (-1);
			}
		}
		redir = redir->next;
	}
	return (0);
}
