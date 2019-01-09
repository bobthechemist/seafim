/*
  flame.c
  A command line interface for the flame spectrometer.
  Written by BoBtheChemist 12/2018
  Uses the SeaBreeze API from Ocean Optics
*/

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SeaBreezeAPI.h"

int main(int argc, char **argv) {

  int num_devices;
  long *dev_ids;
  long device;
  int num_features;
  long *feat_ids;
  long features;
  int num_lamps;
  long *lamp_ids;

  int flag, error = 0; // For error checking
  int c; // options character
  extern char *optarg; // needed for options
  int iflag = 0; // Informs code to run integration routine
  int lflag = 0; // Informs code to run lamp enable routine
  int cflag = 0; // Informs code to collect spectrum
  int aflag = 0; // Informs code to average scans
  long integration_time = 0; // integration time in microseconds
  unsigned short int average_scans = 1; // Number of scans to average
  int lamp;
  int verbose = 0;
  
  while ((c = getopt(argc, argv, "hli:cva:")) != -1)
    switch (c) {
    case 'h':
      lflag = 1;
      lamp = 1;
      break;
    case 'l':
      lflag = 1;
      lamp = 0;
      break;
    case 'i':
      iflag = 1;
      integration_time = atoi(optarg);
      break;
    case 'c':
      cflag = 1;
      break;
    case 'a':
      aflag = 1;
      average_scans = atoi(optarg);
      break;
    case 'v':
      verbose = 1;
      break;
    }
  if (0) { // template for mandatory flag
    printf("Missing a mandatory flag, exiting.\n");
    exit(1);
  } else if (argc == 1) {
    printf("I don't know what to do, exiting.\n", argc);
    exit(1);
  }

  if(verbose)printf("Verbose mode is ON.\n"); 
  
  /* Initialize driver */
  sbapi_initialize();
  
  if(verbose)printf("Searching for devices...\n"); 
  sbapi_probe_devices();
  num_devices = sbapi_get_number_of_device_ids();
  
  /* If no devices were found, exit */
  if(num_devices < 1) {
    printf("***No devices found, exiting.\n"); 
  }
  else {
    if(verbose)printf("Device(s) found: %d.  Using device 0.\n",num_devices);
  }
  
  /* Allocate space for device_ids.  Only the first device is used */
  if(verbose)printf("Getting device IDs...\n");
  dev_ids = (long *)calloc(num_devices, sizeof(long));
  num_devices = sbapi_get_device_ids(dev_ids, num_devices);
  device = dev_ids[0];
  free(dev_ids);
  
  /* Open the device */
  if(verbose)printf("Opening device\n");
  flag = sbapi_open_device(device, &error);
  if(flag) {
    printf("***Problem opening device. (%d) [%s].\n", flag, sbapi_get_error_string(error));
  }
  
  /* Get features */
  if(verbose)printf("Getting spectrometer features.\n");
  num_features = sbapi_get_number_of_spectrometer_features(device, &error);
  if(error){
    printf("***Problem with getting # of features. [%s].\n", sbapi_get_error_string(error));
  }
  feat_ids = (long *)calloc(num_features, sizeof(long));
  num_features = sbapi_get_spectrometer_features(device, &error, feat_ids, num_features);
  if(error){
    printf("***Problem with getting features. [%s].\n", sbapi_get_error_string(error));
  }
  features = feat_ids[0];
  free(feat_ids);
  
  /* Set integration time, making sure that the provided value is > minimum integration time */
  if(iflag){
    if(verbose)printf("Setting integration time to %d.\n",integration_time);
    long min_integration_time;
    min_integration_time = sbapi_spectrometer_get_minimum_integration_time_micros(device, features, &error);
    if(error)printf("Problem getting minimum integration time. [%s]\n",sbapi_get_error_string(error));
    if(integration_time > min_integration_time) {
      sbapi_spectrometer_set_integration_time_micros(device,features, &error, integration_time);
      if(error)printf("Problem setting integration time. [%s]\n", sbapi_get_error_string(error));
    }
  }
  
  /* Set the lamp, if requested */
  if(lflag){
    if(verbose)printf("Turning lamp %s.\n",lamp ? "on" : "off");
    num_lamps = sbapi_get_number_of_lamp_features(device, &error);
    if(error)printf("Problem with getting number of lamps, [%s].\n",sbapi_get_error_string(error));
    lamp_ids = (long *)calloc(num_lamps, sizeof(long));
    num_lamps = sbapi_get_lamp_features(device, &error, lamp_ids, num_lamps);
    if(error)printf("Problem with getting lamp features, [%s].\n",sbapi_get_error_string(error));
    sbapi_lamp_set_lamp_enable(device,lamp_ids[0],&error,lamp);
    if(error)printf("Problem with enabling/disabling lamp, [%s].\n",sbapi_get_error_string(error));
    free(lamp_ids);
  } 
  
     
  /* Collect spectrum */
  if(cflag){
    // Get formatted spectrum length
    int l;
    l = sbapi_spectrometer_get_formatted_spectrum_length(device,features,&error);
    if(error)printf("Problem with getting spectrum length, [%s]\n", sbapi_get_error_string(error));
    
    // Get wavelength array
    int wl;
    double *w;
    w = (double*)calloc(l,sizeof(double));
    wl = sbapi_spectrometer_get_wavelengths(device,features,&error,w,l);
    if(error)printf("Problem with getting wavelengths, [%s].\n", sbapi_get_error_string(error));
    
    // Get formatted spectrum
    double *b;
    b = (double *)calloc(l, sizeof(double));
    l = sbapi_spectrometer_get_formatted_spectrum(device, features, &error, b, l);
    if(error)printf("Problem with getting spectrum, [%s].\n", sbapi_get_error_string(error));
    if(aflag){
      double *b2;
      b2 = (double *)calloc(l, sizeof(double));
      for (int j = 1; j < average_scans; j++) {
        l = sbapi_spectrometer_get_formatted_spectrum(device, features, &error, b2, l);
        if(error)printf("Problem with signal averaging, [%s].\n", sbapi_get_error_string(error));
        for (int i = 0; i < l; i++) {
          b[i]+=b2[i];
        }
      }
      for (int i = 0; i < l; i++) {
        b[i] = b[i]/average_scans;
      }
      free(b2);
    }
    // Print spectrum to stdout
    for (int i = 0; i < l; i++) {
      printf("%1.1f\t%1.1f\n", w[i], b[i]);
    }
    
    free(w);
    free(b);
  }
  
  /* Close the device */
  if(verbose)printf("Closing device.\n");
  sbapi_close_device(device,&error);
  if(error)printf("Problem closing device, [%s].\n",sbapi_get_error_string(error));
  
  
  /* Clean up memory allocated by the driver */
  sbapi_shutdown();
  
  
  
  
  return 0;

}

