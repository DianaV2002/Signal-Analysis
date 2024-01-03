/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  FREQ_PANEL                       1       /* callback function: onFreqPanelCB */
#define  FREQ_PANEL_GRAPH_SPEC_UNFILT     2       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_BTT_SPEC              3       /* control type: textButton, callback function: OnGenSpectrum */
#define  FREQ_PANEL_SWITCHPANEL           4       /* control type: binary, callback function: switchPanelCB */
#define  FREQ_PANEL_WINDOW_TYPE           5       /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_NR_OF_POINTS          6       /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_TIMER                 7       /* control type: timer, callback function: timerCB */
#define  FREQ_PANEL_GRAPH_FILTERED_2      8       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FILTER_TYPE           9       /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_SECOND                10      /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_GRAPH_RAW_2           11      /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_GRAPH_SPEC_FILT       12      /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FREQ_PEAK             13      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_POWER_PEAK            14      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_SAVE_GRAPHS           15      /* control type: toggle, callback function: (none) */

#define  FREQUENCY                        2

#define  MAINPANEL                        3       /* callback function: OnMainPanel */
#define  MAINPANEL_RAWDATA                2       /* control type: graph, callback function: (none) */
#define  MAINPANEL_FILTEREDDATA           3       /* control type: graph, callback function: (none) */
#define  MAINPANEL_NEXT                   4       /* control type: command, callback function: OnPrevNext */
#define  MAINPANEL_PREV                   5       /* control type: command, callback function: OnPrevNext */
#define  MAINPANEL_INTERVAL               6       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_START                  7       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_STOP                   8       /* control type: textMsg, callback function: (none) */
#define  MAINPANEL_APLICA                 9       /* control type: command, callback function: OnAplica */
#define  MAINPANEL_HISTOGRAM              10      /* control type: command, callback function: OnHistogram */
#define  MAINPANEL_HISTOGRAMA             11      /* control type: graph, callback function: (none) */
#define  MAINPANEL_START_INTERVAL         12      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_STOP_INTERVAL          13      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_MEAN                   14      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_MEDIANA                15      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_DISPERSIE              16      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_TRECERI0               17      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_MIN                    18      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_MAX                    19      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_INDEX_MIN              20      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_INDEX_MAX              21      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_LOAD                   22      /* control type: command, callback function: OnLoad */
#define  MAINPANEL_FILTRU                 23      /* control type: ring, callback function: OnFiltru */
#define  MAINPANEL_DIMENSIUNE             24      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_ALPHA                  25      /* control type: slide, callback function: (none) */
#define  MAINPANEL_ANVELOPA               26      /* control type: command, callback function: OnAnvelopa */
#define  MAINPANEL_DERIVATA               27      /* control type: command, callback function: OnDerivata */
#define  MAINPANEL_SKEWNESS               28      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_KURTOSIS               29      /* control type: numeric, callback function: (none) */
#define  MAINPANEL_SKEWKURTOSIS           30      /* control type: command, callback function: OnSkewKurtosis */
#define  MAINPANEL_DECORATION             31      /* control type: deco, callback function: (none) */
#define  MAINPANEL_DECORATION_2           32      /* control type: deco, callback function: (none) */
#define  MAINPANEL_DECORATION_3           33      /* control type: deco, callback function: (none) */
#define  MAINPANEL_SWITCHPANEL            34      /* control type: binary, callback function: switchPanelCB */
#define  MAINPANEL_DECORATION_4           35      /* control type: deco, callback function: (none) */
#define  MAINPANEL_DECORATION_5           36      /* control type: deco, callback function: (none) */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_WAVE                     2
#define  MENUBAR_WAVE_MINUS               3
#define  MENUBAR_WAVE_SQUARE              4
#define  MENUBAR_WAVE_EXIT                5
#define  MENUBAR_WAVE_ITEM4               6


     /* Callback Prototypes: */

int  CVICALLBACK OnAnvelopa(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnAplica(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDerivata(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFiltru(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onFreqPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnGenSpectrum(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnHistogram(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoad(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrevNext(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSkewKurtosis(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK switchPanelCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK timerCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif