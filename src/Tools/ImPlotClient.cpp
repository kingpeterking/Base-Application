#include "pch.h"
#include "Tools/ImPlotClient.h"

ImPlotClient::ImPlotClient()
{
}

ImPlotClient::~ImPlotClient()
{
}

void ImPlotClient::RenderDemoWindow(bool* isOpen)
{
    ImPlot::ShowDemoWindow(isOpen);
}
