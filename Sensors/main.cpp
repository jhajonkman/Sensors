//
//  Main file
//  ----------------------------------
//  Developed with embedXcode
//
//  Sensors
//  Created by jeroenjonkman on 22-06-15
//   


#include "WProgram.h"
#include "Sensors.pde"

int main(void)
{
    init();
	setup();
	for (;;) loop();
	return 0;
}
