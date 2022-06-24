/*********************************************************************
**                                                                  **
**      File:           phantomSensorProg.cpp                       **
**      Author:         Marc Ernst                                  **
**      Modified by:    Lukas kime, then Aaron Zoeller              **
**      Date:           5.5.2008                                    **
**                                                                  **
**                                                                  **
**      File type: C++                                              **
**      Libraries: OpenGL, GLUT, GHOST                              **
**                                                                  **
*********************************************************************/

/* System */
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <math.h>
#include <glut.h> // macht glu automatisch:#include <GL/gl.h> #include <GL/glu.h>
#include <png.h>


/* Phantom */
#include <gstScene.h>
#include <gstSeparator.h>
#include <gstPHANToM.h>
#include <gstCube.h>
#include <gstBoundaryCube.h>
#include <gstErrorHandler.h>
#include <gstCylinder.h>

/*own*/
#include "stimulus.h"
#include "dataManager.h"
#include "localSTEREoLib.c"
#include "sound.h"
#include "calibSetup.h"
#include "phantButton.h"



/*****************************************************************************/
/**::: constants :::**
/*****************************************************************************/

#define WORKSPACE_LENGTH  150
#define WORKSPACE_WIDTH   300
#define WORKSPACE_HEIGHT  400

#define SAFETYMARGIN	100.0

#define MIRROR             -1   // 1=no mirror, -1 = mirror
#define SCR_WIDTH_PIX    1600   // window (or screen) width in pixel
#define SCR_HEIGHT_PIX    900   // window (or screen) height in pixel

#define TEX_WIDTH   8          //Texture on BOTTOM_PLANE vorher:16

#define STIM_WEIGHT_BUF_LEN      32   // Länge des Array für die Messung der Stimulus-Gewichte
#define WAIT_FOR_SEN_TIME       500

#define STIM_WEIGHT               0.8 // Geschätztes Gewicht der Stimuli in N

#define STIM_WEIGHT_TOLLERANCE    0.3   // Wie weit darf der gemessene Wert von dem
                                          // geschätzten Wert abweichen (+/-), damit der Gemessene
                                          // Wert als gültig bewertet werden kann (in N)
#define BUTTON_SIZE    40.0 //ALEX statt:70
#define CURSOR_SIZE     5.0


// Colors
#define COLOR_RED         0
#define COLOR_GREEN       1
#define COLOR_BLUE        2
#define COLOR_PLANE       3
#define COLOR_BUTTON      4
#define COLOR_GREENTEXT   5
#define COLOR_STIM        6
#define COLOR_BLACK       7
// CONTROL_SCENES

#define START		    1
#define INIT_TRIAL      2
#define WAIT_ON_START   3
#define WAIT_ON_SENSOR  4
#define PAUSE           5
#define STIM1a          6 //ALEX
#define STIM1b          7 //ALEX
#define STIM2           8 //ALEX
#define DECISION        9
#define END             10
#define PRIOR			11

#define START_SCENE		    1
#define WAIT_ON_START_SCENE 2
#define PAUSE_SCENE         3
#define STIM_SCENE1a        4 // ALEX
#define STIM_SCENE1b        5 // ALEX
#define STIM_SCENE2         6 // ALEX
#define PRIOR_SCENE         7 // ALEX
#define END_SCENE           8


// SOUND
#define PLAY_SOUND        0             // 0 = off, 1 = on
#define WHITE_NOISE_FILE  "niag.wav"
#define BEEP_FILE         "blip.wav"
#define RETURN_ERROR_MESSAGE  0 // 0 = es wird kein error-Fenster angezeigt
                                //     (d.h. das Programm wird nicht unterbrochen)

// Video

// #define PICS_Prior	360 // Aaron 

/*****************************************************************************/
/**::: global varibles :::**
/*****************************************************************************/
const gstPoint  offScenePointPos_glob(-10000, -10000, -10000);
const gstPoint  originPointPos_glob(0.0, 0.0, 0.0);
gstPoint        cursPos_glob = offScenePointPos_glob;       // position of the PHANToM cursor
gstScene        *hapticScene_glob;           // the haptic scene

static GLubyte texImage_glob[TEX_WIDTH][TEX_WIDTH][4]; // Original = 4
static int Pro_ri=0;
static int SessNr;

gstPHANToM  *phantom_glob;

Stimulus    *stimulus_glob;
Sensor      *sensor_glob;

gstSeparator *stimulusSep_glob,
             *buttonsSep_glob;


// Phatom buttons
PhantButton *decButtonR_glob,
            *decButtonL_glob,
            *waitButton_glob;

gstVector posButtonR_glob ( BUTTON_SIZE/2 + 70, 105, -40); // original 30
gstVector posButtonL_glob ( -BUTTON_SIZE/2 - 70,105, -40);
gstVector posWaitButton_glob (-110.0, 0, 85.0);


// Vorkompilierte grafische Objekte
static GLuint planeGraphObj_glob,
              cursGraphObj_glob,
              cylinderGraphObj_glob,
              diskGraphObj_glob,
			  leftdiskGraphObj_glob[360],
			  rightdiskGraphObj_glob[360];

StimManage  *stimManage_glob;
StimPair    *stimPair_glob;

int graphicStatus_glob = START_SCENE;

bool stimTouched_glob[2];

bool pause_glob = false;
bool newTrial_glob = false;
bool drawCursor_glob = true;

TimerK  *breakTimer_glob;
TimerK  *waitTimer_glob;
TimerK  *videoTimer_glob; // Aaron Hier Timer für Prior

// audio
ALuint	WhiteNoiseBuffer, WhiteNoiseSource;
ALuint	BeepBuffer, BeepSource;

CalibSetup  *calib_glob = new CalibSetup();

// video

int *width_glob;
int *height_glob;
int cc;
int TimerBreak	= 0;	//	Pausen werden nicht nach Block, sondern nach trialzahl gesetzt
int PriorForDefault;

/*****************************************************************************/
/**::: local function headers :::**
/*****************************************************************************/
double analyseSensorForceArray(double*);
void    setColor(int);
void    makeTexImage(void);
void    initDisplay(int argc, char **argv);         // initialize GLUT
void    initGraphics(void);                         // set up display lists ...

gstScene *initHaptics(void);                        // Set up the haptic scene

// callbacks called by GLUT
void    display(void);                              // update the display
void    keyboard(unsigned char key, int x, int y);  // keyboard function
void    reshape(int width, int hight);              // what to do on window reshape
void    controlLoop(int value);                     // does the control for the exp
void    drawScene(void);                            // draw everything
void    cleanUpOnQuit(void);                        // what to do on quit
void    PhantomErrorCallback(int, char*, void*);
// callbacks set up by GHOST
void    updatePhantomCB(gstTransform *aPhantom, void *cbData, void *param);
void    drawText(char *aText, int aXAlign, int aYAlign);  // draw text on the buttons
bool    ALInit();
void    ALStop();


/*****************************************************************************/
/**::: main :::**
/*****************************************************************************/
/** Initializes GLUT for window and device control.
/** Initializes haptics and graphic primitives and display lists.
/** During the initalistion callback functions are set.
/** Prompts for reset.
/** Starts the haptic and graphic processes. */
int main(int argc, char** argv) {
    char fileName[32],
         subInit[32],
		 sess [64], //ALEX
         sessNr[4],
		 SubNr[4], //ALEX
         input[64],
         controlFileName[64];
    bool continuePrevious = false;


    ofstream fileOutputStream;
    ifstream fileInputStream;

    FILE *fp = NULL;


cout << "\n\nBitte Sitzung eingeben: (exp; test) " << flush;
cin >> sess;
cout<< endl << endl;

    //strcpy(controlFileName, "control.red");
 // if (argc == 2) {
        if (!strcmp (sess, "exp")) 
		{
			strcpy (sessNr, "1");
			strcpy(controlFileName, "controlfiles\\control_exp.red");
            cout << "***********************" << endl;
            cout << "**  Exp - Direction  **" << endl;
            cout << "***********************" << endl << endl;
			SessNr=1;

			PriorForDefault = 0; // Aaron Indexe für Testtrials setzen
        }


		else if (!strcmp (sess, "test"))
		{
			strcpy (sessNr, "0");
			strcpy(controlFileName, "controlfiles\\control_default.red");
			cout << "****************" << endl;
			cout << "**  Testlauf  **" << endl;
			cout << "****************" << endl << endl;

			PriorForDefault = 1; // Aaron Indexe für Testtrials setzen
		}




    /* Initialize GLUT, create the window and the callbacks */
    initDisplay(argc, argv);

    stimManage_glob = new StimManage();

    breakTimer_glob = new TimerK;
    waitTimer_glob = new TimerK;
	videoTimer_glob = new TimerK;
	//stereoTimer_glob = new TimerK; //ANNA
	//stereoTimer_glob->SetZero(); //ANNA

    srand((unsigned)time(NULL));


    // Den Sensor-Faktor aus Datei lesen. Mit diesem Wert wird der Sensor initialisiert.
    float sensorFactor;
    if ( (fp = fopen("controlfiles\\sensorFactor.dat", "r")) != NULL)
    {
        fscanf(fp, "%f", &sensorFactor);
        fclose(fp);
    }


    sensor_glob = new Sensor(sensorFactor);
    sensor_glob->checkSensorCalib();


    cout << "\n\nExperiment fortsetzen (j/n)? " << flush;
    cin >> input;


    // While-Schleife bis Eingabe korrekt
    while (true) {
        if ( (input[0] == 'j') ||
             (input[0] == 'J') ||
             (input[0] == 'y') ||
             (input[0] == 'Y') ||
             (input[0] == 'n') ||
             (input[0] == 'N') ) {
            break;
        }
        else {  // Falsche Eingabe, wiederholen.,
            cout << "\n\nFalsche Eingabe! 'j' oder 'n' erwartet. " << flush;
            cin >> input;
        }
    }


    // Continue previous experiment
    if ( (input[0] == 'j') ||
         (input[0] == 'J') ||
         (input[0] == 'y') ||
         (input[0] == 'Y') ) {

        continuePrevious = true;

        if ( (fp = fopen("controlfiles\\continue.dat", "r")) == NULL) {
            cout << "\n\nThere is no experiment to continue! Start a new. " << flush;
            input[0] = 'n';
        }

        else
        {
            while (fscanf(fp, "%s", input) != EOF)
            {

                if (!strcmp (input, "Initials:")) {
                    fscanf(fp, "%s", subInit);
                }

				if (!strcmp (input, "SubjectNr:")) {
                   fscanf(fp, "%s", SubNr);
				}
				if (!strcmp (input, "SessionNr:")) { //ALEX
                    fscanf(fp, "%s", sessNr);
                }
            }

            strcpy (fileName, subInit);
			strcat(fileName, SubNr); //ALEX
            strcat (fileName, sessNr);

            cout << "fileName = " << fileName << endl << endl;

            // Trials shuffled and latinized
            stimManage_glob->Init(controlFileName, fileName, continuePrevious, atoi(SubNr), true, 1);
        }
    }




    // Start new experiment
    if (input[0] == 'n' || input[0] == 'N') {

        continuePrevious = false;

        cout << "\nSensor kalibrieren (j/n)?" << flush;
        cin >> input;

        cout << endl;

        if ( (input[0] == 'j') || (input[0] == 'J') || (input[0] == 'y') || (input[0] == 'Y') ) {
            sensor_glob->reset();
        }

        //sensor_glob->printSensorData();

        cout << "Probanden Initialien: " << flush;
        cin >> subInit;
        cout << endl << endl;

		cout << "Probanden Nummer: " << flush; //ALEX
        cin >> SubNr;
        cout << endl << endl;

        strcpy(fileName, subInit);
        strcat(fileName, SubNr); //ALEX
		strcat(fileName, sessNr);

        stimManage_glob->Init(controlFileName, fileName, continuePrevious, atoi(SubNr), true, 2); // Shuffle Aaron


        // Die continue-Datei erzeugen
        fp = fopen("controlfiles\\continue.dat", "w");
        fclose (fp);

        // Die continue-Datei zum schreiben öffnen
        fileOutputStream.open("controlfiles\\continue.dat", ios::out);

        fileOutputStream << "Initials: "  << subInit << "\n" <<
                            "SubjectNr: " << SubNr << "\n" << //ALEX
							"SessionNr:"  << sessNr << endl;
        fileOutputStream.close();
    }


    // initialize the sound, graphics and haptics scene
    ALInit();
    initGraphics();
    initHaptics();

    /* start the haptic and graphic process */
    hapticScene_glob->startServoLoop();
    glutMainLoop();   // has to be last call before return

    return(0);
}




/*****************************************************************************/
/**:: initDisplay ::**
/*****************************************************************************/
/** Initalises GLUT, the window and the controll callback functions. */
void initDisplay(int argc, char **argv)
{
    glutInit(&argc, argv);          // initialise GLUT and create Window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO);
    glutInitWindowSize(SCR_WIDTH_PIX, SCR_HEIGHT_PIX);
    glutCreateWindow("MovementParameters"); // create the window
    glutFullScreen();               // set FullScreen mode (no borders)
    glutInitWindowSize(SCR_WIDTH_PIX, SCR_HEIGHT_PIX);
    glutDisplayFunc(display);       // display callback
    glutReshapeFunc(reshape);       // how to handle reshapes
    glutKeyboardFunc(keyboard);     // keyboard callback
    glutTimerFunc(10,controlLoop,0);       // control loop callback
    glutIdleFunc(display);          // idle callback -> on idle refresh display
}


/*****************************************************************************/
/**:::initHaptics:::**
/*****************************************************************************/
/** Creates scene graph. The scene graph is composed of
/** the scene node, then a root separator, then the actual
/** scene graph containing all the haptics objects. */
gstScene *initHaptics() {


    hapticScene_glob = new gstScene();  // create the scene node
    gstSeparator *rootSep = new gstSeparator();
    hapticScene_glob->setRoot(rootSep); // set root of scene graph


    gstSeparator *transformSep = new gstSeparator();


    // Move (translate) the root seperator to the aimed position
	transformSep->setTranslate(-STIM_X_POS, -STIM_Y_POS, -STIM_Z_POS);
	transformSep->rotate(gstVector(1.0, 0.0, 0.0),
                            calib_glob->getPhantRotX());
	transformSep->rotate(gstVector(0.0, 1.0, 0.0),
                            calib_glob->getPhantRotY());
	transformSep->rotate(gstVector(0.0, 0.0, 1.0),
                            calib_glob->getPhantRotZ());
	transformSep->setScale(calib_glob->getPhantScale());



    // PHANToM nodesNODES
    // Das Argument 'flase' in dem folgenden Konstruktor bewirkt, dass das
    // Phantom die neutrale Positioin aus der Config-Datei 'Default PHANToM'
    // liest. Damit ist es nicht notwändig den Phantom-Arm beim starten des
    // Programms in die neutrale Pos. zu bringen.
    gstPHANToM *phantom = new gstPHANToM("Default PHANToM", false);
    if (!phantom || !phantom->getValidConstruction()) {
        cerr << "Unable to initialize PHANToM device." << endl;
        exit(-1);
    }
    // set the graphics callback function for the phantom position
    phantom->setGraphicsCallback(updatePhantomCB, &cursPos_glob);
    // Add phantom to the haptic scene
    transformSep->addChild(phantom);



    // Make workspace boundaries (Phantom bounderies).
    // The bounderies are linked to the phantom and move with
    // the coordinate system of the Phantom.
 	gstBoundaryCube *workspace = new gstBoundaryCube();
    workspace->setWidth (WORKSPACE_WIDTH);
    workspace->setLength(WORKSPACE_LENGTH);
    workspace->setHeight(WORKSPACE_HEIGHT);
    // Bound the phantom object. If this is not set, the phantom
    // will ignore the boundary object.
    phantom->setBoundaryObj(workspace);

    rootSep->addChild(transformSep);


    // Stimulus
    stimulus_glob = new Stimulus(sensor_glob);
    stimulus_glob->setTranslate(0, 0, 0); // Stimulus position
    stimulusSep_glob = new gstSeparator();
    stimulusSep_glob->setPosition_WC(offScenePointPos_glob);
    stimulusSep_glob->addChild(stimulus_glob);
    transformSep->addChild(stimulusSep_glob);


    // Phantom Buttons
    decButtonR_glob = new PhantButton(posButtonR_glob, BUTTON_SIZE);
    decButtonL_glob = new PhantButton(posButtonL_glob, BUTTON_SIZE);
    waitButton_glob = new PhantButton(posWaitButton_glob, BUTTON_SIZE);


    buttonsSep_glob = new gstSeparator();
    buttonsSep_glob->addChild (decButtonR_glob);
    buttonsSep_glob->addChild (decButtonL_glob);
    buttonsSep_glob->addChild (waitButton_glob);
    buttonsSep_glob->setPosition_WC(offScenePointPos_glob);

    transformSep->addChild (buttonsSep_glob);

    setErrorCallback(PhantomErrorCallback, &cursPos_glob);

    return hapticScene_glob;
}

//VIDEO and Texture Load

/*****************************/
/*** load image to texture ***/
/*****************************/

GLuint png_texture_load(const char * imagepath, int * width_glob, int * height_glob)

{ png_byte header[8];
// Open the file
FILE * fp = fopen(imagepath,"rb");
if (fp==0){
	perror(imagepath);
	printf("Image could not be opened\n");
	return 0;}

// read the header
fread(header, 1, 8, fp);
if (png_sig_cmp(header, 0,8 )){
    fprintf(stderr,"error: %s is not a PNG file\n", imagepath);
	fclose(fp);
    return 0;}

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);


    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width_glob){ *width_glob = temp_width; }
    if (height_glob){ *height_glob = temp_height; }
	// Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	/*
    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);
*/
    // Allocate the image_data as a big block, to be given to opengl
    png_byte *image_data = new png_byte[rowbytes * temp_height];
   if (!image_data) {
     //clean up memory and close stuff
	   fprintf(stderr, "error: could not allocate memory for PNG image data\n");
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     fclose(fp);
     return 0;
   }

    // row_pointers is for pointing to image_data for reading the png with libpng

      png_bytep *row_pointers = new png_bytep[temp_height];
   if (!row_pointers) {
     //clean up memory and close stuff
	 fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
     png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
     delete[] image_data;
     fclose(fp);
     return 0;
   }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (size_t i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, temp_width, temp_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	// clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;

}

/*****************************************************************************/
/**:::  initGraphics     :::**
/*****************************************************************************/
/** Initialize GL graphics objects.
/** Set up the display lists to draw the cursor and the workspace
/** as well the lighting model and other settings. */
void initGraphics (void) {

    local_setSTEREoScrSize( SCR_WIDTH_PIX,
                            SCR_HEIGHT_PIX,
                            calib_glob->getScreenWidth() * MIRROR,
                            calib_glob->getScreenHeight() );


    GLfloat diffuse_light[]  = {0.5, 0.5, 0.5, 1.0};
    GLfloat ambient_light[]  = {0.2, 0.2, 0.2, 1.0};
    GLfloat lightPos[] = { 0.3, 0.2, 0.7, 0.0f}; // Pos. d. Lichtquelle
	

    // set model and light appearence
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glEnable(GL_LIGHT0);

    // Enable depth buffering for hidden surface removal
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);


    // Create cursor object
    GLUquadricObj *cursorSphereQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (cursorSphereQuadObj, GLU_FILL);
    gluQuadricNormals (cursorSphereQuadObj, GLU_SMOOTH);
    cursGraphObj_glob = glGenLists (1);
    glNewList (cursGraphObj_glob, GL_COMPILE_AND_EXECUTE);
        gluSphere (cursorSphereQuadObj, CURSOR_SIZE, 16, 16);
    glEndList ();
    gluDeleteQuadric(cursorSphereQuadObj);

//+++++++++++++++++++ Aaron: nach oben verschoben

	    //Texture
    GLuint TexName;
    makeTexImage();
    glGenTextures(1, &TexName);
    glBindTexture(GL_TEXTURE_2D, TexName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImage_glob);

    // Create plane object
    planeGraphObj_glob = glGenLists (1);
    glNewList (planeGraphObj_glob, GL_COMPILE_AND_EXECUTE);
    //setColor (COLOR_PLANE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Original GL_DECAL
    glBindTexture(GL_TEXTURE_2D, TexName);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f((-WORKSPACE_WIDTH/2), 0.0, -206.6/2); // WORKSPACE_LENGTH statt -206.6 / 195.0
        glTexCoord2f(0.0, 1.0);
        glVertex3f((WORKSPACE_WIDTH/2), 0.0, -206.6/2);
        glTexCoord2f(1.0, 1.0);
        glVertex3f((WORKSPACE_WIDTH/2), 0.0,  206.6/2);
        glTexCoord2f(1.0, 0.0);
        glVertex3f((-WORKSPACE_WIDTH/2), 0.0,  206.6/2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEndList();

//+++++++++++++++++++

    // Zylinder (Stimulus)
    GLUquadricObj *stimCylinderQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimCylinderQuadObj, GLU_FILL);
    gluQuadricNormals (stimCylinderQuadObj, GLU_SMOOTH);
    cylinderGraphObj_glob = glGenLists (1);
    glNewList (cylinderGraphObj_glob, GL_COMPILE_AND_EXECUTE);
        gluCylinder(stimCylinderQuadObj, STIM_RADIUS, STIM_RADIUS, STIM_HEIGHT, 20, 1);
    glEndList ();
    gluDeleteQuadric(stimCylinderQuadObj);



    // Disk (Stimulus)
    GLUquadricObj *stimDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimDiskQuadObj, GLU_INSIDE);	
	diskGraphObj_glob = glGenLists (1);
    glNewList (diskGraphObj_glob, GL_COMPILE_AND_EXECUTE);
	gluDisk(stimDiskQuadObj, 0, STIM_RADIUS, 20, 1);
    glEndList ();
    gluDeleteQuadric(stimDiskQuadObj);

//************************************** Aaron Texture inread!
	// Leider sehr umständlich, da immer Links und rechts unterschiedliche Texturen haben
			int i=0;
			int j=0;
			int k=0;
			int l=0;
            char textur[80];

//**************************************************************
			//Run 1


		for (j=0; j < (6); j++) /// Nach Winkel sortiert
		{
			for (i=0; i < (5); i++) /// Nach % sortiert 
			{
				for (k=0; k < (2); k++) /// Nach Wiederholungen sortiert
				{
					strcpy(textur,"Prior_Stim/Right/ID_");
				
					if ( k == 0) // Sortiert nach Wiederholungen
					{
						strcat(textur,"1_");
					}
					else if (k == 1)
					{
						strcat(textur,"2_");
					}				
					
					if ( i == 0) // Sortiert nach % korrekt
					{
						strcat(textur,"0_");
					}
					else if (i == 1)
					{
						strcat(textur,"15_");
					}				
					else if (i == 2)
					{
						strcat(textur,"25_");
					}
					else if (i == 3)
					{
						strcat(textur,"35_");
					}
					else if (i == 4)
					{
						strcat(textur,"50_");
					}
					

					if ( j == 0) // Sortiert nach Winkeln
					{
						strcat(textur,"15");
					}
					else if (j == 1)
					{
						strcat(textur,"45");
					}				
					else if (j == 2)
					{
						strcat(textur,"75");
					}
					else if (j == 3)
					{
						strcat(textur,"105");
					}
					else if (j == 4)
					{
						strcat(textur,"135");
					}
					else if ( j == 5)
					{
						strcat(textur,"165");
					}

				strcat(textur,".png");
				cout << textur << endl;
				

				char *path=textur;

		GLuint PngTex = png_texture_load(path, width_glob, height_glob); // replace "filename.png" with the actual name of the texture file



// left disk

    GLUquadricObj *stimleftDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimleftDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimleftDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimleftDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimleftDiskQuadObj, GL_TRUE );

    leftdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (leftdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimleftDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimleftDiskQuadObj);


// right disk

    GLUquadricObj *stimrightDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimrightDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimrightDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimrightDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimrightDiskQuadObj, GL_TRUE );

    rightdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (rightdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimrightDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimrightDiskQuadObj);



	l= l++; // Für K = 1-2 also 60 Bilder

			}
		}
	}

//**************************************************************
			//Run 2


		for (j=0; j < (6); j++) /// Nach Winkel sortiert
		{
			for (i=0; i < (5); i++) /// Nach % sortiert 
			{
				for (k=2; k < (4); k++) /// Nach Wiederholungen sortiert
				{
					strcpy(textur,"Prior_Stim/Right/ID_");
				
					if ( k == 2) // Sortiert nach Wiederholungen
					{
						strcat(textur,"3_");
					}
					else if (k == 3)
					{
						strcat(textur,"4_");
					}				
					

					if ( i == 0) // Sortiert nach % korrekt
					{
						strcat(textur,"0_");
					}
					else if (i == 1)
					{
						strcat(textur,"15_");
					}				
					else if (i == 2)
					{
						strcat(textur,"25_");
					}
					else if (i == 3)
					{
						strcat(textur,"35_");
					}
					else if (i == 4)
					{
						strcat(textur,"50_");
					}
					

					if ( j == 0) // Sortiert nach Winkeln
					{
						strcat(textur,"15");
					}
					else if (j == 1)
					{
						strcat(textur,"45");
					}				
					else if (j == 2)
					{
						strcat(textur,"75");
					}
					else if (j == 3)
					{
						strcat(textur,"105");
					}
					else if (j == 4)
					{
						strcat(textur,"135");
					}
					else if ( j == 5)
					{
						strcat(textur,"165");
					}

				strcat(textur,".png");
				cout << textur << endl;
				

				char *path=textur;

		GLuint PngTex = png_texture_load(path, width_glob, height_glob); // replace "filename.png" with the actual name of the texture file



// left disk

    GLUquadricObj *stimleftDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimleftDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimleftDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimleftDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimleftDiskQuadObj, GL_TRUE );

    leftdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (leftdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimleftDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimleftDiskQuadObj);


// right disk

    GLUquadricObj *stimrightDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimrightDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimrightDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimrightDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimrightDiskQuadObj, GL_TRUE );

    rightdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (rightdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimrightDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimrightDiskQuadObj);



	l= l++; // Für K = 3-4 also 60 Bilder

			}
		}
	}


//**************************************************************
			//Run 3


		for (j=0; j < (6); j++) /// Nach Winkel sortiert
		{
			for (i=0; i < (5); i++) /// Nach % sortiert 
			{
				for (k=4; k < (6); k++) /// Nach Wiederholungen sortiert
				{
					strcpy(textur,"Prior_Stim/Right/ID_");
				
					if ( k == 4) // Sortiert nach Wiederholungen
					{
						strcat(textur,"5_");
					}
					else if (k == 5)
					{
						strcat(textur,"6_");
					}				
					
					if ( i == 0) // Sortiert nach % korrekt
					{
						strcat(textur,"0_");
					}
					else if (i == 1)
					{
						strcat(textur,"15_");
					}				
					else if (i == 2)
					{
						strcat(textur,"25_");
					}
					else if (i == 3)
					{
						strcat(textur,"35_");
					}
					else if (i == 4)
					{
						strcat(textur,"50_");
					}
					

					if ( j == 0) // Sortiert nach Winkeln
					{
						strcat(textur,"15");
					}
					else if (j == 1)
					{
						strcat(textur,"45");
					}				
					else if (j == 2)
					{
						strcat(textur,"75");
					}
					else if (j == 3)
					{
						strcat(textur,"105");
					}
					else if (j == 4)
					{
						strcat(textur,"135");
					}
					else if ( j == 5)
					{
						strcat(textur,"165");
					}

				strcat(textur,".png");
				cout << textur << endl;
				

				char *path=textur;

		GLuint PngTex = png_texture_load(path, width_glob, height_glob); // replace "filename.png" with the actual name of the texture file



// left disk

    GLUquadricObj *stimleftDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimleftDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimleftDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimleftDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimleftDiskQuadObj, GL_TRUE );

    leftdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (leftdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimleftDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimleftDiskQuadObj);


// right disk

    GLUquadricObj *stimrightDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimrightDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimrightDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimrightDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimrightDiskQuadObj, GL_TRUE );

    rightdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (rightdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimrightDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimrightDiskQuadObj);



	l= l++; // Für K = 5-6 also 60 Bilder

			}
		}
	}



//**************************************************************
			//Run 4


		for (j=0; j < (6); j++) /// Nach Winkel sortiert
		{
			for (i=0; i < (5); i++) /// Nach % sortiert 
			{
				for (k=6; k < (8); k++) /// Nach Wiederholungen sortiert
				{
					strcpy(textur,"Prior_Stim/Right/ID_");
				
					if ( k == 6) // Sortiert nach Wiederholungen
					{
						strcat(textur,"7_");
					}
					else if (k == 7)
					{
						strcat(textur,"8_");
					}				
					
					if ( i == 0) // Sortiert nach % korrekt
					{
						strcat(textur,"0_");
					}
					else if (i == 1)
					{
						strcat(textur,"15_");
					}				
					else if (i == 2)
					{
						strcat(textur,"25_");
					}
					else if (i == 3)
					{
						strcat(textur,"35_");
					}
					else if (i == 4)
					{
						strcat(textur,"50_");
					}
					

					if ( j == 0) // Sortiert nach Winkeln
					{
						strcat(textur,"15");
					}
					else if (j == 1)
					{
						strcat(textur,"45");
					}				
					else if (j == 2)
					{
						strcat(textur,"75");
					}
					else if (j == 3)
					{
						strcat(textur,"105");
					}
					else if (j == 4)
					{
						strcat(textur,"135");
					}
					else if ( j == 5)
					{
						strcat(textur,"165");
					}

				strcat(textur,".png");
				cout << textur << endl;
				

				char *path=textur;

		GLuint PngTex = png_texture_load(path, width_glob, height_glob); // replace "filename.png" with the actual name of the texture file



// left disk

    GLUquadricObj *stimleftDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimleftDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimleftDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimleftDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimleftDiskQuadObj, GL_TRUE );

    leftdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (leftdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimleftDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimleftDiskQuadObj);


// right disk

    GLUquadricObj *stimrightDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimrightDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimrightDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimrightDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimrightDiskQuadObj, GL_TRUE );

    rightdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (rightdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimrightDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimrightDiskQuadObj);



	l= l++; // Für K = 7-8 also 60 Bilder

			}
		}
	}



//**************************************************************
			//Run 5


		for (j=0; j < (6); j++) /// Nach Winkel sortiert
		{
			for (i=0; i < (5); i++) /// Nach % sortiert 
			{
				for (k=8; k < (10); k++) /// Nach Wiederholungen sortiert
				{
					strcpy(textur,"Prior_Stim/Right/ID_");
				
					if ( k == 8) // Sortiert nach Wiederholungen
					{
						strcat(textur,"9_");
					}
					else if (k == 9)
					{
						strcat(textur,"10_");
					}				
					
					if ( i == 0) // Sortiert nach % korrekt
					{
						strcat(textur,"0_");
					}
					else if (i == 1)
					{
						strcat(textur,"15_");
					}				
					else if (i == 2)
					{
						strcat(textur,"25_");
					}
					else if (i == 3)
					{
						strcat(textur,"35_");
					}
					else if (i == 4)
					{
						strcat(textur,"50_");
					}
					

					if ( j == 0) // Sortiert nach Winkeln
					{
						strcat(textur,"15");
					}
					else if (j == 1)
					{
						strcat(textur,"45");
					}				
					else if (j == 2)
					{
						strcat(textur,"75");
					}
					else if (j == 3)
					{
						strcat(textur,"105");
					}
					else if (j == 4)
					{
						strcat(textur,"135");
					}
					else if ( j == 5)
					{
						strcat(textur,"165");
					}

				strcat(textur,".png");
				cout << textur << endl;
				

				char *path=textur;

		GLuint PngTex = png_texture_load(path, width_glob, height_glob); // replace "filename.png" with the actual name of the texture file



// left disk

    GLUquadricObj *stimleftDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimleftDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimleftDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimleftDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimleftDiskQuadObj, GL_TRUE );

    leftdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (leftdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimleftDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimleftDiskQuadObj);


// right disk

    GLUquadricObj *stimrightDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimrightDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimrightDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimrightDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimrightDiskQuadObj, GL_TRUE );

    rightdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (rightdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimrightDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimrightDiskQuadObj);



	l= l++; // Für K = 9-10 also 60 Bilder

			}
		}
	}



//**************************************************************
			//Run 6


		for (j=0; j < (6); j++) /// Nach Winkel sortiert
		{
			for (i=0; i < (5); i++) /// Nach % sortiert 
			{
				for (k=10; k < (12); k++) /// Nach Wiederholungen sortiert
				{
					strcpy(textur,"Prior_Stim/Right/ID_");
				
					if ( k == 10) // Sortiert nach Wiederholungen
					{
						strcat(textur,"11_");
					}
					else if (k == 11)
					{
						strcat(textur,"12_");
					}				

					if ( i == 0) // Sortiert nach % korrekt
					{
						strcat(textur,"0_");
					}
					else if (i == 1)
					{
						strcat(textur,"15_");
					}				
					else if (i == 2)
					{
						strcat(textur,"25_");
					}
					else if (i == 3)
					{
						strcat(textur,"35_");
					}
					else if (i == 4)
					{
						strcat(textur,"50_");
					}
					

					if ( j == 0) // Sortiert nach Winkeln
					{
						strcat(textur,"15");
					}
					else if (j == 1)
					{
						strcat(textur,"45");
					}				
					else if (j == 2)
					{
						strcat(textur,"75");
					}
					else if (j == 3)
					{
						strcat(textur,"105");
					}
					else if (j == 4)
					{
						strcat(textur,"135");
					}
					else if ( j == 5)
					{
						strcat(textur,"165");
					}

				strcat(textur,".png");
				cout << textur << endl;
				

				char *path=textur;

		GLuint PngTex = png_texture_load(path, width_glob, height_glob); // replace "filename.png" with the actual name of the texture file



// left disk

    GLUquadricObj *stimleftDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimleftDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimleftDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimleftDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimleftDiskQuadObj, GL_TRUE );

    leftdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (leftdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimleftDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimleftDiskQuadObj);


// right disk

    GLUquadricObj *stimrightDiskQuadObj = gluNewQuadric ();
    gluQuadricDrawStyle (stimrightDiskQuadObj, GLU_FILL);
    gluQuadricNormals (stimrightDiskQuadObj, GLU_SMOOTH);
    gluQuadricOrientation (stimrightDiskQuadObj, GLU_INSIDE);
    gluQuadricTexture(stimrightDiskQuadObj, GL_TRUE );

    rightdiskGraphObj_glob[l] = glGenLists (1);
    glNewList (rightdiskGraphObj_glob[l], GL_COMPILE_AND_EXECUTE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, PngTex);
    gluDisk(stimrightDiskQuadObj, 0, STIM_RADIUS, 20, 1);

    glDisable(GL_TEXTURE_2D);
    glEndList ();
    gluDeleteQuadric(stimrightDiskQuadObj);



	l= l++; // Für K = 11-12 also 60 Bilder

			}
		}
	}


//**************************************************************



    

  }


/*****************************************************************************/
/**:: reshape callback ::**
/*****************************************************************************/
/** Setup the projection for a given window.
/** This function has to be called on every reshape of the
/** window in order to fit the scene in the resized window
*/
void reshape(int width, int height) {
    local_querySTEREoWinSize(); // this works with and without
}




/*****************************************************************************/
/** :: display callback ::**
/*****************************************************************************/
/** Redraw graphics objects.
/** This function is called on idle by the window system to redraw
/** the scene as the cursor and freebodies move. */
void display(void) {

    // Update objects with graphics and event callbacks from GHOST
    hapticScene_glob->updateGraphics();
    hapticScene_glob->updateEvents();
	

    // STEREO
    // Draw left subfield
    local_STEREoSetBuffer(STEREo_LEFTEYE);               // set the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear for refresh
    glClearColor(0.0, 0.0, 0.0, 0.0);                    // set black background

    // initalize Projection
    local_STEREoProjInit(10.0, 2000.0, calib_glob->getMiddleEye_TransX() );

    local_STEREoProjectionCalib(calib_glob->getEyeL_TransX(),
                                calib_glob->getEyeL_TransY(),
                                calib_glob->getEyeL_TransZ());

    drawScene();


    // Draw right subfield
    local_STEREoSetBuffer(STEREo_RIGHTEYE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    local_STEREoProjInit(10.0, 2000.0, calib_glob->getMiddleEye_TransX());
    local_STEREoProjectionCalib(calib_glob->getEyeR_TransX(),
                                calib_glob->getEyeR_TransY(),
                                calib_glob->getEyeR_TransZ());

	drawScene();


    glutSwapBuffers();  // update scene
}




/*****************************************************************************/
void drawScene(){

    char text[30];
    static double screenRotX = calib_glob->getScreenRotX();
	static double screenRotY = calib_glob->getScreenRotY();
	static double screenRotZ = calib_glob->getScreenRotZ();



    glMatrixMode(GL_MODELVIEW); // draw the Model (graphic scene)
    glLoadIdentity();           // start with 1-Matrix


	// NOTE: transformation are done by OpenGl in opposite direction!:
    // translation, x-, y-, z-Rotation
	glRotatef(calib_glob->getScreenRotZ(), 0.0, 0.0, 1.0);
	glRotatef(calib_glob->getScreenRotY(), 0.0, 1.0, 0.0);
	glRotatef(calib_glob->getScreenRotX(), 1.0, 0.0, 0.0);

	glTranslatef(calib_glob->getScreenTransX()+STIM_X_POS,
                 calib_glob->getScreenTransY()+STIM_Y_POS,
                 calib_glob->getScreenTransZ()+STIM_Z_POS);


	glNormal3f(1.0, 1.0, 1.0);

    glPushMatrix();



    // Draw plain
	 setColor (COLOR_PLANE);
	 glPushMatrix();
		glTranslatef(0.0, 8.5, 0.0);// Textur ALEX    
		glCallList(planeGraphObj_glob);
	 glPopMatrix();


//    // Testausgabe auf dem Phantom-Bildschirm -----------------------
//    glPushMatrix(); {
//        glRotatef(-screenRotX, 1.0, 0.0, 0.0);
//	    glTranslatef(-100.0, 100.0, 0.0);
//	    glRotatef(180.0, 0.0, 1.0, 0.0);
//	    setColor (COLOR_RED);
//	    glScalef(-0.08,0.08,0.08);
//	    glLineWidth(2.0);
//        sprintf(text,"%3.0f %3.0f %3.0f  %d", 
//                   cursPos_glob[0], 
//                   cursPos_glob[1], 
//                   cursPos_glob[2],
//                   stimulus_glob->getCurrCursorPos() );
//	  drawText(text, 1, 1);
//    } glPopMatrix();
    


    // Der cursor wird um seinen Radius nach ober versetzt,
    // damit ist die untere seite der Cursor-Kugel die position des Phatom
    if (drawCursor_glob)
    {
        setColor (COLOR_GREEN);
        glPushMatrix();
	        glTranslatef(cursPos_glob[0],
                         cursPos_glob[1]+CURSOR_SIZE,
                         cursPos_glob[2]);
	        glCallList(cursGraphObj_glob);
        glPopMatrix();
    }


    switch (graphicStatus_glob)
    {

        case START_SCENE:
        {	

            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(0.0, 90.0, 40.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.25, 0.25, 0.25);
	            glLineWidth(4.0);
	            drawText("Start", 1, 1);
            glPopMatrix();
        } break;


        case PAUSE_SCENE:
        {
            // Wenn Pause, dann wird nur eine rote Schrift 'PAUSE' gezeigt

            itoa(stimManage_glob->getBreakLength()/1000, text, 10);
            strcat(text, " Sekunden");

            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(0.0, 70.0, 0.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.25, 0.25, 0.25);
	            glLineWidth(4.0);
	            drawText("PAUSE", 1, 1);
            glPopMatrix();

            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(0.0, 20.0, 0.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.17, 0.17, 0.2);
	            glLineWidth(2.0);

	            drawText(text, 1, 1);
            glPopMatrix();
        } break;


        case WAIT_ON_START_SCENE:
        {    

            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(-110, -40.0, 70.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.13, 0.15, 0.15);
	            glLineWidth(2.0);
	            drawText("warten", 1, 1);
            glPopMatrix();
			

        } break;


		case STIM_SCENE1a:
		{      
			setColor (COLOR_RED);
            glPushMatrix();
	            glTranslatef(-STIM_BETWEEN/2.0 - (STIM_RADIUS+6), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(diskGraphObj_glob);
				glPopMatrix();

			setColor (COLOR_BUTTON);
            glPushMatrix();
	            glTranslatef(STIM_BETWEEN/2.0 + (STIM_RADIUS-5), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(diskGraphObj_glob);
				glPopMatrix();

		} break;


		case STIM_SCENE1b:
		{  
			setColor (COLOR_BUTTON);
            glPushMatrix();
	            glTranslatef(-STIM_BETWEEN/2.0 - (STIM_RADIUS+6), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(diskGraphObj_glob);
				glPopMatrix();
			
			setColor (COLOR_RED);
            glPushMatrix();
	            glTranslatef(STIM_BETWEEN/2.0 + (STIM_RADIUS-5), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(diskGraphObj_glob);
				glPopMatrix();
		
		} break;



        case STIM_SCENE2:
        {   

            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(-BUTTON_SIZE/2 - 70, 105.0, 10.0); //( BUTTON_SIZE/2 + 30, 105, -40);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.12, 0.15, 0.15);
	            glLineWidth(2.0);
	            drawText("hoehere Freq.", 1, 1);
            glPopMatrix();


            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(BUTTON_SIZE/2 + 70, 105.0, 10.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.12, 0.15, 0.15);
	            glLineWidth(2.0);
	            drawText("hoehere Freq.", 1, 1);
            glPopMatrix();

			setColor (COLOR_BUTTON); // left Stim
            glPushMatrix();
	            glTranslatef(-STIM_BETWEEN/2.0 - (STIM_RADIUS+6), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(diskGraphObj_glob);
				glPopMatrix();

			setColor (COLOR_BUTTON); // Right Stim
            glPushMatrix();
	            glTranslatef(STIM_BETWEEN/2.0 + (STIM_RADIUS-5), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(diskGraphObj_glob);
				glPopMatrix();

        } break;


		case PRIOR_SCENE:
        { 	
			
			int PriorPattern = stimPair_glob->priorIndex_int; // Der Index beginnt bei 1, das Array bei 0

				if (PriorForDefault == 1)	// Aaron für Test Trials
				{
				PriorPattern = PriorPattern * 10; // 10ner sind immer 0% beim einlesen
				}
			
			
			
			setColor (COLOR_BUTTON); // left Stim
            glPushMatrix();
	            glTranslatef(-STIM_BETWEEN/2.0 - (STIM_RADIUS+6), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(leftdiskGraphObj_glob[PriorPattern]);
				glPopMatrix();

			setColor (COLOR_BUTTON); // Right Stim
            glPushMatrix();
	            glTranslatef(STIM_BETWEEN/2.0 + (STIM_RADIUS-5), STIM_HEIGHT, -12.0);
                glRotatef(90, 1.0, 0.0, 0.0);
	            glCallList(cylinderGraphObj_glob);
                glCallList(rightdiskGraphObj_glob[PriorPattern]);
				glPopMatrix();


			glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(-110, -40.0, 70.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.13, 0.15, 0.15);
	            glLineWidth(2.0);
	            drawText("warten", 1, 1);
            glPopMatrix();

        }
		break;


        case END_SCENE:
        {	
            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(0.0, 70.0, 0.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.25, 0.25, 0.25);
	            glLineWidth(4.0);
	            drawText("Ende :-)", 1, 1);
            glPopMatrix();


            glPushMatrix();
	            glRotatef(-screenRotX, 1.0, 0.0, 0.0);
	            glTranslatef(0.0, 20.0, 0.0);
	            glRotatef(180.0, 0.0, 1.0, 0.0);
	            setColor (COLOR_RED);
	            glScalef(-0.2, 0.2, 0.2);
	            glLineWidth(2.0);
	            sprintf(text,"%i %% richtige Antworten", Pro_ri); // ALEX
				drawText(text, 1, 1); // drawText("Super gemacht!", 1, 1);
            glPopMatrix();
        } break;

    }   // End switch(graphicStatus_glob)

    glFinish();

    glPopMatrix();
}


/*****************************************************************************/
/*                          HAPTIC  --  GRAPHIC                              */
/*  	                  COMMUNICATION ROUTINES                             */
/*****************************************************************************/



/*****************************************************************************/
/**:::updatePhantomCB:::**
/*****************************************************************************/
/** Graphics callback for phantom pointer.
/** - [*aPhantom] is the haptic phantom object.
/** - [*cbDataAddr] contains the callback data associated with the phantom.
/** - [*param] contains the user-defined parameter that is passed
/**            into the callback as an argument.  In this case,
/** 	       it is the pointer to the GL cursor position.
/** Updates the graphics phantom object to the position of the haptics
/** phantom. This is processed whenever scene->updateGraphics is called. */
void updatePhantomCB(gstTransform *aPhantom, void *cbDataAddr, void *param) {
    /* Cast callback data */
    gstPHANToMGraphicsCBData *cbData = (gstPHANToMGraphicsCBData *) cbDataAddr;
    /* Cast the parameter as the cursor position. */
    gstPoint *ph_pos = (gstPoint *)param;

    /* Set the cursor position to the phantom position in world
    * coordinates (which is translation of cumulativeTransform).
    * If you use transform instead of cumulative transform you
    * get position in local coordinates. */
    cbData->cumulativeTransform.getTranslation(*ph_pos);
};




/**************************************************************************************/
/*                           CONTROL AND RECORD FUNCTIONS                             */
/**************************************************************************************/
void controlLoop(int value)
{
    static int status = START;
    static int currBlockNr = stimManage_glob->getCurrentBlockOrderIndex();
    static bool soundOn = true; // Aaron für WhiteNoise
    static bool sensorOK = false;
    static int measuredSensorValuesNr = 0;
    static double measuredSensorValuesBuff[STIM_WEIGHT_BUF_LEN];
	static int riAntw_sum=0;
	static int Trial_anz=1;



    switch (status)
    {
        // Status direkt nach Programmstart
	    case START:

            // Zum starten den Cursor auf die Grundflaeche (plane) legen
		    if ( (cursPos_glob[0] > -SAFETYMARGIN) &&
                 (cursPos_glob[0] < SAFETYMARGIN) &&
                 (cursPos_glob[1] > SAFETYMARGIN) &&
                 (cursPos_glob[2] > -SAFETYMARGIN) &&
                 (cursPos_glob[2] < SAFETYMARGIN) )
            {
                stimulus_glob->setStimParams();
                stimulusSep_glob->setPosition_WC(originPointPos_glob);
			    status = INIT_TRIAL;
            }
    	break;



        // Ein neuer Trial wird initialisiert
        case INIT_TRIAL:

            drawCursor_glob = true;

            if (!(stimPair_glob = stimManage_glob->getNext()))
            {
                status = END;
                breakTimer_glob->SetZero();
                graphicStatus_glob = END_SCENE;
                delete stimManage_glob;
                break;
            }

            stimulus_glob->setStimParams();

            newTrial_glob = true;

            breakTimer_glob->SetZero();  // für die Pause

            buttonsSep_glob->setPosition_WC(originPointPos_glob);

            graphicStatus_glob = WAIT_ON_START_SCENE;

            status = WAIT_ON_START;


            printf (" %d  %d", stimPair_glob->blNr_int, stimPair_glob->trNr_int);
            printf ("     L = %*.1f", 4, stimPair_glob->leftStiff_int);

			alSourcePlay(WhiteNoiseSource); // Aaron für WhiteNoise

            if ( stimPair_glob->leftStiff_int == stimPair_glob->stiff_int[REF_STIM] ) 
			{
                printf ("    R = %*.1f", 4, stimPair_glob->stiff_int[COMP_STIM]);
            }
            else 
			{
                printf ("    R = %*.1f", 4, stimPair_glob->stiff_int[REF_STIM]);
            }
			
			// Can be deleted once it is working
			printf ("    Orientation = %*.1f", 4, stimPair_glob->orientCond_int);	// Print Aaron
		//	printf ("	 Quality = %*.1f", 4, stimPair_glob->quality_int);			// Print Aaron
		//	printf ("    Prior Index = %*.1f", 4, stimPair_glob->priorIndex_int);   // Print Aaron  original: 
						
			//cout << stimPair_glob->priorIndex_int << endl;
			
			waitButton_glob->setOff();
        break;



        case WAIT_ON_START:

            //cout << cursPos_glob[0] << " " << cursPos_glob[1] << " " << cursPos_glob[2] << endl;

            // Die Pause von einem neuen Block wird angefangen
            if ( (stimManage_glob->getBreakLength() > 0)  &&
                 //(stimPair_glob->blNr_int != currBlockNr) &&
				 (TimerBreak == 60) &&								// Hier ändern auf 60
                 (breakTimer_glob->GetMS() < stimManage_glob->getBreakLength()) )
            {
                pause_glob = true;
                graphicStatus_glob = PAUSE_SCENE;


                if (soundOn) 
				{
                    alSourceStop(WhiteNoiseSource);
                    soundOn = false;
                }
            }

            // Nach der Pause
            else
            {
                if (pause_glob)
                {
                    pause_glob = false;
					TimerBreak = 0; // Der Timer wird zurückgesetzt
                    graphicStatus_glob = WAIT_ON_START_SCENE;
                }

                if (PLAY_SOUND && !soundOn && !pause_glob) 
				{
                    alSourcePlay(WhiteNoiseSource);
                    soundOn = true;

                }


                // Vpn hatte die Schaltfläche 'warten' kontaktiert und
                // die Waage hatte eine Kraft > 10 N gemessen hat.
                if ( (waitButton_glob->getStatus() == ON) &&
                     (stimulus_glob->sensor_int->getValue() > 15) )	// Aaron hier zum beesern Arbeiten mit dem Aufsatz
                {
                    status = WAIT_ON_SENSOR;
                    sensorOK = false;
                    waitTimer_glob->SetZero();
                }
            }
        break;


        // Warten, bis sich die Waage eingeschwungen hat.
        case WAIT_ON_SENSOR:


            if ( waitTimer_glob->GetMS() > WAIT_FOR_SEN_TIME )
            {
                // Es wurde gewartet.
                // Nun wird getestet, OB sich die Waage eingeschwungen.
                // Dazu werden zu erst 100 ms lang Sensor-Werte in einem Array gespeichert.
                // Danach wird geprüft, ob der Unterschied zw. dem kleinsten und dem
                // größten gespeichereten Wert < 1 N ist.

                if (measuredSensorValuesNr < STIM_WEIGHT_BUF_LEN)
                {
                    measuredSensorValuesBuff[measuredSensorValuesNr++] =
                        stimulus_glob->sensor_int->getValue();
                }

                else
                {
                    double stimsWeight = analyseSensorForceArray(measuredSensorValuesBuff);
                    if (stimsWeight > 0) 
					{

                        // Des Gewicht der Stimuli wurde erfolgreich gemessen.
                        // Es wird in dem im ..raw file für jeden Trial gespeichert.
                        stimPair_glob->stimPairWeight_int = stimsWeight;
                        sensorOK = true;

                        //cout << endl << stimPair_glob->stimPairWeight_int << endl;
					}
				
					else 
					{
                        sensorOK = false;
                        measuredSensorValuesNr = 0;
					}
                
				}

			}
            // Warten, bis sich die Waage beruhigt (eingeschwungen) hat
            if (sensorOK) 
			{

                measuredSensorValuesNr = 0; // Für die Bestimmung der Stimuli-Gewichte im nächsten Trial

                newTrial_glob = false;

                currBlockNr = stimPair_glob->blNr_int;

		        stimulus_glob->stimOn();

                // Ab jetzt sollen die Phantom-Werte aufgezeichnet werden.
		        stimulus_glob->startRecPhantData();

                stimTouched_glob[RIGHT_STIM] = false;
                stimTouched_glob[LEFT_STIM] = false;

				videoTimer_glob->SetZero();
				status = PRIOR;
                //alSourcePlay(BeepSource);
			}
        break;

		case PRIOR:
					
			if ( videoTimer_glob ->GetMS() < 2500) // Aaron: hier kannst du die Darbietungslänge des Textes verändern, momentan 5 sek. fest 
			{ 
				graphicStatus_glob = PRIOR_SCENE;
			}
			else 	
			{	videoTimer_glob->SetZero();
				
				int c;
				srand((unsigned)time(NULL));
				c=rand();
				if ( (c % 2) == 0) // // Gerade Zeilennummer
                { // left
					graphicStatus_glob = STIM_SCENE1a;
					status = STIM1a;
				stimPair_glob->moveStart_int = 0;
				}

				else 
				{ // right
						graphicStatus_glob = STIM_SCENE1b;
						status = STIM1b;
				stimPair_glob->moveStart_int = 1;
				}

                alSourcePlay(BeepSource);
			}
		break;
		
        // Status nachdem der Cursor auf die Grundflaeche gelegt wurde, die VP
        // muss nun den Start-Button auf der Grundflaeche betaetigen
	    case STIM1a: // ALEX

			if (stimulus_glob->sensor_int->getValue() > stimPair_glob->stimPairWeight_int+20)
			{
              drawCursor_glob = false;
            }
			else drawCursor_glob = true;


			if ( stimulus_glob->getCurrCursorPos() == LEFT_STIM ) 
			{
                stimTouched_glob[LEFT_STIM] = true;
                //cout << " L " << endl;
            }
			if ((stimTouched_glob[LEFT_STIM] == true) ) 
			{
				if(stimulus_glob->getCurrCursorPosInStim()== OUTER_STIM) 
				{
		 	graphicStatus_glob = STIM_SCENE2;

			status = STIM2;
				}
			}
		break;

		case STIM1b: // ALEX

			if (stimulus_glob->sensor_int->getValue() > stimPair_glob->stimPairWeight_int+20)
              drawCursor_glob = false;
            else drawCursor_glob = true;

			if ( stimulus_glob->getCurrCursorPos() == RIGHT_STIM ) 
			{
                stimTouched_glob[RIGHT_STIM] = true;
                //cout << " R " << endl;
            }

            // Beide stimuli wurden exploriert
            if ( (stimTouched_glob[RIGHT_STIM] == true) ) 
			{
				if(stimulus_glob->getCurrCursorPosInStim()== OUTER_STIM) 
				{
			graphicStatus_glob = STIM_SCENE2;

			status = STIM2;
				}
			}

		break;

		case STIM2:
			
			if (stimulus_glob->sensor_int->getValue() > stimPair_glob->stimPairWeight_int+20) //ALEX
                drawCursor_glob = false;
            else drawCursor_glob = true;


            if ( stimulus_glob->getCurrCursorPos() == LEFT_STIM ) 
			{
                stimTouched_glob[LEFT_STIM] = true;
                //cout << " L " << endl;
            }

            if ( stimulus_glob->getCurrCursorPos() == RIGHT_STIM ) 
			{
                stimTouched_glob[RIGHT_STIM] = true;
                //cout << " R " << endl;
            }

            // Beide stimuli wurden exploriert
            if ( (stimTouched_glob[RIGHT_STIM] == true) &&
                 (stimTouched_glob[LEFT_STIM] == true) ) 
			{

                decButtonR_glob->setOff();
                decButtonL_glob->setOff();

                status = DECISION;
            }

        break;


        // Decision scene: "Welcher Stimulus war rauer?"
	    case DECISION:

            if (stimulus_glob->sensor_int->getValue() > stimPair_glob->stimPairWeight_int+20)
                drawCursor_glob = false;
            else drawCursor_glob = true;

            if (((decButtonR_glob->getStatus() == ON) ||
                 (decButtonL_glob->getStatus() == ON)) &&
                (newTrial_glob == false) ) 
			{

                stimulus_glob->stimOff();

                if (decButtonR_glob->getStatus() == ON)
                {
                    if (stimPair_glob->leftStiff_int == stimPair_glob->stiff_int[REF_STIM]) 
					{
                        stimPair_glob->decision_int = stimPair_glob->stiff_int[COMP_STIM];
                    }
                    else 
					{
                        stimPair_glob->decision_int = stimPair_glob->stiff_int[REF_STIM];
                    }
                    printf ("      dec = %*.1f \n", 4, stimPair_glob->decision_int);
                }

                else if (decButtonL_glob->getStatus() == ON)
                {
                    if (stimPair_glob->leftStiff_int == stimPair_glob->stiff_int[REF_STIM]) 
					{
                        stimPair_glob->decision_int = stimPair_glob->stiff_int[REF_STIM];
                    }
                    else 
					{
                        stimPair_glob->decision_int = stimPair_glob->stiff_int[COMP_STIM];
                    }
                    printf ("      dec = %*.1f \n", 4, stimPair_glob->decision_int);
                }

				int riAntw;
				if (stimPair_glob->stiff_int[REF_STIM]> stimPair_glob->stiff_int[COMP_STIM]) 
				{
					if (stimPair_glob->decision_int==stimPair_glob->stiff_int[REF_STIM]) riAntw=1;
					else riAntw=0;
				}
				else if (stimPair_glob->stiff_int[REF_STIM]< stimPair_glob->stiff_int[COMP_STIM]) 
				{
					if (stimPair_glob->decision_int==stimPair_glob->stiff_int[COMP_STIM]) riAntw=1;
					else riAntw=0;
				};

				riAntw_sum= riAntw_sum + riAntw;
				Pro_ri=riAntw_sum*100/Trial_anz;
				Trial_anz++;
				TimerBreak++;
//				cout << "TrialCounter =" << TimerBreak << endl;	// Print Aaron

                stimulus_glob->storePhantomData(stimPair_glob->moveFileName_int);

                stimManage_glob->saveStimPair(stimPair_glob);

                newTrial_glob = true;

                status = INIT_TRIAL;
            }
        break;



		case END:

            graphicStatus_glob = END_SCENE;
            while ( breakTimer_glob->GetMS() < 2500 ) { }
		    cleanUpOnQuit();
		break;
    }

    glutTimerFunc(10, controlLoop, value);// call again in 10 ms
}


/*****************************************************************************/
void drawText(char *aText, int aXAlign, int aYAlign){
    char *p;
    int length, height;

    length = glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char *)aText);
    height = glutStrokeWidth(GLUT_STROKE_ROMAN, 49);

    switch(aXAlign) {
        case 0: // Align Left
        break;
        case 1: // Align Center
            glTranslatef(-length/2.0, 0.0, 0.0);
        break;
        case 2: // Align Right
            glTranslatef(-length, 0.0, 0.0);
        break;
    }

    switch(aYAlign) {
        case 0: // Align Bottom
        break;
        case 1: // Align Center
            glTranslatef(0.0,-height/2.0, 0.0);
        break;
        case 2: // Align TOP
            glTranslatef(0.0,-2*height, 0.0);
        break;
    }

    glEnable(GL_LINE_SMOOTH);

    for (p = aText; *p; p++)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);

    glDisable(GL_LINE_SMOOTH);
}




/*****************************************************************************/
/**::: Keyfunctions :::**/
/*****************************************************************************/
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'q':
            cleanUpOnQuit();
        break;

        case 27 :
            cleanUpOnQuit();
        break;  // Esc
    }
}





/*****************************************************************************/
/**::: clean up on qiut :::**/
/*****************************************************************************/
/** shut down all processes and clean up
**/
void cleanUpOnQuit(void) {
    alSourceStop(WhiteNoiseSource);
    hapticScene_glob->stopServoLoop();
    cout << "Thank you for participating. The experiment is now finished! <ENTER>." << endl;
    exit(0);
}



/*****************************************************************************/
/**::: PhantomErrorCallback :::**/
/*****************************************************************************/
void PhantomErrorCallback(int errorNumber, char *description, void *param)
{
    static int temperatureErrorNr = 0;

	if ( (errorNumber == -1) && (temperatureErrorNr < 3) ) {
		temperatureErrorNr +=1;

		if (RETURN_ERROR_MESSAGE == 0) {
			printErrorMessages(FALSE);
			cout << "Temperatur error nr: " << temperatureErrorNr << endl;
		}
		else  printErrorMessages(TRUE);
	}

    else {
	    printErrorMessages(TRUE);
    }

}



/*****************************************************************************/
void makeTexImage(void)
{
    int i, j,c;

    for (i=0; i < TEX_WIDTH; i++) {
	    for (j=0; j < TEX_WIDTH; j++) {
		    if ((i%2)==(j%2)) c=255; else c=0; //c=100; else c=155; zahlen von rot grün
		    texImage_glob[i][j][0]=(GLubyte)c;
		    texImage_glob[i][j][1]=(GLubyte)c;
		    texImage_glob[i][j][2]=(GLubyte)c; // 0
		    texImage_glob[i][j][3]=(GLubyte)255;
	    }
    }
}




/*****************************************************************************/
// Dient zum bestimmen des Gewichts der Stimuli
// Prüft ob die Schwankung der gemessenen Werte < 2 N
// Falls ja, dann wird eine Mittelwerte der Gespeicherte werde ermittelt
// Der Mittelwert ist das Gewicht der Stimuli
double analyseSensorForceArray(double* aArray)
{
    double maxPress = -100.0;
    double minPress =  100.0;
    double arraySum =    0.0;
    int arrayElemNr =    0;


    for (int i = 1; i < STIM_WEIGHT_BUF_LEN; i++)
    {
        // Min und Max wert im Array suchen
        if (aArray[i] < minPress)
            minPress = aArray[i];
        if (aArray[i] > maxPress)
            maxPress = aArray[i];

        // Alle Array-Elemente summieren (für den Mittelwert)
        arrayElemNr++;
        arraySum += aArray[i];
    }

    if ( (maxPress < STIM_WEIGHT + STIM_WEIGHT_TOLLERANCE) &&
         (minPress > STIM_WEIGHT - STIM_WEIGHT_TOLLERANCE) )
        return arraySum/arrayElemNr;
    else            // Die Schwankung der gemessenen Werte war zu groß
        return -1;
}




/*****************************************************************************/
void setColor(int color)
{
    if(color == COLOR_RED) {
        float mat_ambient_and_diffuse[]  = {0.8, 0.0, 0.0, 1.0};
        float mat_emission[] = {0.1, 0.0, 0.0, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }

    if(color == COLOR_GREENTEXT) {
        float mat_ambient_and_diffuse[]  = {0.0, 0.0, 0.0, 1.0};
        float mat_emission[] = {0.0, 0.8, 0.0, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }

    else if(color == COLOR_GREEN) {
        float mat_ambient_and_diffuse[]  = {0.0, 1.0, 0.0, 1.0};
        float mat_emission[] = {0.0, 0.0, 0.0, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }

    else if(color == COLOR_BLUE) {
        float mat_ambient_and_diffuse[]  = {0.1, 0.1, 1.0, 1.0};
        float mat_emission[] = {0.0, 0.0, 1.0, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }

    else if(color == COLOR_PLANE) {
        float mat_ambient_and_diffuse[]  = {0.8, 0.7, 0.0, 1.0};
        float mat_emission[] = {0.0, 0.0, 0.0, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }

    else if(color == COLOR_BUTTON) {
        float mat_ambient_and_diffuse[]  = {0.2, 0.2, 0.2, 1.0};
        float mat_emission[] = {0.1, 0.1, 0.1, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }

    else if(color == COLOR_STIM) {
        float mat_ambient_and_diffuse[]  = {0.0, 0.7, 0.7, 1.0};
        float mat_emission[] = {0.0, 0.0, 0.0, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }

    else if(color == COLOR_BLACK) {
        float mat_ambient_and_diffuse[]  = {0.2, 0.2, 0.2, 1.0};
        float mat_emission[] = {0.2, 0.2, 0.2, 1.0};

        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION,  mat_emission);
    }
}




/*****************************************************************************/
/**::: sound functions :::**/
/*****************************************************************************/

bool ALInit()
{
	alSettingUpOpenAL();    // start OpenAL
	// set up the white sound
	alSettingASource(WhiteNoiseBuffer, WhiteNoiseSource, WHITE_NOISE_FILE);
    alSettingASource(BeepBuffer, BeepSource, BEEP_FILE);

	// setup the ObjectSoundSource parameters
	alSourcei(WhiteNoiseSource,AL_LOOPING, AL_TRUE);    // Noise loopen
    alSourcei(BeepSource,AL_LOOPING, AL_FALSE);         // einmal abspielen

    alSourcef(BeepSource, AL_GAIN, 2.0);

	return true;
}


// to stop the sound
void ALStop()
{
	alSourceStop(WhiteNoiseSource);
	alDeleteSources(1,&WhiteNoiseSource);
	alDeleteBuffers(1,&WhiteNoiseBuffer);
}
