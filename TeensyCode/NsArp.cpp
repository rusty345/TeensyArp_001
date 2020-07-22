// NsArp.cpp
#include "nsArp.h"

#include "nsDisplay.h"
#include "nsNoteManager.h"

#include <Arduino.h>
#include "usb_midi.h"


//-------------------------------------------------------------------------------------
NsArp::NsArp()
{
    mMidiChannel = 1;
    mNumKeysCurrentlyDown=0;
    mLastNotePlayedIdx=-1;

    for(int i=0; i<mKeyListSize; i++)    
    {
        mKeyVelocity[i]=0;
        mKeyTime[i]=0;
    }
}

//-------------------------------------------------------------------------------------
void NsArp::reset(void)
{
    mNumKeysCurrentlyDown=0;
    mLastNotePlayedIdx=-1;

    for(int i=0; i<mKeyListSize; i++)    
    {
        mKeyVelocity[i]=0;
        mKeyTime[i]=0;
    }

}

//-------------------------------------------------------------------------------------
void NsArp::playNextNote(unsigned long songPosition,NoteQueue* pQueue)
  {
  int noteToPlay=-1;
  int indexToPlay=-1;
  int numFound=0;

  /*
  indexToPlay = mLastNotePlayedIdx+1;   // play in sequence
  
  if( indexToPlay >= mNumKeysCurrentlyDown-1) //  wrap
    indexToPlay=0;

  for(int i=0; i<mKeyListSize; i++)    
    {
      if(mKeyVelocity[i]>0)
      {
        if( numFound == indexToPlay )
        {
          noteToPlay = i;
          indexToPlay = numFound;
          break;
        }

      numFound++;
      }
    }
*/

  indexToPlay = random(0, mNumKeysCurrentlyDown);

  if(songPosition%96==0)
    indexToPlay=0;

  for(int i=0; i<mKeyListSize; i++)    
    {
      if(mKeyVelocity[i]>0)
      {
        if( numFound == indexToPlay )
        {
          noteToPlay = i;
          indexToPlay = numFound;
          break;
        }

      numFound++;
      }
    }

  
  if( noteToPlay>0)
  {
    pQueue->addNote(noteToPlay, 90, 1);                   // noteQueue - add note on
    pQueue->addNote(noteToPlay, 0, 1,songPosition+6);     // noteQueue - add note off
  }


  mLastNotePlayedIdx = indexToPlay;
  }

//-------------------------------------------------------------------------------------
void NsArp::updateDisplay(nsDisplay*pDisplay)
{
  int pos=0;
  int row=24;
  char strBuff[32];
  
  for(int i=0; i<mKeyListSize; i++)
  {
    if(mKeyVelocity[i]!=0)
    {
      dtostrf(i, 3, 0, strBuff);  
      pDisplay->drawString( pos,row , strBuff);      
      pos+=18;
      
      pDisplay->drawString( pos, row, ":");
      pos+=6;   
      
      dtostrf(mKeyTime[i], 3, 0, strBuff);  
      pDisplay->drawString( pos,row , strBuff);      
      pos+=20;
      
      if(pos>90)
      {
      pos=0;
      row+=16;
      }
    }
  }
}


//-------------------------------------------------------------------------------------
void NsArp::midiNoteOnReceived(int note, int vel, int channel,unsigned long songPosition)
{
  if(mKeyVelocity[note]==0)
    mNumKeysCurrentlyDown++;
    
  mKeyVelocity[note]=vel;
  mKeyTime[note]=songPosition;
}

//-------------------------------------------------------------------------------------
void NsArp::midiStopReceived(void)
{
    //reset();
}

//-------------------------------------------------------------------------------------
void NsArp::midiNoteOffReceived(int note, int vel, int channel,unsigned long songPosition)
{
  if(mKeyVelocity[note]!=0)
    mNumKeysCurrentlyDown--;
 
  mKeyVelocity[note]=0;
  mKeyTime[note]=0;
}

//-------------------------------------------------------------------------------------
void NsArp::midiClockReceived(unsigned long songPosition,NoteQueue* pQueue)
{
    if(songPosition%12==0)
    {
      playNextNote(songPosition,pQueue);
    }
}
