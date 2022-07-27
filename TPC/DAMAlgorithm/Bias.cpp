#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <iomanip>
#include <assert.h>
#include <time.h>

#ifndef __CINT__
//#include "fftw3.h"
////#include <QtGui>
//#  include <QSqlDatabase>
//#  include <QSqlError>
//#  include <QStringList>
//#  include <QSqlQuery>
//#  include <QSqlRecord>
//#  include <QVariant>
#endif

#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TImage.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TF2.h>
#include <TMath.h>
#include <TStyle.h>
#include <TRandom.h>
#include "TMinuit.h"
#include "TMultiGraph.h"


#include "fitsio.h"
#include "longnam.h"

#include "fftw3.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

// GLOBALS ------------------------------------------------------------
const double TwoPi = 2. * 3.14159265358979323846 ;
const unsigned int T1995 = 788936400; //UTC time of 19950101 00:00
const char * KeyList[] = {
		"CTIME", "USEC", "EXPOSURE", "EXPTIME","MONOWL",
		"AMP2.VOLTAGE", "AMP2.CURRENT", 
		"CRYO.A.TEMP", "CRYO.B.TEMP", "CRYO.C.TEMP", "CRYO.D.TEMP", "CRYO.1.SETPT",
        "MONOCH-WAVELENG", "MONOCH.WAVELENG",
        "AMP0.MEAS_NPLC",
		0
	};
	enum { Nkey = sizeof(KeyList)/sizeof(char*)-1 };
	vector <double>::iterator V_Iter;
	vector<double> Vval[Nkey];

//---------------------------------------------------------------------
// FFileDB class ---------------------------------------------------------
//class FFileDB {
//public:
//	FFileDB( const string & d_name );
//	~FFileDB(){};
//	int SelectFiles( void );
//	set<string> GetFnames( void );
//	void GetFparams();
//	int get_dbOK()   { return dbOK;};
//	int get_dataOK() { return dataOK;};
//	int get_bitpix() { return bitpix;};
//	int get_naxis()  { return naxis;};
//    int get_naxis1() { return naxis1;};
//	int get_naxis2() { return naxis2;};
//private:
//	const char *  DB_host;
//	const char *  DB_name;
//	const char *  DB_user;
//	const char *  DB_pswd;
//	const char *  TABLE_name; 
//	const char *  SERVER_name;
//#ifndef __CINT__
//    QSqlDatabase db;
//#endif
//	static int dbOK;
//	static int dataOK;
//    static int bitpix;
//	static int naxis;
//	static int naxis1;
//	static int naxis2;
//	string dir_name;
//	int Nfiles;
//	set <string> FName;
//};
//
//int FFileDB::dbOK = 0;
//int FFileDB::dataOK = 0;
//int FFileDB::bitpix = 0;
//int FFileDB::naxis  = 0;
//int FFileDB::naxis1 = 0;
//int FFileDB::naxis2 = 0;
//
//FFileDB::FFileDB( const string & d_name ) : 
//	DB_host ("localhost"),
//	DB_name ("LSSTsensor"),
//	DB_user ("ccdtest"),
//	DB_pswd ("sensor"),
//	TABLE_name ("TestInfo"), 
//	SERVER_name ("mysql"),
//	dir_name(d_name),
//	Nfiles(0)
//{
//	char * driver = "QMYSQL";
//	QSqlError err;
//	int  cCount = 0;
//	db = QSqlDatabase::addDatabase(driver, QString("Browser%1").arg(++cCount));
//	db.setDatabaseName( DB_name );
//	db.setHostName( DB_host);
//
//	cout << "MySQL CONNECTION: ------------------------------------" << endl;
//	if ( !db.open(DB_user, DB_pswd) ) {
//		err = db.lastError();
//		db = QSqlDatabase();
//		QSqlDatabase::removeDatabase(QString("Browser%1").arg(cCount));
//		cout << err.text().toLocal8Bit().constData() << endl;
//		return;
//	}
//	dbOK = 1;
//	cout << "CONNECTED ------------------------------------" << endl;
////*******************************************************************
//// DB is open
//	QStringList t = db.tables();
//	cout << "SQL TABLES: --------------------------------------" << endl;
//	for (int i = 0; i < t.size(); ++i)
//		cout << t.at(i).toLocal8Bit().constData() << endl;
////*******************************************************************
//}
//
//int FFileDB::SelectFiles( void )
//{
//	char stmnt[200];
//	Nfiles = 0;
//	cout << "SQL: Select from FileLocation TABLE-------------------" << endl;
//	sprintf(stmnt,"SELECT FileName FROM FileLocation WHERE DirName ='%s'", dir_name.c_str());
//	cout << stmnt << endl;
//	QSqlQuery query = db.exec(stmnt);
//	QSqlRecord rec = query.record();
//	QString qValue;
//	while (query.next()) {
//		  qValue = query.value(0).toString();
//		  FName.insert(qValue.toLocal8Bit().constData()); // qValue.toString());
//		  Nfiles++;
//		  cout << Nfiles << ". " << qValue.toLocal8Bit().constData() << endl;
//	}
//	cout << "DB.Nfiles=" << Nfiles << endl;
//	return Nfiles;
//}
//
//set<string> FFileDB::GetFnames( void )
//{
//	string filename;
//	set<string> CFiles;
//	set<string>::iterator ii = FName.begin();
//	for(; ii != FName.end(); ii++)
//	{
//       filename = dir_name + "/" + *ii + ".fits";
//	   CFiles.insert(filename);
//	}
//	   return CFiles;
//}
//
//void FFileDB::GetFparams()
//{
//	dataOK = 1;
//	char stmnt[200];
//	cout << "SQL: Select from TestInfo TABLE-------------------" << endl;
// 	set<string>::iterator it = FName.begin();
//	for( ; it != FName.end(); it++)
//	{
//        sprintf(stmnt,"SELECT BITPIX, NAXIS, NAXIS1, NAXIS2 FROM TestInfo WHERE FileName ='%s'", it->c_str());
//		cout << stmnt << endl;
//		QSqlQuery query = db.exec(stmnt);
//		QSqlRecord rec = query.record();
//     	//QString qValue;
//		while (query.next()) {
//			for (int j = 0; j <rec.count(); j++)
//			{
//				//qValue = query.value(j).toString();
//				//qValue = query.value(j).toInt();
//				int iValue = query.value(j).toInt();
//				switch(j){
//					case 0:
//						if (!bitpix) bitpix = iValue;
//						else
//							if (bitpix != iValue){
//								printf("Error:: bitpix has new value %d\n", iValue);
//								dataOK = 0;
//							}
//                        break;
//					case 1:
//						if(!naxis) naxis = iValue;
//						else
//							if (naxis != iValue){
//								printf("Error:: naxis has new value %d\n", iValue);
//								dataOK = 0;
//							}
//						break;
//					case 2:
//						if(!naxis1) naxis1 = iValue;
//						else
//							if (naxis1 != iValue){
//								printf("Error:: naxis1 has new value %d\n", iValue);
//								dataOK = 0;
//							}
//						break;
//					case 3:
//						if(!naxis2) naxis2 = iValue;
//						else
//							if (naxis2 != iValue){
//								printf("Error:: naxis2 has new value %d\n", iValue);
//								dataOK = 0;
//							}
//						break;
//					default:
//								printf("Error:: default for j=%i \n", j);
//								dataOK = 0;
//						 break;
//				} //end switch
//			}  //end records  
//		}  //end query
//	}  //end files
//} //end GetFparams()
//

//---------------------------------------------------------------------
// DataStr class ---------------------------------------------------------
class DataStr{
private:
	static DataStr * pInstance;
protected:
	double pixsz;
	int device;
	int xmin, xmax;   //"active" range
	int ymin, ymax;
	int NXpres, NYpres;       //pre-scan
	int NXover, NYover;
	int oxmin, oxmax;  //overscan range
	int oymin, oymax;

	DataStr (int nx, int ny);
	~DataStr(){};
public:
	static DataStr * Instance( int nx, int ny);
	double PixSz(){ return pixsz;}
	int minX(){ return xmin; }
	int maxX(){ return xmax; }
	int NoverX(){ return NXover; }
	int OmiXs(){ return (oxmin+8); }
	int OmiX(){ return oxmin; }
	int OmaX(){ return oxmax; }
	int minY(){ return ymin; }
	int maxY(){ return ymax; }
	int IsOver( int x, int y);
	int IsOverX( int x);
};

DataStr * DataStr::pInstance = 0;
DataStr * DataStr::Instance(int nx, int ny)
{
	//static DataStr inst (nx, ny);
	//return &inst;
	if ( pInstance == 0 ) {
		pInstance = new DataStr (nx,ny);
	}
	return pInstance;
}

DataStr::DataStr (int nx, int ny) 
{
	device = -1;
	NYpres=0;

    // SIX device 2018
    if ( (nx == 3302) && (ny == 1612) ){
        pixsz = 16.;
        device = 3;
        NXpres = 1665;  //0;
        NXover = 1665;  //1665;
        NYpres = 1;
        NYover = 1;
        printf(" DataStr::DataStr device = XCAM \n");
        printf(" PixSz= %f device_id=%i \n", pixsz, device);
        xmin  = NXpres;
        xmax  = nx - 1;  // for SIX LS: nx-1;
        oxmin = 1;       // for SIX LS: 1
        oxmax = xmin;    // for SIX LS: xmin
        ymin  = NYpres;
        ymax  = ny - NYover;
        oymin = ymax;
        oymax = ny;
        return;
    }
    if ( (nx == 1652) && (ny == 1612) ){
        pixsz = 16.;
        device = 3;
        NXpres = 16;  //0;
        NXover = 1;  //1665;
        NYpres = 1;
        NYover = 1;
        printf(" DataStr::DataStr device = XCAM active area \n");
        printf(" PixSz= %f device_id=%i \n", pixsz, device);
        xmin  = NXpres;
        xmax  = nx - NXover;  // for SIX LS: nx-1;
        oxmin = 1;       // for SIX LS: 1
        oxmax = xmin-1;    // for SIX LS: xmin
        ymin  = NYpres;
        ymax  = ny - NYover;
        oymin = ymax;
        oymax = ny;
        return;
    }
    //Swiss Light Source Camera
    if ( (nx == 817) && (ny == 1606) ){
        pixsz = 10.;
        device = 4;
        NXpres = 1;
        NXover = 1;
        NYpres = 1;
        NYover = 1;
        printf(" DataStr::DataStr device = Swiss \n");
        printf(" PixSz= %f device_id=%i \n", pixsz, device);
        xmin  = NXpres;
        xmax  = nx - 1;  // for SIX LS: nx-1;
        oxmin = 1;       // for SIX LS: 1
        oxmax = xmin;    // for SIX LS: xmin
        ymin  = NYpres;
        ymax  = ny - NYover;
        oymin = ymax;
        oymax = ny;
        return;
    }
    
	// STA3800B
//	if ( (nx == 544) && (ny == 2048) ){
//		pixsz = 10.;
//		device = 2;
//		NXpres = 3;
//		NXover = 32;
//		NYover = 48;
//		printf(" DataStr::DataStr device = STA3800B \n");
//		printf(" PixSz= %f device_id=%i \n", pixsz, device);
//	}
    
    // e2v CCD250 March 2017
//    if ( (nx == 572) && (ny == 2048) ){
//        pixsz = 10.;
//        device = 1;
//        NXpres = 10;
//        NXover = nx-NXpres-512;
//        if (NXover < 0) NXover=0;
//        NYover = ny-2002;
//        if (NYover < 0) NYover=0;
//        printf(" DataStr::DataStr device = e2v CCD250 \n");
//        printf(" PixSz= %f device_id=%i \n", pixsz, device);
//    }
    
	if ( device < 0) {
		printf(" DataStr::DataStr - not recognized device, use default \n");
		pixsz = 10.;
		NXpres = 10;
		NXover = nx-NXpres-512;
		if (NXover < 0) NXover=0;
		NYover = ny-2002;
		if (NYover < 0) NYover=0;
		printf(" DataStr::DataStr default as e2v CCD250 %i (%i,%i) %i (%i,%i) \n", 
						nx,NXpres,NXover, ny,NYpres,NYover);
		printf(" PixSz= %f device_id=%i \n", pixsz, device);
	}
// calculate dev parameters

		xmin  = NXpres;
        xmax  = nx - NXover;  // - NXover;   for SIX LS: nx-1;
        oxmin = xmax;         //xmax;  for SIX LS: 1
        oxmax = nx;           //nx; for SIX LS: xmin
		ymin  = NYpres;
		ymax  = ny - NYover;  
		oymin = ymax;
		oymax = ny;

} //end constructor DataStr

int DataStr::IsOver( int x, int y)
{
	if ( x >= oxmin &&
         x <= oxmax    ) {return 1;}
	if ( y >= oymin  &&
        y <= oymax     ) {return 1;}
	return 0;
}

int DataStr::IsOverX( int x)
{
	if ( x >= oxmin &&
        x <= oxmax ) {return 1;}
//	if ( x <  xmin ) return 1;
    else {return 0;}
}

//---------------------------------------------------------------------
// FileF class ---------------------------------------------------------
class FileF{
public:
	const char * fname;
	fitsfile *fptr;
	fitsfile *fout;
	int iEOF;
	int Nhdu;
	int hdu;
	int hdutype;
	enum { MaxP = 22 };			// Max number of hdu (channels/amplifiers)

	long * buffer;
//	unsigned short * buffer;
//	vector<unsigned short> buffer; 

	static unsigned short nullval; 
	int anynull;
	char strnull[10];
	int status;
	char comment[FLEN_CARD];
	int bitpix; 
    double bzero;
    double bscale;
	int naxis;
	enum { MAXIS = 10 };
	long naxes[MAXIS];
	long nx, ny;
	unsigned long npixels;    /* number of pixels in the image */
	const long fpixel;
	unsigned short datamin;
	unsigned short datamax;

	static const int klength;
	static const char valuebegin;
	static const char valuend;
	int Ncards;
	enum { Lcard = FLEN_CARD };
	char card[Lcard];
	char value_str[Lcard];
	char keyword[Lcard];
	double value;
    
	double ltv1;
	double ltv2;
	double ltm1_1;
	double ltm1_2;
	double ltm2_1;
	double ltm2_2;
    
	typedef pair <string, double> H_Pair;
	map <string, double>::iterator H_Iter;
	std::pair<map <string, double>::iterator, bool> pptr;
	map <string, double> mHeader;

// table variables
    int lTable;                 // 1= good table
    char TableName[FLEN_VALUE];
    int  ncols;
	static long nrows;
	enum { NbCol = 2 };			// number of columns
    char * ColName[NbCol];
    static vector<double> TabData[NbCol];
	static const char * FieldName[NbCol];

public:
	FileF ();
	~FileF();
	int Open( const char * file_name );
	void Close();
	int Read(int copy =0);
    void PrimeOut (long Nx, long Ny );
	void CloseOut ();
	int OpenOutF ( const char * fOutName = "Out_bufzs" );
	int OutFitsF( unsigned short * buf);
	int getValue( const char * name, double * value );
//	vector<unsigned short> &Pbuf()  {return buffer;};
//	unsigned short * Pbuf()  {return buffer;};
	void PrintKeys( void );
};

// initialization of static members
unsigned short FileF::nullval  = 0;  // don't check for null values in the image 
const int  FileF::klength = 8;
const char FileF::valuebegin = '=';
const char FileF::valuend = '/';
const char * FileF::FieldName[NbCol]={"TIMES","CURRENT"}; 
long FileF::nrows;
vector<double> FileF::TabData[NbCol];

FileF::~FileF(){
//	  delete [] buffer; buffer = 0; 
		for (int i = 0; i < NbCol; i++) {      /* deallocate space for the column labels */
			delete [] ColName[i]; ColName[i]=0; 
			//delete [] TabData[i]; TabData[i]=0;
		}
}

FileF::FileF ():
	fptr(0),
	buffer(0),
	anynull(0),
	status(0),
	fpixel(1),
	datamin(65535),
	datamax(0),
	Ncards(0)
{
	strcpy(strnull, " ");
	for (int i = 0; i < NbCol; i++) {      /* allocate space for the column labels */
		ColName[i] = new char[FLEN_VALUE];  /* max label length = 69 */
		memset(ColName[i], 0, FLEN_VALUE);
		//TabData[i]= new double[2500];
		//memset(TabData[i], 0, 2500*sizeof(double));
	}

} //end constructor FileF


int FileF::OpenOutF( const char * fOutName  )
{
    status = 0;
    char foutname[200];
    strcpy (foutname, fOutName);
    strcat (foutname, ".fits");
    printf( " OpenOutF name = %s \n", foutname);
    
    remove(foutname);
    fits_create_file( &fout, foutname, &status); // create output FITS file
    if ( status ) {printf(" create_file status error: %i \n", status); return -6;}
    printf(" OpenOutF:: hdu=%i \n", fout->HDUposition);
    
    return 0;
}

void FileF::CloseOut ()
{
    fits_close_file( fout, &status);
    if ( status ) printf(" CloseOut status error: %i \n", status);
}

void FileF::PrimeOut (long Nx, long Ny )
{
    nx = Nx;
    ny = Ny;

    bitpix  =  16;  // 64-bit floating point values
    naxis   =  2;   // 2-dimensional image
    status = 0;
    bzero = 32768.0;
    bscale = 1.0;
    naxis   =  2;   // 2-dimensional image
    naxes[0] = nx;
    naxes[1] = ny;
    int channels =2;
    
    if (fits_create_img( fout, bitpix, 0,  NULL, &status))
        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TINT, "BZERO", &bzero,"", &status) )
        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TINT, "BSCALE", &bscale,"", &status) )
        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TINT, "NEXTEND", &channels,"", &status) )
        fits_report_error(stderr, status );
//    if ( fits_update_key(fout, TSTRING, "DATE", &sdate,"", &status) )
//        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TSTRING, "COMMENT", &comment,"", &status) )
        fits_report_error(stderr, status );
    printf(" primary header done \n");
}

int FileF::OutFitsF( unsigned short  * buf)
{
	status = 0;
	npixels = nx*ny;
    char detector[] = "XCAM             ";
    char cname[10] = "Ch";
    
// HEADER ----------------
    if (fits_create_img( fout, bitpix, naxis, naxes, &status)) {fits_report_error(stderr,status);}
    if ( fits_update_key(fout, TSTRING, "EXTNAME", cname,"Channel number", &status) )
        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TDOUBLE, "BZERO", &bzero,"", &status) )
        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TINT, "BITPIX", &bitpix,"BITS PER PIXEL", &status) )
        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TDOUBLE, "BSCALE", &bscale,"", &status) )
        fits_report_error(stderr, status );
    if ( fits_update_key(fout, TSTRING, "DETECTOR", detector,"DETECTOR ID", &status) )
        fits_report_error(stderr, status );
    
// DATA ---------------------------------------------------
//    unsigned short * outb = new unsigned short[npixels];
//	for (unsigned long i=0; i<npixels;i++)  {
//		double amp = buf[i] + 100.;
//		if (  amp < 0.) amp = 0.;
//		if (  amp > 65535.) amp = 65535.;
//		outb[i] = (unsigned short)amp;
//	}
	fits_write_img( fout, TUSHORT, fpixel, npixels, buf, &status);
	if ( status ) {printf(" write_img status error: %i \n", status); return -6;}
    
	//delete [] outb;
	return 0;
}

void FileF::Close ()
{
	fits_close_file (fptr, &status);
	delete [] buffer; buffer = 0;
//       buffer.clear();
}

int FileF::Open( const char * file_name )
{
	iEOF = 0;
	//fname = file_name;
	fits_open_file (&fptr, file_name, READONLY, &status);
	if (status) { printf(" FileF::Open::status error: %i  %s \n", status, file_name);
		status=0;
		return -1;
	}
	hdu = fptr->HDUposition + 1;   // get the current HDU number
	//fits_get_num_hdus(fptr, &Nhdu, &status);  //get number of HDUs
	//if (status) { printf(" FileF::Open::status:num_hdus error: %i \n", status);
	//	status=0;
	//	return -1;
	//} else { printf(" Number of HDUs: %i \n", Nhdu); }
	strcpy(TableName, " ");

 	return 0;
}

int FileF::Read( int copy)
{
	lTable=0;
	hdu = fptr->HDUposition + 1;   // get the current HDU number
    fits_get_hdu_type(fptr, &hdutype, &status);  // get the HDU type

// main header -------------------------------------------------------------
	if (hdu == 1) {  // read keys of the main header
		mHeader.clear(); 
		int morekeys=0;
		fits_get_hdrspace( fptr, &Ncards, &morekeys, &status);
		if (status)  {printf( "FileF::ReadHeader status %i \n", status ); return -3;}
		if ( Ncards < 1) return -3;
		
		char * cr = 0;
		char * pdest =0;
		int lngth;
		int L;
		for (int ik=0; ik<Ncards; ik++){
			value=0.0;
            memset(card, '\0',Lcard);
            memset(value_str, '\0',Lcard);
			memset(keyword, '\0',Lcard);

			fits_read_record (fptr, ik+1, card, &status);
			if (status) {printf( "Read card %i status %i \n", ik, status ); return -3;}

			// keyword field
			lngth = klength;
			strncpy( keyword, card, lngth);
			cr = card + lngth + 1;  //move pointer to "behind the name" position
			L = lngth-1;
			while (L >= 0 && keyword[L]==' ') {keyword[L]='\0'; L--;}
			int cond =	strcmp( keyword, "COMMENT") && 
						strcmp( keyword, "HISTORY") &&
						strcmp( keyword, "SCRIPT")  && 
						strcmp( keyword, "CONTINUE");
			if ( !cond ) continue;
			if ( !strcmp( keyword, "HIERARCH") ){
				   pdest = strchr( cr, valuebegin );
				   lngth = (int)( pdest - cr ); 
				   strncpy( keyword, cr, lngth);
				   keyword[lngth] = '\0';
				   cr += lngth + 1; // move to "behind the name" position
			}
			L = lngth-1;
			while (L >= 0 && keyword[L]==' ') {keyword[L]='\0'; L--;}
			
			// value field
		   pdest = strchr( cr, valuend );
		   if (pdest != 0) {
			   lngth = (int)( pdest - cr );
			   strncpy( value_str, cr, lngth);
			   value_str[lngth]='\0';
			   cr += lngth + 1; // move to "behind the name" position
		   }
		   else lngth = 0;
			
			L = lngth-1; 
			while (L >= 0 && value_str[L]==' ') {value_str[L]='\0'; L--;}
			lngth = L+1;

			cr = value_str;
			L = 0;
			while (L <lngth && value_str[L]==' ') {cr++; L++;} 
			lngth -= --L;
			strncpy( value_str, cr, lngth );
			value_str[lngth]='\0';
			
			if ( value_str[0] =='T' ) value = 1.0;
			else value = atof( value_str );
			pptr = mHeader.insert( H_Pair(keyword, value) );
			if (!pptr.second){
				printf (" This key: %s already exists; attemted value %f \n", keyword, value);
			}
		}
		//PrintKeys();
	}

// copy header
	if ( copy && (hdutype == IMAGE_HDU) ) {
		printf(" OutFitsF:: fptr.hdu=%i fout.hdu=%i \n", 
							fptr->HDUposition, fout->HDUposition);  
		fits_copy_header( fptr, fout, &status);  //ffcphd
		if ( status ) {printf(" create_img status error: %i \n", status); return -6;}
		printf(" OutFitsF:: fout.hdu=%i \n", fout->HDUposition);  
	}

// data ---------------------------------------------------------------
	
 	naxis=0;
    if (hdutype == IMAGE_HDU)   
    {
		fits_get_img_param( fptr, MAXIS, &bitpix, &naxis, naxes, &status);
		if (status) {printf( "Get_img_param status %i \n", status ); return -5;}
		//printf(" NAXIS=%i naxes[0]=%ld naxes[1]=%ld \n", naxis, naxes[0], naxes[1]);
        
		// coordinate transformation parameters
		ltv1=ltv2 =0.;
		ltm1_1=ltm2_2= 1.;
		ltm1_2=ltm2_1= 0.;
        
		fits_read_key(fptr, TDOUBLE, "LTV1", &ltv1, 0, &status);
		if (status) {/*printf( "Get_LTV1_param status %i \n", status );*/ status=0;}
        
		fits_read_key(fptr, TDOUBLE, "LTV2", &ltv2, 0, &status);
		if (status) {/*printf( "Get_LTV2_param status %i \n", status );*/ status=0;}
        
		fits_read_key(fptr, TDOUBLE, "LTM1_1", &ltm1_1, 0, &status);
		if (status) {/*printf( "Get_LTM1_1_param status %i \n", status );*/ status=0;}
        
		fits_read_key(fptr, TDOUBLE, "LTM2_2", &ltm2_2, 0, &status);
		if (status) {/*printf( "Get_LTM2_2_param status %i \n", status );*/ status=0;}
        
        
		if ( naxis > 0 ) {
			nx = naxes[0];
			ny = naxes[1];
			npixels  = naxes[0] * naxes[1];       // number of pixels in the image 
			//if (buffer == 0) buffer = new unsigned short[npixels]; // buffer.resize(npixels,0);
			//fits_read_img ( fptr, TUSHORT, fpixel, npixels, &nullval,
			//						   &buffer[0], &anynull, &status);

			if (buffer == 0) {buffer = new long[npixels];} // buffer.resize(npixels,0);
			fits_read_img ( fptr, TLONG, fpixel, npixels, &nullval,
									   &buffer[0], &anynull, &status);
			if (status) { printf( "Read_Img status %i \n", status ); return -5; }
		}
	}

	if (hdutype == BINARY_TBL)   
	{
		// get TableName from EXTNAME, example "AMP0.MEAS_TIMES"
		fits_read_key_str(fptr, "EXTNAME", TableName, NULL, &status);

		//printf("\nReading binary table %s from HDU %d:\n", TableName, hdu); 

		/* gets number of rows and columns */
		fits_get_num_rows(fptr, &nrows, &status) ;
		fits_get_num_cols(fptr, &ncols, &status); 
		//printf(" NRows = %i NCols = %i\n", nrows, ncols);

	/* read the column names from the TTYPEn keywords */
		int colnum[NbCol] = {0};
		long frow =1; 
		long felem=1; 
		int nfound;
		fits_read_keys_str(fptr, "TTYPE", 1, NbCol, ColName, &nfound, &status);
		//printf(" names found =%i Row  %10s %10s\n", nfound, ColName[0], ColName[1]);
		if ( nfound != NbCol ) goto BadTable;

		for (int i=0; i<NbCol; i++){
			if ( strstr(ColName[i], FieldName[0])) { colnum[0]=i+1;}
			if ( strstr(ColName[i], FieldName[1])) { colnum[1]=i+1;}
            TabData[i].assign(nrows,0.0);
		}

		/*  read columns */ 
		for (int i=0; i<NbCol; i++){
			int  typecode;
			long repeat, width;
			fits_get_coltype(fptr, colnum[i], &typecode, &repeat, &width, &status);
			fits_read_col(fptr, typecode, colnum[i], frow, felem, nrows, strnull, &TabData[i][0], &anynull,
					&status);
		}
		if ( status ) {
			printf(" FileF::Read BINARY_TBL problem: status=%i \n", status);
			lTable=0; status = 0;
		} else {  lTable=1;}

		BadTable: ; //printf( "Jump over BadTable:%s nfound=%i\n", TableName, nfound); 
	}// end tbl if
	
	Nhdu = hdu; 
	fits_movrel_hdu( fptr, 1, NULL, &status);  // move to the next HDU
	//printf( " Movrel: %s Nhdu=%i hdu=%i status=%i \n", fname, Nhdu, hdu, status); 
	if ( status ) {status = 0; iEOF=1;}   // Reset normal error, == END_OF_FILE
	return 0;
}

int FileF::getValue ( const char * name, double * Value )
{
	H_Iter =mHeader.find(name);
    if ( H_Iter == mHeader.end( ) ) {*Value=0.; return 1;}
	else {*Value= H_Iter->second; return 0; }

	return 1;
}
	
void FileF::PrintKeys (  )
{
	int msize = mHeader.size();
	printf(" FileF::PrintKeys Ncards= %i MapSize= %i \n", Ncards, msize);
	int i=0;
	for (H_Iter=mHeader.begin(); H_Iter != mHeader.end(); H_Iter++, i++){
		string name = H_Iter->first;
		double val = H_Iter->second;
		printf(" %2i. %s:: %f \n", i, name.c_str(), val );
	}
	return;
}

//---------------------------------------------------------------------
// FileList class ---------------------------------------------------------
class FileList {
public:

	string dir_name;
	string fname; 
	int Nfile;
	set <string> FName;
	set <string>::iterator FName_Iter;

public:
	FileList ( string dir_name );
	~FileList(){};
};

FileList::FileList ( string dir_nm ):
	Nfile(0)
{
	printf(" FileList::dir_nm: %s \n", dir_nm.c_str());
	const char *afile;
	const char fstr[] = ".fits";
    const char * pdest = strstr( dir_nm.c_str(), fstr );

    if ( pdest != 0 ){
		FName.insert( dir_nm );
		Nfile++;
		printf(" FileList:: Single file %i: %s \n", Nfile, dir_nm.c_str());
    }
	else {    // list files from the directory -----------------------------
        void *dirp = gSystem->OpenDirectory(dir_nm.c_str());
        while( (afile = gSystem->GetDirEntry(dirp)) ) {
            pdest = strstr( afile, fstr );
            if ( pdest == 0 ) continue;
            fname = afile;
			fname = dir_nm + "/" + fname;
            FName.insert( fname );
            printf("  %i: %s \n", Nfile, fname.c_str());
            Nfile++;
        }
        printf(" FileList::Nfiles = %i \n", Nfile);
    }

	return;
} //end FileList constructor



//---------------------------------------------------------------------
// FFT class ---------------------------------------------------------
class FFT
{  
public:

	const char * FFTName;
	const int minData;  
	const int maxData;  
	const int minLine;  
	const int maxLine; 
	const int stepD;  
	const int stepL;  

	const int Nline;
	const int Ndata;
	const int Nfreq;

	double * inA;
	fftw_complex * outF;
	fftw_plan p1d;

	vector<double> Freq;
 	vector<double> PSDs;
	enum {  MaxP = 20 };			
	vector<double> PSDav[MaxP];

	double * DaAv;

	TH1D * hbuf[4];

	int jb;
	int sbtr;
	const int kf;  //58 for rows 20130123
	const double Fk;
	const double Farg;
	const double Ak;

public:
	FFT( const char * Name,
		 const int minD,  const int maxD, 
		 const int minL,  const int maxL, 
		 const int stepD, const int stepL,
		 const int FrInd=0, const double Afk=2. );
	void DTrans(double * buf, const int idu, int sub); 
	void Plot_FFT(const int Nch);
	~FFT();
}; //end FFT class

FFT::~FFT()
{
	fftw_free(inA);  inA = 0;
	fftw_free(outF); outF = 0;
	fftw_destroy_plan(p1d); p1d=0;

	delete [] DaAv; DaAv = 0;
}

void FFT::Plot_FFT( int Nch )
{
	 const float left_margin = (float)0.04;
	 const float right_margin = (float)0.001;
	 const float top_margin = (float)0.01;
	 const float bot_margin = (float)0.04;

	if (Nch > MaxP) Nch=MaxP;
	const int Ncha=Nch;
 	TCanvas	* Tr[MaxP];
for (int u=0; u<Ncha; u++){  //Nhdu
	int ich=u+1;
	char tiname[50];
	char PDFname[50];
	sprintf(tiname, "%s %i", FFTName, ich);
	sprintf(PDFname, "%s_%i.pdf", FFTName, ich);
 	 Tr[u] = new TCanvas( tiname, tiname, 200+4*u, 10+2*u, 800, 600);
	 Tr[u]->SetBorderMode  (1);			//to remove border
	 Tr[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	 Tr[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	 Tr[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	 Tr[u]->SetBottomMargin(bot_margin);   //default
	 Tr[u]->SetFrameFillColor(0);
	 Tr[u]->Divide(1,1); 

/***************** Plot Power Spectrum  *************/
     printf(" Start draw Power Spectrum for %s %i\n\n", FFTName, u);
	 Tr[u]->cd(1);
	 gPad->SetLogy();
	 gPad->SetLogx();

	 do {
		 int Npsd = PSDav[ich].size();
		 int Nfrq = Freq.size();
 		 printf(" RowPSD points =%i RowFreq size=%i *** \n", Npsd, Nfrq);
		 if ( Npsd <= 3 ) continue;
		 int nn = Npsd -3;
		 if ( Nfrq < nn ) continue;
		 TGraph * std6 = new TGraph( nn, &Freq[0], &PSDav[ich][0]);
		 std6->SetTitle(FFTName);
		 std6->GetYaxis()->SetTitle("PSD ");
		 std6->GetXaxis()->SetTitle("k/N, 1/pixel");
		 std6->SetMarkerColor(4);
		 std6->SetMarkerStyle(21);
		 std6->Draw("AP");
			// cout << "i" << " - " << "Freq" << " - " << FFTName << " PSD" << endl;
		 //for (int i=0; i<Npsd; i++){
			// cout << i << ". - " << Freq[i] << " - " << PSDav[u][i] << endl;
		 //}
	 } while(0);

	Tr[u]->Update();
	Tr[u]->Print(PDFname);
}

/***************** Plot subtraction histos *************/

	TCanvas	*	Tb = new TCanvas( "subtract", "subtract", 250, 20, 800, 600);
	 Tb->SetBorderMode  (1);			//to remove border
	 Tb->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	 Tb->SetRightMargin (right_margin); //to move plot to the right 0.05
	 Tb->SetTopMargin   (top_margin);   //to use more space 0.07
	 Tb->SetBottomMargin(bot_margin);   //default
	 Tb->SetFrameFillColor(0);
	 Tb->Divide(2,2); 

	int Entry = (int)hbuf[0]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(1);
		gPad->SetLogy();
		gStyle->SetOptFit(1);
		hbuf[0]->Draw();
		hbuf[0]->Fit("gaus");
	}
	
	Entry = (int)hbuf[1]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(2);
		gPad->SetLogy();
		gStyle->SetOptFit(1);
		hbuf[1]->Draw();
		hbuf[1]->Fit("gaus");
	}

	Entry = (int)hbuf[2]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(3);
		gPad->SetLogy();
		hbuf[2]->Draw();
	}

	Entry = (int)hbuf[3]->GetEntries();
	if ( Entry >1 ) {
		Tb->cd(4);
		gPad->SetLogy();
		hbuf[3]->Draw();
	}

	 return;
} // end FFT::Plot_FFT

FFT::FFT(const char * Name,
		 const int minD, 
		 const int maxD, 
		 const int minL, 
		 const int maxL, 
		 const int stepDa,
		 const int stepLi,
		 const int FrInd,
		 const double Afk ):
	FFTName(Name),
	minData(minD),  
	maxData(maxD),  
	minLine(minL),  
	maxLine(maxL), 
	stepD(stepDa),
	stepL(stepLi),
	Nline(maxLine-minLine),
	Ndata(maxData-minData),
	Nfreq(Ndata/2 + 1),
	jb(0),
	sbtr(0),
	kf(FrInd),
	Fk( (double)kf/(double)Ndata ),
	Farg(TwoPi * Fk),
	Ak(Afk)
{
	int size = sizeof(double) * Ndata;
	int size2 = sizeof(fftw_complex) * Nfreq;
	inA = (double*) fftw_malloc(size);
	outF = (fftw_complex *)fftw_malloc(size2);
	p1d = fftw_plan_dft_r2c_1d(Ndata, inA, outF, FFTW_MEASURE);

	Freq.resize(Nfreq, 0.);
	for(int i =1; i < Nfreq; i++) { 
		Freq[i] = (double)i/Ndata;
 	}
	PSDs.resize(Nfreq,0.);
	for(int i =0; i < MaxP; i++) {PSDav[i].resize(Nfreq,0.);}

	DaAv = new double [Ndata];

	char title[40];
	char titl2[40];
	sprintf(title, "buf in %i", Ndata);
	hbuf[0] = new TH1D(title,title, 400,-100.,300.);
	hbuf[0]->SetStats(1);
	sprintf(title, "buf out %i", Ndata);
	hbuf[1] = new TH1D(title,title, 400,-100.,300.);
	hbuf[1]->SetStats(1);
	sprintf(title, "Asub %i", Ndata);
	sprintf(titl2, "kf %i", kf);
	hbuf[2] = new TH1D(title,titl2, 80,-16.,16.);
	hbuf[2]->SetStats(1);
	sprintf(title, "WA %i", Ndata);
	hbuf[3] = new TH1D(title,titl2, 80,0.,20.);
	hbuf[3]->SetStats(1);
}


void FFT::DTrans(double * buf, const int idu, int sub)
{
	sbtr = sub;
//  FFT direct transform 

	//for (int k=0, iL=minLine; iL<maxLine; iL++, k++) {
	//	DaAv[k]=0.;
	//	jb = iL*stepL + minData*stepD;
	//	for (int iD=minData; iD<maxData; iD++) {
	//		DaAv[k] += buf[jb];
	//		jb += stepD;
	//	}
	//	DaAv[k] /= Ndata; 
	//}

	PSDs.clear();
	PSDs.resize(Nfreq, 0.);  
	for (int k=0, iL=minLine; iL<maxLine; iL++, k++) {
		jb = iL*stepL + minData*stepD;
		for (int j=0, iD=minData; iD<maxData; iD++, j++) {
			inA[j] = buf[jb];  // - DaAv[k];  		  
			jb += stepD;
		}
		fftw_execute(p1d);

		for(int i=1; i<Nfreq; i++) {  
			PSDs[i] += outF[i][0]*outF[i][0]+outF[i][1]*outF[i][1];	 
		}

		if (sbtr) {
			double ReA = outF[kf][0]/Nfreq;  
			double ImA = outF[kf][1]/Nfreq;
			double WA = sqrt(ReA*ReA + ImA*ImA);
				   hbuf[3]->Fill( WA );
			//if ( WA > 0.000001) { ReA/=WA; ImA/=WA;}		
			jb = iL*stepL + minData*stepD;
			for (int j=0, iD=minData; iD<maxData; iD++, j++) {
				double arg = Farg * j;
				double A_subtr = (ReA*cos(arg) - ImA*sin(arg));  //Ak*
				hbuf[0]->Fill( buf[jb] );
				buf[jb] -= A_subtr; 
				hbuf[1]->Fill( buf[jb] );
				hbuf[2]->Fill( A_subtr );
				jb += stepD;
			}
		}
	}

	double Norm = (double)Nfreq * (double)Nfreq * (double)Nline;
	int ind=idu;
	if (ind >= MaxP ) {ind=0;}
	for(int i =1; i < Nfreq; i++) {  
	    PSDav[ind][i] += PSDs[i]/Norm; }

	return;
} // end FFT::DTrans


// other constants ++++++++++++++++++++++++++++++++++++++++++++++++++++
const double eQ = 1.60217657;  //charge of electron in C*10**19
const double gain[16] = {  // e2v-216 20170307
    2.898321,
    2.917557,
    2.912855,
    2.932351,
    2.919847,
    2.909128,
    2.941916,
    2.937061,
    3.003625,
    2.975065,
    2.975082,
    2.963982,
    2.922063,
    2.949454,
    2.927139,
    2.916869
};

struct SpectrResp {
    double wave;
    double Sresp;
    double qe;
};
const SpectrResp PDresp[] = {
    //wl(nm)	Sensitivity(A/W)	QE =1240*S/lambda
    { 200.,	0.125,	0.775},
    { 210.,	0.132,	0.779428571},
    { 220.,	0.133,	0.749636364},
    { 230.,	0.139,	0.749391304},
    { 240.,	0.141,	0.7285},
    { 250.,	0.133,	0.65968},
    { 260.,	0.12,	0.572307692},
    { 270.,	0.103,	0.473037037},
    { 280.,	0.103,	0.456142857},
    { 290.,	0.117,	0.500275862},
    { 300.,	0.131,	0.541466667},
    { 310.,	0.139,	0.556},
    { 320.,	0.142,	0.55025},
    { 330.,	0.149,	0.559878788},
    { 340.,	0.154,	0.561647059},
    { 350.,	0.153,	0.542057143},
    { 360.,	0.154,	0.530444444},
    { 370.,	0.154,	0.516108108},
    { 380.,	0.164,	0.535157895},
    { 390.,	0.179,	0.569128205},
    { 400.,	0.189,	0.5859},
    { 420.,	0.206,	0.608190476},
    { 440.,	0.222,	0.625636364},
    { 460.,	0.235,	0.633478261},
    { 480.,	0.247,	0.638083333},
    { 500.,	0.262,	0.64976},
    { 520.,	0.275,	0.655769231},
    { 540.,	0.286,	0.656740741},
    { 560.,	0.297,	0.657642857},
    { 580.,	0.311,	0.664896552},
    { 600.,	0.321,	0.6634},
    { 620.,	0.334,	0.668},
    { 640.,	0.347,	0.6723125},
    { 660.,	0.359,	0.674484848},
    { 680.,	0.368,	0.671058824},
    { 700.,	0.379,	0.671371429},
    { 720.,	0.391,	0.673388889},
    { 740.,	0.403,	0.675297297},
    { 760.,	0.413,	0.673842105},
    { 780.,	0.425,	0.675641026},
    { 800.,	0.434,	0.6727},
    { 820.,	0.445,	0.672926829},
    { 840.,	0.459,	0.677571429},
    { 860.,	0.469,	0.676232558},
    { 880.,	0.48,	0.676363636},
    { 900.,	0.491,	0.676488889},
    { 920.,	0.5,	0.673913043},
    { 940.,	0.512,	0.675404255},
    { 960.,	0.522,	0.67425},
    { 980.,	0.513,	0.649102041},
    {1000.,	0.492,	0.61008},
    {1020.,	0.439,	0.533686275},
    {1040.,	0.354,	0.422076923},
    {1060.,	0.244,	0.285433962},
    {1080.,	0.168,	0.192888889}
};
enum { Ncal = sizeof(PDresp)/sizeof(SpectrResp) };

struct SpectrRatio {
    double wave;
    double ratio;
};
const SpectrRatio PDratio[] = {
    //wl  ratio
    {310.,	270.9},
    {320.,	271.6},
    {330.,	272.2},
    {340.,	272.9},
    {350.,	273.5},
    {360.,	274.8},
    {370.,	277.4},
    {380.,	279.5},
    {390.,	280.5},
    {400.,	281.6},
    {410.,	282.0},
    {420.,	282.2},
    {430.,	282.2},
    {440.,	282.3},
    {450.,	282.4},
    {460.,	282.4},
    {470.,	282.6},
    {480.,	282.6},
    {490.,  282.6},
    {500.,	282.7},
    {510.,	282.7},
    {520.,	282.8},
    {530.,	282.9},
    {540.,  282.9},
    {550.,	283.0},
    {560.,	283.0},
    {570.,	283.2},
    {580.,	283.4},
    {590.,	283.6},
    {600.,	283.8},
    {610.,	284.0},
    {620.,  284.0},
    {630.,	284.1},
    {640.,	284.3},
    {650.,	284.5},
    {660.,	284.7},
    {670.,	284.8},
    {680.,	284.9},
    {690.,	285.0},
    {700.,	285.1},
    {710.,	285.2},
    {720.,	285.4},
    {730.,	285.6},
    {740.,	285.9},
    {750.,	286.4},
    {760.,	286.3},
    {770.,	286.6},
    {780.,	286.8},
    {790.,	287.1},
    {800.,	287.3},
    {810.,	287.6},
    {820.,  287.6},
    {830.,	287.7},
    {840.,	288.0},
    {850.,	288.3},
    {860.,	288.4},
    {870.,	288.6},
    {880.,	288.9},
    {890.,	289.0},
    {900.,	289.3},
    {910.,	289.5},
    {920.,	289.8},
    {930.,	290.3},
    {940.,	291.5},
    {950.,	291.8},
    {960.,	291.1},
    {970.,	290.9},
    {980.,	289.9},
    {970.,	289.9},
    {1010.,	287.9},
    {1020.,	285.8},
    {1020.,	282.7},
    {1030.,	278.7},
    {1040.,	273.7},
    {1060.,	268.8},
    {1070.,	265.0},
    {1080.,	262.1},
    {1090.,	260.6},
    {1090.,	266.1},
    {1100., 262.0}
};

enum { Nratio = sizeof(PDratio)/sizeof(SpectrRatio) };

double QEPD (double waw )
{
    double QE;
    if (waw <= PDresp[0].wave)      {QE=PDresp[0].qe; return QE;}
    if (waw > PDresp[Ncal -1].wave) {QE=PDresp[Ncal-1].qe; return QE;}
    int i=0;
    for (; i<Ncal; i++){
        if (waw > PDresp[i].wave) continue;
        break;
    }
    double w1=PDresp[i-1].wave;
    double w2=PDresp[i].wave;
    double q1=PDresp[i-1].qe;
    double q2=PDresp[i].qe;
    QE = q1 + (q2-q1)*(waw-w1)/(w2-w1);
    //printf(" QEPD: waw=%f i=%i w1=%f w2=%f q1=%f q2=%f qe=%f \n",waw,i, w1,w2,q1,q2,QE );
    
    return QE;
}

double RaPD (double waw )
{
    double Ra;
    if (waw <= PDratio[0].wave)        {Ra=PDratio[0]. ratio; return Ra;}
    if (waw > PDratio[Nratio -1].wave) {Ra=PDratio[Nratio-1]. ratio; return Ra;}
    int i=0;
    for (; i<Nratio; i++){
        if (waw > PDratio[i].wave) continue;
        break;
    }
    double w1=PDratio[i-1].wave;
    double w2=PDratio[i].wave;
    double q1=PDratio[i-1]. ratio;
    double q2=PDratio[i]. ratio;
    Ra = q1 + (q2-q1)*(waw-w1)/(w2-w1);
    //printf(" RaPD: waw=%f i=%i w1=%f w2=%f q1=%f q2=%f  ratio=%f \n",waw,i, w1,w2,q1,q2,Ra );
    
    return Ra;
}

//*********************************************************************
int PeakRange(TH1D * hist, double * maxpos, double * Rmin, double * Rmax, double * maxV)
{
    const double Fract = 0.04;
    int maxbin =    hist->GetMaximumBin();
    *maxpos =	    hist->GetBinCenter(maxbin);
    double maxval = hist->GetBinContent(maxbin);
    *maxV = maxval;
    
    cout << " maxbin=" << maxbin << " maxval=" <<maxval <<endl;
    cout << " max/maxpos=" << *maxpos << endl;
    
    int bin = maxbin;
    while (bin>0 && hist->GetBinContent(bin) > Fract*maxval) bin--;
    *Rmin = hist->GetBinCenter(bin);
    cout << " Rmin=" << *Rmin << " A(Rmin)=" << hist->GetBinContent(bin) << endl;
    
    int xlast   = hist->GetXaxis()->GetLast();
    bin = maxbin;
    while (bin<xlast && hist->GetBinContent(bin) > Fract*maxval) bin++;
    bin++;
    *Rmax = hist->GetBinCenter(bin);
    cout << " Rmax=" << *Rmax << " A(Rmax)=" << hist->GetBinContent(bin) << endl;
    
    return 0;
}

//---------------------------------------------------------------------
// PhotoDiode class ---------------------------------------------------------
class PhDi {
public:
    // canvas constants
    static const float left_margin;
    static const float right_margin;
    static const float top_margin;
    static const float bot_margin;
    
    static const int Npar;  //fit parameters
    
    // Photo Diode
    
    const int Nsets;
    const char * PDname;
    int Nf;
    int PDbad;
    double SiLe;
    double SiIn;
    int PDreads;
    double eTime;
    
    TCanvas	*	PDh;
    int NXpad;
    int NYpad;
    
    vector<TGraph *> PDplot;
    vector<TH1D   *> PDhisB;
    vector<TH1D   *> PDhisS;
    vector<TF1    *> fitB;
    vector<TF1    *> fitS;
    TH1D   * PDdtm;
    TH1D   * PD_Int;
    
    
    vector<double> FileNb;
    vector<double> WaveL;
    vector<double> PDbase;
    vector<double> PDbw;
    vector<int>    PDstat;
    vector<double> SiL;
    vector<double> SiI;
    vector<double> SinT;
    vector<double> SiE;
    TGraph * SiLplot;
    TGraph * SiIplot;
    TGraph * SiEplot;
    
public:
    PhDi ( int Nfiles, const char * PDn );
    ~PhDi(){};
    static double  Gauss ( double *v, double *par );  //fiting function
    double Integral( vector<double>::iterator first,
                    vector<double>::iterator last,
                    vector<double>::iterator point,
                    const double base,
                    const double b_rms,
                    int & readings,
                    vector<double> & Time,
                    double & iTime,
                    int & istat );
    void GetValue( int fNb, double wave, double time );
    void PlotData( void );
};

// initialization of static members
const float PhDi::left_margin =  (float)0.06;
const float PhDi::right_margin = (float)0.006;
const float PhDi::top_margin =   (float)0.01;
const float PhDi::bot_margin =   (float)0.04;
const int   PhDi::Npar = 4;

PhDi::PhDi( int Nfiles, const char * PDn ):
Nsets(Nfiles),
PDname(PDn)
{
    NXpad = 7;
    NYpad = 7;
    // PD histos +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    PDh = new TCanvas( PDname, PDname, 80, 60, 900, 800);
    PDh->SetBorderMode  (0);      //to remove border
    PDh->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    PDh->SetRightMargin (right_margin);  //to move plot to the right 0.05
    PDh->SetTopMargin   (top_margin);    //to use more space 0.07
    PDh->SetBottomMargin(bot_margin);    //default
    PDh->SetFrameFillColor(0);
    PDh->Divide(NXpad,NYpad);
    PDh->SetFillStyle(4100);
    PDh->SetFillColor(0);
    PDh->Update();
    
    PDplot.resize(Nsets);  //assign(Nsets,0);
    PDhisB.resize(Nsets);  //assign(Nsets,0);
    PDhisS.resize(Nsets);  //assign(Nsets,0);
    fitB.resize(Nsets);  //assign(Nsets,0);
    fitS.resize(Nsets);  //assign(Nsets,0);
    PDbad=0;
    
    char hname[50];
    sprintf(hname, "%s time", PDname);
    PDdtm = new TH1D (hname, hname, 100, 0., 25.);
    PDdtm->SetStats(1);
    sprintf(hname, "%s integral", PDname);
    PD_Int = new TH1D (hname, hname, 1000, 0., 100.);
    PD_Int->SetStats(1);
    
    //FileNb.resize(Nsets,0.0);
    //PDbase.resize(Nsets,0.0);
    //PDstat.resize(Nsets,0);
    //SiL.resize(Nsets,0.0);
    //SiI.resize(Nsets,0.0);
    //SinT.resize(Nsets,0.0);
    //SiE.resize(Nsets,0.0);
    //WaveL.resize(Nsets,0.0);
    
    //for (int i=0; i<Nsets; i++) {FileNb.push_back(i);}
    
    printf("PhDi object %s is constructed for %i files \n", PDname, Nsets);
    printf("Number of PD calibration points %i  *** \n", Ncal);
    //Nsets=-1;
    
}
//----------------------------------------------------------------
double  PhDi::Gauss ( double *v, double *par )
{
    
    double x = v[0];
    double s1 = par[2];
    if (s1 < 0.000001) s1=0.000001;
    //double GNorm = 1./(TMath::Sqrt(2.*TMath::Pi())*s1);
    double argG1 = (x - par[1])/s1;
    double fitval =	par[0]*TMath::Exp(-0.5*argG1*argG1);  //GNorm*
    return fitval;
}
//-----------------------------------------------------------------
double PhDi::Integral(vector<double>::iterator first,
                      vector<double>::iterator last,
                      vector<double>::iterator point,
                      const double base,
                      const double b_rms,
                      int & readings,
                      vector<double> & Time,
                      double & iTime,
                      int & istate )
{
    int indx;
    double ti;
    double Val = base - *point;
    double InT = 0.;
    double limit =  4.*abs(b_rms);
    if (abs(b_rms) < 0.0001 )  limit = 0.1*(base - *point);
    
    readings=0.;
    iTime=0.;
    istate=0;
    if (first == last) {PDbad++; return InT;}
    
    indx = std::distance(first,point);
    if (point==first)     {ti = Time[indx+1] - Time[indx]   ; }
    else if (point==last) {ti = Time[indx]   - Time[indx-1] ; }
    else                  {ti =(Time[indx+1] - Time[indx-1])/2.;}
    if (Val < limit) {PDbad++; return InT;}
    InT = Val*ti;
    
    vector<double>::iterator stop, start;
    start = stop = point;
    stop++;
    for (; stop <= last; stop++) {
        Val = base - *stop;
        if ( Val < limit) {stop--; break;}
        indx = std::distance(first,stop);
        if (stop==last) {ti = Time[indx]   - Time[indx-1] ;}
        else            {ti =(Time[indx+1] - Time[indx-1])/2.;}
        InT += Val*ti;
    }
    start--;
    for (; start >= first; start--) {
        Val = base - *start;
        if (Val < limit) {start++; break;}
        indx = std::distance(first,start);
        if (start==first) {ti = Time[indx+1] - Time[indx]   ;}
        else              {ti =(Time[indx+1] - Time[indx-1])/2.;}
        InT += Val*ti;
    }
    istate=1;
    int before = std::distance(first, start);
      readings = std::distance(start, stop) + 1;
    int after  = std::distance(stop,  last);
    if (before < 1) {istate=-1; PDbad++;}
    if (after  < 1) {istate=-2; PDbad++;}
    
    int ind_strt = std::distance(first,start);
    int ind_stop = std::distance(first,stop);
    iTime = Time[ind_stop] - Time[ind_strt];
    printf(" Integral: strt=%i pnt=%li stop=%i \n ",ind_strt, std::distance(first,point),  ind_stop);
    printf(" Integral: base=%f rms=%f limit=%f \n",base,b_rms,limit);
    
    return InT;
}  //Integral

void PhDi::GetValue( int fNb, double wave, double time )
{
    Nf=fNb;
    SiLe=0.;
    SiIn=0.;
    PDreads =0;
    eTime = time;
    double inTime;
    int istat;
    
    char title[50];
    
    double maxpos;
    double Rmin;
    double Rmax;
    double maxVa;
    
    const char fitname[] = "fitGauss";
    int base_flag = 10;
    double position = 0.;
    double width    = 0.;
    double peak_tot = 0.;
    //*****************************************************************************
    if ( Nf >= Nsets ) return;
    WaveL.push_back(wave);  // [Nf] = wave;
    FileNb.push_back(Nf);
    
    printf("PhDi::%s file=%i wave=%f \n", PDname, Nf, WaveL[Nf]);
    
    // units conversion to pA
    for (int i=0; i<FileF::nrows; i++) {FileF::TabData[1][i] *= 1.0e+12;}
    
    vector<double>::iterator iTab = std::min_element(FileF::TabData[1].begin(), FileF::TabData[1].end());
    double min = *iTab;
    double max = *std::max_element(FileF::TabData[1].begin(), FileF::TabData[1].end());
    printf("GetValue: nfile=%i nrows=%li min=%f max=%f \n", Nf, FileF::nrows, min, max);
    
    //Calculate baseline(B) and signal(S) levels
    double middle = (max + min) / 2.;
    double numB = 0., AvB =0.;
    double numS = 0., AvS =0.;
    for(int i = 0; i < FileF::nrows; i++){
        if(FileF::TabData[1][i] >= middle){
            AvB += FileF::TabData[1][i]; numB++; }
        else{   AvS += FileF::TabData[1][i]; numS++; }
    }
    if (numB > 0) {AvB /= numB;}
    else {AvB =0.;}
    if (numS > 0) {AvS /= numS;}
    else {AvS=0.;}
    printf("GetValue: numB=%f AvB=%f numS=%f AvS=%f \n", numB, AvB, numS, AvS);
    //find range of histograms (max-rms1 and rms2-min)
    double dB = max - AvB;
    double dS = AvS - min;
    double rangeB = AvB - 2.*dB;
    double rangeS = AvS + 2.*dS;
    cout << "RANGEB= " << rangeB << "RANGES= " << rangeS << endl;
    
    sprintf(title, "PD_S %i", Nf);
    PDhisS[Nf] = new TH1D (title, title, 16, min-dS, rangeS);
    PDhisS[Nf]->SetStats(1);
    sprintf(title, "PD_B %i", Nf);
    PDhisB[Nf] = new TH1D (title, title, 20, rangeB, max);
    PDhisB[Nf]->SetStats(1);
    double dt;
    double time_read= 0.;
    printf("time::");
    for (int i=0; i<FileF::nrows; i++) {
        PDhisB[Nf]->Fill(FileF::TabData[1][i]);
        PDhisS[Nf]->Fill(FileF::TabData[1][i]);
        dt =  FileF::TabData[0][i] - time_read;
        time_read = FileF::TabData[0][i];
        if (i<10) printf(" %7.4f",  dt);  //   %7.4f  FileF::TabData[0][i],
        if (i==0) continue;
        PDdtm->Fill(dt*1000.);
    }
    printf(" \n");
    PDplot[Nf]  = new TGraph(FileF::nrows, &FileF::TabData[0][0], &FileF::TabData[1][0]);
    
    // get base line and signal levels
    ////b_shift = hs[ch_idx][1]->GetMean();
    ////b_rms   = hs[ch_idx][1]->GetRMS();
    ////printf (" OS: shift=%f rms=%f \n", b_shift, b_rms);
    
    //int Entry;
    //Entry = (int)PDhisB[Nf]->GetEntries();
    //cout<<"PD base entries =" << Entry << endl;
    PeakRange( PDhisB[Nf], &maxpos, &Rmin, &Rmax, &maxVa);
    cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax <<endl;
    cout << endl;
    if ( maxVa>0. ) {
        
        if ( Nf < (NXpad*NYpad/2) ) {
            PDh->cd(1+2*Nf);
            gPad->SetFillStyle(4100);
            gPad->SetFillColor(0);
            //gPad->SetLogy();
            gPad->SetGrid(1,0);
            PDhisB[Nf]->SetLineColor(4);
            PDhisB[Nf]->Draw();
        } else { PDh->cd(NXpad*NYpad); }
        
        fitB[Nf] = new TF1(fitname, PhDi::Gauss, Rmin, Rmax, 3);
        fitB[Nf]->SetParameter(0, maxVa);
        fitB[Nf]->SetParameter(1, maxpos);
        fitB[Nf]->SetParameter(2, (Rmax - Rmin)/4.);
        cout << " par0=" << maxVa <<  endl;
        cout << " par1=" << maxpos << " par2=" << (Rmax - Rmin)/4. << endl;
        cout << "fitname=" << fitname << endl;
        
        base_flag = PDhisB[Nf]->Fit(fitname,"LR");  //NS
        peak_tot = fitB[Nf]->GetParameter(0);
        position = fitB[Nf]->GetParameter(1);
        width    = fitB[Nf]->GetParameter(2);
        printf (" PD base: peak_events=%f position=%f rms=%f fit_flag=%i \n",
                peak_tot, position, width, base_flag);
        PDh->Update();
        
        SiLe = position;
        PDbase.push_back(position);
        PDbw.push_back(width);
    }
    
    SiIn = Integral( FileF::TabData[1].begin(), FileF::TabData[1].end(), iTab, PDbase[Nf], width, PDreads,
                    FileF::TabData[0], inTime, istat );
    SiIn /=eQ;  // in 10^-12 [C]/(eQ*10^-19)[C] = 10^7/eQ in e- => in 10^7 e- = in 10 Me-
    SiIn /=100.; // convert in Ge-
    SiI.push_back(SiIn);
    SiE.push_back(PDreads);
    SinT.push_back(inTime);
    PDstat.push_back(istat);
    PD_Int->Fill(SiIn);
    printf(" SiIn=%f readings=%i inTime=%f istat=%i \n", SiI[Nf], PDreads, inTime, istat);
    
    //    Entry = (int)PDhisS[Nf]->GetEntries();
    //    cout<<"PD base entries =" << Entry << endl;
    PeakRange( PDhisS[Nf], &maxpos, &Rmin, &Rmax, &maxVa);
    cout << " maxpos=" << maxpos << " Rmin=" << Rmin << " Rmax" << Rmax <<endl;
    cout << endl;
    if ( maxVa>0. ) {
        
        if ( Nf < (NXpad*NYpad/2) ) {
            PDh->cd(2+2*Nf);
            gPad->SetFillStyle(4100);
            gPad->SetFillColor(0);
            //gPad->SetLogy();
            gPad->SetGrid(1,0);
            PDhisS[Nf]->SetLineColor(4);
            PDhisS[Nf]->Draw();
        } else { PDh->cd(NXpad*NYpad); }
        
        fitS[Nf] = new TF1(fitname, PhDi::Gauss, Rmin, Rmax, 3);
        fitS[Nf]->SetParameter(0, maxVa);
        fitS[Nf]->SetParameter(1, maxpos);
        fitS[Nf]->SetParameter(2, (Rmax - Rmin)/4.);
        cout << " par0=" << maxVa <<  endl;
        cout << " par1=" << maxpos << " par2=" << (Rmax - Rmin)/4. << endl;
        cout << "fitname=" << fitname << endl;
        
        base_flag = PDhisS[Nf]->Fit(fitname,"LR");  //NS
        peak_tot = fitS[Nf]->GetParameter(0);
        position = fitS[Nf]->GetParameter(1);
        width    = fitS[Nf]->GetParameter(2);
        printf (" PD sgnl: peak_events=%f position=%f rms=%f fit_flag=%i \n",
                peak_tot, position, width, base_flag);
        SiLe -= position;
        
    }
    
    
    PDh->Update();
    
    SiL.push_back(SiLe * eTime);
    
    return;
}  //end PhDi::GetValue

void PhDi::PlotData( void )
{
    char title[40];
    //int Entry;
    NXpad=10;
    NYpad=10;
    int Npads = NXpad*NYpad;
    int NH = (Npads<Nsets)?Npads:Nsets;
    
    // PD plots +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    sprintf(title, "%s_0", PDname);
    TCanvas	*	PD = new TCanvas( title,title, 60, 40, 900, 800);
    PD->SetBorderMode  (0);      //to remove border
    PD->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    PD->SetRightMargin (right_margin);  //to move plot to the right 0.05
    PD->SetTopMargin   (top_margin);    //to use more space 0.07
    PD->SetBottomMargin(bot_margin);    //default
    PD->SetFrameFillColor(0);
    PD->Divide(NXpad,NYpad);
    
    gPad->SetFillStyle(4100);
    gPad->SetFillColor(0);
    
    for (int i=0; i<NH; i++){
        if ( !PDplot[i] ) continue;
        PD->cd(1+i);
        //gPad->SetLogy();
        gPad->SetGrid(1,0);
        PDplot[i]->SetTitle("exposure");
        PDplot[i]->GetXaxis()->SetTimeDisplay(1);
        PDplot[i]->GetXaxis()->SetLabelOffset((float)0.02);
        PDplot[i]->SetMarkerColor (4);
        PDplot[i]->SetMarkerStyle (8);
        PDplot[i]->SetMarkerSize  (1);
        PDplot[i]->SetLineColor   (4);
        PDplot[i]->SetLineWidth   (2);
        PDplot[i]->Draw("ALP");
    }
    
    PD->Update();
    //PD->Print("PD0.pdf");
    
    // PD signal plot +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    sprintf(title, "%s_s", PDname);
    TCanvas	*	PDs = new TCanvas( title, title, 65, 45, 900, 800);
    PDs->SetBorderMode  (0);      //to remove border
    PDs->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    PDs->SetRightMargin (right_margin);  //to move plot to the right 0.05
    PDs->SetTopMargin   (top_margin);    //to use more space 0.07
    PDs->SetBottomMargin(bot_margin);    //default
    PDs->SetFrameFillColor(0);
    PDs->Divide(3,3);
    
    gPad->SetFillStyle(4100);
    gPad->SetFillColor(0);
    
    TGraph * PDsp  = new TGraph(Nsets, &WaveL[0], &SiL[0]);
    PDs->cd(1);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s current", PDname);
    PDsp->SetTitle(title);
    PDsp->GetXaxis()->SetTitle("Wavelength, nm");
    PDsp->GetXaxis()->SetLabelOffset((float)0.02);
    PDsp->GetYaxis()->SetTitle("Charge, pC");
    PDsp->SetMarkerColor (4);
    PDsp->SetMarkerStyle (8);
    PDsp->SetMarkerSize  (1);
    PDsp->SetLineColor   (4);
    PDsp->SetLineWidth   (2);
    PDsp->Draw("ALP");
    
    PDs->Update();
    
    TGraph * PDip  = new TGraph(Nsets, &WaveL[0], &SiI[0]);
    PDs->cd(2);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s integral", PDname);
    PDip->SetTitle(title);
    PDip->GetXaxis()->SetTitle("Wavelength, nm");
    PDip->GetXaxis()->SetLabelOffset((float)0.02);
    PDip->GetYaxis()->SetTitle("Charge, Ge-");
    PDip->SetMarkerColor (4);
    PDip->SetMarkerStyle (8);
    PDip->SetMarkerSize  (1);
    PDip->SetLineColor   (4);
    PDip->SetLineWidth   (2);
    PDip->Draw("ALP");
    
    PDs->Update();
    
    TGraph * PDep  = new TGraph(Nsets, &WaveL[0], &SiE[0]);
    PDs->cd(3);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s signal readings", PDname);
    PDep->SetTitle(title);
    PDep->GetXaxis()->SetTitle("Wavelength, nm");
    PDep->GetXaxis()->SetLabelOffset((float)0.02);
    PDep->SetMarkerColor (4);
    PDep->SetMarkerStyle (8);
    PDep->SetMarkerSize  (1);
    PDep->SetLineColor   (4);
    PDep->SetLineWidth   (2);
    PDep->Draw("ALP");
    
    PDs->Update();
    
    
    TGraph * PDit  = new TGraph(Nsets, &WaveL[0], &SinT[0]);
    PDs->cd(4);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s exposure", PDname);
    PDit->SetTitle(title);
    PDit->GetXaxis()->SetTitle("Wavelength, nm");
    PDit->GetXaxis()->SetLabelOffset((float)0.02);
    PDit->SetMarkerColor (4);
    PDit->SetMarkerStyle (8);
    PDit->SetMarkerSize  (1);
    PDit->SetLineColor   (4);
    PDit->SetLineWidth   (2);
    PDit->Draw("ALP");
    
    PDs->Update();
    
    TGraph * PDsip  = new TGraph(Nsets, &SiL[0], &SiI[0]);
    PDs->cd(5);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s Current vs Int", PDname);
    PDsip->SetTitle(title);
    PDsip->GetXaxis()->SetTitle("Charge L, pC");
    PDsip->GetXaxis()->SetLabelOffset((float)0.02);
    PDsip->GetYaxis()->SetTitle("Charge, Ge-");
    PDsip->SetMarkerColor (4);
    PDsip->SetMarkerStyle (8);
    PDsip->SetMarkerSize  (1);
    PDsip->SetLineColor   (4);
    PDsip->SetLineWidth   (2);
    PDsip->Draw("ALP");
    
    PDs->Update();
    
    TGraph * PDba  = new TGraph(Nsets, &FileNb[0], &PDbase[0]);
    PDs->cd(6);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s base line", PDname);
    PDba->SetTitle(title);
    PDba->GetXaxis()->SetTitle("File Nb");
    PDba->GetXaxis()->SetLabelOffset((float)0.02);
    PDba->GetYaxis()->SetTitle("Current, pA");
    PDba->SetMarkerColor (4);
    PDba->SetMarkerStyle (8);
    PDba->SetMarkerSize  (1);
    PDba->SetLineColor   (4);
    PDba->SetLineWidth   (2);
    PDba->Draw("ALP");
    
    PDs->Update();
    
    TGraph * PDbawi = new TGraph(Nsets, &FileNb[0], &PDbw[0]);
    PDs->cd(7);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s base line width", PDname);
    PDbawi->SetTitle(title);
    PDbawi->GetXaxis()->SetTitle("File Nb");
    PDbawi->GetXaxis()->SetLabelOffset((float)0.02);
    PDbawi->GetYaxis()->SetTitle("Current, pA");
    PDbawi->SetMarkerColor (4);
    PDbawi->SetMarkerStyle (8);
    PDbawi->SetMarkerSize  (1);
    PDbawi->SetLineColor   (4);
    PDbawi->SetLineWidth   (2);
    PDbawi->Draw("ALP");
    
    PDs->Update();
    
    TGraph * PDw  = new TGraph(Nsets, &FileNb[0], &WaveL[0]);
    PDs->cd(8);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s wavelength", PDname);
    PDw->SetTitle(title);
    PDw->GetXaxis()->SetTitle("File Nb");
    PDw->GetXaxis()->SetLabelOffset((float)0.02);
    PDw->GetYaxis()->SetTitle("Wavelength, nm");
    PDw->SetMarkerColor (4);
    PDw->SetMarkerStyle (8);
    PDw->SetMarkerSize  (1);
    PDw->SetLineColor   (4);
    PDw->SetLineWidth   (2);
    PDw->Draw("ALP");
    
    PDs->Update();

    PDs->cd(9);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s time interval, ms", PDname);
    PDdtm->GetXaxis()->SetTitle(title);
    PDdtm->GetXaxis()->SetLabelOffset((float)0.02);
    PDdtm->GetYaxis()->SetTitle("Counts");
    PDdtm->SetMarkerColor (4);
    PDdtm->SetMarkerStyle (8);
    PDdtm->SetMarkerSize  (1);
    PDdtm->SetLineColor   (4);
    PDdtm->SetLineWidth   (2);
    PDdtm->Draw("");
    
    PDs->Update();
    
    PDs->Print("PD0s.pdf");
    
    // PD signal QA plot +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    sprintf(title, "%s_QA", PDname);
    TCanvas	*	PDQA = new TCanvas( title, title, 65, 45, 900, 800);
    PDQA->SetBorderMode  (0);      //to remove border
    PDQA->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    PDQA->SetRightMargin (right_margin);  //to move plot to the right 0.05
    PDQA->SetTopMargin   (top_margin);    //to use more space 0.07
    PDQA->SetBottomMargin(bot_margin);    //default
    PDQA->SetFrameFillColor(0);
    PDQA->Divide(2,3);
    
    gPad->SetFillStyle(4100);
    gPad->SetFillColor(0);
    
    TGraph * PDqa1  = new TGraph(Nsets, &FileNb[0], &SiI[0]);
    PDQA->cd(1);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s charge", PDname);
    PDqa1->SetTitle(title);
    PDqa1->GetXaxis()->SetTitle("File Nb");
    PDqa1->GetXaxis()->SetLabelOffset((float)0.02);
    PDqa1->GetYaxis()->SetTitle("Charge, Me-");
    PDqa1->SetMarkerColor (4);
    PDqa1->SetMarkerStyle (8);
    PDqa1->SetMarkerSize  (1);
    PDqa1->SetLineColor   (4);
    PDqa1->SetLineWidth   (2);
    PDqa1->Draw("ALP");
    
    PDQA->Update();
    
    TGraph * PDqa2  = new TGraph(Nsets, &FileNb[0], &SiE[0]);
    PDQA->cd(2);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s signal readings", PDname);
    PDqa2->SetTitle(title);
    PDqa2->GetXaxis()->SetTitle("File Nb");
    PDqa2->GetXaxis()->SetLabelOffset((float)0.02);
    PDqa2->SetMarkerColor (4);
    PDqa2->SetMarkerStyle (8);
    PDqa2->SetMarkerSize  (1);
    PDqa2->SetLineColor   (4);
    PDqa2->SetLineWidth   (2);
    PDqa2->Draw("ALP");
    
    PDQA->Update();
    
    
    TGraph * PDqa3  = new TGraph(Nsets, &FileNb[0], &SinT[0]);
    PDQA->cd(3);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s exposure", PDname);
    PDqa3->SetTitle(title);
    PDqa3->GetXaxis()->SetTitle("File Nb");
    PDqa3->GetXaxis()->SetLabelOffset((float)0.02);
    PDqa3->SetMarkerColor (4);
    PDqa3->SetMarkerStyle (8);
    PDqa3->SetMarkerSize  (1);
    PDqa3->SetLineColor   (4);
    PDqa3->SetLineWidth   (2);
    PDqa3->Draw("ALP");
    
    PDQA->Update();
    
    TGraph * PDqa4  = new TGraph(Nsets, &FileNb[0], &PDbase[0]);
    PDQA->cd(4);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    sprintf(title, "%s base line", PDname);
    PDqa4->SetTitle(title);
    PDqa4->GetXaxis()->SetTitle("File Nb");
    PDqa4->GetXaxis()->SetLabelOffset((float)0.02);
    PDqa4->GetYaxis()->SetTitle("Current, pA");
    PDqa4->SetMarkerColor (4);
    PDqa4->SetMarkerStyle (8);
    PDqa4->SetMarkerSize  (1);
    PDqa4->SetLineColor   (4);
    PDqa4->SetLineWidth   (2);
    PDqa4->Draw("ALP");
    
    PDQA->Update();
    
    TGraph * PDqa5  = new TGraph(Nsets, &FileNb[0], &WaveL[0]);
    PDQA->cd(5);
    //gPad->SetLogy();
    gPad->SetGrid(1,0);
    //sprintf(title, "%s wavelength", PDname);
    PDqa5->SetTitle("Wavelength");
    PDqa5->GetXaxis()->SetTitle("File Nb");
    PDqa5->GetXaxis()->SetLabelOffset((float)0.02);
    PDqa5->GetYaxis()->SetTitle("Wavelength, nm");
    PDqa5->SetMarkerColor (4);
    PDqa5->SetMarkerStyle (8);
    PDqa5->SetMarkerSize  (1);
    PDqa5->SetLineColor   (4);
    PDqa5->SetLineWidth   (2);
    PDqa5->Draw("ALP");
    
    PDQA->Update();
    
    PDQA->cd(6);
    if ( PD_Int->GetEntries() > 0 ){
        //gPad->SetLogy();
        PD_Int->Draw();
    }
    
    PDQA->Print("PD0qa.pdf");
    
    return;
}  //end PhDi::PlotPDdata


//---------------------------------------------------------------------
// Bias class ---------------------------------------------------------
class Bias: public FileF 
{
public:
	int Flag;

 	const string dname;
 	const string outNm;
	string filename;

    int XminUse;  
	int XmaxUse;  
	int YminUse;  
	int YmaxUse;  

	int Nchan;
	int ch_idx;
	//vector<int> ChanIdu;

// FFT 
	const int doFFT;

	FFT * Col;
	FFT * Row;

//	double * bufd;
	double * tmpbuf;

// averages (vectors)

	static const double maxdata;
	vector<double> avbuf[MaxP];
	vector<double> mibuf[MaxP];
	vector<double> mabuf[MaxP];
//	vector<double> m2buf[MaxP];
	vector<double> trbuf[MaxP];
	vector<double> pix_rms[MaxP];

	double bias_rms[MaxP];

	enum { Nhist = 5 };
	TH1D * hb[MaxP][Nhist];
	TH1D * htrA[MaxP];
	TH1D * htrS[MaxP];
	TH1D * htrD[MaxP];
	enum { Nfhst = 16 }; //49
	TH1F * hfz[Nfhst];
	TGraph * gPDc[Nfhst];
	double PDbl[Nfhst];

public:
//	Bias ( const string & dir_name, const int dofft );
	Bias ( const string & dir_name,
		   const string & outFnm,
		   const int dofft        );
	void Plot_FFT();
	void Plot();
	int OutFitsF ( double * outbuf, const char * fOutName = "bias_outf" );
	int PrintVal( void  ); 
	~Bias();
};
const double Bias::maxdata=pow(2., 18);

Bias::~Bias(){
	//if (doFFT) {
	//	delete [] tmpbuf; tmpbuf = 0;
	//	delete    Col; Col = 0;
	//	delete    Row; Row = 0;
	//}
	//Col = 0;
	//Row = 0;
}

Bias::Bias ( const string & dir_name,
			 const string & outFnm,
			 const int dofft           ):
	Flag(-1),
	dname(dir_name),
	outNm(outFnm),
	Nchan(0),
	doFFT(dofft),
//	bufd(0),
	Col(0),
	Row(0)
{
	for (int i= 0; i< MaxP; i++) 
		{bias_rms[i]=0.; trbuf[i].resize(1,0);}
//FFileDB: *************************************************************
	//FFileDB mDB(dname);
	//int dbFiles=mDB.SelectFiles();
	//mDB.GetFparams();
	//cout << "dataOK=" << mDB.get_dataOK() << endl;
	//cout << "bitpix=" << mDB.get_bitpix() << endl;
	//cout << "naxis="  << mDB.get_naxis()  << endl;
 //   cout << "naxis1=" << mDB.get_naxis1() << endl;
	//cout << "naxis2=" << mDB.get_naxis2() << endl;
// files:  ************************************************************
	FileList FL(dname);
	if (FL.Nfile <= 0) {Flag= -10; return;}
	
// averages ***********************************************************

	int Nf = 0;
	int FFTinit=0;
	FL.FName_Iter=FL.FName.begin();
	cout << "Set starts:" << *FL.FName_Iter << endl;
	for (FL.FName_Iter=FL.FName.begin(); FL.FName_Iter != FL.FName.end(); FL.FName_Iter++ ) 
	{
		ch_idx=0;
		//filename = dname + "/" + *FL.FName_Iter;
		filename = *FL.FName_Iter;
		cout << "File: " << *FL.FName_Iter << endl;
		if ( Open(filename.c_str()) ) continue;

		while( !iEOF ){              
			if ( hdu > MaxP )   break;
			if ( Read() )    continue;
			if ( hdu == 1){
				for (int i=0; i<Nkey; i++){
					double v; 
					int ir = getValue( KeyList[i], &v );
					if (!i) v -= T1995; //convert CTIME to root time
					if (!ir) Vval[i].push_back( v ); 
				}
			}

			if ( naxis != 2) continue;

			if ( Nf == 0) {
				Nchan++;
				avbuf[ch_idx].resize(npixels,0);
				mibuf[ch_idx].resize(npixels,maxdata);
				mabuf[ch_idx].resize(npixels,0);
//				m2buf[ch_idx].resize(npixels,0);
			}

			for (unsigned long i=0; i<npixels; i++){
				double amp = (double)buffer[i];
				avbuf[ch_idx][i] += amp;
				if ( amp < mibuf[ch_idx][i] ) mibuf[ch_idx][i] = amp;
				if ( amp > mabuf[ch_idx][i] ) mabuf[ch_idx][i] = amp;
				//if ((amp > m2buf[ch_idx][i]) &&
				//	(amp < mabuf[ch_idx][i])) m2buf[ch_idx][i] = amp;
			}

			ch_idx++;
		}

		Nf++;
		Close();
	} //end "bias files for loop"

	printf(" Average over %i zero exposures Nhdu=%i \n", Nf, Nhdu);
	if ( Nf < 1 ) {printf(" No bias files \n"); Flag = -2; return;}
	if ( Nchan < 1 ) {printf(" No channels detected \n"); Flag = -2; return;}

	for (int u=0; u<Nchan; u++){
		printf(" Bias::Bias ch_idx=%i Nchan=%i avbuf_size=%lu npixels=%lu \n", u, Nchan, (long unsigned)avbuf[u].size(), npixels); 
		if ( avbuf[u].size() < npixels) continue;
				trbuf[u].resize(npixels,0);
		if ( Nf > 2 ) {
			for (unsigned long i=0; i<npixels; i++){ 
				trbuf[u][i]  = avbuf[u][i]-mabuf[u][i];
				trbuf[u][i] -= mibuf[u][i];
				trbuf[u][i] /= (Nf-2);
				avbuf[u][i] /= Nf;
				//htrA[u]->Fill( trbuf[u][i] );
			}
		}
		else {
			for (unsigned long i=0; i<npixels; i++) {
				avbuf[u][i] /= Nf;
				trbuf[u][i] = avbuf[u][i];
			}
		}
	 } //end Nhdu loop 

	if ( Nf < 2 ) {printf(" ** Only one bias file :( \n"); Flag = 0; return;}

// Histograms for Average Amplitudes & Spread
	for (int u=0; u<Nchan; u++){
		 char title[20];
		 char hname[50];
		 if ( u > MaxP )   break;
		 sprintf(title, "hb0%i", u);
		 sprintf(hname, "pixel average amplitude");
		 hb[u][0] = new TH1D( title, hname, 2000, 1900., 41900.);
		 hb[u][0]->GetXaxis()->SetNdivisions(505);
		 hb[u][0]->SetStats(0);

		 sprintf(title, "hb1%i", u);
		 sprintf(hname, "pixel simple average subtracted");
		 hb[u][1] = new TH1D( title, hname, 1000, -100., 100.);
		 hb[u][1]->GetXaxis()->SetNdivisions(505);
		 hb[u][1]->SetStats(0);

		 sprintf(title, "hb2%i", u);
		 sprintf(hname, "pixel rms");
		 hb[u][2] = new TH1D( title, hname, 400, 0., 40.);
		 hb[u][2]->GetXaxis()->SetNdivisions(505);
		 hb[u][2]->SetStats(0);

		 sprintf(title, "hb3%i", u);
		 sprintf(hname, "over_scan subtracted amplitude");
		 hb[u][3] = new TH1D( title, hname, 600, -100., 500.);
 		 hb[u][3]->SetDirectory(0);
		 hb[u][3]->GetXaxis()->SetNdivisions(505);
		 hb[u][3]->SetStats(0);

		 sprintf(title, "hb4%i", u);
		 sprintf(hname, "X_overscan average amplitude");
		 hb[u][4] = new TH1D( title, hname, 2000, 1900., 41900.);
		 hb[u][4]->GetXaxis()->SetNdivisions(505);
		 hb[u][4]->SetStats(0);

		 // truncated amplitude histos  
		 sprintf(title, "htrA%i", u);
		 sprintf(hname, "Traverage amplitudes");
		 htrA[u] = new TH1D( title, hname, 2000, 1900., 41900.);
		 htrA[u]->GetXaxis()->SetNdivisions(505);
		 htrA[u]->SetStats(0);
		 sprintf(title, "hmi%i", u);    
		 sprintf(hname, "pixel truncated average subtracted");
		 htrS[u] = new TH1D( title, hname, 1000, -100., 100.);
		 htrS[u]->GetXaxis()->SetNdivisions(505);
		 htrS[u]->SetStats(0);
		 sprintf(title, "hma%i", u);
		 sprintf(hname, "Tr rms");
		 htrD[u] = new TH1D( title, hname, 400, 0., 40.);
		 htrD[u]->GetXaxis()->SetNdivisions(505);
		 htrD[u]->SetStats(0);
	}

// get device description ************************

		DataStr * Dev = DataStr::Instance(nx,ny);

// fill average: Overscan Amp; Active Amp; Trunkated Amp;  
	for (int u=0; u<Nchan; u++){
		if ( u > MaxP )   break;
		if ( avbuf[u].size() < npixels) continue;
		for (int iy=Dev->minY(); iy<Dev->maxY(); iy++) {
			 for (int ix=0; ix<nx; ix++) {
				 int j = iy*nx + ix;
				 if ( Dev->IsOverX(ix) ) hb[u][4]->Fill( avbuf[u][j] );
				 else {
					 hb[u][0]->Fill( avbuf[u][j] );
					 htrA[u]->Fill(  trbuf[u][j] );
				 }
			 }
		 }
	}

// distribution arround average ---------------------------------------
	 FL.FName_Iter=FL.FName.begin();
	 for (int i=0; i<Nfhst; i++){
		gPDc[i]=0;
		PDbl[i]=0.;
		char htitle[40];
		sprintf(htitle, "File %i", i);
		if ( i >= FL.Nfile ) {hfz[i]=0; }
		else {
			filename = *FL.FName_Iter++; 
			hfz[i] = new TH1F( htitle, filename.c_str(), 1000, -250., 750.);
			hfz[i]->SetStats(1);
		}
	 }

// prepare FFT stuf -----------------

	if (doFFT && (!FFTinit)) {
		Col = new FFT(	"Bias Col",
						Dev->minY(), Dev->maxY(),
						Dev->minX(), Dev->maxX(),
						nx, 1);
		Row = new FFT(	"Bias Row",
						Dev->minX(), Dev->maxX(),
						Dev->minY(), Dev->maxY(),
						1, nx ); 
		FFTinit=1;
	}
		tmpbuf   = new double [npixels];
		memset(tmpbuf,0,npixels * sizeof(double));

	Nf=0;

	for (FL.FName_Iter=FL.FName.begin(); FL.FName_Iter != FL.FName.end(); FL.FName_Iter++ ) 
	{
		ch_idx=0;
		//filename = dname + "/" + *FL.FName_Iter;
		filename = *FL.FName_Iter;
		cout << "Distribution, File: " << *FL.FName_Iter << endl;
		if ( Open(filename.c_str()) ) continue;

		while( !iEOF ){              
			if ( hdu > MaxP )   break;
			if ( Read() )    continue;
			if ( lTable && strstr(TableName,"AMP0") && (Nf<Nfhst) ) {
				gPDc[Nf] = new TGraph(nrows, &TabData[0][0], &TabData[1][0]);
				for (int i=0; i<nrows; i++) {PDbl[Nf]+=TabData[1][i]/nrows;}
			}
			if ( naxis != 2) continue; 
			if ( Nf == 0) pix_rms[ch_idx].resize(npixels,0);
		
			double act = 0;
			double b_shift = 0;
			for (int iy=Dev->minY(); iy<Dev->maxY(); iy++) {
				for (int ix=Dev->minX(); ix<Dev->maxX(); ix++) {
				  int j = iy*nx + ix;
				  double amp = (double)buffer[j] - trbuf[ch_idx][j];
				  tmpbuf[j] = amp;
				  if (TMath::Abs(amp) < 100. ) {b_shift += amp; act+=1.;}
				}
			}
			if (act > 100000.) b_shift /= act;
			else b_shift=0.;
			//printf( " Bias: act=%f b_shift=%f \n", act, b_shift);
			if (doFFT) { 
				Row->DTrans(tmpbuf, ch_idx, 1); 
				Col->DTrans(tmpbuf, ch_idx, 0); 
			}

			for (int iy=Dev->minY(); iy<Dev->maxY(); iy++) {
				double a_over = 0.;
				for (int ix=nx-1; ix>=Dev->minX(); ix--) {
					int j = iy*nx + ix;
					if ( Dev->IsOverX(ix) ) a_over += (double)buffer[j]/Dev->NoverX(); 
					if ( Dev->IsOverX(ix) ) continue;
					double Amp = (double)buffer[j] - avbuf[ch_idx][j] - b_shift;
					double Atr = tmpbuf[j] - b_shift;  //(double)buffer[j] - trbuf[ch_idx][j] - b_shift;
					//if (doFFT) { tmpbuf[j] = Atr; }
					hb[ch_idx][1]->Fill(  Amp );
					hb[ch_idx][3]->Fill( (double)buffer[j] - a_over );
					htrS[ch_idx]->Fill(  Atr );
					if (Nf < Nfhst) hfz[Nf]->Fill( Atr );
					pix_rms[ch_idx][j] += Atr*Atr;
				}
			}

			ch_idx++;

		} //end while

		Nf++;
		Close();
	} //end "distributions" for loop

// Norm FFT vectors +++++++++++++++++++++++++++++++++++++++++++++++++++++

	if (doFFT) {
		for (int u=0; u<Nchan; u++){   
			for(int i =1; i < Col->Nfreq; i++) {Col->PSDav[u][i] /= Nf;}
			for(int i =1; i < Row->Nfreq; i++) {Row->PSDav[u][i] /= Nf;}
		}
	}		
//  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	if ( Nf < 2 ) {printf(" Not enough bias files for RMS: %i \n", Nf); Flag = 0; return;}

	// output file: *******************************************************
	string foName = "BiasNoise_";
	foName += outNm;
	foName += ".txt";
	printf(" Output File: %s  \n", foName.c_str());
	FILE * pFile = fopen (foName.c_str(),"wt");
	if (pFile == 0) {printf(" File %s cann't be open \n", foName.c_str()); Flag=-1; return;}
	fprintf (pFile,"// %s  input files from: %s \n", foName.c_str(), dname.c_str());
	fprintf (pFile,"// Channel   Noise rms \n");

	for (int u=0; u<Nchan; u++){
		bias_rms[u] = 0.;
		if ( pix_rms[u].size() < npixels) continue;
		for (unsigned long i=0; i<npixels; i++) pix_rms[u][i] /= (Nf-1.);
		for (int iy=Dev->minY(); iy<Dev->maxY(); iy++) {
			for (int ix=Dev->minX(); ix<Dev->maxX(); ix++) {
				int j = iy*nx + ix;
				double rms = TMath::Sqrt(pix_rms[u][j]);
				hb[u][2]->Fill( rms );
				if ( Nf < 4 ) continue;
				double S2 = pix_rms[u][j]*(Nf-1.);
				S2 -= (mibuf[u][j]-trbuf[u][j])*(mibuf[u][j]-trbuf[u][j]);
				S2 -= (mabuf[u][j]-trbuf[u][j])*(mabuf[u][j]-trbuf[u][j]);
				double Trms = TMath::Sqrt( S2/(Nf-3.) );
				htrD[u]->Fill( Trms );
			 }
		}
		bias_rms[u] = hb[u][2]->GetMean();
		fprintf (pFile," %d    %f \n", u, bias_rms[u]);
	}
	fclose( pFile );

	Flag = 0;
	return;
} //end Bias constructor

void Bias::Plot_FFT()
{
 	 const float left_margin = (float)0.04;
	 const float right_margin = (float)0.001;
	 const float top_margin = (float)0.01;
	 const float bot_margin = (float)0.04;

	TCanvas	*	Tr[MaxP];
for (int u=0; u<Nchan; u++){
	if ( avbuf[u].size() < npixels) continue;
	char tiname[50];
	char PDFname[50];
	sprintf(tiname, "Bias FFT %i", u);
	sprintf(PDFname, "BiasFFT_%i.pdf", u);
	if ( u > MaxP )   break;
 	 Tr[u] = new TCanvas( tiname,tiname, 200+4*u, 10+2*u, 800, 600);
	 Tr[u]->SetBorderMode  (1);			//to remove border
	 Tr[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	 Tr[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	 Tr[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	 Tr[u]->SetBottomMargin(bot_margin);   //default
	 Tr[u]->SetFrameFillColor(0);
	 Tr[u]->Divide(1,2); 

/***************** Plot Power Spectrum for Rows *************/
     printf(" Start draw Power Spectrum for Rows\n\n");
	 Tr[u]->cd(1);
	 gPad->SetLogy();
	 gPad->SetLogx();

	 do {
     int Npsd = Row->PSDav[u].size();
     int Nfrq = Row->Freq.size();
 	 printf(" RowPSD points =%i RowFreq size=%i *** \n", Npsd, Nfrq);
	 if ( Npsd <= 3 ) continue;
	 int nn = Npsd -3;
	 if ( Nfrq < nn ) continue;
	 TGraph * std6 = new TGraph( nn, &Row->Freq[0], &Row->PSDav[u][0]);
	 std6->SetTitle("Rows");
	 std6->GetYaxis()->SetTitle("PSD ");
	 std6->GetXaxis()->SetTitle("k/N, 1/pixel");
	 std6->SetMarkerColor(4);
	 std6->SetMarkerStyle(21);
	 std6->Draw("AP");
	 } while(0);

/*********************Plot Power Spectrum for Columns*********/
	 Tr[u]->cd(2);
	 gPad->SetLogy();
	 gPad->SetLogx();
 	 do {
     int Npsd = Col->PSDav[u].size();
     int Nfrq = Col->Freq.size();
 	 printf(" ColPSD points =%i ColFreq size=%i *** \n", Npsd, Nfrq);
	 if ( Npsd <= 3 ) continue;
	 int nn = Npsd -3;
	 if ( Nfrq < nn ) continue;
	 TGraph * std7 = new TGraph( nn, &Col->Freq[0], &Col->PSDav[u][0]);
	 std7->SetTitle("Columns");
	 std7->GetYaxis()->SetTitle("PSD ");
	 std7->GetXaxis()->SetTitle("k/N, 1/pixel");
	 std7->SetMarkerColor(6);
	 std7->SetMarkerStyle(21);
	 std7->Draw("AP");
	 } while(0);

	Tr[u]->Update();
	Tr[u]->Print(PDFname);
}
	 return;
}

void Bias::Plot( void )
{
	const float left_margin = (float)0.04;
	const float right_margin = (float)0.001;
	const float top_margin = (float)0.01;
	const float bot_margin = (float)0.04;
	TCanvas	*	Bi[MaxP];

	int NH = 0;   // used in canvas 2 as well
	double mean[Nfhst];
	while ( hfz[NH] && (NH < Nfhst) ){
		mean[NH] = hfz[NH]->GetMean();
		NH++;
	}
	//printf(" Bias::Plot NH=%i pVval=%p \n", NH, &Vval[0][0]);

for (int u=0; u<Nchan; u++){
	if ( avbuf[u].size() < npixels) continue;
	char tiname[50];
	char PDFname[50];
	sprintf(tiname, "Bias BL %i", u);
	sprintf(PDFname, "BiasBL_%i.pdf", u);
	if ( u > MaxP )   break;

// set bias canvas 1 ****************************************************
	Bi[u] = new TCanvas( tiname, tiname, 10+5*u, 10+5*u, 900, 800);
	Bi[u]->SetBorderMode  (0);			//to remove border
	Bi[u]->SetLeftMargin  (left_margin);  //to move plot to the right 0.14
	Bi[u]->SetRightMargin (right_margin); //to move plot to the right 0.05
	Bi[u]->SetTopMargin   (top_margin);   //to use more space 0.07
	Bi[u]->SetBottomMargin(bot_margin);   //default
	Bi[u]->SetFrameFillColor(0);
	Bi[u]->Divide(5,2);

	gPad->SetFillStyle(4100);
	gPad->SetFillColor(0);

	for (int i = 0; i<Nhist; i++){
		int Entry = (int)hb[u][i]->GetEntries();
		//printf(" Bias::Plot: hb%d GetEntries %i \n", i, Entry);
		if ( Entry < 1 ) continue;
		Bi[u]->cd(i+1);
		gPad->SetLogy(); 
		hb[u][i]->Draw();
	}

	Bi[u]->cd(6);
	gPad->SetLogy(); 
	htrA[u]->Draw();

	Bi[u]->cd(7);
	gPad->SetLogy(); 
	htrS[u]->Draw();
	
	Bi[u]->cd(8);
	gPad->SetLogy(); 
	htrD[u]->Draw();

	if ( (NH > 0) & (Vval[0].size() > 0) ){
		Bi[u]->cd(9);
		TGraph * pGmean = new TGraph( NH, &Vval[0][0], mean);
		pGmean->GetXaxis()->SetTimeDisplay(1);
		pGmean->GetXaxis()->SetLabelOffset((float)0.02);
		pGmean->GetXaxis()->SetTimeFormat("#splitline{%Y-%m-%d}{%H:%M:%S}"); 
		pGmean->SetTitle("Mean level"); 
		pGmean->SetMarkerColor (4);
		pGmean->SetMarkerStyle (8);
		pGmean->SetMarkerSize  (1);
		pGmean->SetLineColor   (4);
		pGmean->SetLineWidth   (2);
		pGmean->GetYaxis()->SetNdivisions (505);
		pGmean->GetYaxis()->SetTitle ("File mean bias level, adu");
		pGmean->GetYaxis()->CenterTitle();
		pGmean->GetYaxis()->SetTitleOffset ((float)1.06);
		pGmean->GetYaxis()->SetLabelSize ((float)0.037);
		pGmean->Draw("ALP");
	}

	Bi[u]->Update();
//	Bi[u]->Print(PDFname);

} // end "section" canvas loop

	if ( NH < 1 ) return;
// Bias canvas 2 ******************************************************
	TCanvas	*	Zi = new TCanvas( "BiasVar", "BiasVar", 50, 30, 900, 800);
	Zi->SetBorderMode  (0);      //to remove border
	Zi->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	Zi->SetRightMargin (right_margin);  //to move plot to the right 0.05
	Zi->SetTopMargin   (top_margin);    //to use more space 0.07
	Zi->SetBottomMargin(bot_margin);    //default
	Zi->SetFrameFillColor(0);
	Zi->Divide(7,7);

	gPad->SetFillStyle(4100);
	gPad->SetFillColor(0);

	for (int i=0; i<NH; i++){
		if ( hfz[i]->GetEntries() < 1 ) continue;
		Zi->cd(1+i);
		gPad->SetLogy(); 
		gPad->SetGrid(1,0);
		hfz[i]->Draw();
	}

	Zi->Update();
	//Zi->Print("BiasVar.pdf");
	
	 if ( !NH ) return;
// Bias canvas 3 ******************************************************
	TCanvas	*	PD = new TCanvas( "PDmonBias", "PDmonBias", 60, 40, 900, 800);
	PD->SetBorderMode  (0);      //to remove border
	PD->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
	PD->SetRightMargin (right_margin);  //to move plot to the right 0.05
	PD->SetTopMargin   (top_margin);    //to use more space 0.07
	PD->SetBottomMargin(bot_margin);    //default
	PD->SetFrameFillColor(0);
	PD->Divide(4,4); //7,7

	gPad->SetFillStyle(4100);
	gPad->SetFillColor(0);

	for (int i=0; i<NH; i++){
		if ( !gPDc[i] ) continue;
		PD->cd(1+i);
		//gPad->SetLogy(); 
		gPad->SetGrid(1,0);
		gPDc[i]->SetTitle("exposure"); 
		gPDc[i]->GetXaxis()->SetTimeDisplay(1);
		gPDc[i]->GetXaxis()->SetLabelOffset((float)0.02);
		gPDc[i]->SetMarkerColor (4);
		gPDc[i]->SetMarkerStyle (8);
		gPDc[i]->SetMarkerSize  (1);
		gPDc[i]->SetLineColor   (4);
		gPDc[i]->SetLineWidth   (2);
		gPDc[i]->Draw("ALP");
	}
	
	PD->cd(16);  //49
	double xnf[Nfhst];
	for (int i=0; i<NH; i++){ xnf[i]=i;}
	 TGraph * pPDa = new TGraph( NH, xnf, PDbl);
	 pPDa->SetTitle("PD average"); 
	 pPDa->GetXaxis()->SetLabelOffset((float)0.02);
	 pPDa->SetMarkerColor (14);
	 pPDa->SetMarkerStyle (8);
	 pPDa->SetMarkerSize  (1);
	 pPDa->SetLineColor   (14);
	 pPDa->SetLineWidth   (2);
	 pPDa->Draw("ALP");

	PD->Update();
	//PD->Print("PD0.pdf");
	return;
} //end Plot	


int Bias::PrintVal( void  ) 
{
	for (int i=0; i<Nkey; i++) {
		printf(" Key= %s \n", KeyList[i]); 
		for ( V_Iter = Vval[i].begin( ); V_Iter != Vval[i].end( ); V_Iter++){
			printf(" val= %f \n", *V_Iter);} 
	}
	return 0;
}

// end Bias class --------------------------------------------------------------------

