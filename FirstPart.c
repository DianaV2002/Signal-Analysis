#include <formatio.h>
#include <ansi_c.h>
#include <userint.h>
#include <advanlys.h>
#include <utility.h>
#include <cvirte.h>
#include <analysis.h>
#include "FirstPart.h"

static int mainpanel, ctrlarray, freqPanel;

#define SAMPLE_RATE 0
#define NPOINTS 1

//downsampling
//fs=fe/4-> vector nou cu mediile pe cate 4 val initiale
//dupa spectru-ss de pus in documentatie
//de asemenea spectru fereastra

double* waveData = 0;
int npoints = 0, pointsPerSecond;
int* waveInfo = 0;
double sampleRate=0.0;
int tipFiltru = 0;//0-mediere, 1-de ordin 1
double maxim=0.0;
double minim=0.0;
int maxIndex=0;
int minIndex=0;
double dispersie=0.0;
double medie=0.0;
double mediana=0.0;

int n;//dimensiune fereastra filtru mediere 16 sau 32
double *filtrat= 0;//vector pentru semnal filtrat
int count = 0;//pt numarare treceri prin 0
//int amAplicatFiltru = 0;//setat pe 1 in Callback onAplica
int start = 0;
int stop = 0;
double skew = 0.0;
double kurtosis = 0.0;
double* anvelopa = 0;
int amAplicatFiltru = 0;
double* positions,*amplitudes, *secondDeriv;
double threshold = 6000;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((mainpanel = LoadPanel (0, "FirstPart.uir", MAINPANEL)) < 0)
		return -1;
	ctrlarray = GetCtrlArrayFromResourceID (mainpanel, CTRLARRAY);
    freqPanel=LoadPanel(0, "FirstPart.uir", FREQ_PANEL);
	DisplayPanel (mainpanel);
	RunUserInterface ();
	
Error:
	/* clean up */
	if (mainpanel > 0)
		DiscardPanel (mainpanel);
	return 0;
}

double *filtruMediere(int n)
{
	double suma=0.0;
	double *filtrat=(double*)malloc(npoints*sizeof(double));
	for(int i=0;i<n-1;++i)
	{
		suma+=waveData[i];
	}
	for(int i=0;i<n-1;++i)
	{
		filtrat[i]=suma/n;
	}
	for(int i=n;i<npoints;++i)
	{
		suma-=waveData[i-n];
		suma+=waveData[i];
		filtrat[i]=suma/n;
	}
	return filtrat;
}

double *filtruOrdinI(double alpha)
{
	double *filtrat=(double*)malloc(npoints*sizeof(double));
	filtrat[1]=waveData[1];
	for(int i=1;i<npoints;++i)
	{
		filtrat[i]=(1-alpha)*filtrat[i-1]+alpha*waveData[i];
	}
	return filtrat;
}


int CVICALLBACK OnLoad (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlAttribute(mainpanel,MAINPANEL_FILTRU, ATTR_DIMMED, 0);
		
			SetCtrlAttribute(mainpanel,MAINPANEL_ANVELOPA, ATTR_DIMMED, 0);
			
			SetCtrlAttribute(mainpanel,MAINPANEL_HISTOGRAMA, ATTR_DIMMED, 0);
			SetCtrlAttribute(mainpanel,MAINPANEL_SKEWKURTOSIS, ATTR_DIMMED, 0);
			SetCtrlAttribute(mainpanel,MAINPANEL_APLICA, ATTR_DIMMED, 0);
		    SetCtrlAttribute(mainpanel,MAINPANEL_HISTOGRAM, ATTR_DIMMED, 0);
			waveInfo = (int*)calloc(2, sizeof(int));
            FileToArray("waveInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
		    sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			pointsPerSecond = npoints/6;
			
			waveData = (double*)calloc(npoints, sizeof(double));
			
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			DeleteGraphPlot(panel, MAINPANEL_RAWDATA ,-1, VAL_IMMEDIATE_DRAW);
			PlotY(mainpanel, MAINPANEL_RAWDATA, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			
			//min&max 
			MaxMin1D(waveData,npoints,&maxim,&maxIndex,&minim,&minIndex);
			SetCtrlVal(panel,MAINPANEL_MAX, maxim);
			SetCtrlVal(panel,MAINPANEL_MIN, minim);
			
			SetCtrlVal(panel,MAINPANEL_INDEX_MAX, (double)maxIndex);
			SetCtrlVal(panel,MAINPANEL_INDEX_MIN, (double)minIndex);
			
			//media
			Mean(waveData,npoints,&medie);
			SetCtrlVal(panel,MAINPANEL_MEAN,medie);
			//mediana
			Median(waveData,npoints,&mediana);
			SetCtrlVal(panel,MAINPANEL_MEDIANA,mediana);
			//disperia
			StdDev(waveData,npoints,&medie,&dispersie);
			SetCtrlVal(panel,MAINPANEL_DISPERSIE, dispersie);
			
			//treceri prin 0
			for(int i=1;i<npoints-1;i++)
			{
				if((waveData[i]>0 && waveData[i+1]<0) || (waveData[i-1]<0 && waveData[i]>0))
				{
					count++;
				}
			}
			SetCtrlVal(panel,MAINPANEL_TRECERI0,(double)count);
			
	}
	return 0;
}




int CVICALLBACK OnMainPanel (int panel, int event, void *callbackData,
							 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
            QuitUserInterface(0);
			break;
	}
	return 0;
}


int CVICALLBACK OnHistogram (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			    int intervale = 20;
			    ssize_t vectfrecvente[intervale];
				double vectorx[intervale];
				Histogram(waveData, npoints, minim, maxim, vectfrecvente, vectorx, intervale);
				DeleteGraphPlot(mainpanel, MAINPANEL_HISTOGRAMA,-1, VAL_IMMEDIATE_DRAW); 
				PlotXY(panel, MAINPANEL_HISTOGRAMA, vectorx, vectfrecvente,intervale,VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			break;
	}
	return 0;
}


int CVICALLBACK OnFiltru (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	int dim = 0;
	double alpha;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(mainpanel, MAINPANEL_FILTRU, &tipFiltru);
			if(tipFiltru==0)//mediere
			{
				SetCtrlAttribute(mainpanel,MAINPANEL_ALPHA, ATTR_DIMMED, 1);//dezactivez alpha
				SetCtrlAttribute(mainpanel,MAINPANEL_DIMENSIUNE, ATTR_DIMMED, 0);
				GetCtrlVal(mainpanel,MAINPANEL_DIMENSIUNE, &dim);
				filtrat=filtruMediere(dim);
				
			}
			else //ordinI
			{
				SetCtrlAttribute(mainpanel,MAINPANEL_DIMENSIUNE, ATTR_DIMMED, 1);//dezactivez dimensiunea 
				SetCtrlAttribute(mainpanel,MAINPANEL_ALPHA, ATTR_DIMMED, 0);
				GetCtrlVal(mainpanel,MAINPANEL_ALPHA, &alpha);
				filtrat=filtruOrdinI(alpha);
			}
			amAplicatFiltru = 1;
			break;
	}

	return 0;
}

int CVICALLBACK OnAplica (int panel, int control, int event,
						   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		    SetCtrlAttribute(mainpanel,MAINPANEL_PREV, ATTR_DIMMED, 0);
			SetCtrlAttribute(mainpanel,MAINPANEL_NEXT, ATTR_DIMMED, 0);
			SetCtrlAttribute(mainpanel,MAINPANEL_DERIVATA, ATTR_DIMMED, 0);
			SetCtrlAttribute(mainpanel,MAINPANEL_START_INTERVAL, ATTR_DIMMED, 0);
			SetCtrlAttribute(mainpanel,MAINPANEL_STOP_INTERVAL, ATTR_DIMMED, 0);
			OnFiltru(mainpanel, MAINPANEL_FILTRU, EVENT_COMMIT, 0,0,0);
			DeleteGraphPlot(panel, MAINPANEL_FILTEREDDATA,-1, VAL_IMMEDIATE_DRAW); 
			PlotY(panel,MAINPANEL_FILTEREDDATA, filtrat, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_GREEN);

			break;
	}
	
	return 0;
}


int CVICALLBACK OnPrevNext (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	n=npoints/6;
	double *signal=(double*)calloc(n,sizeof(double));
	double *filtru=(double*)calloc(n,sizeof(double));
	switch (event)
	{
		case EVENT_COMMIT:
			if(amAplicatFiltru)
			{
				GetCtrlVal(mainpanel,MAINPANEL_START_INTERVAL,&start);
				GetCtrlVal(mainpanel,MAINPANEL_STOP_INTERVAL,&stop);
				SetCtrlAttribute(mainpanel,MAINPANEL_ANVELOPA, ATTR_DIMMED, 1);
				switch(control)
				{
					case MAINPANEL_NEXT:
						switch(event)
						{
							case EVENT_COMMIT:
								if(stop < 6)
								{
									++start;
									++stop;
									SetCtrlVal(mainpanel,MAINPANEL_STOP_INTERVAL,stop);
									SetCtrlVal(mainpanel,MAINPANEL_START_INTERVAL,start);
									for(int i=0;i<n;++i)
									{
										filtru[i]=filtrat[start*n+i];
									}
									
									for(int i=0;i<n;++i)
									{
										signal[i]=waveData[start*n+i];
									}
									DeleteGraphPlot (mainpanel,MAINPANEL_RAWDATA, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainpanel, MAINPANEL_RAWDATA, signal, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
									
									DeleteGraphPlot (mainpanel,MAINPANEL_FILTEREDDATA, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainpanel, MAINPANEL_FILTEREDDATA, filtru, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
								
									/////////////////////////////////////////////////////////
									//min&max 
									MaxMin1D(filtru,n,&maxim,&maxIndex,&minim,&minIndex);
									SetCtrlVal(mainpanel,MAINPANEL_MAX, maxim);
									SetCtrlVal(mainpanel,MAINPANEL_MIN, minim);
									
									//indexii
									SetCtrlVal(mainpanel,MAINPANEL_INDEX_MAX, (double)maxIndex);
									SetCtrlVal(mainpanel,MAINPANEL_INDEX_MIN, (double)minIndex);
									
									//media
									Mean(filtru,n,&medie);
									SetCtrlVal(mainpanel,MAINPANEL_MEAN,medie);
									//mediana
									Median(filtru,n,&mediana);
									SetCtrlVal(mainpanel,MAINPANEL_MEDIANA,mediana);
									//disperia
									StdDev(filtru,n,&medie,&dispersie);
									SetCtrlVal(mainpanel,MAINPANEL_DISPERSIE, dispersie);
									
									//treceri prin 0
									for(int i=1;i<n-1;i++)
									{
										if((filtru[i]>0 && filtru[i+1]<0) || (filtru[i-1]<0 && filtru[i]>0))
										{
											count++;
										}
									}
									SetCtrlVal(mainpanel,MAINPANEL_TRECERI0,(double)count);
									
									//skewness si kurtosis
									
									Moment(signal, 256, 3, &skew);
									SetCtrlVal(mainpanel,MAINPANEL_SKEWNESS,skew);
									
							        Moment(signal, 256, 4, &kurtosis);
									SetCtrlVal(mainpanel,MAINPANEL_KURTOSIS,kurtosis);
							
								    //salvare imagini pe intervale			
									if(tipFiltru==0)
									{
										char *file = (char*)malloc(40*sizeof(char));
										int img; 
										sprintf(file,"filtruMedianInterval%d-%d.png", start, stop);//numele cu care salvez imaginea
										GetCtrlDisplayBitmap(mainpanel, MAINPANEL_FILTEREDDATA, 1, &img);
										SaveBitmapToJPEGFile(img, file, JPEG_PROGRESSIVE, 100);
										free(file);
									}
									else
									{
										char *file = (char*)malloc(40*sizeof(char));
										int img; 
										sprintf(file, "filtruOrdinIInterval%d-%d.png", start, stop);//numele cu care salvez imaginea
										GetCtrlDisplayBitmap(mainpanel, MAINPANEL_FILTEREDDATA, 1, &img);
										SaveBitmapToJPEGFile(img, file, JPEG_PROGRESSIVE, 100);
										free(file);
									}
								}
								break;
						}
								
						break;
					case MAINPANEL_PREV:
						switch(event)
						{
							case EVENT_COMMIT:
								if(stop>1)
								{
									--stop;
									--start;
									SetCtrlVal(mainpanel,MAINPANEL_STOP_INTERVAL,stop);
									SetCtrlVal(mainpanel,MAINPANEL_START_INTERVAL,start);
									for(int i=0;i<n;++i)
									{
										filtru[i]=filtrat[start*npoints/6+i];
									}
									
									for(int i=0;i<n;++i)
									{
										signal[i]=waveData[start*n+i];
									}
									DeleteGraphPlot (mainpanel,MAINPANEL_RAWDATA, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainpanel, MAINPANEL_RAWDATA, signal, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_RED);
									
									DeleteGraphPlot (mainpanel, MAINPANEL_FILTEREDDATA, -1, VAL_IMMEDIATE_DRAW);
									PlotY (mainpanel, MAINPANEL_FILTEREDDATA, filtru, n, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
								
									/////////////////////////////////////////////
									//min&max 
									MaxMin1D(filtru,n,&maxim,&maxIndex,&minim,&minIndex);
									SetCtrlVal(mainpanel,MAINPANEL_MAX, maxim);
									SetCtrlVal(mainpanel,MAINPANEL_MIN, minim);
									
									//indexii
									SetCtrlVal(mainpanel,MAINPANEL_INDEX_MAX, (double)maxIndex);
									SetCtrlVal(mainpanel,MAINPANEL_INDEX_MIN, (double)minIndex);
									
									//media
									Mean(filtru,n,&medie);
									SetCtrlVal(mainpanel,MAINPANEL_MEAN,medie);
									//mediana
									Median(filtru,n,&mediana);
									SetCtrlVal(mainpanel,MAINPANEL_MEDIANA,mediana);
									//disperia
									StdDev(filtru,n,&medie,&dispersie);
									SetCtrlVal(mainpanel,MAINPANEL_DISPERSIE, dispersie);
									
									//treceri prin 0
									for(int i=1;i<n-1;i++)
									{
										if((filtru[i]>0 && filtru[i+1]<0) || (filtru[i-1]<0 && filtru[i]>0))
										{
											count++;
										}
									}
									SetCtrlVal(mainpanel,MAINPANEL_TRECERI0,(double)count);
									//skewness si kurtosis
									
									Moment(signal, 256, 3, &skew);
									SetCtrlVal(mainpanel,MAINPANEL_SKEWNESS,skew);
									
							        Moment(signal, 256, 4, &kurtosis);
									SetCtrlVal(mainpanel,MAINPANEL_KURTOSIS,kurtosis);
								
									//salvare imagini pe intervale
									if(tipFiltru==0)
									{
										char *file = (char*)malloc(40*sizeof(char));
										int img; 
										sprintf(file, "filtruMedianInterval%d-%d.png", start, stop);//numele cu care salvez imaginea
										GetCtrlDisplayBitmap(mainpanel, MAINPANEL_FILTEREDDATA, 1, &img);
										SaveBitmapToJPEGFile(img, file, JPEG_PROGRESSIVE, 100);
										free(file);
									}
									else
									{
										char *file = (char*)malloc(40*sizeof(char));
										int img; 
										sprintf(file,"filtruOrdinIInterval%d-%d.png", start, stop);//numele cu care salvez imaginea
										GetCtrlDisplayBitmap(mainpanel, MAINPANEL_FILTEREDDATA, 1, &img);
										SaveBitmapToJPEGFile(img, file, JPEG_PROGRESSIVE, 100);
										free(file);
									}
									
									}
							
							break;	
						}
						break;
				}
			}
			free(signal);
			free(filtru);
			break;
			
			
	}
	return 0;
}


int CVICALLBACK OnSkewKurtosis (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	double skew,kurtosis;
	switch (event)
	{
		case EVENT_COMMIT:
        Moment(waveData, 256, 3, &skew);
		SetCtrlVal(mainpanel,MAINPANEL_SKEWNESS,skew);
        Moment(waveData, 256, 4, &kurtosis);
		SetCtrlVal(mainpanel,MAINPANEL_KURTOSIS,kurtosis);    
			break;
	}
	return 0; 
}

int CVICALLBACK OnAnvelopa (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	
    int count = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			anvelopa=(double *) calloc(npoints, sizeof(double));
		    positions=(double *) calloc(npoints, sizeof(double));
			amplitudes=(double *) calloc(npoints, sizeof(double));
			secondDeriv = (double *)calloc(npoints, sizeof(double));
			PeakDetector(waveData,npoints,100,3,0,1,1,&count,&positions,&amplitudes,&secondDeriv);
			double max = 0.0;
			int maxIndex = 0;
			double *waveData1=(double *) calloc(npoints, sizeof(double));
			double* index1=(double *) calloc(npoints, sizeof(double));
			
			int k=0;
			for(int i=0;i<count;i+=32)
			{
				maxIndex=positions[i];
				max=amplitudes[i];
				for(int j=i+1;j<i+32;j++)
				{
					if(amplitudes[j]>max)
					{
						max=amplitudes[j];
						maxIndex=positions[j];
					}
				}
				waveData1[k]=max;
				index1[k]=maxIndex;
				k++;
			}
			
			
			PlotXY (mainpanel, MAINPANEL_RAWDATA, index1, waveData1, k, VAL_DOUBLE, VAL_DOUBLE, VAL_CONNECTED_POINTS, VAL_SOLID_CIRCLE, VAL_SOLID, 32, VAL_WHITE);
			free(anvelopa);
			free(amplitudes);
			free(positions);
			free(secondDeriv);
			break;
	}
	return 0;
}

int CVICALLBACK OnDerivata (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
          if (npoints == 0)
				return -1;
			
			double* derivata = (double *) calloc(npoints, sizeof(double));
			DifferenceEx(waveData, npoints, 1. / sampleRate ,  waveData + 1, 1,  waveData + npoints - 1 , 1, DIFF_SECOND_ORDER_CENTRAL, derivata);
			DeleteGraphPlot(panel, MAINPANEL_FILTEREDDATA, -1, VAL_IMMEDIATE_DRAW);
			PlotY (mainpanel, MAINPANEL_FILTEREDDATA, derivata, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_DK_CYAN);
			free(derivata);
			break;
	}
	return 0;
}

int CVICALLBACK switchPanelCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
		switch (event)
	{
		case EVENT_COMMIT:
			if(panel == mainpanel)
			{
				SetCtrlVal(freqPanel, FREQ_PANEL_SWITCHPANEL, 1);
				DisplayPanel(freqPanel);
				HidePanel(panel);
			}
			else
			{
				SetCtrlVal(mainpanel, MAINPANEL_SWITCHPANEL, 0);
				DisplayPanel(mainpanel);
				HidePanel(panel);
			}
			break;
	}
	return 0;
}

