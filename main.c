#include "javac.h"

int main(int argc, char *argv[])
{
    compile_file("./HelloWorld.java", "./HelloWorld.class", 0);
    return 0;
}