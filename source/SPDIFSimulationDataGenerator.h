#ifndef SPDIF_SIMULATION_DATA_GENERATOR
#define SPDIF_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
#include "SPDIFTypes.h"
class SPDIFAnalyzerSettings;

class SPDIFSimulationDataGenerator
{
public:
	SPDIFSimulationDataGenerator();
	~SPDIFSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, SPDIFAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	SPDIFAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSPDIFFrame(SPDIFFrame_t frame_type);
    double t;

	SimulationChannelDescriptor mSPDIFSimulationData;

};
#endif //SPDIF_SIMULATION_DATA_GENERATOR
