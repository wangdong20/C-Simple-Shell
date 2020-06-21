# C-Simple-Shell
用C语言实现简单的命令行解析器，支持用户输入命令行并运行在其他的进程中。该命令行解析器可以运行在任何Linux或Mac系统。

每行命令有osh> 开始，然后由用户输入命令行，例如cat prog.c，该命令将prog.c文件展示于终端上。
```
osh> cat prog.c
```
同时也支持&修饰命令行，使命令进入后台运行。本质上就是使命令行在子进程运行于后台，或者子进程和父进程同时运行。

这个project主要分成两部分，一部分是解析用户命令并使其在子进程运行，另一部分是支持我们自己命令行终端特有的history功能。即：当用户输入history命令，系统将展示最近使用的10条命令。

例如：当前history里面存储了6条输入过的命令（从最近使用的到最远使用的顺序排列）

ps -l, ls -l, top, date, cal, whoami

当输入history命令后，系统将会输出
```
6 ps -l

5 ls -l

4 top

3 date

2 cal

1 whoami
```
当用户输入!!命令，就运行最近运行的命令，在当前环境下也就是ps -l，

当用户输入!N命令，N代表数字，就运行第N个命令在history中，!2就运行cal.

如果输入!!命令时没有命令在history中，就输出No command in history.

如股票输入!N命令没有对应第N个命令在history中，就输出No such command in history.

用户输入exit将退出当前命令行解析器。

## 实现
我在实现这个命令行解析器，基本上分为两部分，一部分是解析用户输入的命令，然后在子进程中调用

execvp(char* command, char* params[]).

例如：当用户输入ps -ael命令，系统将会把命令解析到args[]字符串数组中，args[] = {"ps", "-ael", NULL}

运行时就调用execvp(args[0], args); 另外要注意解析命令里是否有&在最后，如果有&在命令最后，那么父进程就要wait子进程exit。
