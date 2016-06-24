#ifndef SPDIF_ANALYZER_H
#define SPDIF_ANALYZER_H

#include <Analyzer.h>
#include "SPDIFAnalyzerResults.h"
#include "SPDIFSimulationDataGenerator.h"

class SPDIFAnalyzerSettings;
class ANALYZER_EXPORT SPDIFAnalyzer : public Analyzer
{
public:
	SPDIFAnalyzer();
	virtual ~SPDIFAnalyzer();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();


protected: 

    int ReadBit(double samples_per_bit);
//vars
	std::auto_ptr< SPDIFAnalyzerSettings > mSettings;
	std::auto_ptr< SPDIFAnalyzerResults > mResults;
	AnalyzerChannelData* mSPDIF;

	SPDIFSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//l analysis vars:
	U32 mSampleRateHz;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //SPDIF_ANALYZER_H
