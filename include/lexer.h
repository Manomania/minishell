/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elagouch <elagouch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 13:50:50 by elagouch          #+#    #+#             */
/*   Updated: 2025/05/06 13:55:10 by elagouch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include "minishell.h"

// *************************************************************************** #
//                            Function Prototypes                              #
// *************************************************************************** #

// lexer_read.c
char						*read_word_lexer(t_lexer *lexer);
char						*read_complex_word(t_lexer *lexer);
char						*read_quoted_string_lexer(t_lexer *lexer,
								char quote_char);

// lexer_read_utils.c
char						*join_and_free(char *s1, char *s2);
char						*handle_dollar_sign(char *result);
char						*handle_word_part(t_lexer *lexer, char *result);
char						*handle_quoted_part(t_lexer *lexer, char *result,
								char quote_char);
char						*handle_dollar_quotes(t_lexer *lexer);

// lexer_read_utils2.c
void						set_quote_flags(t_lexer *lexer, char quote_char);
char						*create_quoted_content(t_lexer *lexer, int start,
								int end);
char						*handle_word_part_by_type(t_lexer *lexer,
								char *result, char quote_char);

// lexer_token.c
t_token						*next_token_lexer(t_lexer *lexer);
void						free_all_token(t_token *token);

// lexer_token_is.c
t_bool						token_is_redirection(t_token_type type);

// lexer_token_utils.c
t_token						*handle_basics_token(t_lexer *lexer);
t_token						*handle_pipe_and_token(t_lexer *lexer);
t_token						*handle_redir_from_and_to_token(t_lexer *lexer);

// lexer_tokenize.c
t_token						*tokenize(t_ctx *ctx, char *input);

// lexer_utils.c
char						get_lexer(t_lexer *lexer);
void						advance_lexer(t_lexer *lexer);
void						skip_whitespace_lexer(t_lexer *lexer);
void						sync_quote_state(t_ctx *ctx, t_lexer *lexer);

#endif
