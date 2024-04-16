
// for detecting state transitions
#define LEFT_DIR 0
#define RIGHT_DIR 1
#define UP_DIR 2
#define DOWN_DIR 3
#define NOT_MOVING -1

extern int currMoving;

void readJoystick(int*joystickVal,int*buttonVal);

void setupJoystick();
