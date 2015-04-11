/*
Author: Jacob Winick
Date: 3/22/2015
About: See demodulate.h
*/

#include "demodulate.h"

#define BAUD_RATE 	75
#define FS 			32000

uint16_t synced = 0, sample_counter = 0, bit_counter = 0, samples_per_bit = FS/BAUD_RATE;
int32_t sum = 0;
char data_bits[8];


char Demodulate(int16_t input) {
	//if we're not synced to a frame
	//and the input falls below 0
	if(input < 0 && synced == 0) {
		synced = 1;
	}
	//if we're synced
	if(synced = 1) {
		//sum the samples
		sum += input;
		//we've got another sample
		sample_counter++;
		//if we've reached the end of the bit then find average
		if(sample_counter == samples_per_bit) {
			sum = sum/(int32_t)
		}

	}
}

//Demodulation
uint16_t demod_synced = 0;
uint16_t demod_counter = 0;
uint16_t max_hold = 427;
int32_t temp_sum = 0;
int16_t demod_output = 4095;
uint16_t bit_counter = 0;

void Demod(int16_t input) {
	if(input < 0 && demod_synced == 0) {
		demod_synced = 1;
		temp_sum = 0;
	}
	if(demod_synced == 1) {
		temp_sum += input;
		demod_counter++;
		if(demod_counter == max_hold) {
			temp_sum = temp_sum/(int32_t)max_hold; //find average
			demod_output = temp_sum < 0 ? 0 : 4095; //set output
			demod_counter = 0;
			bit_counter++;
			temp_sum = 0;
			if(bit_counter == 11) {
				demod_synced = 0;
			}
		}
	}

	DAC_SetChannel1Data(DAC_Align_12b_R, demod_output);
}