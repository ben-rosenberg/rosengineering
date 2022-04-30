#include "plugin.hpp"
#include "common/CVQuantizer.hpp"

struct GuideQuant : Module 
{
	enum ParamId 
	{
		DEFAULT_ROOT_NOTE_PARAM,
		DEFAULT_SCALE_PARAM,
		RANGE_PARAM,
		PARAMS_LEN
	};
	enum InputId 
	{
		GUIDE_INPUT,
		UNQUANTIZED_INPUT,
		INPUTS_LEN
	};
	enum OutputId 
	{
		QUANTIZED_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId 
	{
		LIGHTS_LEN
	};

	GuideQuant() 
	{
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DEFAULT_ROOT_NOTE_PARAM, 0.0f, 11.0f, 0.0f, "Default Root Note");
		configParam(DEFAULT_SCALE_PARAM, 0.f, 3.f, 0.f, "Default Scale");
		configParam(RANGE_PARAM, 0.f, 1.f, 1.f, "Range", "%", 0.0f, 100.0f);
		configInput(GUIDE_INPUT, "Polyphonic guide input");
		configInput(UNQUANTIZED_INPUT, "Input to be quantized");
		configOutput(QUANTIZED_OUTPUT, "Quantized output");
	}

	CVGuidedQuantizer quant;

	float inputToBeQuantized;
	uint8_t noteNumber;
	uint8_t scaleNumber;
	uint8_t numGuideNotes;
	float* guideNotesArray; 

	void process(const ProcessArgs& args) override 
	{
		if (!(inputs[UNQUANTIZED_INPUT].isConnected()) || !(inputs[QUANTIZED_OUTPUT].isConnected()))
			return;

		inputToBeQuantized = inputs[UNQUANTIZED_INPUT].getVoltage() * params[RANGE_PARAM].getValue();
		noteNumber = static_cast<uint8_t>(params[DEFAULT_ROOT_NOTE_PARAM].getValue());
		scaleNumber = static_cast<uint8_t>(params[DEFAULT_SCALE_PARAM].getValue());

		if (inputs[GUIDE_INPUT].isConnected())
		{
			numGuideNotes = inputs[GUIDE_INPUT].getChannels();
			guideNotesArray = inputs[GUIDE_INPUT].getVoltages();
		}
		else
		{
            
		}

		quant.SetAllowedNotes(guideNotesArray, numGuideNotes);
		outputs[QUANTIZED_OUTPUT].setVoltage(quant.quantize(inputToBeQuantized));
	}
};


struct GuideQuantWidget : ModuleWidget {
	GuideQuantWidget(GuideQuant* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/GuideQuant.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(20.32, 23.379)), module, GuideQuant::DEFAULT_ROOT_NOTE_PARAM));
		addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(20.32, 48.423)), module, GuideQuant::DEFAULT_SCALE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.32, 72.964)), module, GuideQuant::RANGE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.32, 94.366)), module, GuideQuant::GUIDE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.15, 109.217)), module, GuideQuant::UNQUANTIZED_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.45, 109.217)), module, GuideQuant::QUANTIZED_OUTPUT));
	}
};


Model* modelGuideQuant = createModel<GuideQuant, GuideQuantWidget>("GuideQuant");