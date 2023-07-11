#include "javac.h"

int main(int argc, char *argv[])
{
    compile_file("./HelloWorld.java", "./HelloWorld", 0);
    return 0;
}