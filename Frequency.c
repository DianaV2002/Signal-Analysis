#include <cvirte.h>		
#include <userint.h>
#include <utility.h>
#include <cvirte.h>		
#include <ansi_c.h>
#include <advanlys.h>
#include <userint.h>

#include <time.h>
//==============================================================================
// Include files

#include "Frequency.h"
#include "FirstPart.h"
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables
int nrPointsInterval = 0;
int second = 0; 
int winType = 0;
double *filteredSignal;
int filterType;

double windowParameter = 0;
WindowConst winConst;
double df = 0;
double searchFreq = 0;
double freqPeak = 0;
double powerPeak = 0;
int fs=22050;
double windowParameter2 = 0;
WindowConst winConst2;
double df2 = 0;
double searchFreq2 = 0;
double freqPeak2 = 0;
double powerPeak2 = 0;
int saveGraphs = 0;
//==============================================================================
// Global functions

double* DownsampleSignal(const double* inputSignal, int inputLength, int factor)
{
    int outputLength = inputLength / factor;
    double* downsampledSignal = (double*)calloc(outputLength, sizeof(double));

    for (int i = 0; i < outputLength; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < factor; ++j)
        {
            sum += inputSignal[i * factor + j];
        }
        downsampledSignal[i] = sum / factor;
    }

    return downsampledSignal;
}

int CVICALLBACK onFreqPanelCB (int panel, int event, void *callbackData,
							   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			free(filteredSignal);
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnGenSpectrum (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlAttribute(panel, FREQ_PANEL_TIMER, ATTR_ENABLED, 1);
			break;
	}
	return 0;
}

int CVICALLBACK timerCB (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	//verificam daca e bifat checkbox-ul pt salvare
	GetCtrlVal(panel, FREQ_PANEL_SAVE_GRAPHS, &saveGraphs); 
	GetCtrlIndex(panel, FREQ_PANEL_FILTER_TYPE, &filterType);
	GetCtrlIndex(panel, FREQ_PANEL_WINDOW_TYPE, &winType);
	
	
	GetCtrlVal(panel, FREQ_PANEL_NR_OF_POINTS, &nrPointsInterval);
	
	char unit[32]="V";
	
	//initializam semnalul filtrat
	filteredSignal = (double *) calloc(nrPointsInterval, sizeof(double));
	
	double* buffer= (double*)calloc(nrPointsInterval, sizeof(double));
	double * autoSpectrum = (double *) calloc (npoints / 2, sizeof(double));
	double *convertedSpectrum = (double *) calloc (npoints / 2, sizeof(double));
	
	double * autoSpectrum2 = (double *) calloc (npoints / 2, sizeof(double));
	double *convertedSpectrum2 = (double *) calloc (npoints / 2, sizeof(double));
	double* downsampledSignal=DownsampleSignal(waveData, npoints, 8);
	switch (event)
	{
		case EVENT_TIMER_TICK:
			
			second++;//incrementam secunda
			if(second < 5)
				SetCtrlAttribute(panel, FREQ_PANEL_SECOND, ATTR_CTRL_VAL, second);
			else if (second > 5)
			{
				second = 0;
				SetCtrlAttribute(panel, FREQ_PANEL_SECOND, ATTR_CTRL_VAL, second);
			}
			
			int fsd=fs/8;
			int sec=second*fsd;
				
				for(int i=sec;i < sec + nrPointsInterval; ++i)
				{
					buffer[i-sec]=downsampledSignal[i];
				}
				//semnal nefiltrat
				DeleteGraphPlot (panel,  FREQ_PANEL_GRAPH_SPEC_UNFILT, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot (panel,   FREQ_PANEL_GRAPH_RAW_2, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, FREQ_PANEL_GRAPH_RAW_2, buffer , nrPointsInterval, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	 
				ScaledWindowEx(buffer, nrPointsInterval, RECTANGLE, windowParameter, &winConst);
				AutoPowerSpectrum(buffer, nrPointsInterval, 1.0 / sampleRate, autoSpectrum, &df);
				PowerFrequencyEstimate(autoSpectrum, nrPointsInterval / 2, searchFreq, winConst, df, 7, &freqPeak, &powerPeak);
				SpectrumUnitConversion(autoSpectrum, nrPointsInterval / 2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df, winConst, convertedSpectrum, unit); 
				
				//afisam spectrul, inainte de filtrare
				PlotY(panel, FREQ_PANEL_GRAPH_SPEC_UNFILT, convertedSpectrum, nrPointsInterval / 2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS , VAL_DK_RED);
				SetCtrlVal(panel, FREQ_PANEL_FREQ_PEAK, freqPeak);
				SetCtrlVal(panel, FREQ_PANEL_POWER_PEAK, powerPeak);
																			 
				//semnal filtrat
				
				//alegem tipul ferestrei 
				
				if(winType == 0)
					HamWin(buffer, nrPointsInterval);
				else
					HanWin(buffer, nrPointsInterval);
				
				//vedem ce funtie de filtrare trebuie folosita				   
				if(filterType == 0)
					Bw_LPF(buffer, nrPointsInterval, sampleRate, sampleRate / 6, 6, filteredSignal);
				else
					Ch_LPF(buffer, nrPointsInterval, sampleRate, sampleRate / 6, 40.0, 4, filteredSignal);				
				//DebugPrintf("%f", filterType);
				DeleteGraphPlot (panel,  FREQ_PANEL_GRAPH_FILTERED_2, -1, VAL_IMMEDIATE_DRAW);
				DeleteGraphPlot (panel,  FREQ_PANEL_GRAPH_SPEC_FILT, -1, VAL_IMMEDIATE_DRAW);
				
				//afisam semnalul filtrat
				PlotY(panel,FREQ_PANEL_GRAPH_FILTERED_2, filteredSignal, nrPointsInterval, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				

				AutoPowerSpectrum(filteredSignal, nrPointsInterval, 1.0 / sampleRate, autoSpectrum2, &df2);
				PowerFrequencyEstimate(autoSpectrum2, nrPointsInterval / 2, searchFreq2, winConst2, df2, 7, &freqPeak2, &powerPeak2);
				SpectrumUnitConversion(autoSpectrum2, nrPointsInterval / 2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VRMS, df2, winConst2, convertedSpectrum2, unit); 

				//afisam spectrul semnalului filtrat 
				PlotY(panel, FREQ_PANEL_GRAPH_SPEC_FILT, convertedSpectrum2, nrPointsInterval / 2, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
			   
				if(saveGraphs){
					
					//construim numele fisierului cu imaginea semnalului filtrat
					char fileName[100] = "Signal_";
					if(filterType == 0)
						strcat(fileName, "ButterworthOrd6_");
					else
						strcat(fileName, "ChebysevOrd4_");
					if(winType == 0)
						strcat(fileName, "Hamming_");
					else
						strcat(fileName, "Hanning_");
					struct tm *localTime;
				    time_t currentTime;
				    time(&currentTime);
				    localTime = localtime(&currentTime);

				    // Format the filename with date, month, year, hour, and minute
				     sprintf(fileName + strlen(fileName), "%04d-%02d-%02d_%02d_%02d",
        				localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
        				localTime->tm_hour, localTime->tm_min);
		
					//salvam imaginea
					int imgHandle = 0;
					GetCtrlDisplayBitmap(panel, FREQ_PANEL_GRAPH_FILTERED_2, 1, &imgHandle);
					SaveBitmapToJPEGFile(imgHandle, fileName, JPEG_INTERLACE, 50);
					DiscardBitmap(imgHandle);
					
					//construim numele fisierului cu imaginea spectrului semnalului filtrat
					char fileName2[100] = "Spectrum_";
					if(filterType == 0)
						strcat(fileName2, "ButterworthOrd6_");
					else
						strcat(fileName2, "ChebysevOrd4_");
					if(winType == 0)
						strcat(fileName2, "Hamming_");
					else
						strcat(fileName2, "Hanning_");
				    sprintf(fileName2 + strlen(fileName2), "%04d-%02d-%02d_%02d_%02d",
        				localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday,
        				localTime->tm_hour, localTime->tm_min);

					//salvam imaginea
					imgHandle = 0;
					GetCtrlDisplayBitmap(panel, FREQ_PANEL_GRAPH_SPEC_FILT, 1, &imgHandle);
					SaveBitmapToJPEGFile(imgHandle, fileName2, JPEG_INTERLACE, 50);
					DiscardBitmap(imgHandle);
					
				}
			free(autoSpectrum);
			free(autoSpectrum2);
			free(convertedSpectrum);
			free(convertedSpectrum2);
			break;
	}
	return 0;
}





