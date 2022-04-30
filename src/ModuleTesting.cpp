#include "plugin.hpp"


struct ModuleTesting : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	ModuleTesting() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	}

	void process(const ProcessArgs& args) override {
	}
};


struct ModuleTestingWidget : ModuleWidget {
	ModuleTestingWidget(ModuleTesting* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ModuleTesting.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// mm2px(Vec(29.374, 20.183))
		addChild(createWidget<Widget>(mm2px(Vec(5.633, 15.413))));
		// mm2px(Vec(29.374, 20.183))
		addChild(createWidget<Widget>(mm2px(Vec(5.633, 54.158))));
		// mm2px(Vec(29.374, 20.183))
		addChild(createWidget<Widget>(mm2px(Vec(5.633, 92.903))));
	}
};


Model* modelModuleTesting = createModel<ModuleTesting, ModuleTestingWidget>("ModuleTesting");