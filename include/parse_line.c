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

t_tok_limit	parse_line(t_ctx ctx, char *line)
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
