##基于跳表实现的轻量级键型值存储引擎，使用c++实现，支持插入、查找、删除、展示、落盘和加载等功能。


# 项目中文件
 bin 生成可执行文件目录 
 src 跳表实现的源码存放文件夹
 store 数据落盘存放的文件夹
 stress_test 压力测试实现文件夹
 makefile 编译脚本


# 提供接口

* insertElement（插入数据）
* deleteElement（删除数据）
* searchElement（查询数据）
* displayList（展示已存数据）
* dumpFile（数据落盘）
* loadFile（加载数据）
* get_size（返回数据规模）


#项目运行方式
```
make skiplist  	//complie
./bin/main 		//run
```



#压力测试
```
make test

```



##该项目是根据Carl哥的项目，改成自己喜欢的文件格式（应该自己不太喜欢将类声明和定义放在同一个文件中，显得##很乱QAQ哈哈哈。
	
##一边看卡哥的项目，理解，一边自己尝试去实现这些接口，不得不说， 卡哥yyds

##他将跳表实现的核心代码放在了一个头文件中

##我把跳表的类模板定义和声明放在了两个文件中，在编译的时候一直会出现一下的问题，查了很久都没有查出问题所在

/usr/bin/ld: main.o: in function `main':
main.cpp:(.text+0x2f): undefined reference to `skip_list<int, int>::skip_list(int)'
/usr/bin/ld: main.cpp:(.text+0x48): undefined reference to `skip_list<int, int>::insert_element(int, int)'
/usr/bin/ld: main.cpp:(.text+0x61): undefined reference to `skip_list<int, int>::insert_element(int, int)'
/usr/bin/ld: main.cpp:(.text+0x7a): undefined reference to `skip_list<int, int>::insert_element(int, int)'
/usr/bin/ld: main.cpp:(.text+0x93): undefined reference to `skip_list<int, int>::insert_element(int, int)'
/usr/bin/ld: main.cpp:(.text+0xac): undefined reference to `skip_list<int, int>::insert_element(int, int)'
/usr/bin/ld: main.o:main.cpp:(.text+0xc5): more undefined references to `skip_list<int, int>::insert_element(int, int)' follow
/usr/bin/ld: main.o: in function `main':
main.cpp:(.text+0x103): undefined reference to `skip_list<int, int>::get_size() const'
/usr/bin/ld: main.cpp:(.text+0x131): undefined reference to `skip_list<int, int>::dump_file()'
/usr/bin/ld: main.cpp:(.text+0x145): undefined reference to `skip_list<int, int>::search_element(int)'
/usr/bin/ld: main.cpp:(.text+0x154): undefined reference to `skip_list<int, int>::display_list()'
/usr/bin/ld: main.cpp:(.text+0x163): undefined reference to `skip_list<int, int>::~skip_list()'
/usr/bin/ld: main.cpp:(.text+0x18f): undefined reference to `skip_list<int, int>::~skip_list()'
collect2: error: ld returned 1 exit status
make: *** [Makefile:5: skiplist] Error 1


##最后在https://www.learncpp.com/cpp-tutorial/template-classes/找到了这个问题的原因和解决办法。

A template is not a class or a function -- it is a stencil used to create classes or functions. As such, it does not work in quite the same way as normal functions or classes. In most cases, this isn’t much of a issue. However, there is one area that commonly causes problems for developers.

With non-template classes, the common procedure is to put the class definition in a header file, and the member function definitions in a similarly named code file. In this way, the source for the class is compiled as a separate project file. However, with templates, this does not work. 

In order for the compiler to use a template, it must see both the template definition (not just a declaration) and the template type used to instantiate the template. Also remember that C++ compiles files individually. When the Array.h header is #included in main, the template class definition is copied into main.cpp. When the compiler sees that we need two template instances, Array<int>, and Array<double>, it will instantiate these, and compile them as part of main.cpp. However, when it gets around to compiling Array.cpp separately, it will have forgotten that we need an Array<int> and Array<double>, so that template function is never instantiated. Thus, we get a linker error, because the compiler can’t find a definition for Array<int>::getLength() or Array<double>::getLength().

There are quite a few ways to work around this.

The easiest way is to simply put all of your template class code in the header file (in this case, put the contents of Array.cpp into Array.h, below the class). In this way, when you #include the header, all of the template code will be in one place. The upside of this solution is that it is simple. The downside here is that if the template class is used in many places, you will end up with many local copies of the template class, which can increase your compile and link times (your linker should remove the duplicate definitions, so it shouldn’t bloat your executable). This is our preferred solution unless the compile or link times start to become a problem.

If you feel that putting the Array.cpp code into the Array.h header makes the header too long/messy, an alternative is to move the contents of Array.cpp to a new file named Array.inl (.inl stands for inline), and then include Array.inl at the bottom of the Array.h header (inside the header guard). That yields the same result as putting all the code in the header, but helps keep things a little more organized.

##大致意思就是用.inl文件（表示内联文件)来代替.cpp文件，在.inl文件中多模板进行定义，并且需要在.h文件的#endif前将 .inl文件包含进来