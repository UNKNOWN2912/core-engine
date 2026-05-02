#pragma once
#include <memory>
#include <vector>

class LayerStack;

class Layer
{
    public:
        virtual void OnAttach() {}
        virtual void OnUpdate() {}
        virtual void OnDetach() {}
        virtual bool OnEvent(uint32_t code, void* data) {return false; }

        void Transition(std::shared_ptr<Layer> source);
        void Transfer(std::shared_ptr<Layer> destination);
        void EnableLayer(bool enable) { mEnabled = enable; }

        bool isEnabled() const { return mEnabled; }
    private:
        friend class LayerStack;
        bool mEnabled = true;
        LayerStack* mLayerStack = nullptr;
};

class LayerStack
{
    public:
        void AttachLayer(std::shared_ptr<Layer> layer);
        void DetachLayer(std::shared_ptr<Layer> layer);
        void InvokeLayerEvents(uint32_t code, void* data);
        void InvokeLayerUpdates();

        void TransitionLayer(std::shared_ptr<Layer> source, std::shared_ptr<Layer> destination);
        void TransferLayer(std::shared_ptr<Layer> source, std::shared_ptr<Layer> destination);
        void ReplaceLayer(std::shared_ptr<Layer> source, std::shared_ptr<Layer> destination);


        uint32_t FindLayer(std::shared_ptr<Layer> layer);

        static const uint32_t InvalidPosition = UINT32_MAX;
    private:
        std::vector<std::shared_ptr<Layer>> mLayers;
};