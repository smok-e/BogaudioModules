#pragma once

#include "bogaudio.hpp"

extern Model* modelTest;

// #define LPF 1
// #define SINE 1
// #define SQUARE 1
// #define SAW 1
// #define TRIANGLE 1
#define SINEBANK 1

#include "pitch.hpp"
#ifdef LPF
#include "dsp/filter.hpp"
#elif SINE
#include "dsp/oscillator.hpp"
#elif SQUARE
#include "dsp/oscillator.hpp"
#elif SAW
#include "dsp/oscillator.hpp"
#elif TRIANGLE
#include "dsp/oscillator.hpp"
#elif SINEBANK
#include "dsp/oscillator.hpp"
#else
#error what
#endif

using namespace bogaudio::dsp;

namespace bogaudio {


struct Test : Module {
	enum ParamsIds {
		PARAM1_PARAM,
		PARAM2_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		CV2_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

#ifdef LPF
	LowPassFilter _lpf;
#elif SINE
	SineOscillator _sine;
#elif SQUARE
	SquareOscillator _square;
#elif SAW
	SawOscillator _saw;
#elif TRIANGLE
	TriangleOscillator _triangle;
#elif SINEBANK
	SineBankOscillator _sineBank;
#endif

	Test()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
#ifdef LPF
	, _lpf(44100.0, 1000.0, 1.0)
#elif SINE
	, _sine(44100.0, 1000.0, 5.0)
#elif SQUARE
	, _square(44100.0, 1000.0, 5.0)
#elif SAW
	, _saw(44100.0, 1000.0, 5.0)
#elif TRIANGLE
	, _triangle(44100.0, 1000.0, 5.0)
#elif SINEBANK
	, _sineBank(44101.0, 1000.0, 50)
#endif
	{
		onReset();

#ifdef SINEBANK
    const float baseAmplitude = 5.0;
		switch (5) {
			case 1: {
				// saw
				float phase = M_PI;
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, baseAmplitude / (float)i, &phase);
				}
				break;
			}

			case 2: {
				// square
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : 0.0);
				}
				break;
			}

			case 3: {
				// triangle
				if (false) {
					float phase = M_PI / 2.0;
					for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
						_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)(i * i) : 0.0, &phase);
					}
				}
				else {
					float phase = 0.0f;
					_sineBank.setPartial(1, 1.0f, baseAmplitude, &phase);
					for (int i = 2, n = _sineBank.partialCount(); i < n; ++i) {
						float k = 2*i - 1;
						_sineBank.setPartial(i, k, powf(-1.0f, k) * baseAmplitude/(k * k), &phase);
					}
				}
				break;
			}

			case 4: {
				// saw-square
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : baseAmplitude / (float)(2 * i));
				}
				break;
			}

			case 5: {
				// ?
				float phase = 0.0;
				float factor = 0.717;
				float factor2 = factor;
				float multiple = 1.0;
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, multiple, baseAmplitude / multiple, &phase);
					multiple += i % 2 == 1 ? factor : factor2;
				}
				break;
			}
		}
#endif
	}

	virtual void onReset() override;
	virtual void step() override;
	float oscillatorPitch();
};

} // namespace bogaudio
