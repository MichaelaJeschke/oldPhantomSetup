#ifndef DATA_MANAGEMENT
#define DATA_MANAGEMENT

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <fstream.h>
#include <iostream.h> 
#include <string.h> 
#include <io.h>

#define MAX_STR_LENGTH  64 

#define REF_STIM    0 
#define COMP_STIM   1  

#define LEFT_STIM   0 
#define RIGHT_STIM  1
#define NO_STIM    -1

#define INNER_STIM  0 // Alex texture
#define OUTER_STIM  1
#define NOT_STIM   -1

#define MAX_TRIALS        5000
#define MAX_CONTROLDAT   20000
#define NOT_FOUND       100001 // bigger than MAX_CONTROLDAT

#define MAX_STIMS_IN_CONDITION   50
#define MAX_STIMPAIRS_IN_BLOCK  500
#define MAX_BLOCKS               50


class StimPair {        

public:
	char moveFileName_int[MAX_STR_LENGTH];  //in dat_man generiert
    double stiff_int[2];
    double leftStiff_int;
    double stimPairWeight_int;
    double decision_int;
    short int blNr_int;
    short int trNr_int; 
	short int moveStart_int; //ALEX
	//short int picMax_int;
	double  orientCond_int; //ALEX Texture
	double priorIndex_int; // Aaron Prior Index
	double quality_int;		// Aaron Quality of PriorInfo


    // RIGHT_STIM, LEFT_STIM
	void put(StimPair* sp)  // kopiere stimuluspaar ins *sp
    {
        strcpy (sp->moveFileName_int, moveFileName_int);

		for (int i=0; i<2; i++) 
		{
			sp->stiff_int[i]=stiff_int[i];

		}

		sp->leftStiff_int=leftStiff_int;
		sp->orientCond_int=orientCond_int;// texture
		sp->priorIndex_int=priorIndex_int; //Aaron
		sp->quality_int=quality_int;	// Aaron

	}
};



// Speichert die Stimulus Werte, die aus control.red ausgelesen wurden.
struct StimulusData {
    double stiff_int;
    bool isRef_int;  
	double orientCond_int; // Texture
	double priorIndex_int; // Index für Bildnummer
	double quality_int;	   // Quality of PriorInfo
}; 


// Speichert die eingelesenen Werte einer Condition. 
struct Condition {
    int stimsNum; 
    StimulusData stimData[MAX_STIMS_IN_CONDITION];
};



struct Block {
    int condsNum; 
    Condition conds[MAX_BLOCKS];
};



struct PresentationBlock {
    int stimPairsNum_int; 
    int presentationOrder[MAX_STIMPAIRS_IN_BLOCK];
    StimPair stimPairs[MAX_STIMPAIRS_IN_BLOCK];
};


class StimManage {
public:
    StimManage (void) : blocksNum_int(0),
                        allPresetTrNrInBl_int(0),
                        correctPresetTrNrInBl_int(0),
                        currBlOrdInd_int(0){}; //InputFile, Outputfile
	void Init(char*, char*, bool, int, bool, int);
	StimPair* getNext();
    void buildStimPairs (void);
    void shuffle();
	void latinize(int *aBlPresentOrd, int aLatinSquareRow, int aBlNr);
    void randomize(int *aBlPresentOrd, int aBlNr);
	void Error (char*);
    void readStimuliDataFromControlFile(char *aFileName);
    void readPresetationOrderFromFile(char *aFileName);
    void writePresetationOrderInFile(char *aFileName);

    void saveStimPair(StimPair * aStimPair); 
      
    int  getBreakLength(void)               { return breakLength_int; }
    int  getCurrentBlockOrderIndex (void)   { return currBlOrdInd_int;}
//	char outfileDatName_int[MAX_STR_LENGTH];

private:
    int allPresetTrNrInBl_int; 
    int correctPresetTrNrInBl_int;   
    int currBlOrdInd_int;
	int condOrder_int;   // Condition order, welche Zeile des 
                            // Latin_Square soll verwendet werden
                            // Legt die Reihenfolge der Präsentation der Trials fest.
    int blPresentOrd_int[MAX_BLOCKS];
    int trPresentOrd_int[MAX_BLOCKS][MAX_STIMPAIRS_IN_BLOCK];
	int repsNum_int; 
	int blocksNum_int; // Anz. d. blocks u. d. conds
    int breakLength_int; // Pausendauer in millisekunden

	StimPair *currStimPair_int; 

    Block blocks_int[MAX_BLOCKS]; 

    PresentationBlock presentBl_int[MAX_BLOCKS];

	char outfileDatName_int[MAX_STR_LENGTH], // ALEX 
         outfilePoolName_int[MAX_STR_LENGTH], 
         outfileCoreName_int[MAX_STR_LENGTH],
   		 contOrderFilename_int[MAX_STR_LENGTH],
         contNumberFilename_int[MAX_STR_LENGTH];
 };


#endif
