air-quality
===========

This tutorial will cover the fundamentals of deploying a solar powered air quality monitor using the cellular network.  Though we will be using a dust particle sensor to measure air quality, the same setup could easily be modified for other sensors such ____.  This setup also uses the Xively platform which allows for real-time data streams to be displayed on the web.  To begin, you will need:

- Arduino Uno
- Arduino GSM Shield
- Active SIM Card (AT&T or T-Mobile)
- Dust Particle Sensor
- Voltaic Solar Kit 

The Arduino GSM shield is a powerful module that enables connection to the cellular network.  Using this shield, we can make phone calls, send text messages and even access  web pages.  All of this functionality relies on an active SIM card from any GSM carrier.  We will be using T-Mobile for our example.  

Image 
http://arduino.cc/en/uploads/Guide/SlideSIMIn.jpg


The Dust Particle Sensor from Groove uses a small LED to detect the number of particles passing through the housing.  To incorporate the sensor into the Arduino GSM shield simple connect the black wire to ground, the red wire to 5v and the yellow wire to pin 8.  The sensor ships with a connector which we simple cut off and soldered header pins onto the wires individually.

Following the hardware setup, we will need to setup a Xively account in order to display our data on the web.  You can sign up for a Xively account [here](https://xively.com/signup).  After signing up for an account, click on "Add Device" which will setup your new data stream.  Also, keep track of you 'Feed ID' which will be needed in the Arduino code.  

If you are using the supplied Arduino Code, the data values should automatically populate in the Xively feed.  The code uses Xively's API to automatically set three streams of data for each PUT request.  These streams are the concentration(con) of particles, the ratio(ratio) of the particles to air and the low pulse occupancy(lpo).  Each of these data points are sent in the content of the PUT request and should appear as three separate streams of data.  For more information about these streams visit this site.

The final stage in this tutorial is implementing the Arduino code to connect both to your Xively account as well as your cellular network.  All of the code can be found here.  To begin, open the code in the Arduino IDE and modify the code to match your Xively 'API Key' and 'Feed ID'.  Then modify the GPRS_APN to match you carrier.  For T-Mobile the GPRS_APN is "epc.tmobile.com" and for AT&T the GPRS_APN is "wap.cingular".

The final piece to modify is the frequency that data is sent to Xively.   This can be change by modifying the postingInterval variable which is set in milliseconds.  Setting this will dramatically affect the battery life.