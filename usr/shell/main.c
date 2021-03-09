#include <stdio.h>

int main(void)
{
    char pwd[] = "/";

    while(1)
    {
        printf(COLOR_GREEN"gipsyh@Lanthorn"COLOR_NONE":"COLOR_BLUE"%s$ "COLOR_NONE, pwd);
        while(1);
    }
}