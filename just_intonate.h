extern int Lattice_TUNING;
#define FIVE_LIMIT_JUST 0 

#define LA_MONTE_SEVEN 1
// sets table of frequencies in given tuning system, with given parameters 
void setupLattice();

// driver function for code that sonifies shape 
double*sonify(int coords[4][2]);

// version of sonify for teensy audio synthesis
double*sonifyFreqMode(int coords[4][2]);

void swapTuning();

 //double* voiceLead(double* chord);

// void playChord(double* chord);

 int getCurrTuning();
