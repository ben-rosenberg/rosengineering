#include "SequencerWidget.hpp"

struct NumStepsPerMeasureItem : rack::ui::MenuItem
{
    SemitoneSequencer *module;
    int numSteps;
    void onAction(const event::Action &event) override
    {
        module->numStepsPerMeasure = numSteps + 1;
    }
};

struct NumStepsPerMeasureChoice : rack::app::LedDisplayChoice
{
    SemitoneSequencer *module;
    void onAction(const event::Action &event) override
    {
        if (!(module))
            return;

        ui::Menu *menu = createMenu();
        menu->addChild(createMenuLabel("Number of steps per measure"));

        for (int numSteps = 0; numSteps < 8; ++numSteps)
        {
            NumStepsPerMeasureItem *item = new NumStepsPerMeasureItem;
            item->module = module;
            item->numSteps = numSteps + 1;
            // item->text = std::to_string(module->numStepsPerMeasure);
            item->text = std::to_string(numSteps + 1);
            item->rightText = CHECKMARK(numSteps == module->numStepsPerMeasure);
            menu->addChild(item);
        }
    }

    void step() override
    {
        // text = (box.size.x >= 200.0f) ? "Steps per measure: " : "";
        text = "Steps per measure: ";
        std::string numStepsName = (module) ? std::to_string(module->numStepsPerMeasure) : "";
        if (numStepsName != "")
        {
            text += numStepsName;
            color.a = 1.0f;
        }
        else
        {
            text += "ERROR";
            color.a = 0.5f;
        }
    }
};

struct NumMeasuresItem : ui::MenuItem
{
    SemitoneSequencer *module;
    int numMeasures;
    void onAction(const event::Action &event) override
    {
        module->numMeasures = numMeasures;
    }
};

struct NumMeasuresChoice : app::LedDisplayChoice
{
    SemitoneSequencer *module;
    void onAction(const event::Action &event) override
    {
        if (!(module))
            return;

        ui::Menu *menu = createMenu();
        menu->addChild(createMenuLabel("Number of measures"));

        for (int numMeasures = 0; numMeasures < 4; ++numMeasures)
        {
            NumMeasuresItem *item = new NumMeasuresItem;
            item->module = module;
            item->numMeasures = numMeasures;
            item->text = std::to_string(numMeasures);
            item->rightText = CHECKMARK(numMeasures == module->numMeasures);
            menu->addChild(item);
        }
    }

    void step() override
    {
        text = "Number of measures: ";
        std::string numMeasuresName = (module) ? std::to_string(module->numMeasures) : "";
        if (numMeasuresName != "")
        {
            text += numMeasuresName;
            color.a = 1.0f;
        }
        else
        {
            text += "ERROR";
            color.a = 0.5f;
        }
    }
};

struct ChordModeItem : ui::MenuItem
{
    SemitoneSequencer *module;
    int chordMode;
    void onAction(const event::Action &event) override
    {
        module->chordMode = chordMode;
    }
};

struct ChordModeChoice : app::LedDisplayChoice
{
    SemitoneSequencer *module;
    void onAction(const event::Action &event) override
    {
        if (!(module))
            return;

        ui::Menu *menu = createMenu();
        menu->addChild(createMenuLabel("Chord mode"));

        for (int chordMode = 0; chordMode < 2; ++chordMode)
        {
            ChordModeItem *item = new ChordModeItem;
            item->module = module;
            item->chordMode = chordMode;
            item->text = (chordMode) ? "On" : "Off";
            item->rightText = CHECKMARK(chordMode == module->chordMode);
            menu->addChild(item);
        }
    }

    void step() override
    {
        text = "Chord mode: ";
        std::string chordModeName = (module) ? ((module->chordMode) ? "On" : "Off") : "";
        if (chordModeName != "")
        {
            text += chordModeName;
            color.a = 1.0f;
        }
        else
        {
            text += "ERROR";
            color.a = 0.5f;
        }
    }
};

struct QuantizationModeItem : ui::MenuItem
{
};

void SequencerWidget::setWidget(SemitoneSequencer *semitoneSequencer)
{
    clearChildren();

    TAR::Math::Vec2D<float> pos;

    NumStepsPerMeasureChoice *numStepsChoice = createWidget<NumStepsPerMeasureChoice>(pos);
    numStepsChoice->box.size.x = box.size.x / 2;
    numStepsChoice->module = semitoneSequencer;
    addChild(numStepsChoice);
    (rack::math::Vec) pos = numStepsChoice->box.getBottomLeft();
    this->numStepsPerMeasureChoice = numStepsChoice;

    this->numStepsPerMeasureSeparator = createWidget<LedDisplaySeparator>(pos);
    this->numStepsPerMeasureSeparator->box.pos.x = box.size.x / 2;
    this->numStepsPerMeasureSeparator->box.size.y = this->numStepsPerMeasureChoice->box.size.y;
    addChild(this->numStepsPerMeasureSeparator);

    NumMeasuresChoice *numMeasuresChoice = createWidget<NumMeasuresChoice>(pos);
    numMeasuresChoice->box.pos.x = box.size.x / 2;
    numMeasuresChoice->box.size.x = box.size.x / 2;
    numMeasuresChoice->module = semitoneSequencer;
    addChild(numMeasuresChoice);
}