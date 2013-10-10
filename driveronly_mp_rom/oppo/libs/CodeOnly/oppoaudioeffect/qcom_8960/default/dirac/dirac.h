/**************************************************************************
 *
 *  Dirac HD Sound API header
 *
 *  Version (LibType/LibVersion/CoeffFileVersion): D/2.02/3
 *  Date: 2012-07-06
 *
 *************************************************************************/


#ifdef __cplusplus
	extern "C" {
#endif

#include "dirac_types.h"

// enum type containing a list of identifiers for the filters in this header.
enum ESelectedFilter_tag
{
      Oppo_R1150268_TgtMJ_48k,
};

// Enum of the available filters - one filter for each combination of playback
// device (like earphones or speaker) and sample rate to be used.
typedef enum ESelectedFilter_tag ESelectedFilter;

// Initialize Dirac HD Sound with a specific filter, must be called before Dirac_Convert().
// Sets filtering to enabled state.
// ESelectedFilter - defined in the coefficient header file and lists the available filters
// applicationId - the application ID for the application calling this function
//
// Returns 0 if suceeded, 1 otherwise.
S32 Dirac_Initialize (ESelectedFilter selectedFilter, U32 applicationId);

// Filter a block of data
// inbuffer - contains 16-bit audio input samples
// outbuffer - contains 16-bit audio output samples
// bufSize - the number of samples in buffer
// mode - 0 - input mono data, output mono data
//        1 - input stereo data, output stereo with mixed channels: (L,R = (L+R/2)). (often used with mono mobile phone speakers)
//        2 - input stereo data, output stereo (don't mix channels, use with stereo speakers or stereo earphones)
// volume_dB - a negative integer giving the volume level in dB re. full scale of the incoming audio stream. Allowed range: -40 to 0. 
//			   Setting volume_dB makes sure the output audio stream does not have peak levels above volume_dB.
//			   Bypass mode is activated if volume_dB is below -40, for energy saving.
// applicationId - the application ID for the application calling this function
//
// returns 0 if successfull, -1 if filter is not initialized.
S32 Dirac_Convert (S16 *inbuffer, S16 *outbuffer, U32 bufSize, U32 mode, S32 volume_dB, U32 applicationId);

// Set Dirac MaxSPL digital volume boost.
// boost_dB - the volume boost in dB. Allowed range: 0 to 6. Defaults to 0 dB after initialization.
// applicationId - the application ID for the application calling this function
void Dirac_SetVolumeBoost(U32 boost_dB, U32 applicationId);

// Enable Dirac HD Sound filtering
// applicationId - the application ID for the application calling this function
void Dirac_Enable (U32 applicationId);

// Disable Dirac HD Sound filtering (bypass-mode)
// attenuation_dB - the number of decibels that the volume is decreased in bypass-mode
//					in order to keep the same volume as when filtering is enabled.
//					Allowed range: 0 to 12.
//					If attenuation_dB is out of range, it is set to 6.
// applicationId - the application ID for the application calling this function
void Dirac_Disable (U32 attenuation_dB, U32 applicationId);

// Returns enumerator value of the currently initialized filter.
// If no filter has been initialized, returns -1.
// applicationId - the application ID for the application calling this function
S32 Dirac_queryInitializedFilter(U32 applicationId);

// Returns 1 if filtering is enabled, otherwise returns 0.
// applicationId - the application ID for the application calling this function
S32 Dirac_queryEnabled(U32 applicationId);


#ifdef __cplusplus
	}
#endif
	