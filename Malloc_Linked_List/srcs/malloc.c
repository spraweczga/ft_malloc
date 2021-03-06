/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: psprawka <psprawka@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/30 16:21:16 by psprawka          #+#    #+#             */
/*   Updated: 2018/10/07 14:30:20 by psprawka         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

t_	*g_tags_tree = NULL;


//----------------------------------------------------------------------------------------

void	map_memory(t_malloc *malloc_info)
{
	if (IS_TINY(malloc_info->size))
		count_size(100 * TINY, malloc_info);
	else if (IS_SMALL(malloc_info->size))
		count_size(100 * SMALL, malloc_info);
	else
		count_size(malloc_info->size + sizeof(t_segment_tag), malloc_info);

	malloc_info->mptr = mmap(NULL, malloc_info->mapped_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}

/*
**	distribute_memory:
**	this function maps memory; first, it traverses through the tree and tries to
**	find a free space where our new size malloc would fit. If it's positive,
**	reuse_tag will be called, will change tag->size on freespace addres to size
**	and set mapped_size to total previous avaliable free space. If node wasn't found,
**	function uses mmap to create a new map based on the size, then section's tag
**	is inserted with size 'size' and flag USED on freespace. Then I insert new
**	free tag on size + header with full avaliable size without desired 'size
**	with flag FREE. Function returns a pointer pointing at section's tag.
*/

void	distribute_memory(t_malloc *malloc_info)
{
	t_rbnode		*free_tag;
	t_segment_tag	*page_tag;

	if ((malloc_info->mptr = find_free_node(malloc_info)))
	{
		printf("REUSING TAG\n");
		reuse_tag(malloc_info);
	}
	else
	{
		printf("MAPPING...\n");
		map_memory(malloc_info);
		printf("%sMAPPED! %p%s\n", YELLOW, malloc_info->mptr, NORMAL);
		add_next_page(malloc_info);
		printf("PAGE ADDED!\n");
		page_tag = (t_segment_tag *)malloc_info->mptr;
		page_tag->pages = malloc_info->pages;
 		malloc_info->mptr += sizeof(t_segment_tag);
		malloc_info->mapped_size -= sizeof(t_segment_tag); //this and below - fix logic later (add tag_size to size at the beginning)
		if (!(malloc_info->mapped_size > (malloc_info->size + sizeof(t_rbnode) + sizeof(t_rbnode))))
		{
			malloc_info->size = malloc_info->size + (malloc_info->mapped_size - malloc_info->size - sizeof(t_rbnode));
			printf("SIZE CHANGED\n");
		}
			
		insert_tag(malloc_info, malloc_info->mptr - sizeof(t_segment_tag), false);
		malloc_info->mapped_size -= sizeof(t_rbnode);
	}
	printf("STARTING TO ADD A FREE TAG, MPTR: %p\n", malloc_info->mptr);
	free_tag = (t_rbnode *)malloc_info->mptr;
	if (malloc_info->mapped_size > (free_tag->size + sizeof(t_rbnode)))
	{
		size_t idk = malloc_info->size;
		malloc_info->size = malloc_info->mapped_size - free_tag->size - sizeof(t_rbnode);
		insert_tag(malloc_info, free_tag->head, true);
	}
	printf("FREE TAG ADDED!\n");
}


void	*ft_malloc(size_t size)
{
	t_malloc	malloc_info;

	if (size == 0)
		return (NULL);

	printf("%sALL SIZES: t_rbnode %lx | t_segment_tag %lx%s\n", ORANGE, sizeof(t_rbnode), sizeof(t_segment_tag), NORMAL);
	
	ft_bzero(&malloc_info, sizeof(t_malloc));
	malloc_info.size = size;
	distribute_memory(&malloc_info);
	
	printf("done with distributing memory!\n");
	// print_tree(g_tags_tree);
	// show_alloc_mem();
	return (malloc_info.mptr += sizeof(t_rbnode));
}


	
int		main(void)
{
	char *ptr;
	char *ptr1;
	char *ptr2 = NULL;

	// ptr = strcpy(ptr, "hello");
	// ptr2 = strcpy(ptr2, "adam sucks");
	// ft_printf("[%s][%s]\n", ptr, ptr2);
	return (0);
}
