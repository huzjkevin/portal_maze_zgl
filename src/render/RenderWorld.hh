#pragma once

#include "../forward.hh"

#include "Graph.hh"

class RenderWorld
{
private:
    static SharedRenderWorld instance;

    SharedRoot root;
    std::vector<SharedPortal> portals;

    std::vector<SharedPointLight> pointLights;
    std::vector<SharedSpotLight> spotLights;
    std::vector<SharedGlobalLight> globalLights;

public:
    RenderWorld() : root() { root = std::make_shared<Root>(); }
    ~RenderWorld() {}

    static SharedRenderWorld getInstance() { return instance; }

    static void initInstance();
    static void reset();

    void init();

    SharedRoot getRoot() { return root; }
    std::vector<SharedPortal>* getPortals() { return &portals; }

    virtual size_t getPortalCount() const { return portals.size(); }
    virtual SharedPortal getPortal(size_t index) { return portals[index]; }

    virtual void insertPortal(size_t index, SharedPortal portal) { portals.insert(portals.begin() + index, portal); }

    virtual void addPortal(SharedPortal portal) { portals.push_back(portal); }

    virtual void removePortal(size_t index) { portals.erase(portals.begin() + index); }

    virtual void removePortal(SharedPortal portal)
    {
        std::vector<SharedPortal>::iterator iterator = std::find(portals.begin(), portals.end(), portal);
        if (iterator != portals.end())
        {
            portals.erase(iterator);
        }
    }

    virtual size_t indexOf(SharedPortal portal) {
        std::vector<SharedPortal>::iterator iterator = std::find(portals.begin(), portals.end(), portal);
        return std::distance(portals.begin(), iterator);
    }

    virtual void clearPortals() { portals.clear(); }

    virtual std::vector<SharedPointLight>* getPointLights() { return &pointLights; }
    virtual std::vector<SharedSpotLight>* getSpotLights() { return &spotLights; }
    virtual std::vector<SharedGlobalLight>* getGlobalLights() { return &globalLights; }
};
