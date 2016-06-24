#ifndef SPDIF_ANALYZER_RESULTS
#define SPDIF_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class SPDIFAnalyzer;
class SPDIFAnalyzerSettings;
#define FRAMING_ERROR_FLAG 0x01
#define PARITY_ERROR_FLAG 0x02
class SPDIFAnalyzerResults : public AnalyzerResults
{
public:
	SPDIFAnalyzerResults( SPDIFAnalyzer* analyzer, SPDIFAnalyzerSettings* settings );
	virtual ~SPDIFAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

protected: //functions

protected:  //vars
	SPDIFAnalyzerSettings* mSettings;
	SPDIFAnalyzer* mAnalyzer;
};

#endif //SPDIF_ANALYZER_RESULTS
