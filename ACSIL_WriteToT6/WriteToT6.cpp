#include "sierrachart.h"
SCDLLName("WriteToT6")

#pragma pack(push, 1)
typedef struct T1
{
	DATE	time;	// GMT timestamp, OLE datetime, 0.5 ms precision
	float fVal;	// positive for ask, negative for bid	
} T1; // single stream tick, .t1 file content

typedef struct T2
{
	DATE  time; // GMT timestamp
	float fVal; // price, positive for ask/last, negative for bid
	float fVol; // volume / size
} T2; // 2-stream tick, f.i. order book data, .t2 file content

typedef struct T6
{
	DATE	time;			// GMT timestamp of fClose
	float fHigh, fLow;	// (f1,f2)
	float fOpen, fClose;	// (f3,f4)	
	float fVal, fVol; // additional data, f.i. spread and volume (f5,f6)
} T6; // candle with extra data, .t6 file content
#pragma pack(pop)

enum eZorroFileType : unsigned int {
	zft_t6 = 0,
	zft_t1 = 1,
	zft_t2 = 2,
};

enum eSplitFiles {
	esf_DoNotSplit = 0,
	esf_YYYY = 1,
	esf_YYYYMM = 2,
	esf_YYYYMMDD = 3,
};

int get_split_id(const SCDateTime& dt, eSplitFiles mode) {
	int Year = 0, Month = 0, Day = 0;
	dt.GetDateYMD(Year, Month, Day);
	
	switch (mode) {
	default:
	case esf_DoNotSplit: return 1;
	case esf_YYYY: return Year;
	case esf_YYYYMM: return (Year * 100) + Month;
	case esf_YYYYMMDD: return (Year *10000) + (Month * 100) + Day;
	}
}
SCString actual_filename(const SCString& base_filename, int split_id) {
	if (split_id == 1)return base_filename;
	SCString sLeft = base_filename.Left(base_filename.GetLength() - 3);
	SCString sRight = base_filename.Right(2);
	SCString out;
	return out.Format("%s_%d.%s", sLeft.GetChars(), split_id, sRight.GetChars());
}


#define printf ___PRINTF_IS_BANNED___

template <typename... Types>
void print_to_log(SCStudyInterfaceRef sc, int ShowLog, Types... var2) { SCString m; sc.AddMessageToLog(m.Format(var2...), ShowLog); }


void write_to_file(SCStudyInterfaceRef sc, const SCString& base_filename, int split_id, int& N, std::unique_ptr<std::ostringstream>& upOos) {
	if (!upOos)return;
	if (!N)return;
	auto fn = actual_filename(base_filename, split_id);
	print_to_log(sc, 1, "Exporting %d ticks to file %s...", N, fn.GetChars());
	int h = 0;
	int filemode = n_ACSIL::FILE_MODE_OPEN_TO_REWRITE_FROM_START;
	sc.OpenFile(fn, filemode, h);
	std::string out = upOos->str();
	unsigned int bytes_written = 0;
	sc.WriteFile(h, out.data(), out.length(), &bytes_written);
	sc.CloseFile(h);
	print_to_log(sc, 1, "Done.");

	//cleanup
	N = 0;
	upOos.reset();
	upOos = std::make_unique<std::ostringstream>();
}

void round_bar_zorro_style(const n_ACSIL::s_BarPeriod& bp, SCDateTime& dt) {
	if (bp.ChartDataType == DAILY_DATA) {
		double dtime = dt.GetAsDouble();
		dtime = (int)dtime;
		dt.SetDateTimeAsDouble(dtime);
	}
	if (bp.ChartDataType == INTRADAY_DATA && bp.IntradayChartBarPeriodType == IBPT_DAYS_MINS_SECS) {
		double dtime = dt.GetAsDouble();
		double one_s = 1. / (1440. * 60.);
		double one_ms = 1. / (1440. * 60. * 1000.);
		double bar_increment = bp.IntradayChartBarPeriodParameter1 * one_s + bp.IntradayChartBarPeriodParameter2 * one_ms;
		dtime = round((dtime + 0.5*bar_increment) / bar_increment) * bar_increment;
		dt.SetDateTimeAsDouble(dtime);
	}
}


/*==========================================================================*/
SCSFExport scsf_WriteBarDataToT6File(SCStudyInterfaceRef sc)
{
	SCInputRef Input_FileType = sc.Input[0];
	SCInputRef Input_UseUTCTime = sc.Input[1];
	SCInputRef Input_SplitBy = sc.Input[2];
	SCInputRef Input_TimeStamp_Style = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Write Bar Data To Zorro File";
		sc.StudyDescription =
			"Write Bar Data To Zorro File. "
			"Step 1: Configure study settings. "
			"Step 2: When ready to export, right-click chart and click \"Begin Write Bar Data To Zorro File\". "
			"Note: split files will be modified like this: "
			"YYYY: \"SPY.t6\" becomes \"SPY_2023.t6\" and \"SPY_2022.t6\""
			"(That is, the date range distinctor will be inserted before the file suffix.)"
			;

		sc.GraphRegion = 0;

		//Input_Separator.Name = "Separator";
		//Input_Separator.SetCustomInputStrings("Comma;Tab");
		//Input_Separator.SetCustomInputIndex(0);

		sc.TextInputName = "File Path";

		Input_FileType.Name = "Zorro History File Type";
		Input_FileType.SetDescription("Set to TICK (*.t1), TICK w/ vol (*.t2), or OHLC BAR (*.t6)");
		Input_FileType.SetCustomInputStrings("OHLC BAR (*.t6);TICK (*.t1);TICK w/ vol (*.t2)");
		Input_FileType.SetCustomInputIndex(0);

		Input_UseUTCTime.Name = "Use UTC Time";
		Input_UseUTCTime.SetDescription("If enabled, all timestamps are UTC (Zorro standard).  If disabled, it uses the time zone of the chart.");
		Input_UseUTCTime.SetYesNo(1);

		Input_SplitBy.Name = "Split files by date range";
		Input_SplitBy.SetDescription("YYYY: split by year, YYYYMM: split by month, YYYYMMDD: split by day.");
		Input_SplitBy.SetCustomInputStrings("Do not split;YYYY;YYYYMM;YYYYMMDD");
		Input_SplitBy.SetCustomInputIndex(0);

		Input_TimeStamp_Style.Name = "Timestamp Style";
		Input_TimeStamp_Style.SetDescription("Zorro Style: Intraday ticks are rounded to the time the bar ends, historical ticks indicate the day only. Sierra Chart Style: Just use the original timestamps.");
		Input_TimeStamp_Style.SetCustomInputStrings("Zorro Style;Sierra Chart Style");
		Input_TimeStamp_Style.SetCustomInputIndex(0);


		sc.AutoLoop = 0;//manual looping for efficiency
		return;
	}

	// set up menu
	int& MenuID = sc.GetPersistentInt(0);
	int& NeedToExportZorroDataNow = sc.GetPersistentInt(1);
	bool zorro_style = Input_TimeStamp_Style.GetIndex() == 0;

	if (MenuID <= 0)
		MenuID = sc.AddACSChartShortcutMenuItem(sc.ChartNumber, "Begin Write Bar Data To Zorro File");

	if (MenuID < 0)
		sc.AddMessageToLog("Add ACS Chart Shortcut Menu Item failed", 1);

	if (sc.LastCallToFunction) {
		if (MenuID > 0) sc.RemoveACSChartShortcutMenuItem(sc.ChartNumber, MenuID);
		return;
	}
	if (sc.MenuEventID != 0 && sc.MenuEventID == MenuID) {
		sc.AddMessageToLog("Beginnning Write Bar Data To Zorro File...", 1);
		NeedToExportZorroDataNow = 1;
	}

	n_ACSIL::s_BarPeriod bp;
	sc.GetBarPeriodParameters(bp);


	if (!NeedToExportZorroDataNow) return;
	if (sc.DownloadingHistoricalData) return;

	// reach this point: we are going to export data.

	eZorroFileType filetype = (eZorroFileType)Input_FileType.GetIndex();
	eSplitFiles SplitFileMode = (eSplitFiles)Input_SplitBy.GetIndex();

	SCString base_filename;
	if (!sc.TextInput.IsEmpty())
	{
		base_filename = sc.TextInput;
	}
	else
	{
		base_filename = sc.DataFilesFolder();
		base_filename += "\\";
		base_filename += sc.Symbol.GetChars();
		if (filetype == zft_t6) base_filename += ".t6";
		if (filetype == zft_t1) base_filename += ".t1";
		if (filetype == zft_t2) base_filename += ".t2";
	}

	bool UseUTCTime = Input_UseUTCTime.GetYesNo() && bp.ChartDataType==INTRADAY_DATA;

	std::unique_ptr<std::ostringstream> upOss;

	int last_split_id = 1;
	int N = 0;
	for (int i = sc.ArraySize - 1; i >= 0; i--) {
		if (sc.GetBarHasClosedStatus(i) != BHCS_BAR_HAS_CLOSED) continue;
		if (!upOss)upOss = std::make_unique<std::ostringstream>();

		SCDateTime time = sc.BaseDateTimeIn[i];
		if (UseUTCTime) time = sc.ConvertDateTimeFromChartTimeZone(time, TIMEZONE_UTC);
		if (zorro_style)round_bar_zorro_style(bp, time);
		double dtime = time.GetAsDouble();


		int split_id = get_split_id(time, SplitFileMode);
		if (split_id != last_split_id && N) {
			write_to_file(sc, base_filename, last_split_id, N, upOss);
		}
		last_split_id = split_id;

		if (filetype == zft_t6) {
			T6 t{};
			t.time = dtime;
			t.fOpen = (float)sc.BaseData[SC_OPEN][i];
			t.fHigh = (float)sc.BaseData[SC_HIGH][i];
			t.fLow = (float)sc.BaseData[SC_LOW][i];
			t.fClose = (float)sc.BaseData[SC_LAST][i];
			t.fVal = (float)sc.BaseData[SC_NUM_TRADES][i];
			t.fVol = (float)sc.BaseData[SC_VOLUME][i];
			upOss->write((char*)&t, sizeof(t));
			N++;
		}
		if (filetype == zft_t1) {
			T1 t{};
			t.time = dtime;
			t.fVal = (float)sc.BaseData[SC_LAST][i];
			upOss->write((char*)&t, sizeof(t));
			N++;
		}
		if (filetype == zft_t2) {
			T2 t{};
			t.time = dtime;
			t.fVal = (float)sc.BaseData[SC_LAST][i];
			t.fVol = (float)sc.BaseData[SC_VOLUME][i];
			upOss->write((char*)&t, sizeof(t));
			N++;
		}
	}


	if (N) {
		write_to_file(sc, base_filename, last_split_id, N, upOss);
	}

	//cleanup
	NeedToExportZorroDataNow = 0;

}

