#ifndef REC2_REC2_MACROS_H
#define REC2_REC2_MACROS_H

#define REC2_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#define REC2_ASIZE(ARR)  ((sizeof(ARR))/sizeof(*(ARR)))

#endif // REC2_REC2_MACROS_H
