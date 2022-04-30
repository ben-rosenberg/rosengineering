#pragma once

#include "plugin.hpp"
#include "app/SvgSwitch.hpp"
#include "app/SvgKnob.hpp"
#include "componentlibrary.hpp"

#include "../plugin.hpp"

using namespace rack;

namespace TAR
{

namespace Components
{

struct HorizontalSwitchThree : app::SvgSwitch
{
    HorizontalSwitchThree()
    {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/HorizontalSwitchThree_1.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/HorizontalSwitchThree_2.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/HorizontalSwitchThree_3.svg")));
    }
};

struct FancyBlackKnob : app::SvgKnob
{
    FancyBlackKnob()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/FancyBlackKnob.svg")));
    }
};

struct FancyBlackSnapKnob : app::SvgKnob
{
    FancyBlackSnapKnob()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/FancyBlackKnob.svg")));
        snap = true;
    }
};

struct MyTrimpot : app::SvgKnob
{
    MyTrimpot()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::system("res/ComponentLibrary/Trimpot.svg")));
    }
};

struct MySnapTrimpot : app::SvgKnob
{
    MySnapTrimpot()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::system("res/ComponentLibrary/Trimpot.svg")));
        snap = true;
    }
};

struct LightKnob : app::SvgKnob
{
    LightKnob()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/LightKnob.svg")));
    }
};

struct LightKnob2 : app::SvgKnob
{
    LightKnob2()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/LightKnob2.svg")));
    }
};

struct BlackScrew : app::SvgScrew
{
    BlackScrew()
    {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/BlackScrew.svg")));
    }
};

struct Port1 : app::SvgPort
{
    Port1()
    {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/Port1.svg")));
    }
};

struct EmptyCenterKnob : app::SvgKnob
{
    EmptyCenterKnob()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/EmptyCenterKnob.svg")));
    }
};

struct EmptyCenterSnapKnob : app::SvgKnob
{
    EmptyCenterSnapKnob()
    {
        minAngle = -0.83 * M_PI;
        maxAngle = 0.83 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/components/EmptyCenterKnob.svg")));
        snap = true;
    }
};

template <typename TBase = GrayModuleLightWidget>
struct TBlueWhiteLight : TBase
{
    TBlueWhiteLight()
    {
        this->addBaseColor(SCHEME_BLUE);
        this->addBaseColor(SCHEME_WHITE);
    }
};
typedef TBlueWhiteLight<> BlueWhiteLight;

struct NonMomentaryLEDBezel : app::SvgSwitch
{
    NonMomentaryLEDBezel()
    {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::system("res/ComponentLibrary/LEDBezel.svg")));
    }
};

} // Components
} // TAR