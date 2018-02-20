/*
  ==============================================================================

    EQ.h
    Created: 6 Feb 2018 7:22:22pm
    Author:  vdorn

  ==============================================================================
*/

#pragma once

class EQ {
public:
	double apply(double wave) {

	}

private:


};


//http://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
/************************************************************************
** Some EQ code to start with?
**
**********************************************************************/

enum {
	bq_type_lowpass = 0,
	bq_type_highpass,
	bq_type_bandpass,
	bq_type_notch,
	bq_type_peak,
	bq_type_lowshelf,
	bq_type_highshelf
};

class Biquad {
	public:
		//first set biquad, then call process

		Biquad();
		Biquad(int type, double Fc, double Q, double peakGainDB);
		~Biquad();
		void setType(int type);
		void setQ(double Q);
		void setFc(double Fc);
		void setPeakGain(double peakGainDB);
		void setBiquad(int type, double Fc, double Q, double peakGain);
		float process(float in);

	protected:
		void calcBiquad(void);

		int type;
		double a0, a1, a2, b1, b2;
		double Fc, Q, peakGain;
		double z1, z2;

		inline float process(float in) {
			double out = in * a0 + z1;
			z1 = in * a1 + z2 - b1 * out;
			z2 = in * a2 - b2 * out;
			return out;
		}
		Biquad() {
			type = bq_type_lowpass;
			a0 = 1.0;
			a1 = a2 = b1 = b2 = 0.0;
			Fc = 0.50;
			Q = 0.707;
			peakGain = 0.0;
			z1 = z2 = 0.0;
		}

		Biquad(int type, double Fc, double Q, double peakGainDB) {
			setBiquad(type, Fc, Q, peakGainDB);
			z1 = z2 = 0.0;
		}

		~Biquad() {
		}

		void setType(int type) {
			this->type = type;
			calcBiquad();
		}

		void setQ(double Q) {
			this->Q = Q;
			calcBiquad();
		}

		void setFc(double Fc) {
			this->Fc = Fc;
			calcBiquad();
		}

		void setPeakGain(double peakGainDB) {
			this->peakGain = peakGainDB;
			calcBiquad();
		}

		void setBiquad(int type, double Fc, double Q, double peakGainDB) {
			this->type = type;
			this->Q = Q;
			this->Fc = Fc;
			setPeakGain(peakGainDB);
		}

		void calcBiquad(void) {
			double norm;
			double V = pow(10, fabs(peakGain) / 20.0);
			double K = tan(M_PI * Fc);
			switch (this->type) {
			case bq_type_lowpass:
				norm = 1 / (1 + K / Q + K * K);
				a0 = K * K * norm;
				a1 = 2 * a0;
				a2 = a0;
				b1 = 2 * (K * K - 1) * norm;
				b2 = (1 - K / Q + K * K) * norm;
				break;

			case bq_type_highpass:
				norm = 1 / (1 + K / Q + K * K);
				a0 = 1 * norm;
				a1 = -2 * a0;
				a2 = a0;
				b1 = 2 * (K * K - 1) * norm;
				b2 = (1 - K / Q + K * K) * norm;
				break;

			case bq_type_bandpass:
				norm = 1 / (1 + K / Q + K * K);
				a0 = K / Q * norm;
				a1 = 0;
				a2 = -a0;
				b1 = 2 * (K * K - 1) * norm;
				b2 = (1 - K / Q + K * K) * norm;
				break;

			case bq_type_notch:
				norm = 1 / (1 + K / Q + K * K);
				a0 = (1 + K * K) * norm;
				a1 = 2 * (K * K - 1) * norm;
				a2 = a0;
				b1 = a1;
				b2 = (1 - K / Q + K * K) * norm;
				break;

			case bq_type_peak:
				if (peakGain >= 0) {    // boost
					norm = 1 / (1 + 1 / Q * K + K * K);
					a0 = (1 + V / Q * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - V / Q * K + K * K) * norm;
					b1 = a1;
					b2 = (1 - 1 / Q * K + K * K) * norm;
				}
				else {    // cut
					norm = 1 / (1 + V / Q * K + K * K);
					a0 = (1 + 1 / Q * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - 1 / Q * K + K * K) * norm;
					b1 = a1;
					b2 = (1 - V / Q * K + K * K) * norm;
				}
				break;
			case bq_type_lowshelf:
				if (peakGain >= 0) {    // boost
					norm = 1 / (1 + sqrt(2) * K + K * K);
					a0 = (1 + sqrt(2 * V) * K + V * K * K) * norm;
					a1 = 2 * (V * K * K - 1) * norm;
					a2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
					b1 = 2 * (K * K - 1) * norm;
					b2 = (1 - sqrt(2) * K + K * K) * norm;
				}
				else {    // cut
					norm = 1 / (1 + sqrt(2 * V) * K + V * K * K);
					a0 = (1 + sqrt(2) * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - sqrt(2) * K + K * K) * norm;
					b1 = 2 * (V * K * K - 1) * norm;
					b2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
				}
				break;
			case bq_type_highshelf:
				if (peakGain >= 0) {    // boost
					norm = 1 / (1 + sqrt(2) * K + K * K);
					a0 = (V + sqrt(2 * V) * K + K * K) * norm;
					a1 = 2 * (K * K - V) * norm;
					a2 = (V - sqrt(2 * V) * K + K * K) * norm;
					b1 = 2 * (K * K - 1) * norm;
					b2 = (1 - sqrt(2) * K + K * K) * norm;
				}
				else {    // cut
					norm = 1 / (V + sqrt(2 * V) * K + K * K);
					a0 = (1 + sqrt(2) * K + K * K) * norm;
					a1 = 2 * (K * K - 1) * norm;
					a2 = (1 - sqrt(2) * K + K * K) * norm;
					b1 = 2 * (K * K - V) * norm;
					b2 = (V - sqrt(2 * V) * K + K * K) * norm;
				}
				break;
			}

			return;
		}
};