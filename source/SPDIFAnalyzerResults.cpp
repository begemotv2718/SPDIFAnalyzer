#include "SPDIFAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "SPDIFAnalyzer.h"
#include "SPDIFAnalyzerSettings.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

SPDIFAnalyzerResults::SPDIFAnalyzerResults( SPDIFAnalyzer* analyzer, SPDIFAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

SPDIFAnalyzerResults::~SPDIFAnalyzerResults()
{
}

void SPDIFAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );
    char frame_letter;
    switch(frame.mType){
      case Frame_B:
        AddResultString("B");
        frame_letter = 'B';
        break;
      case Frame_M:
        AddResultString("M");
        frame_letter='M';
        break;
      case Frame_W:
        AddResultString("W");
        frame_letter='W';
        break;
    }
	char number_str[128];
    char result_str[128];
    snprintf(result_str,128,"%c%d",frame_letter,frame.mData2);
    if(frame.mFlags & FRAMING_ERROR_FLAG){
      frame_letter='E';
    }
	AnalyzerHelpers::GetNumberString( frame.mData1/256, display_base, 8, number_str, 128 );
    snprintf(result_str,128,"%c %s %d %c",frame_letter,number_str,frame.mData2,(frame.mFlags & PARITY_ERROR_FLAG)?'?':' ');
	AddResultString( result_str);
}

void SPDIFAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
  /*
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
    */
}

void SPDIFAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
  /*
	Frame frame = GetFrame( frame_index );
	ClearResultStrings();

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );
	AddResultString( number_str );
    */
}

void SPDIFAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void SPDIFAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	ClearResultStrings();
	AddResultString( "not supported" );
}
