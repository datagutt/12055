// Dirac Type definitions
// Version (LibType/LibVersion/CoeffFileVersion): D/2.02/3

#ifndef __dirac_types__
#define __dirac_types__

// Define 32-bit and 16-bit data types
typedef short int S16;
typedef int S32;
typedef long long S64;
typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned short int U16;
typedef unsigned char U8;



// Data type of fir-coefficients
typedef const short int FIR_COEFFS;

// Data type for holding filter states and quantization error
typedef struct IIR_STATES_tag
{
   S32    yn_minus_one, yn_minus_two;   // output history
   U32    quantError;	                // quantization error for error feedback
} IIR_STATES;

// Data type for holding IIR filter coefficients
typedef const struct IIR_COEFFS_tag
{
   S32    b0, b1, a1, a2;               // second-order-section filter coefficients
} IIR_COEFFS;

// Data type for holding biquad filter coefficients
typedef const struct BIQUAD_COEFFS_tag
{
   S32    b0, b1, b2, a1, a2;
} BIQUAD_COEFFS;

// Data type for holding all filter data for a biquad filter
typedef struct BIQUAD_tag 
{
   BIQUAD_COEFFS    *biquadCoeffs;      // biquad filter coefficients
   S32    xn_minus_one, xn_minus_two; 	// input history
   S32    yn_minus_one, yn_minus_two;   // output history
   U32    quantError;	                // quantization error for error feedback
} BIQUAD;

// Structure containing one set of filter coefficients for several channels
typedef struct CHANNEL_COEFFS_tag
{
    U32    n_fir;                     // number of FIR coefficients
    U32    n_iir;                     // number of IIR coefficients
    FIR_COEFFS *fir_taps;             // FIR coefficients
    IIR_COEFFS *iir_coeffs;           // S-O-S coefficients
} CHANNEL_COEFFS;

// Structure containing settings for a compressor/limiter (duplicated for each filterSet)
typedef struct LIMITER_SETTINGS_tag
{
	U32	limiterEnabledDefault;		// nonzero if limiter is enabled by default for this filter set
	U32 threshold;					// Compressor threshold in linear scale (Q1.4)
	U32 alphaRel;					// Release parameter (Qx.23)
	U32 alphaAtt;					// Attack parameter (Qx.23)
	U32 lookaheadSamples;			// Size of look-ahead buffer for this compressor
	U32 makeupGain;					// linear make-up gain (Q2.3)
} LIMITER_SETTINGS;

// Structure containing settings for clip protection (duplicated for each filterSet)
typedef struct SATURATOR_SETTINGS_tag
{
	U32	LFsaturationEnabledDefault;	  // nonzero if LF saturation is enabled by default for this filter set
    BIQUAD_COEFFS *clipProtBiqCoeffs; // biquads coefficients used in LF saturation block (biq 1 is a LP-filter, biq 2 is a HP-filter)
} SATURATOR_SETTINGS;

// Structure containing settings for clip protection (duplicated for each filterSet)
typedef struct CLIP_SETTINGS_tag
{
	SATURATOR_SETTINGS *saturatorSettings;
	LIMITER_SETTINGS *limiterSettings; 
} CLIP_SETTINGS;

// Structure containing data for a clip protection (LF saturation) block (contained in each filterInstance)
typedef struct SATURATOR_DATA_tag
{
    BIQUAD clipProtectBiquads[4];	  // biquads used in LF saturation block (biq 1&2 are LP-filters, biq 3&4 are HP-filters)
} SATURATOR_DATA;

// Structure containing a list of filter configurations
typedef struct FILTER_SET_tag
{
    U32   fs;							// filter coeffs sample rate
    U32   numChannels;					// number of channels in filter set
    CHANNEL_COEFFS *channel;			// HD Sound channel coefficients
	CLIP_SETTINGS  *clipSettings;		// settings for clip protection
} FILTER_SET;


#endif
