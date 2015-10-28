**2014.3.7更新：**
能生成图形化的Inter-procedure CFG,文件名统一为ProgramCFG.dot.在linux下可以用xdot等第三方工具打开文件查看。

**本次更新：**
删除无关的功能，这个llvm pass的目的是生成程序的控制流图（CFG），并做可达性分析。
使用方法查看buildCFG.cpp的doInitialization()函数。


 
