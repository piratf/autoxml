###简介

本项目的目标是简化 xml 的读取过程，把冗长的读取 xml 过程简化到一行解决

###项目特性：
* 支持自动类型推导赋值
* 支持任意变长路径搜索
* 支持使用相对路径进行节点间跳转
* 支持自动打印错误和调试信息
* 使用原生 xml 规范，不需要额外 xml 规则
* C++98 可用。**当前实现依赖 tinyxml**

###使用方法
    在项目中使用 "include/autoxml.h" 文件即可
    语法参见 test/*Test.cpp


    include "include/autoxml.h" in your project to read xml easily.
    check test/*Test.cpp to get usage.