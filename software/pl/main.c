/// \file main.c VS1010C VSOS Executable main file
/// This is a starting point for developing VS1010 DLX libraries and executables
///own player

#include <vo_stdio.h>
#include <volink.h>     // Linker directives like DLLENTRY
#include <apploader.h>  // RunLibraryFunction etc
#include <vs1010bRom.h>
//#include <vs1010cRom.h>
#include <vo_gpio.h>
#include <vs1010b.h>
#include <playerinfo.h>
#include <string.h>
#include <protocol.h>
#include <lowlevels.h>

SETHANDLER(97, MyPlayerCallback)

u_int16 button_pause_pressed=0;
u_int16 button_change_song=0;
u_int16 button_volume_pressed=0;
u_int16 button_volume_pressed2=0;
u_int16 button_volume_pressed3=0;

u_int16 file_at_end=0;
volatile s_int32 delay_samples=0;

u_int16 nowPressed=1;
//u_int32 samplerateout=44100;

u_int16 PlayNamedFile(register const char *filename) {
	strncpy(player.fileSpec,filename,126); //126
	PlayerPlayFile();
}

static u_int16 lastPressed1=1;
static u_int16 lastPressed2=1;
static u_int16 lastPressed3=1;
static u_int16 lastPressed4=1;
static u_int16 lastPressed5=1;

static u_int16 lastPressed8=1;

void MyPlayerCallback(AUDIO_DECODER *auDec, u_int16 samples) {



		nowPressed = GpioReadPin(0x20);	
		if (!nowPressed && lastPressed1) { // Next track,button0 xhld0/GPIO_1_12
				printf("next\n");
				player.nextStep=1;
				player.auDec.pause = 0;	// Un-pause	
				player.auDec.cs.cancel = 1; //Stop playing current file.
				button_change_song=1;
		}
		lastPressed1 = nowPressed;
		
		nowPressed = GpioReadPin(0x26);
		if (!nowPressed && lastPressed2) { //pause button1 xwp0/GPIO_1_13
				printf("pause\n");
				player.auDec.pause ^= 1;
				button_pause_pressed=1;
		}
		lastPressed2 = nowPressed;
		
		/*nowPressed = GpioReadPin(0x2f);
		if (!nowPressed && lastPressed8) { //pause from HP /GPIO_2_15
				printf("pause\n");
				player.auDec.pause ^= 1;
				button_pause_pressed=1;
		}
		lastPressed8 = nowPressed;
*/

		
		nowPressed = GpioReadPin(0x1a);	
		if (!nowPressed && lastPressed3) { // prev track,button2 mosi0/GPIO_1_0
				printf("previous\n");
				player.nextStep=-1;
				player.auDec.pause = 0;	// Un-pause	
				player.auDec.cs.cancel = 1; //Stop playing current file.
				button_change_song=1;	
		}
		lastPressed3 = nowPressed;

		//setting player volume from 1 button, where short click and release of <0.5s
		//increases volume (decreases attenuation by 2db)
		//,and pressing button continously inceases volume by 2db every ~0.5secs , until max
		//has been reached 
		nowPressed = GpioReadPin(0x23);	
		if (!nowPressed && lastPressed4) { // volume,button
				//printf("pressed vol\n");
				delay_samples=player.auDec.sampleRate/2; //samplerate/2, directly from decoder
				button_volume_pressed=1;	
				button_volume_pressed2=1;
				button_volume_pressed3=1;	
		}
		lastPressed4 = nowPressed;
			
		nowPressed = GpioReadPin(0x23);	
		if (nowPressed && !lastPressed5) { //volume,button 
				//printf("released vol\n");
				delay_samples=0;
				button_volume_pressed=0;	
		}
		lastPressed5 = nowPressed;	
		
		if(button_volume_pressed){
			if(delay_samples >0){
				delay_samples-=samples;
			}else{
				button_volume_pressed2=0;	
				delay_samples=player.auDec.sampleRate/2;
				if(player.volume<96){
					printf("vol-\n");
					player.volume++; //increase attenuation by 0.5dB
					player.volume++;
					player.volume++;
					player.volume++;
				}
			}
		}	
		if(!button_volume_pressed && button_volume_pressed2 && button_volume_pressed3){
				button_volume_pressed3=0;
				if(player.volume>3){
					printf("vol+\n");
					player.volume--; //decrease attenuation by 0.5dB
					player.volume--;
					player.volume--;
					player.volume--;
				}
		}	
}

s_int32 totalFiles=0;

u_int16 mask = (1 << (0x20 & 0xf));

ioresult main (char *params) {	
	printf("VS1010D Player\n");
	//printf("Press [S0] to switch to next song,  [S1] to pause/play.\n");
	//printf("Press [Sx] to switch to previous song,  [Sx] controls volume, clicks +, long -.\n");
	
//set gpio20 as input and gpio
PERIP(GPIO2_DDR) &= ~(1<<0);
PERIP(GPIO2_SET_MASK) &= ~(1<<0);


//set gpio2_15 as input and gpio
//PERIP(GPIO2_DDR) &= ~(1<<0xf);
//PERIP(GPIO2_SET_MASK) &= ~(1<<0xf);

//set start player volume to normal hearing
player.volume=64;


	totalFiles = RunLib("CountFiles","S:*.MP3");
	if (totalFiles == -31234) {
		printf("CountFiles not found\n");
	}

	//player.currentFile = 0;
	//PlayerPlayFile();
	while(1) {
		//if(PlayerPlayFile() == S_OK){
			player.nextStep = 1;
			PlayNamedFile("S:*.MP3");
		//}
		if (player.nextFile) {
			player.currentFile = player.nextFile;
			player.nextFile = 0;
		}
		
		printf("player.files: %d\n", player.totalFiles);
		printf("files: %d\n", totalFiles);


		if (player.currentFile <= 0 || !(PlayerPlayFile() == S_OK)) {
			player.currentFile = 1;
		}
		/*
		else if (player.currentFile > totalFiles ) {
			player.currentFile = 1; //beginning if at last file
		}*/
	}
	return S_OK;
}





