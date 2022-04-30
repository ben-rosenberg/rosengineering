#include <string>
#include "plugin.hpp"


struct PolyOsc : Module {
	enum ParamId {
		OCT_OSC_1_PARAM,
		OCT_OSC_2_PARAM,
		OCT_OSC_3_PARAM,
		OCT_OSC_4_PARAM,
		SEMI_OSC_1_PARAM,
		SEMI_OSC_2_PARAM,
		SEMI_OSC_3_PARAM,
		SEMI_OSC_4_PARAM,
		FINE_OSC_1_PARAM,
		FINE_OSC_2_PARAM,
		FINE_OSC_3_PARAM,
		FINE_OSC_4_PARAM,
		WAVEFORM_OSC_1_PARAM,
		WAVEFORM_OSC_2_PARAM,
		WAVEFORM_OSC_3_PARAM,
		WAVEFORM_OSC_4_PARAM,
		FM_1_MOD_1_PARAM,
		FM_1_MOD_2_PARAM,
		FM_1_MOD_3_PARAM,
		FM_1_MOD_4_PARAM,
		FM_2_MOD_1_PARAM,
		FM_2_MOD_2_PARAM,
		FM_2_MOD_3_PARAM,
		FM_2_MOD_4_PARAM,
		FM_3_MOD_1_PARAM,
		FM_3_MOD_2_PARAM,
		FM_3_MOD_3_PARAM,
		FM_3_MOD_4_PARAM,
		FM_4_MOD_1_PARAM,
		FM_4_MOD_2_PARAM,
		FM_4_MOD_3_PARAM,
		FM_4_MOD_4_PARAM,
		MIX_OSC_1_PARAM,
		MIX_OSC_2_PARAM,
		MIX_OSC_3_PARAM,
		MIX_OSC_4_PARAM,
		MIX_MASTER_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		V_OCT_OSC_1_INPUT,
		V_OCT_OSC_2_INPUT,
		V_OCT_OSC_3_INPUT,
		V_OCT_OSC_4_INPUT,
		GATE_OSC_1_INPUT,
		GATE_OSC_2_INPUT,
		GATE_OSC_3_INPUT,
		GATE_OSC_4_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OSC_1_OUTPUT,
		OUT_OSC_2_OUTPUT,
		OUT_OSC_3_OUTPUT,
		OUT_OSC_4_OUTPUT,
		OUT_MIX_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	PolyOsc() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        for (int i = 0; i < 16; ++i) {
		    configParam(OCT_OSC_1_PARAM + i, -4.0f, 4.0f, 0.f, "Octave Osc " + std::to_string(i + 1), "octave", );
        }
		configParam(SEMI_OSC_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FINE_OSC_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WAVEFORM_OSC_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_4_MOD_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_4_MOD_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_4_MOD_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_4_MOD_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(OCT_OSC_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SEMI_OSC_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FINE_OSC_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WAVEFORM_OSC_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_3_MOD_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_3_MOD_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_3_MOD_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_3_MOD_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(OCT_OSC_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SEMI_OSC_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FINE_OSC_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WAVEFORM_OSC_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_2_MOD_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_2_MOD_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_2_MOD_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_2_MOD_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(OCT_OSC_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SEMI_OSC_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FINE_OSC_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WAVEFORM_OSC_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_1_MOD_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_1_MOD_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_1_MOD_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_1_MOD_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_OSC_1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_OSC_2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_OSC_3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_OSC_4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(MIX_MASTER_PARAM, 0.f, 1.f, 0.f, "");
		configInput(V_OCT_OSC_1_INPUT, "");
		configInput(V_OCT_OSC_2_INPUT, "");
		configInput(V_OCT_OSC_3_INPUT, "");
		configInput(V_OCT_OSC_4_INPUT, "");
		configInput(GATE_OSC_1_INPUT, "");
		configInput(GATE_OSC_2_INPUT, "");
		configInput(GATE_OSC_3_INPUT, "");
		configInput(GATE_OSC_4_INPUT, "");
		configOutput(OUT_OSC_1_OUTPUT, "");
		configOutput(OUT_OSC_2_OUTPUT, "");
		configOutput(OUT_OSC_3_OUTPUT, "");
		configOutput(OUT_OSC_4_OUTPUT, "");
		configOutput(OUT_MIX_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct PolyOscWidget : ModuleWidget {
	PolyOscWidget(PolyOsc* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/PolyOsc.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.774, 18.69)), module, PolyOsc::OCT_OSC_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(26.432, 18.69)), module, PolyOsc::SEMI_OSC_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.011, 18.69)), module, PolyOsc::FINE_OSC_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.91, 18.69)), module, PolyOsc::WAVEFORM_OSC_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(77.627, 18.69)), module, PolyOsc::FM_4_MOD_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(90.285, 18.69)), module, PolyOsc::FM_4_MOD_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(103.864, 18.69)), module, PolyOsc::FM_4_MOD_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(117.763, 18.69)), module, PolyOsc::FM_4_MOD_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.774, 33.748)), module, PolyOsc::OCT_OSC_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(26.432, 33.748)), module, PolyOsc::SEMI_OSC_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.011, 33.748)), module, PolyOsc::FINE_OSC_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.91, 33.748)), module, PolyOsc::WAVEFORM_OSC_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(77.627, 33.748)), module, PolyOsc::FM_3_MOD_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(90.285, 33.748)), module, PolyOsc::FM_3_MOD_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(103.864, 33.748)), module, PolyOsc::FM_3_MOD_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(117.763, 33.748)), module, PolyOsc::FM_3_MOD_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.774, 48.806)), module, PolyOsc::OCT_OSC_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(26.432, 48.806)), module, PolyOsc::SEMI_OSC_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.011, 48.806)), module, PolyOsc::FINE_OSC_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.91, 48.806)), module, PolyOsc::WAVEFORM_OSC_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(77.627, 48.806)), module, PolyOsc::FM_2_MOD_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(90.285, 48.806)), module, PolyOsc::FM_2_MOD_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(103.864, 48.806)), module, PolyOsc::FM_2_MOD_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(117.763, 48.806)), module, PolyOsc::FM_2_MOD_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.774, 63.865)), module, PolyOsc::OCT_OSC_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(26.432, 63.865)), module, PolyOsc::SEMI_OSC_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.011, 63.865)), module, PolyOsc::FINE_OSC_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.91, 63.865)), module, PolyOsc::WAVEFORM_OSC_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(77.627, 63.865)), module, PolyOsc::FM_1_MOD_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(90.285, 63.865)), module, PolyOsc::FM_1_MOD_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(103.864, 63.865)), module, PolyOsc::FM_1_MOD_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(117.763, 63.865)), module, PolyOsc::FM_1_MOD_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(70.056, 89.073)), module, PolyOsc::MIX_OSC_1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(82.738, 89.073)), module, PolyOsc::MIX_OSC_2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(95.42, 89.073)), module, PolyOsc::MIX_OSC_3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(108.102, 89.073)), module, PolyOsc::MIX_OSC_4_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(120.785, 89.073)), module, PolyOsc::MIX_MASTER_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.774, 90.677)), module, PolyOsc::V_OCT_OSC_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.432, 90.677)), module, PolyOsc::V_OCT_OSC_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.011, 90.677)), module, PolyOsc::V_OCT_OSC_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.91, 90.677)), module, PolyOsc::V_OCT_OSC_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.774, 110.362)), module, PolyOsc::GATE_OSC_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(26.432, 110.362)), module, PolyOsc::GATE_OSC_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.011, 110.362)), module, PolyOsc::GATE_OSC_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.91, 110.362)), module, PolyOsc::GATE_OSC_4_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.056, 108.506)), module, PolyOsc::OUT_OSC_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.738, 108.506)), module, PolyOsc::OUT_OSC_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(95.42, 108.506)), module, PolyOsc::OUT_OSC_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(108.102, 108.506)), module, PolyOsc::OUT_OSC_4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(120.785, 108.506)), module, PolyOsc::OUT_MIX_OUTPUT));
	}
};


Model* modelPolyOsc = createModel<PolyOsc, PolyOscWidget>("PolyOsc");