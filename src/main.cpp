#include "pch.h"
#include "Application.h"

int main(int, char**)
{
    Application app;

    if (!app.Initialize())
        return 1;

    app.Run();
    app.Shutdown();

    return 0;
}
