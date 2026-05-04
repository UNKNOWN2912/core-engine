#include "LayerStack.hpp"
#include <Core/Application.hpp>

void LayerStack::InvokeEvents(uint32_t code, void* data) 
{
    for (const std::unique_ptr<Layer>& layer : mLayers) 
    {
        if (layer->isEnabled())
            layer->OnEvent(code, data);
    }
}

void LayerStack::InvokeUpdates() 
{
    for (const std::unique_ptr<Layer>& layer : mLayers) 
    {
        if (layer->isEnabled())
            layer->OnUpdate();
    }
}

template<typename Destination> requires std::derived_from<Destination, Layer>
void Layer::Transition()
{
    mLayerStack->Transition<Destination>(this);
}


Window& Layer::GetWindow() 
{
    return Application::GetInstance()->GetWindowRef();
}

Renderer& Layer::GetRenderer() 
{
    return Application::GetInstance()->GetRendererRef();    
}

