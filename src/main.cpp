#include "Global.h"
#include "Logger.h"

#include "GLInjectInput.h"

int main()
{
    Logger logger;
    GLInjectInput gii("", false, false, false, 0);
    gii.SetCapturing(true);
    while (1)
    {
        sleep(10);
    }
    return 0;
}
