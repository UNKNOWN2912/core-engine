#pragma once
#include "Core/Macro.hpp"
#include "Core/Window.hpp"
#include "Renderer/Renderer.hpp"
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

        template<typename Destination> requires std::derived_from<Destination, Layer>
        void Transition();

        template<typename T> requires std::derived_from<T, Layer>
        T& GetLayer();

        void EnableLayer(bool enable) { mEnabled = enable; }
        bool isEnabled() const { return mEnabled; }


        
        using DerivedType = Layer;
    
    protected:
        Window& GetWindow();
        Renderer& GetRenderer();
    private:
        friend class LayerStack;

        void SetLayerData(LayerStack* layerStack, size_t id)
        {
            mLayerStack = layerStack;
            mTypeId = id;
        }

        bool mEnabled = true;
        LayerStack* mLayerStack = nullptr;
        size_t mTypeId = 0;
};

class LayerStack
{
    public:
        template<typename T, typename ...Args> requires std::derived_from<T, Layer>
        void Attach(Args... args)
        {
            assert(Find<T>() == InvalidPosition);

            std::unique_ptr<Layer> layer = std::make_unique<T>(args...);
            SetLayerData<T>(layer);
            layer->OnAttach();
            mLayers.push_back(std::move(layer));
        }

        template<typename T> requires std::derived_from<T, Layer>
        void Detach()
        {
            uint32_t position = Find<T>();
            assert(position != InvalidPosition);

            mLayers[position]->OnDetach();
            mLayers.erase(mLayers.begin() + position);
        }

        template<typename T> requires std::derived_from<T, Layer>
        void SetLayerData(const std::unique_ptr<Layer>& layer)
        {
            layer->SetLayerData(this, typeid(T).hash_code());
        }

        void InvokeEvents(uint32_t code, void* data);
        void InvokeUpdates();

        template<typename Source, typename Destination> requires std::derived_from<Source, Layer> && std::derived_from<Destination, Layer>
        void Transition()
        {
            assert(Find<Destination>() == InvalidPosition);
            assert(Find<Source>() != InvalidPosition);

            uint32_t position = Find<Source>();
            mLayers[position].reset(new Destination);
            SetLayerData<Destination>(mLayers[position]);
        }

        template<typename Destination> requires std::derived_from<Destination, Layer>
        void Transition(const Layer* layer)
        {
            assert(Find<Destination>() == InvalidPosition);
            assert(Find(layer) != InvalidPosition);

            uint32_t position = Find(layer);
            mLayers[position].reset(new Destination);
            SetLayerData<Destination>(mLayers[position]);
        }

        template<typename T> requires std::derived_from<T, Layer>
        const T& Get() const
        {
            uint32_t location = Find<T>();
            assert(location != InvalidPosition);

            return *mLayers[location].get();
        }


        template<typename T> requires std::derived_from<T, Layer>
        T& Get()
        {
            uint32_t location = Find<T>();
            assert(location != InvalidPosition);

            return *(T*)mLayers[location].get();
        }


        template<typename T> requires std::derived_from<T, Layer>
        uint32_t Find()
        {
            for (size_t i = 0; i < mLayers.size(); i++)
            {
                if(mLayers[i]->mTypeId == typeid(T).hash_code())
                    return i;
            }

            return InvalidPosition;
        }

        uint32_t Find(const Layer* layer)
        {
            for (int i = 0; i < mLayers.size(); i++)
            {
                if(mLayers[i].get() == layer)
                    return i;
            }

            return InvalidPosition;
        }

        static const uint32_t InvalidPosition = UINT32_MAX;
    private:
        std::vector<std::unique_ptr<Layer>> mLayers;
};

template<typename T> requires std::derived_from<T, Layer>
T& Layer::GetLayer() 
{
    return mLayerStack->Get<T>();
}
