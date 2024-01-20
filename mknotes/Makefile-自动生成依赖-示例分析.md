# Makefile 自动生成依赖关系
## 1. 编译行为带来的缺陷
* 预处理器将头文件中的代码直接插入源文件
* 编译器只通过预处理后的源文件产生目标文件
* 因此，规则中以源文件为依赖，命令可能无法执行

**示例1**观察以下makefile文件是否正确:当修改func.h中宏HELLO的内容后，执行make命令发现，编译器无法更新main.c和func.c，进而无法更新执行的结果：原因在于func.h中更新的内容无法自动更新到func.c和main.c文件中，进而导致编译的hello.out文件结果无任何变化。

**func.h**
```
#ifndef FUNC.H
#define FUNC.H

#define HELLO "hello makefile"

void foo();

#endif
```

**func.c**
```
#include "stdio.h"
#include "func.h"

void foo()
{
    printf("void foo():%s\n",HELLO);
    
}
```
**main.c**
```
#include "stdio.h"
#include "func.h"

extern void foo();

int main()
{
    foo();
    
    return 0;
}
```

**Makefile**
```
OBJS := func.o main.o

hello.out := $(OBJS)
    @gcc -o $@ $^
    @echo "Target File => $@"
    
$(OBJS) : %.o : %.c
    @gcc -o $@ -c $^
```
**运行结果**
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
In file included from func.c:2:
func.h:1:13: warning: extra tokens at end of #ifndef directive
    1 | #ifndef FUNC.H
      |             ^
func.h:2:9: warning: ISO C99 requires whitespace after the macro name
    2 | #define FUNC.H
      |         ^~~~
In file included from main.c:2:
func.h:1:13: warning: extra tokens at end of #ifndef directive
    1 | #ifndef FUNC.H
      |             ^
func.h:2:9: warning: ISO C99 requires whitespace after the macro name
    2 | #define FUNC.H
      |         ^~~~
Target File => hello.out
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# ./hello.out 
void foo():hello makefile
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
make: 'hello.out' is up to date.
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# ./hello.out 
void foo():hello makefile
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```
## 解决方法1：修改makefile文件--将头文件作为依赖条件出现于每个目标对应的规则中
```
OBJS := func.o main.o

hello.out : $(OBJS)
    @gcc -o $@ $^
    @echo "Target File => $@"
    
$(OBJS) : %.o : %.c func.h
    @gcc -o $@ -c $<
```
**运行结果**
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
Target File => hello.out
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# ./hello.out 
void foo():hello makefile!!!
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
Target File => hello.out
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# ./hello.out
void foo():hello makefile!!
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```
