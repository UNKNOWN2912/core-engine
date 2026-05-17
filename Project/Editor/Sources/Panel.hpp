#pragma once
#include <memory>
#include <unordered_map>

#define GetTypeId(type) typeid(type).hash_code()

class Panel
{
    public:
        virtual void OnAttach(){}
        virtual void OnUpdate() {}
        virtual void OnRenderUi() {}
        virtual void OnDetach() {}

        void Enable(bool enable) { mEnabled = enable; }
        bool IsEnabled() { return mEnabled; }

        void SetTitle(std::string_view title) { mTitle = title; }
        void SetIcon(char icon) { mIcon = icon; }

        char GetIcon() const { return mIcon; }
        const std::string& GetTitle() const { return mTitle;}

    protected:
        std::string mTitle = "Untitled";
        char mIcon = 'W';
        bool mEnabled = true;
};

class PanelManager
{
    public:
        template<typename T, typename ...Args>
        void AddPanel(Args ...args)
        {
            mPanels[GetTypeId(T)] = std::make_shared<T>(args...);
            mPanels[GetTypeId(T)]->OnAttach();
        }

        template<typename T>
        void RemovePanel()
        {
            mPanels[GetTypeId(T)]->OnDetach();
            mPanels[GetTypeId(T)].reset();
        }

        template<typename T>
        T* GetPanel()
        {
            return (T*)mPanels[GetTypeId(T)].get();
        }

        void InvokeOnUpdate()
        {
            for(auto [id, panel] : mPanels)
            {
                if(panel->IsEnabled())
                    panel->OnUpdate();
            }
        }

        void InvokeOnRenderUi()
        {
            for(auto [id, panel] : mPanels)
            {
                if(panel->IsEnabled())
                    panel->OnRenderUi();
            }
        }

        std::unordered_map<size_t, std::shared_ptr<Panel>> GetPanelMap()
        {
            return mPanels;
        }
    private:
        std::unordered_map<size_t, std::shared_ptr<Panel>> mPanels;
};