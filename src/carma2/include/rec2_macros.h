#ifndef REC2_REC2_MACROS_H
#define REC2_REC2_MACROS_H

#define REC2_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#endif // REC2_REC2_MACROS_H
