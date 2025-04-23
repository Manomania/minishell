/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 13:59:38 by elagouch          #+#    #+#             */
/*   Updated: 2025/04/23 12:58:56 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include "error.h"
#include "minishell.h"

/**
 * @brief Affiche un avertissement pour EOF dans un heredoc
 *
 * Message formaté en jaune pour indiquer la fin d'un heredoc par EOF
 * Utilise les constantes de couleur définies pour les messages
 */
static void	display_heredoc_eof_warning(void)
{
	char	*warning_part1;
	char	*warning_part2;
	char	*warning_part3;

	warning_part1 = "minishell: warning: heredoc: ";
	warning_part2 = "here-document delimited by end-of-file ";
	warning_part3 = "(`eof')\n";
	ft_printf_fd(STDERR_FILENO, YELLOW "%s%s%s" RESET, warning_part1,
		warning_part2, warning_part3);
}

/**
 * @brief Vérifie si le heredoc a été interrompu par un signal
 * @return true si interrompu, false sinon
 *
 * Utilise la variable globale g_signal_status pour détecter SIGINT
 * Fonctionne sans nécessiter de variables globales supplémentaires
 */
static t_bool	is_heredoc_interrupted(void)
{
	return (g_signal_status == 130);
}

/**
 * @brief Lit une ligne du heredoc et vérifie le délimiteur
 * @param delimiter Délimiteur de fin du heredoc
 * @param line Pointeur pour stocker la ligne lue
 * @return 1 si délimiteur trouvé, 0 pour continuer, -1 si EOF ou interruption
 *
 * Version optimisée pour Linux/Windows avec détection d'interruption
 * améliorée avant et après l'appel à readline
 */
int	read_heredoc_line(char *delimiter, char **line)
{
	int		delimiter_len;

	delimiter_len = ft_strlen(delimiter);

	/* Vérification critique avant d'appeler readline */
	if (is_heredoc_interrupted())
		return (-1);

	/* Paramétrage de readline pour cette ligne */
	rl_catch_signals = 0;          /* Laisse notre gestionnaire gérer les signaux */
	rl_catch_sigwinch = 0;         /* Ignore les signaux de redimensionnement */

	/* Lecture de la ligne avec le prompt spécifique au heredoc */
	*line = readline("> ");

	/* Vérification immédiate après readline */
	if (is_heredoc_interrupted())
	{
		if (*line)
		{
			free(*line);
			*line = NULL;
		}
		return (-1);
	}

	/* Gestion d'EOF (Ctrl+D) */
	if (!(*line))
	{
		display_heredoc_eof_warning();
		return (-1);
	}

	/* Vérification du délimiteur */
	if (ft_strncmp(*line, delimiter, delimiter_len + 1) == 0)
	{
		free(*line);
		return (1);
	}

	return (0);
}

/**
 * @brief Configure les pipes pour un heredoc
 * @param pipe_fds Tableau pour stocker les descripteurs
 * @return 0 si succès, -1 si erreur
 *
 * Crée un pipe pour la communication entre le processus
 * de lecture et le processus utilisant le heredoc
 */
int	setup_heredoc_pipes(int pipe_fds[2])
{
	if (pipe(pipe_fds) == -1)
		return (error(NULL, "heredoc", ERR_PIPE));
	return (0);
}

/**
 * @brief Vérifie si une commande contient des redirections heredoc
 * @param cmd Commande à vérifier
 * @return t_bool true si contient au moins un heredoc, false sinon
 *
 * Parcourt la liste des redirections pour trouver une redirection
 * de type heredoc (<<)
 */
t_bool	has_heredoc_redirection(t_command *cmd)
{
	t_redirection	*redir;

	if (!cmd || !cmd->redirection)
		return (false);
	redir = cmd->redirection;
	while (redir)
	{
		if (redir->type == TOK_HERE_DOC_FROM)
			return (true);
		redir = redir->next;
	}
	return (false);
}
