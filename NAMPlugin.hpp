#pragma once
#include "DistrhoPlugin.hpp"
#include "NAMEngine.hpp"

START_NAMESPACE_DISTRHO

class NAMPlugin final : public Plugin {
public:
    enum { kParamInputGain = 0 };
    NAMPlugin();

protected:
    const char* getLabel()   const noexcept override { return "NAM"; }
    const char* getMaker()   const noexcept override { return "Example"; }
    const char* getLicense() const noexcept override { return "MIT"; }
    uint32_t    getVersion() const noexcept override { return DISTRHO_PLUGIN_VERSION; }
    int64_t     getUniqueId()const noexcept override { return 0x4E414D30; }

    void initParameter(uint32_t index, Parameter& p) override;
    void activate() override;
    void deactivate() override {}
    void run(const float** in, float** out, uint32_t frames) override;

    float getParameterValue(uint32_t index) const override;
    void  setParameterValue(uint32_t index, float value) override;

private:
    NAMEngine engine_;
    float     gainDb_ = 0.0f;
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NAMPlugin)
};

END_NAMESPACE_DISTRHO
