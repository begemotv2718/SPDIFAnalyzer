#ifndef SPDIF_ANALYZER_SETTINGS
#define SPDIF_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class SPDIFAnalyzerSettings : public AnalyzerSettings
{
public:
	SPDIFAnalyzerSettings();
	virtual ~SPDIFAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mSampleRate;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mSampleRateInterface;
};

#endif //SPDIF_ANALYZER_SETTINGS
