#pragma once
#include "../core/graph.h"
#include "../utils/camera.h"

struct RenderOptions {
    bool showEdges   = true;
    bool showBounds  = true;
    bool haloHover   = true;
};

class GraphRenderer {
public:
    void render(const Graph& graph, const Camera3D& cam, int w, int h,
                int hoveredId, const RenderOptions& ro);
private:
    static void drawSphere(float r, int stacks, int slices);
    static void beginLighting();
    static void endLighting();
    static void drawBounds(float B);
    static void setColorByState(NodeState st);
};
