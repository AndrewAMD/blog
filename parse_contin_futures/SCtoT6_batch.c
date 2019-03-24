////////////////////////////////////////////////
// Convert price history from .csv to .t6
// The Format string determines the CSV format (see examples)
////////////////////////////////////////////////

/*
FOR BATCH SCRIPTING

Example usage:
zorro -run SCtoT6_batch -d AMZN

*/


#define SPLIT_YEARS	// split into separate years
//#define FIX_ZONE	-1 // add a time zone difference, f.i. for converting CST -> EST
#define REMOVE_SPACES // Modify input file to have no spaces.

string InFolder = ".\\sierra_csv\\stocks\\";
//string TickerSymbol = Define;
string InExt = ".csv";
string OutExt = ".t6";

char InName[64];// = ".\\sierra_csv\\stocks\\AMZN.csv";  // name of a CSV file
char OutName[64];// = "AMZN.t6";

/*
typedef struct T6
{
	DATE	time;	
	float fHigh, fLow;	// (f1,f2)
	float fOpen, fClose;	// (f3,f4)	
	float fVal, fVol; // optional data, like spread and volume (f5,f6)
} T6; // 6-stream tick, .t6 file content
*/
// SIERRA CHART FORMAT:
// Header:
// Date, Time, Open, High, Low, Close, Volume, NumberOfTrades, BidVolume, AskVolume
// Content:
// 2008/05/05, 13:30:00, 25.99, 26.03, 25.93, 26.02, 3044299, 0, 0, 0
// Spaces Removed:
// 2008/05/05,13:30:00,25.99,26.03,25.93,26.02,3044299,0,0,0
string Format = "+%Y/%m/%d%,%H:%M:%S,f3,f1,f2,f4,f6,f5";

function main()
{
	// build up strings for batching
	sprintf(InName,"%s%s%s",InFolder,Define,InExt);
	sprintf(OutName,"%s%s",Define,OutExt);
	
#ifdef REMOVE_SPACES
	// first, remove all spaces from the input csv, and overwrite the file (no need to keep original).
	{
		string original = file_content(InName);
		if(!original){
			printf("\nError, cannot read original file!");
			return;
		}
		string output;
		int i; // position for input
		int j; // position for output
		int len = strlen(original);
		output = malloc(len);
		if(!output){
			printf("\nMalloc failed!");
			return;
		}
		memset(output,0,len);
		for(i=0; i< len; i++){
			char ch = original[i];
			switch(ch){
			case ' ':
				// do nothing
				break;
			default:
				output[j] = ch;
				j++;
			}
		}
		file_write(InName, output, strlen(output));
	}
#endif // REMOVE_SPACES
	
	int Records = dataParse(1,Format,InName);
	printf("\n%d lines read",Records);
#ifdef FIX_ZONE
	int i;
	for(i=0; i<Records; i++)
		dataSet(1,i,0,dataVar(1,i,0)+FIX_ZONE/24.);
#endif
#ifndef SPLIT_YEARS
	if(Records) dataSave(1,OutName);
#else
	int i, Start = 0, Year, LastYear = 0;
	for(i=0; i<Records; i++) {
		Year = atoi(strdate("%Y",dataVar(1,i,0)));
		if(!LastYear) LastYear = Year;
		if(i == Records-1) { // end of file
			LastYear = Year; Year = 0;
		}
		if(Year != LastYear) {
			string NewName = strf("%s_%4i.t6",strxc(OutName,'.',0),LastYear);
			printf("\n%s",NewName);		
			dataSave(1,NewName,Start,i-Start);
			Start = i;
			LastYear = Year;
		}
	}
#endif
}