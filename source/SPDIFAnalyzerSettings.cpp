#include "SPDIFAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


SPDIFAnalyzerSettings::SPDIFAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mSampleRate( 48000 )
{
	mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mInputChannelInterface->SetTitleAndTooltip( "SPDIF", "Standard S/PDIF" );
	mInputChannelInterface->SetChannel( mInputChannel );

	mSampleRateInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mSampleRateInterface->SetTitleAndTooltip( "Audio sample rate (Samples/S)",  "Specify audio rate in frames per second." );
	mSampleRateInterface->SetMax( 48000 );
	mSampleRateInterface->SetMin( 8000 );
	mSampleRateInterface->SetInteger( mSampleRate );

	AddInterface( mInputChannelInterface.get() );
	AddInterface( mSampleRateInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "SPDIF", false );
}

SPDIFAnalyzerSettings::~SPDIFAnalyzerSettings()
{
}

bool SPDIFAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface->GetChannel();
	mSampleRate = mSampleRateInterface->GetInteger();

	ClearChannels();
	AddChannel( mInputChannel, "SPDIF", true );

	return true;
}

void SPDIFAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface->SetChannel( mInputChannel );
	mSampleRateInterface->SetInteger( mSampleRate );
}

void SPDIFAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mSampleRate;

	ClearChannels();
	AddChannel( mInputChannel, "SPDIF", true );

	UpdateInterfacesFromSettings();
}

const char* SPDIFAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mSampleRate;

	return SetReturnString( text_archive.GetString() );
}
