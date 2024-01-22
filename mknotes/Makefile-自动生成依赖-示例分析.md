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

**Makefile中$^和$<的区别**
```
![avatar](https://github.com/nullemp/unpv22e/blob/master/mknotes/images/Makefile%E4%B8%AD$%5E%E5%92%8C$%3C%E7%9A%84%E5%8C%BA%E5%88%AB.png?raw=true)

**错误示例**
```
OBJS := func.o main.o

hello.out : $(OBJS)
	@gcc -o $@ $^
	@echo "Target File => $@"
    
$(OBJS) : %.o : %.c func.h
	@gcc -o $@ -c $^ #error, should use $<

wgg@wgg-MacBookPro:~/unpv22e/mknotes$ make
gcc: fatal error: cannot specify ‘-o’ with ‘-c’, ‘-S’ or ‘-E’ with multiple files
compilation terminated.
make: *** [Makefile:8: func.o] Error 1
wgg@wgg-MacBookPro:~/unpv22e/mknotes$ 
```
**优点**

头文件的更改会更新到相关的源文件中，并更新到最终的目标文件。

**缺点**

* <font color="red">当头文件改动，任何源文件都将被重新编译（编译低效）</font>
* 当项目中头文件数量巨大时，makefile将很难维护

## 解决方法2

* 通过命令自动生成对头文件的依赖
* 将生成的依赖自动包含进makefile中
* 当头文件改动后，自动确认需要重新编译的文件

### 针对解决方案2需要使用的技术：
**(1) linux的sed命令**

sed是一个流编辑器，用于流文本的修改（增/删/改/查)
sed可用于流文本中的字符串替换
sed的字符串替换方式为：sed 's:src:des:g'
例如：
```
echo "test=>abc+abc+abc" | sed 's:abc:xyz:g'
```
test的内容将变为xyz+xyz+xyz

**sed的正则表达式支持**
* 在sed中可以用正则表达式匹配替换目标
* 并且可以使用匹配的目标生成替换结果
例如：
```
sed 's,\(.*\)\.o[ :]*,objs/\1.o : ,g'
wgg@wgg-MacBookPro:~/unpv22e/mknotes$ echo "main.o : main.c main.h"  | sed 's,\(.*\)\.o[ :]*,objs/\1.o : ,g'
objs/main.o : main.c main.h
wgg@wgg-MacBookPro:~/unpv22e/mknotes$
```
**(2) 编译器依赖生成选项gcc -MM(gcc -M)**
* gcc -M des
获取目标des的完整依赖关系
* gcc -MM des
获取目标des的部分依赖关系
(-E 仅对依赖关系做初步解析)
```
wgg@wgg-MacBookPro:~/unpv22e/mknotes$ gcc -M main.c
main.o: main.c /usr/include/stdc-predef.h /usr/include/stdio.h \
 /usr/include/x86_64-linux-gnu/bits/libc-header-start.h \
 /usr/include/features.h /usr/include/features-time64.h \
 /usr/include/x86_64-linux-gnu/bits/wordsize.h \
 /usr/include/x86_64-linux-gnu/bits/timesize.h \
 /usr/include/x86_64-linux-gnu/sys/cdefs.h \
 /usr/include/x86_64-linux-gnu/bits/long-double.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs.h \
 /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
 /usr/lib/gcc/x86_64-linux-gnu/11/include/stddef.h \
 /usr/lib/gcc/x86_64-linux-gnu/11/include/stdarg.h \
 /usr/include/x86_64-linux-gnu/bits/types.h \
 /usr/include/x86_64-linux-gnu/bits/typesizes.h \
 /usr/include/x86_64-linux-gnu/bits/time64.h \
 /usr/include/x86_64-linux-gnu/bits/types/__fpos_t.h \
 /usr/include/x86_64-linux-gnu/bits/types/__mbstate_t.h \
 /usr/include/x86_64-linux-gnu/bits/types/__fpos64_t.h \
 /usr/include/x86_64-linux-gnu/bits/types/__FILE.h \
 /usr/include/x86_64-linux-gnu/bits/types/FILE.h \
 /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h \
 /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
 /usr/include/x86_64-linux-gnu/bits/floatn.h \
 /usr/include/x86_64-linux-gnu/bits/floatn-common.h func.h
wgg@wgg-MacBookPro:~/unpv22e/mknotes$ gcc -MM main.c
main.o: main.c func.h
wgg@wgg-MacBookPro:~/unpv22e/mknotes$ gcc -MM -E main.c
main.o: main.c func.h
wgg@wgg-MacBookPro:~/unpv22e/mknotes$ 
```
**(3)makefile中的include关键字**
类似C语言中的include
将其他文件的内容原封不动的搬入当前文件
语法：include filename
例如：
include foo.make
include *.mk
include $(var)

**make对include关键字的处理方式**

**在当前目录搜索或指定目录搜索目标文件**

搜索成功：将文件内容搬入当前makefile中

搜索失败：产生警告

-- 以文件名作为目标查找并执行相应规则

-- 当文件名对应的规则不存在时，最终产生错误

**示例3-1--include用法--目标文件不存在，目标规则不存在--无操作，直接报错**
```
#b.Makefile
.PHONY : all

include test.txt

all :
    @echo "this is $@"
```

```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
Makefile:3: test.txt: No such file or directory
make: *** No rule to make target 'test.txt'.  Stop.
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```

**示例3-2--include用法--当目标文件不存在，目标规则存在----以文件名查找规则，并执行**

```
#c.Makefile
.PHONY : all

include test.txt

all :
    @echo "this is $@"
    
test.txt :
    @echo "this is $@"
```

```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
this is test.txt
this is all
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```

**示例3-3--include用法--目标文件存在，目标规则存在**
```
#d.Makefile
.PHONY : all

include test.txt

all :
    @echo "this is $@"
    
test.txt :
    @echo "creating $@"
    @echo "other : ; @echo "this is other" " > test.txt
```
**注意第一次执行，此时没有test.txt文件**
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
creating test.txt
this is other
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```

--执行make命令，将执行makefile中的第一条规则
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
this is other
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```
--执行make all命令将执行all规则
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make all
this is all
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```

总结：
If an included makefile cannot be found in any of these directories it is not an immediately fatal error; processing of the makefile containing the include continues. Once it has finished reading makefiles, make will try to remake any that are out of date or don’t exist. See How Makefiles Are Remade. <font color="red">Only after it has failed to find a rule to remake the makefile, or it found a rule but the recipe failed, will make diagnose the missing makefile as a fatal error.</font>

**include 暗黑操作一：**
*  使用减号（-）不但关闭了include发出的警告，同时关闭了错误；当错误发生时make将忽略做这些错误！

**还是使用示例3-1，include不使用（-）会报告所有的错误和警告,加上"-"就不会了**

```
.PHONY : all

-include test.txt

all ：
    @echo "this is $@"
```
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
this is all
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```
**include 暗黑操作二：如果include触发规则创建了文件，之后还会执行规则中的命令，然后重新执行include后的命令**

**下面例子中include执行的规则中不存在依赖；则会将规则直接包含进makefile**
```
.PHONY : all

-include test.txt

all :
    @echo "this is $@"
    
test.txt :
    @echo "creating $@ ..."
    @echo "other"
```
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
creating test.txt ...
other
this is all
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```
**示例3-6-2--include执行时先判断规则是否存在，如果存在会再检查依赖是否是最新的，如果依赖比当前规则要新，会直接执行依赖；否则直接执行规则**
```
#e.Makefile
.PHONY : all

-include test.txt

all :
    @echo "this is $@"
    
test.txt : b.txt
    @echo "creating $@ ..."
    @echo "all : c.txt" > test.txt
```
**目录中没有test.txt和b.txt，直接执行make**
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
this is all
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```
**目录中只有test.txt没有b.txt，直接执行make**
```
touch test.txt
echo "all => a.txt" > test.txt
```
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# touch test.txt
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# echo "all => a.txt" > test.txt
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
this is all
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
make: *** No rule to make target 'b.txt', needed by 'test.txt'.  Stop.
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```

**创建b.txt,直接执行make**
```
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# touch b.txt
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# make
creating test.txt ...
make: *** No rule to make target 'c.txt', needed by 'all'.  Stop.
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# cat test.txt
all : c.txt
root@wgg-MacBookPro:/home/wgg/unpv22e/mknotes# 
```

>include的总结：
* 当目标文件不存在----以文件名查找规则，并执行.

* 当目标文件不存在，且查找到的规则中创建了目标文件----将创建成功的目标文件包含进当前makefile

* 当目标文件存在，将目标文件包含进当前makefile；并以目标文件名查找是否有相应规则--如果有，比较规则的依赖关系，决定是否执行规则的命令；否则，无操作。

* 当目标文件存在，且目标名对应的规则被执行.如果规则中的命令更新了目标文件--make重新包含目标文件，替换之前包含的内容;如果目标文件未被更新，无操作。
 
**(4) makefile中命令的执行机制**
* 规则中的每个命令默认是在一个新的进程中执行（Shell）

* 可以通过接续符（；）将多个命令组合成一个命令

* 组合的命令依次在同一个进程中被执行

* set -e指定发生错误后立即退出执行

**示例4-1--规则中命令的执行--无接续符--规则中的每个命令默认是在一个新的进程中执行**

```
.PHONY : all

all :
    mkdir test
    cd test
    mkdir subtest
```

**示例4-2--规则中命令的执行--接续符--组合的命令依次在同一个进程中被执行**
```
.PHONY : all

all :
    set -e;\
    mkdir test;\
    cd test;\
    mkdir subtest
```
## 解决方案2的初步思路
* 通过gcc -MM和sed得到.dep依赖文件（目标的部分依赖）

    技术点：规则中命令的连续执行

* 通过include指令包含所有的.dep依赖文件

    技术点：当.dep依赖文件不存在时，使用规则自动生成
* 当include发现.dep文件不存在时

    通过规则和命令创建deps文件

    将所有.dep文件创建到deps文件夹

    .dep文件中记录目标文件的依赖关系

代码部分设计如下:

```
$(DIR_DEPS):
    $(MKDIR) $@

$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
    @echo "Creating $@ ..."
    @set -e;\
    $(CC) -MM -E $(filter %.c,$^)) | sed 's,\(.*\)\.o[ :]*,objs/\1.o : ,g' > $@
```

观察上述代码，(DIR_DEPS) %.c 语句存在问题：当第一次执行时会创建deps文件夹和对应的.dep文件，而第二次deps文件夹内容会被新的.dep文件进行更新，导致第一次生成的.dep文件因为依赖更新而会被重复执行。
解决方法如下：使用ifeq动态决定.dep目标的依赖，实现代码如下：
```
ifeq ("$(wildcard $(DIR_DEPS))","")
$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
else
$(DIR_DEPS)/%.dep : %.c
endif
```
示例5：
```
/*func.h*/
#ifndef __FUNC_H
#define __FUNC_H

#define HELLO "hello world"

void foo();

#endif

```
```
/*func.c*/
#include "stdio.h"
#include "func.h"

void foo()
{
    printf("void foo():%s\n",HELLO);    
}
```
```
/*main.c*/
#include "stdio.h"
#include "func.h"

extern void foo();

int main()
{
    foo();

    return 0;
}
```
```
#g.makefile
.PHONY : all clean

MKDIR := mkdir
RM := rm -rf
CC := gcc

DIR_DEPS := deps

SRCS := $(wildcard *.c)
DEPS := $(SRCS:.c=.dep)
DEPS := $(addprefix $(DIR_DEPS)/,$(DEPS))

ifeq ("$(MAKECMDGOALS)","all")
-include $(DEPS)
endif

ifeq ("$(MAKECMDGOALS)","")
-include $(DEPS)
endif

all :
    @echo "$@"
    
$(DIR_DEPS) :
    $(MKDIR) $@
    
ifeq ("$(wildcard $(DIR_DEPS))","")
$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
else
$(DIR_DEPS)/%.dep : %.c
endif

    @echo "Creating $@ ..."
    @set -e;\
    $(CC) -MM -E $(filter %.c,$^) | sed 's,\(.*\)\.o[ :]*,objs/\1.o : ,g' > $@
    
clean :
    $(RM) $(DIR_DEPS)

```

自动生成依赖关系的解决方案：

* 将依赖文件名作为目标加入自动生成的依赖关系中

* 通过include加载依赖文件时判断是否执行规则

* 在规则执行时重新生成依赖关系文件

* 最后加载新的依赖文件

**示例6--最终代码实现：**

```
.PHONY : all clean

MKDIR := mkdir
RM := rm -rf
CC := gcc

DIR_DEPS := deps
DIR_EXES := exes
DIR_OBJS := objs

DIRS:= $(DIR_DEPS) $(DIR_EXES) $(DIR_OBJS)

EXE := app.out
EXE := $(addprefix $(DIR_EXES)/,$(EXE))

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
OBJS := $(addprefix $(DIR_OBJS)/,$(OBJS))
DEPS := $(SRCS:.c=.dep)
DEPS := $(addprefix $(DIR_DEPS)/,$(DEPS))



all : $(DIR_OBJS) $(DIR_EXES) $(EXE)

ifeq ("$(MAKECMDGOALS)","all")
-include $(DEPS)
endif

ifeq ("$(MAKECMDGOALS)","")
-include $(DEPS)
endif

$(EXE) : $(OBJS)
    $(CC) -o $@ $^
    @echo "Success! Target => $@"

$(DIR_OBJS)/%.o : %.c
    $(CC) -o $@ -c $(filter %.c,$^) 

$(DIRS) :
    $(MKDIR) $@

ifeq ("$(wildcard $(DIR_DEPS))","")
$(DIR_DEPS)/%.dep : $(DIR_DEPS) %.c
else
$(DIR_DEPS)/%.dep : %.c
endif

    @echo "Creating $@ ..."
    @set -e;\
    $(CC) -MM -E $(filter %.c,$^) | sed 's,\(.*\)\.o[ :]*,objs/\1.o $@: ,g' > $@

clean :
    $(RM) $(DIRS)
```
小结：
* makefile中可以将目标的依赖拆分写到不同的地方

* include关键字能够触发相应规则的执行

* 如果规则的执行到政治依赖更新，可能导致再次解释执行相应规则

* 依赖文件也需要依赖于源文件得到正确的编译决策

* 自动生成文件间的依赖关系能够提高makefile的移植性


