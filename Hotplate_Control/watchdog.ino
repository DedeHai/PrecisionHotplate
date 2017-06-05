void setupWatchtog(void)
{
  //set timer1 interrupt in CTC mode
  

  TCNT1  = 0;//initialize counter value to 0


    TCCR1A = (0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);  //setup timer 1 as CTC
  TCCR1B = (0<<ICNC1) | (0<<ICES1) |(0<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10);  //clock prescaler: 1024 -> clock is 15.625kHz

  OCR1A = 1562; //make it a 10Hz interrup

  TIMSK1 |= (1 << OCIE1A); 
}


//watchdog interrupt, restarts the software if anything goes wrong (except if it hangs in another interrupt)

ISR(TIMER1_COMPA_vect){
  if(watchdog > 5) 
  {
    asm volatile ("  jmp 0"); //software reset
  }
  else watchdog++;
  
  
  
 
}



