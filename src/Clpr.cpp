
#include "Clpr.hpp"

void Clpr::onReset() {
	_modulationStep = modulationSteps;
}

void Clpr::step() {
	if (!(outputs[LEFT_OUTPUT].active || outputs[RIGHT_OUTPUT].active)) {
		return;
	}

	++_modulationStep;
	if (_modulationStep >= modulationSteps) {
		_modulationStep = 0;

		_thresholdDb = params[THRESHOLD_PARAM].value;
		if (inputs[THRESHOLD_INPUT].active) {
			_thresholdDb *= clamp(inputs[THRESHOLD_INPUT].value / 10.0f, 0.0f, 1.0f);
		}
		_thresholdDb *= 30.0f;
		_thresholdDb -= 24.0f;

		float outGain = params[OUTPUT_GAIN_PARAM].value;
		if (inputs[OUTPUT_GAIN_INPUT].active) {
			outGain = clamp(outGain + inputs[OUTPUT_GAIN_INPUT].value / 5.0f, 0.0f, 1.0f);
		}
		outGain *= 24.0f;
		if (_outGain != outGain) {
			_outGain = outGain;
			_outLevel = decibelsToAmplitude(_outGain);
		}

		_softKnee = params[KNEE_PARAM].value > 0.97f;
	}

	float leftInput = inputs[LEFT_INPUT].value;
	float rightInput = inputs[RIGHT_INPUT].value;
	float env = abs(leftInput + rightInput);
	float detectorDb = amplitudeToDecibels(env / 5.0f);
	float compressionDb = _compressor.compressionDb(detectorDb, _thresholdDb, Compressor::maxEffectiveRatio, _softKnee);
	_amplifier.setLevel(-compressionDb);
	if (outputs[LEFT_OUTPUT].active) {
		outputs[LEFT_OUTPUT].value = _saturator.next(_amplifier.next(leftInput) * _outLevel);
	}
	if (outputs[RIGHT_OUTPUT].active) {
		outputs[RIGHT_OUTPUT].value = _saturator.next(_amplifier.next(rightInput) * _outLevel);
	}
}

struct ClprWidget : ModuleWidget {
	static constexpr int hp = 6;

	ClprWidget(Clpr* module) : ModuleWidget(module) {
		box.size = Vec(RACK_GRID_WIDTH * hp, RACK_GRID_HEIGHT);

		{
			SVGPanel *panel = new SVGPanel();
			panel->box.size = box.size;
			panel->setBackground(SVG::load(assetPlugin(plugin, "res/Clpr.svg")));
			addChild(panel);
		}

		addChild(Widget::create<ScrewSilver>(Vec(0, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 15, 365)));

		// generated by svg_widgets.rb
		auto thresholdParamPosition = Vec(26.0, 52.0);
		auto outputGainParamPosition = Vec(26.0, 134.0);
		auto kneeParamPosition = Vec(39.5, 199.5);

		auto leftInputPosition = Vec(16.0, 244.0);
		auto rightInputPosition = Vec(50.0, 244.0);
		auto thresholdInputPosition = Vec(16.0, 280.0);
		auto outputGainInputPosition = Vec(50.0, 280.0);

		auto leftOutputPosition = Vec(16.0, 320.0);
		auto rightOutputPosition = Vec(50.0, 320.0);
		// end generated by svg_widgets.rb

		addParam(ParamWidget::create<Knob38>(thresholdParamPosition, module, Clpr::THRESHOLD_PARAM, 0.0, 1.0, 0.8));
		addParam(ParamWidget::create<Knob38>(outputGainParamPosition, module, Clpr::OUTPUT_GAIN_PARAM, 0.0, 1.0, 0.0));
		addParam(ParamWidget::create<SliderSwitch2State14>(kneeParamPosition, module, Clpr::KNEE_PARAM, 0.95, 1.0, 0.0));

		addInput(Port::create<Port24>(leftInputPosition, Port::INPUT, module, Clpr::LEFT_INPUT));
		addInput(Port::create<Port24>(rightInputPosition, Port::INPUT, module, Clpr::RIGHT_INPUT));
		addInput(Port::create<Port24>(thresholdInputPosition, Port::INPUT, module, Clpr::THRESHOLD_INPUT));
		addInput(Port::create<Port24>(outputGainInputPosition, Port::INPUT, module, Clpr::OUTPUT_GAIN_INPUT));

		addOutput(Port::create<Port24>(leftOutputPosition, Port::OUTPUT, module, Clpr::LEFT_OUTPUT));
		addOutput(Port::create<Port24>(rightOutputPosition, Port::OUTPUT, module, Clpr::RIGHT_OUTPUT));
	}
};

Model* modelClpr = createModel<Clpr, ClprWidget>("Bogaudio-Clpr", "CLPR", "clipper", DYNAMICS_TAG, DISTORTION_TAG);
