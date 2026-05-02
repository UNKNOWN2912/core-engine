#include "LayerStack.hpp"
#include "Core/Macro.hpp"

void Layer::Transition(std::shared_ptr<Layer> destination) 
{
    mLayerStack->TransitionLayer(std::shared_ptr<Layer>(this), destination);
}

void Layer::Transfer(std::shared_ptr<Layer> destination) 
{
    mLayerStack->TransferLayer(std::shared_ptr<Layer>(this), destination);
}

void LayerStack::AttachLayer(std::shared_ptr<Layer> layer) 
{
    if(FindLayer(layer) != LayerStack::InvalidPosition)
    {
        ERROR("Layer exist in stack");
        return;
    }

    layer->mLayerStack = this;
    layer->OnAttach();
    mLayers.push_back(layer);    
}

void LayerStack::DetachLayer(std::shared_ptr<Layer> layer) 
{
    uint32_t position = FindLayer(layer);

    if(position == LayerStack::InvalidPosition)
    {
        ERROR("Layer does not exist in stack");
        return;
    }
    
    layer->mLayerStack = nullptr;
    layer->OnDetach();
    mLayers.erase(mLayers.begin() + position);
}

void LayerStack::InvokeLayerEvents(uint32_t code, void* data) 
{
    for (std::shared_ptr<Layer> layer : mLayers)
    {
        if(layer->isEnabled())
        {
            bool handled = layer->OnEvent(code, data);
            if(handled == true)
                return;
        }
    }
}


void LayerStack::InvokeLayerUpdates() 
{
    for (std::shared_ptr<Layer> layer : mLayers)
    {
        if(layer->isEnabled())
            layer->OnUpdate();
    }
}

void LayerStack::TransitionLayer(std::shared_ptr<Layer> source, std::shared_ptr<Layer> destination) 
{
    source->OnDetach();
    destination->OnAttach();
    ReplaceLayer(source, destination);
}

void LayerStack::TransferLayer(std::shared_ptr<Layer> source, std::shared_ptr<Layer> destination) 
{
    ReplaceLayer(source, destination);    
}

void LayerStack::ReplaceLayer(std::shared_ptr<Layer> source, std::shared_ptr<Layer> destination) 
{
    uint32_t sourceLocation = UINT32_MAX;

    for (int i = 0; i < mLayers.size(); i++)
    {
        if(mLayers[i] == source)
            sourceLocation = i;   
    }

    mLayers[sourceLocation] = destination;
}

uint32_t LayerStack::FindLayer(std::shared_ptr<Layer> layer) 
{
    for (int position = 0; position < mLayers.size(); position++)
    {
        if(mLayers[position] == layer)
            return position;   
    }

    return LayerStack::InvalidPosition;
}

