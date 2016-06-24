#include "SPDIFSimulationDataGenerator.h"
#include "SPDIFAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <math.h>
#define PI 3.1415926

SPDIFSimulationDataGenerator::SPDIFSimulationDataGenerator()
: t(0)
{
}

SPDIFSimulationDataGenerator::~SPDIFSimulationDataGenerator()
{
}

void SPDIFSimulationDataGenerator::Initialize( U32 simulation_sample_rate, SPDIFAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mSPDIFSimulationData.SetChannel( mSettings->mInputChannel );
	mSPDIFSimulationData.SetSampleRate( simulation_sample_rate );
	mSPDIFSimulationData.SetInitialBitState( BIT_LOW );
}

U32 SPDIFSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mSPDIFSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreateSPDIFFrame(Frame_B);
        CreateSPDIFFrame(Frame_W);
        for(int i=1;i<192;i++){
            CreateSPDIFFrame(Frame_M);
            CreateSPDIFFrame(Frame_W);
        }
	}

	*simulation_channel = &mSPDIFSimulationData;
	return 1;
}

void SPDIFSimulationDataGenerator::CreateSPDIFFrame(SPDIFFrame_t frame_type)
{
    U32 bitrate = (mSettings->mSampleRate*2*64);
    if(frame_type == Frame_B || frame_type == Frame_M){
        t+=1.0/(double)(mSettings->mSampleRate);
    }
	U32 samples_per_bit = (U32)round((double)mSimulationSampleRateHz /(double)bitrate);
    U8 parity=0;
    switch(frame_type){
      case Frame_B:
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*3);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*1);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*1);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*3);
        break;
      case Frame_M:
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*3);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*3);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*1);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*1);
        break;
      case Frame_W:
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*3);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*2);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*1);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*2);
        break;
      default:
        break;
    }
    U32 sample = (U32)(32768*(0.5+0.5*sin(2*PI*400*t)));
	for(int i=0;i<3;i++){
      mSPDIFSimulationData.Transition();
      mSPDIFSimulationData.Advance(samples_per_bit*2);
    }
    for(int i=0;i<20;i++){
      if(sample & (1<<(19-i))){
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*1);
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*1);
        parity^=1;
      }else{
        mSPDIFSimulationData.Transition();
        mSPDIFSimulationData.Advance(samples_per_bit*2);
      }
    }

    /*Validity bit 28*/
    mSPDIFSimulationData.Transition();
    mSPDIFSimulationData.Advance(samples_per_bit*2);

    /*Subcode data bit 29*/
    mSPDIFSimulationData.Transition();
    mSPDIFSimulationData.Advance(samples_per_bit*2);
    
    /*Channel status info bit 30*/
    mSPDIFSimulationData.Transition();
    mSPDIFSimulationData.Advance(samples_per_bit*2);
    
    /*Parity bit 31*/
    if(parity){
      mSPDIFSimulationData.Transition();
      mSPDIFSimulationData.Advance(samples_per_bit*1);
      mSPDIFSimulationData.Transition();
      mSPDIFSimulationData.Advance(samples_per_bit*1);
    }else{
      mSPDIFSimulationData.Transition();
      mSPDIFSimulationData.Advance(samples_per_bit*2);
    }

}
