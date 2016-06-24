#include "SPDIFAnalyzer.h"
#include "SPDIFAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <math.h>

SPDIFAnalyzer::SPDIFAnalyzer()
:	Analyzer(),  
	mSettings( new SPDIFAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

SPDIFAnalyzer::~SPDIFAnalyzer()
{
	KillThread();
}

int SPDIFAnalyzer::ReadBit(double samples_per_bit){
    U32 twoBitMin = floor(2*samples_per_bit);
    U32 twoBitMax = twoBitMin+2;
    U64 curSample = mSPDIF->GetSampleNumber();
    U64 delta = mSPDIF->GetSampleOfNextEdge()-curSample;
    if(delta>twoBitMax)
      return -1;
    if(delta<=twoBitMax && delta>=twoBitMin){
      mSPDIF->AdvanceToNextEdge();
      return BIT_LOW;
    }
    if(delta<twoBitMin){
      mSPDIF->AdvanceToNextEdge();
      delta= mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
      if(delta<twoBitMin){
        mSPDIF->AdvanceToNextEdge();
        return BIT_HIGH;
      }else{
        return -1;
      }
    }
}

void SPDIFAnalyzer::WorkerThread()
{
	mResults.reset( new SPDIFAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );

    U32 bitrate = mSettings->mSampleRate*128;
	mSampleRateHz = GetSampleRate();
    double samples_per_bit = (double)mSampleRateHz/(double)bitrate;
    U32 oneBitMin = floor(samples_per_bit);
    U32 oneBitMax = oneBitMin+2;
    U32 twoBitMin = floor(2*samples_per_bit);
    U32 twoBitMax = twoBitMin+2;
    U32 threeBitMin = floor(3*samples_per_bit);
    U32 threeBitMax = threeBitMin+2;
    U32 startofFrame,endofFrame;


	mSPDIF = GetAnalyzerChannelData( mSettings->mInputChannel );

    while(1){
      /*Reset condition*/

      mSPDIF->AdvanceToNextEdge();
      U64 curSample=mSPDIF->GetSampleNumber();
      /*Skip to frame start*/
      while((mSPDIF->GetSampleOfNextEdge()-curSample)<threeBitMin){
        mSPDIF->AdvanceToNextEdge();
        curSample=mSPDIF->GetSampleNumber();
      }
      while(1){
        if(
            mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber()>threeBitMax
          )
        {
          break;
        }
        Frame frame;
		frame.mStartingSampleInclusive = mSPDIF->GetSampleNumber();
		frame.mFlags = 0;
        /*Find frame type*/
        mSPDIF->AdvanceToNextEdge();
        U64 delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
        if(delta>threeBitMax){
            frame.mFlags|=FRAMING_ERROR_FLAG;
        }else if(delta>=threeBitMin){
            frame.mType = Frame_M;
            mSPDIF->AdvanceToNextEdge();
            delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
            if(delta>oneBitMax)
              frame.mFlags|=FRAMING_ERROR_FLAG;
            mSPDIF->AdvanceToNextEdge();
            delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
            if(delta>oneBitMax)
              frame.mFlags|=FRAMING_ERROR_FLAG;
        }else if(delta>=twoBitMin){
            frame.mType = Frame_W;
            mSPDIF->AdvanceToNextEdge();
            delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
            if(delta>oneBitMax)
              frame.mFlags|=FRAMING_ERROR_FLAG;
            mSPDIF->AdvanceToNextEdge();
            delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
            if(delta>twoBitMax || delta <twoBitMin)
              frame.mFlags|=FRAMING_ERROR_FLAG;
        }else if(delta<twoBitMin){
            frame.mType = Frame_B;
            mSPDIF->AdvanceToNextEdge();
            delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
            if(delta>oneBitMax)
              frame.mFlags|=FRAMING_ERROR_FLAG;
            /*
            mSPDIF->AdvanceToNextEdge();
            delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
            if(delta>oneBitMax)
              frame.mFlags|=FRAMING_ERROR_FLAG;
              */
            mSPDIF->AdvanceToNextEdge();
            delta = mSPDIF->GetSampleOfNextEdge()-mSPDIF->GetSampleNumber();
            if(delta>threeBitMax || delta<threeBitMin)
              frame.mFlags|=FRAMING_ERROR_FLAG;
        }
        mSPDIF->AdvanceToNextEdge();
        
        /*Decode bits*/
        int bit;
        frame.mData1=0;
        frame.mData2=0;
        unsigned int parity =0;
        for(int i=0;i<24;i++){
          bit = ReadBit(samples_per_bit);
          if(bit<0){
            frame.mFlags|=FRAMING_ERROR_FLAG;
          }else{
            //frame.mData1=(frame.mData1<<1)+(bit==BIT_HIGH?1:0);
            frame.mData1|=(bit==BIT_HIGH?1:0)<<i;
            parity^= (bit==BIT_HIGH?1:0); 
          }
        }
        /*Validity bit*/
        bit = ReadBit(samples_per_bit);
        if(bit<0){
          frame.mFlags|=FRAMING_ERROR_FLAG;
        }else{
          parity^= (bit==BIT_HIGH?1:0); 
        }
        /*Subcode bit*/
        bit = ReadBit(samples_per_bit);
        if(bit<0){
          frame.mFlags|=FRAMING_ERROR_FLAG;
        }else{
          parity^= (bit==BIT_HIGH?1:0); 
        }
        /*Channel info bit*/
        bit = ReadBit(samples_per_bit);
        if(bit<0){
          frame.mFlags|=FRAMING_ERROR_FLAG;
        }else{
          frame.mData2=(bit==BIT_HIGH?1:0);
          parity^= (bit==BIT_HIGH?1:0); 
        }
        /*Parity bit */
        bit = ReadBit(samples_per_bit);
        if(bit<0){
          frame.mFlags|=FRAMING_ERROR_FLAG;
        }else{
          frame.mFlags|=(parity==bit)?0:PARITY_ERROR_FLAG;
        }
        frame.mEndingSampleInclusive=mSPDIF->GetSampleNumber()-1;

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
        
      }
        

    }


}

bool SPDIFAnalyzer::NeedsRerun()
{
	return false;
}

U32 SPDIFAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 SPDIFAnalyzer::GetMinimumSampleRateHz()
{
	return 24000000;
}

const char* SPDIFAnalyzer::GetAnalyzerName() const
{
	return "SPDIF";
}

const char* GetAnalyzerName()
{
	return "SPDIF";
}

Analyzer* CreateAnalyzer()
{
	return new SPDIFAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
