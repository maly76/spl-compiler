/*
 * memory.h -- memory management
 */

#ifndef SPL_MEMORY_H
#define SPL_MEMORY_H

void *allocate(unsigned size);
void release(void *p);

#endif /* SPL_MEMORY_H */
