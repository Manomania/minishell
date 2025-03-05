/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_line.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 10:59:00 by elagouch          #+#    #+#             */
/*   Updated: 2025/03/05 11:30:28 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_lexer *create_lexer(char* input)
{
	t_lexer *lexer;

	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (NULL);
	lexer->input = input;
	lexer->position = 0;
	lexer->length = (int)ft_strlen(input);
	return (lexer);
}

char	get_lexer(t_lexer *lexer)
{
	if (lexer->position >= lexer->length)
		return ('\0');
	return (lexer->input[lexer->position]);
}

void	advance_lexer(t_lexer *lexer)
{
	if (lexer->position < lexer->length)
		lexer->position++;
}

void	skip_whitespace_lexer(t_lexer *lexer)
{
	while (get_lexer(lexer) == ' ' || get_lexer(lexer) == '\t')
		advance_lexer(lexer);
}

char	*read_quoted_string(t_lexer *lexer, char quote_char)
{
	int	start;

	start = lexer->position + 1;

}




void	free_token(t_token *token)
{
	if (token)
	{
		free(token->value);
		free(token);
	}
}

t_token	*create_token(t_token_type type, char *value)
{
	t_token *token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = value;
	return (token);
}



int	main(int argc, char **argv)
{
	t_lexer	*lexer;

	lexer = malloc(sizeof(t_lexer));
	if (!lexer)
		return (1);
	// printf(YELLOW"DEBUG: \n"RESET, lexer);
}

t_token	*parse_line(t_ctx ctx, char *line)
{

}

/*
// Redirections
<
>

// Here_doc
<<
>>

// Pipes
|

// Env
$ENV
$?

// Logical operations
&&
||

// Wildcards (no glob)
*
hello/*
hello*

// Le reste: tokens de programme
echo hello
echo "hello|"

// Example
```sh
bash-5.1$ echo "hello 'world "shrek" swamp' bitch"
hello 'world shrek swamp' bitch
bash-5.1$ echo "hello 'world "sh|rek" swamp' bitch"
bash: rek swamp' bitch: command not found
bash-5.1$ echo "hello 'worl|d "shrek" swamp' bitch"
hello 'worl|d shrek swamp' bitch
```

// La regle d'or:
- Quand on rentre dans un simple/double quote, on attend la fin
du simple/double quote, c'est tout.
*/
