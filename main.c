#include "javac.h"
#include "fsm.h"

void fsm_init();

int main(int argc, char *argv[])
{
    // 初始化状态机
    fsm_init();
    // 编译文件
    compile_file("./HelloWorld.java", "./HelloWorld.class", 0);
    return 0;
}

void fsm_init()
{
    fsm_signal_symbol_init();
    fsm_identifier_init();
    fsm_string_init();
}