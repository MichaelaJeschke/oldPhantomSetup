#include <direct.h>
#include "dataManager.h"
#include <string.h>
#include <math.h>
     
#define LATIN_SQUARE_MAX    10



// Parameters:
//  aInFileName:        name of the control file   
//  aOutFile:           name if the output file  
//  aContinuePrevious:  should the previous experiment be continued
//  aSubNr:             integer number needed for latinization 
//  aShuffleStimPairs:  true = shuffle the condition order in block
//  aShuffleBlocks:     0 = no shuffle
//                      1 = latinise block order
//                      2 = randomise block order

void StimManage::Init(char *aInFileName, char *aOutFile, bool aContinuePrevious, 
                      int aSubNr, bool aShuffleStimPairs, int aShuffleBlocks) {

    int k;
 
    ofstream fileOutputStream; 
    ifstream fileInputStream;

    // init_datacollection
	currStimPair_int = new StimPair;

	// Outfiledefinition
	k = sprintf(outfileCoreName_int, "data\\");
	sprintf(k+outfileCoreName_int, aOutFile);

	k = sprintf(outfileDatName_int, outfileCoreName_int);
	sprintf(k+outfileDatName_int,".raw");
	
    k = sprintf(contOrderFilename_int, outfileCoreName_int); 
	
    k += sprintf(contOrderFilename_int+k, "\\ContOrder.dat");
	
    k = sprintf(contNumberFilename_int, outfileCoreName_int); 
	
    k += sprintf(contNumberFilename_int+k, "\\ContNumber.dat");

	_mkdir(outfileCoreName_int);
	


    if (aContinuePrevious == false) 
    {
		fileOutputStream.open(outfileDatName_int, ios::noreplace);
        if (fileOutputStream.is_open()) fileOutputStream.close(); 
        else                            Error("File already exists!");
	}

    else 
    {
		fileOutputStream.open(outfileDatName_int, ios::nocreate | ios::app);
		if (fileOutputStream.is_open()) fileOutputStream.close(); 
        else                            Error("File does not exist!");
	}

    readStimuliDataFromControlFile(aInFileName);

    condOrder_int = aSubNr % blocksNum_int;   // Muss aufgerufen werden nachdem
                                              // die cotrol.red Datei eingelesen wurde
	buildStimPairs();

    if (aContinuePrevious == false) 
    {
        if (aShuffleStimPairs) 
	        shuffle ();

        if (aShuffleBlocks == 1)
            latinize(&blPresentOrd_int[0], condOrder_int, blocksNum_int);

        else if (aShuffleBlocks == 2)
            randomize(&blPresentOrd_int[0], blocksNum_int);

        writePresetationOrderInFile(contOrderFilename_int);  
    }

    else
    {
        readPresetationOrderFromFile(contOrderFilename_int);
    }
}






StimPair* StimManage::getNext() {

    int k;
    static int currBlockNr = 0;
    static int currTrialNr = 0;
	

    // Wenn alle Simuli bereits präsentiert wurden wird NULL zurückgegeben.
    if (currBlOrdInd_int >= blocksNum_int) 
    {
        return NULL;
    }


    // Die aktuelle Block-Nummer aus dem Array holen
    currBlockNr = blPresentOrd_int[currBlOrdInd_int];

    currTrialNr = trPresentOrd_int[currBlockNr][correctPresetTrNrInBl_int];
 

    presentBl_int[currBlockNr].stimPairs[currTrialNr].put(currStimPair_int);

    currStimPair_int->blNr_int = currBlOrdInd_int;
    currStimPair_int->trNr_int = allPresetTrNrInBl_int;


    k = sprintf(currStimPair_int->moveFileName_int, outfileCoreName_int);

    k+=sprintf(currStimPair_int->moveFileName_int+k, 
               "\\ET#_%d_%d_", currBlOrdInd_int, allPresetTrNrInBl_int);

    sprintf(currStimPair_int->moveFileName_int+k, ".trj");

    return currStimPair_int;
}




// Speichert die Parameter des letzten Trials in einer .row Datei.
// Nach dem Speichern werden die Zählervariablen für das nächst zu präsentiernde
// StimPair aktualisiert und anschließend in der Datei "continue.dat" gespeichert.
void StimManage::saveStimPair(StimPair * aStimPair) 
{
    ofstream fos;

    // Die Werte des zu leztet präsetierten Stilulus-Paares in dem RAW-File speichern
    fos.open(outfileDatName_int, ios::app); 

    fos << aStimPair->blNr_int              << " " <<  
           aStimPair->trNr_int              << " " <<
		   aStimPair->priorIndex_int		<< " " <<  // Picture Index
           aStimPair->orientCond_int		<< " " <<  // Orientation name
		   aStimPair->quality_int			<< " " <<  // Prior Quality
	       aStimPair->moveStart_int         << " " <<  // movementstart right (1) or left (0) ALEX
           aStimPair->stiff_int[REF_STIM]   << " " <<
           aStimPair->stiff_int[COMP_STIM]  << " " << 
           aStimPair->leftStiff_int         << " " <<  // left stim 
           aStimPair->decision_int          << " " <<  // chosen stim
           aStimPair->stimPairWeight_int    << " " <<  // Stim weight
           endl;
    fos.close();

    allPresetTrNrInBl_int++;

    // Die Werte des letzten korrekten Trials auf der Konsole ausgeben
    // (zur Kontrolle für den VL)
    // Die Zählervarirablen für stimPairs inkrementieren
    correctPresetTrNrInBl_int++;

    // Falls alle stimPairs dieses Blocks presentiert wurden:
    if (correctPresetTrNrInBl_int >= presentBl_int[blPresentOrd_int[currBlOrdInd_int]].stimPairsNum_int) 
    {
        correctPresetTrNrInBl_int = 0;
        allPresetTrNrInBl_int = 0;
        currBlOrdInd_int++;
    }

    // Block und Trial-Nummer des zu letzt abgeschlossenen und gespeicherten 
    // trials in der datei speichern.
    fos.open(contNumberFilename_int);
    fos << currBlOrdInd_int << " " << 
           correctPresetTrNrInBl_int << " " << 
           allPresetTrNrInBl_int;
    fos.close();
}



// Liesst die Daten aus dem control.red file aus und speichert sie in 
// einem array ab.
void StimManage::readStimuliDataFromControlFile(char *aFileName) {

    //int numberOfBlocks = 0;
    FILE *file = NULL;
    char input[64];
    int blNr = 0;
    int condNr  = 0;
    int stimNr  = 0;
	double PInd = 0;
    

    if ( (file = fopen(aFileName, "r")) == NULL) 
    {
        Error("Cannot open control file!");
    }

    while (fscanf(file, "%s", input) != EOF) 
    {
        if (!strcmp (input, "Repetitions:")) 
        {
            fscanf(file, "%s", input); 
            repsNum_int = atoi(input);
        }

        if (!strcmp (input, "Breaklength:")) 
        {
            fscanf(file, "%s", input);   
            breakLength_int = atoi(input) * 1000; // intern wird die Pausenlänge in
                                                     // ms gespeichert
        }

        if (!strcmp (input, "Block:")) 
        {
            condNr = 0;

            // Das Schlüsselwort "Cond" sollte folgen.
            fscanf(file, "%s", input);
            
            // Die conditions lesen, bis "EndBlock" kommt.
            while (strcmp (input, "EndBlock")) 
            {
				stimNr = 0;
                // Den 1. Stimulus-Wert dieser condition lesen 
                fscanf(file, "%s", input);

                // Solange kein "End" kommt werden Stimulus-Werte gesesen
                while (strcmp (input, "EndCond")) 
                {		
			
						// Der erste Stimulus ist der Ref-stimulus
						if (strcmp(input,"ref") == 0)
							{ 
							fscanf(file, "%s", input);
							blocks_int[blNr].conds[condNr].stimData[stimNr].stiff_int = (float)atof(input);
							blocks_int[blNr].conds[condNr].stimData[stimNr].isRef_int = true;
                    		stimNr++;
							}

						// Hier wird die Stimulus Orientierung festgelegt
						if(strcmp(input,"orient") == 0)
						{
						fscanf(file, "%s", input);
						float orientCond_int = (float)atof(input);
						while (strcmp (input, "orientEnd")) 
						{
							fscanf(file, "%s", input);

							// Hier wird die Prior Qualität festgelegt
							if(strcmp(input,"Quality") ==0)
							{
							fscanf(file, "%s", input);
							float quality_int = (float)atof(input);
							while (strcmp (input, "QualityEnd"))
							{
								fscanf(file, "%s", input);

								// Der Comp Stimulus enthält alle Daten über Prior, Quality und Orientierung
								if(strcmp(input,"comp") == 0) 
								{
									fscanf(file, "%s", input);
									blocks_int[blNr].conds[condNr].stimData[stimNr].stiff_int = (float)atof(input); 
									blocks_int[blNr].conds[condNr].stimData[stimNr].isRef_int = false;
									blocks_int[blNr].conds[condNr].stimData[stimNr].orientCond_int= orientCond_int;
									blocks_int[blNr].conds[condNr].stimData[stimNr].quality_int = quality_int;
									blocks_int[blNr].conds[condNr].stimData[stimNr].priorIndex_int= PInd;
									cout << PInd << endl; // Aaron!
									PInd++;
									PInd++;
									stimNr++;

								
								}
							}
							}
						}
						}
			
				
					
					fscanf(file, "%s", input);



                } // Alle Stimuli-Werte in der Condition gelesen.

                blocks_int[blNr].conds[condNr].stimsNum = stimNr;

                condNr++;

                fscanf(file, "%s", input);

            } //  Alle Condition in dem Block gelesen.

            blocks_int[blNr].condsNum = condNr;

            blNr++;
        }

    }   // Control.red Datei komplett gelesen

    // Anzahl der Blöcke entspricht d. Anz. d. conditions.
    blocksNum_int = blNr;

    fclose(file);
}




// Liesst die Präsentationsreihenfolge der Trials aus der Datei 'ContOrder.dat'
void StimManage::readPresetationOrderFromFile(char *aFileName) 
{	
    int blNr, stimPairIndexInBlock; 
    
    ofstream fileOutputStream;
	ifstream fileInputStream;

    FILE *file = NULL;
    char input[64];


    if ( (file = fopen(aFileName, "r")) == NULL) 
    {
        Error("Cannot open file 'ContOrder.dat'!");
    }


    for (blNr = 0; blNr < blocksNum_int; blNr++) 
    {
        fscanf(file, "%s", input);
        blPresentOrd_int[blNr] = atoi(input);
    }


    for (blNr = 0; blNr < blocksNum_int; blNr++) 
    {
        for (stimPairIndexInBlock = 0; 
             stimPairIndexInBlock < presentBl_int[blNr].stimPairsNum_int; 
             stimPairIndexInBlock++)
        {
            fscanf(file, "%s", input);
            trPresentOrd_int[blNr][stimPairIndexInBlock] = atoi(input); 
        }
    }
	fclose(file);


    if ( (file = fopen(contNumberFilename_int, "r")) == NULL) 
    {
        Error("Cannot open file 'ContNumber.dat'!");
    }

    fscanf(file, "%s", input);
    currBlOrdInd_int = atoi(input);

    fscanf(file, "%s", input);
    correctPresetTrNrInBl_int = atoi(input);

    fscanf(file, "%s", input);
    allPresetTrNrInBl_int = atoi(input);
	
    fclose(file);
}




// Schreibt die Präsentationsreihenfolge der Trials in die Datei 'ContOrder.dat'
void StimManage::writePresetationOrderInFile(char *aFileName) 
{
    int blNr, stimPairIndexInBlock; 
    
    ofstream fileOutputStream;
	ifstream fileInputStream;

    FILE *file = NULL;
    
    // File öffnen zum schreiben der Präsentationsreihenfolge der Trials
    fileOutputStream.open(aFileName);

    for (blNr = 0; blNr < blocksNum_int; blNr++) 
	{
        // Präsentationsreihenfolge der Blocks in die Datei schreiben
        fileOutputStream << blPresentOrd_int[blNr] << " ";
    }
    fileOutputStream << endl;


    for (blNr = 0; blNr < blocksNum_int; blNr++) 
	{
        for (stimPairIndexInBlock = 0; 
             stimPairIndexInBlock < presentBl_int[blNr].stimPairsNum_int; 
             stimPairIndexInBlock++)
        {
            // Präsentationsreihenfolge der Trials in die Datei schreiben
            fileOutputStream << trPresentOrd_int[blNr][stimPairIndexInBlock] << " ";
        }
        fileOutputStream << endl;
    }
	fileOutputStream.close();

}



// Bildet die Hubbelpaare (stimpairs).
void StimManage::buildStimPairs(void) 
{ 
    StimPair master;
    StimulusData refStim;
    StimulusData compStim;
    int stimPairIndexInBlock = 0;


    for (int blNr = 0; blNr < blocksNum_int; blNr++) 
    { 
        for (int condNr = 0; condNr < blocks_int[blNr].condsNum; condNr++) 
        {   
            for (int stimNr = 1; 
                 stimNr < blocks_int[blNr].conds[condNr].stimsNum; 
                 stimNr++) 
            {
                refStim = blocks_int[blNr].conds[condNr].stimData[0];
                
                compStim = blocks_int[blNr].conds[condNr].stimData[stimNr];


                master.stiff_int[REF_STIM]   = refStim.stiff_int;
                master.stiff_int[COMP_STIM] = compStim.stiff_int;
				master.orientCond_int= compStim.orientCond_int; // ALEX Texturorientierung
				master.priorIndex_int= compStim.priorIndex_int; // Aaron Prior Index
				master.quality_int= compStim.quality_int;		// Aaron PriorInfoQuality

				for (int repNr = 0; repNr < repsNum_int; repNr++) 
                {
                    // Es werden zwei stimPairs mit den selben Werten gebildet
                    // Bei dem ersten Paar ist der erste Hubbel der  
                    // Referenz-Hubbel (leftStiff_int = 0) und bei dem zweiten
                    // ist der erste Hubbel der Vergleichshubbel (leftStiff_int = 1).
                    for (int i=0; i<2; i++)
                    {
                        master.put(&presentBl_int[blNr].
                            stimPairs[stimPairIndexInBlock]);

                        //presentBl_int[blNr].stimPairs[stimPairIndexInBlock].leftStiff_int = i;
                        presentBl_int[blNr].stimPairs[stimPairIndexInBlock].leftStiff_int = master.stiff_int[i];
						
						presentBl_int[blNr].stimPairs[stimPairIndexInBlock].priorIndex_int = master.priorIndex_int+i;

                        trPresentOrd_int[blNr][stimPairIndexInBlock] = 
                            stimPairIndexInBlock;

                        stimPairIndexInBlock++;
                    }
                }
				 
			}
        }
        blPresentOrd_int[blNr] = blNr;

        presentBl_int[blNr].stimPairsNum_int = stimPairIndexInBlock;

        stimPairIndexInBlock = 0;
    }
}




// Mischt die Indexe der Trials und zwar in allen Blöcken
void StimManage::shuffle() 
{ 
	int zufall, blNr, stimPairIndexInBlock; 
    int presentationOrder[MAX_BLOCKS][MAX_STIMPAIRS_IN_BLOCK]; 

	srand((unsigned)time(NULL));

    // Werte kopieren
    for (blNr = 0; blNr < blocksNum_int; blNr++) 
	{
        for (stimPairIndexInBlock = 0; 
             stimPairIndexInBlock < presentBl_int[blNr].stimPairsNum_int; 
             stimPairIndexInBlock++)
        {
            presentationOrder[blNr][stimPairIndexInBlock] = 
                trPresentOrd_int[blNr][stimPairIndexInBlock];
        }
    }

    for (blNr = 0; blNr < blocksNum_int; blNr++) 
	{
        for (stimPairIndexInBlock = 0; 
             stimPairIndexInBlock < presentBl_int[blNr].stimPairsNum_int; 
             stimPairIndexInBlock++)
        {
            zufall = rand() % presentBl_int[blNr].stimPairsNum_int;

            while (presentationOrder[blNr][zufall] == NOT_FOUND) // NOT_FOUND==100001	
            {
				zufall = rand() % presentBl_int[blNr].stimPairsNum_int;
            }

            trPresentOrd_int[blNr][stimPairIndexInBlock] = 
                presentationOrder[blNr][zufall]; 

            presentationOrder[blNr][zufall] = NOT_FOUND;
        }
    }
}



// Hilfsroutine, shiftet die Elemente im Feld aArray der 
// Länge aElemNum um aShiftNum positionen nach rechts, wenn 
// aShiftNum negativ und nach links, wenn positiv. 
void shiftElems(int *aArray, int aElemNum, int aShiftNum) {
     
    int arrayTmp[10];
    int i = 0;

    for (i = 0; i < aElemNum; i++)
        arrayTmp[i] = aArray[i];       

    for (i = 0; i < aElemNum; i++) {
        aArray[i] = arrayTmp[ (aElemNum + i + aShiftNum) % aElemNum ];       
    }
}



// Gibt in aBlPresentOrd die durch aLatinSquareRow festgelegt Zeile
// eines lateinischen Quadrates der Größe aBlNr x aBlNr
void StimManage::latinize(int *aBlPresentOrd, int aLatinSquareRow, int aBlNr) 
{
	int row = 0; 
    int col = 0;
    int latinSquareTmp[LATIN_SQUARE_MAX][LATIN_SQUARE_MAX];


    // Temporäre Matix erstellen
    // Elemente:
    // ....3 2 1 0
    // ....3 2 1 0
    // ....3 2 1 0
    // ....3 2 1 0
    // ...........
    for (row = 0; row < aBlNr; row++) {
        for (col = 0; col < aBlNr; col++) {
            latinSquareTmp[row][col] = aBlNr - 1 - col;
        }  
    }


    // 1. Schritt: Elemente in den Spalten entsprechend verschieben 
    for (row = 1; row < aBlNr; row++) {
        int shift = 0;

        if ( (row % 2) == 0 )   // Gerade Zeilennummer 
            shift = row / 2;
        else                    // Ungerade Zeilennummer
        {
            shift = -((int)(row / 2) + 1);
        }
        shiftElems (&latinSquareTmp[row][0], aBlNr, shift); 
    }

    for (row = 0; row < aBlNr; row++) {
        for (col = 0; col < aBlNr; col++) {
            
            if ( col == (aBlNr-1) - aLatinSquareRow )
            {
                aBlPresentOrd[row] = latinSquareTmp[row][col];
            }
        }
    }
}



// Randomisiert die Blöcke
void StimManage::randomize(int *aBlPresentOrd, int aBlNr) 
{
    int zufall = 0;
    int blNr;
    int blPresentOrd[MAX_BLOCKS];

	srand((unsigned)time(NULL));

    // Werte kopieren
    for (blNr = 0; blNr < aBlNr; blNr++) {
        blPresentOrd[blNr] = aBlPresentOrd[blNr];
    }

    for (blNr = 0; blNr < aBlNr; blNr++) 
	{
        zufall = rand() % aBlNr;

        while (blPresentOrd[zufall] == NOT_FOUND) {
			zufall = rand() % aBlNr;
        }
        aBlPresentOrd[blNr] = blPresentOrd[zufall]; 
        blPresentOrd[zufall] = NOT_FOUND;
    } 
}




void StimManage::Error(char * msg) {
    cout << endl << msg << endl;
    cout << "Press return to continue." << endl;
    while(getchar() != 10) {}   //Bei Enter abbrechen
    exit(0);
}

