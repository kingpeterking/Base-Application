#pragma once

class ImPlotClient
{
public:
    ImPlotClient();
    ~ImPlotClient();

    // Render the ImPlot demo window
    void RenderDemoWindow(bool* isOpen);
};
