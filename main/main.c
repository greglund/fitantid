/*
Copyright 2015 Greg Lund

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "fit.h"
#include "fit_convert.h"
#include "fit_product.h"
#include "revision.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#define MAIN_USAGE "\
Usage:\n\
   fitantid input_file\n\
\n"

static const struct
{
   const char* manufacturer_name;
   const FIT_MANUFACTURER manufacturer_id;
} main_manufacturers[] =
{
   {"GARMIN", 1},
   {"GARMIN FR405 ANTFS", 2},
   {"ZEPHYR", 3},
   {"DAYTON", 4},
   {"IDT", 5},
   {"SRM", 6},
   {"QUARQ", 7},
   {"IBIKE", 8},
   {"SARIS", 9},
   {"SPARK HK", 10},
   {"TANITA", 11},
   {"ECHOWELL", 12},
   {"DYNASTREAM OEM", 13},
   {"NAUTILUS", 14},
   {"DYNASTREAM", 15},
   {"TIMEX", 16},
   {"METRIGEAR", 17},
   {"XELIC", 18},
   {"BEURER", 19},
   {"CARDIOSPORT", 20},
   {"A AND D", 21},
   {"HMM", 22},
   {"SUUNTO", 23},
   {"THITA ELEKTRONIK", 24},
   {"GPULSE", 25},
   {"CLEAN MOBILE", 26},
   {"PEDAL BRAIN", 27},
   {"PEAKSWARE", 28},
   {"SAXONAR", 29},
   {"LEMOND FITNESS", 30},
   {"DEXCOM", 31},
   {"WAHOO FITNESS", 32},
   {"OCTANE FITNESS", 33},
   {"ARCHINOETICS", 34},
   {"THE HURT BOX", 35},
   {"CITIZEN SYSTEMS", 36},
   {"MAGELLAN", 37},
   {"OSYNCE", 38},
   {"HOLUX", 39},
   {"CONCEPT2", 40},
   {"ONE GIANT LEAP", 42},
   {"ACE SENSOR", 43},
   {"BRIM BROTHERS", 44},
   {"XPLOVA", 45},
   {"PERCEPTION DIGITAL", 46},
   {"BF1SYSTEMS", 47},
   {"PIONEER", 48},
   {"SPANTEC", 49},
   {"METALOGICS", 50},
   {"4IIIIS", 51},
   {"SEIKO EPSON", 52},
   {"SEIKO EPSON OEM", 53},
   {"IFOR POWELL", 54},
   {"MAXWELL GUIDER", 55},
   {"STAR TRAC", 56},
   {"BREAKAWAY", 57},
   {"ALATECH TECHNOLOGY LTD", 58},
   {"MIO TECHNOLOGY EUROPE", 59},
   {"ROTOR", 60},
   {"GEONAUTE", 61},
   {"ID BIKE", 62},
   {"SPECIALIZED", 63},
   {"WTEK", 64},
   {"PHYSICAL ENTERPRISES", 65},
   {"NORTH POLE ENGINEERING", 66},
   {"BKOOL", 67},
   {"CATEYE", 68},
   {"STAGES CYCLING", 69},
   {"SIGMASPORT", 70},
   {"TOMTOM", 71},
   {"PERIPEDAL", 72},
   {"WATTBIKE", 73},
   {"MOXY", 76},
   {"CICLOSPORT", 77},
   {"POWERBAHN", 78},
   {"ACORN PROJECTS APS", 79},
   {"LIFEBEAM", 80},
   {"BONTRAGER", 81},
   {"WELLGO", 82},
   {"SCOSCHE", 83},
   {"MAGURA", 84},
   {"WOODWAY", 85},
   {"ELITE", 86},
   {"NIELSEN KELLERMAN", 87},
   {"DK CITY", 88},
   {"TACX", 89},
   {"DIRECTION TECHNOLOGY", 90},
   {"MAGTONIC", 91},
   {"1PARTCARBON", 92},
   {"INSIDE RIDE TECHNOLOGIES", 93},
   {"SOUND OF MOTION", 94},
   {"STRYD", 95},
   {"DEVELOPMENT", 255},
   {"HEALTHANDLIFE", 257},
   {"LEZYNE", 258},
   {"SCRIBE LABS", 259},
   {"ZWIFT", 260},
   {"WATTEAM", 261},
   {"RECON", 262},
   {"FAVERO ELECTRONICS", 263},
   {"DYNOVELO", 264},
   {"STRAVA", 265},
   {"ACTIGRAPHCORP", 5759},
};
static const struct
{
   const char* product_name;
   const FIT_GARMIN_PRODUCT product_id;
} main_garmin_products[] =
{
   {"HRM1", 1},
   {"AXH01", 2},
   {"AXB01", 3},
   {"AXB02", 4},
   {"HRM2SS", 5},
   {"DSI_ALF02", 6},
   {"HRM3SS", 7},
   {"HRM_RUN_SINGLE_BYTE_PRODUCT_ID", 8},
   {"BSM", 9},
   {"BCM", 10},
   {"AXS01", 11},
   {"FR301_CHINA", 473},
   {"FR301_JAPAN", 474},
   {"FR301_KOREA", 475},
   {"FR301_TAIWAN", 494},
   {"FR405", 717},
   {"FR50", 782},
   {"FR405_JAPAN", 987},
   {"FR60", 988},
   {"DSI_ALF01", 1011},
   {"FR310XT", 1018},
   {"EDGE500", 1036},
   {"FR110", 1124},
   {"EDGE800", 1169},
   {"EDGE500_TAIWAN", 1199},
   {"EDGE500_JAPAN", 1213},
   {"CHIRP", 1253},
   {"FR110_JAPAN", 1274},
   {"EDGE200", 1325},
   {"FR910XT", 1328},
   {"EDGE800_TAIWAN", 1333},
   {"EDGE800_JAPAN", 1334},
   {"ALF04", 1341},
   {"FR610", 1345},
   {"FR210_JAPAN", 1360},
   {"VECTOR_SS", 1380},
   {"VECTOR_CP", 1381},
   {"EDGE800_CHINA", 1386},
   {"EDGE500_CHINA", 1387},
   {"FR610_JAPAN", 1410},
   {"EDGE500_KOREA", 1422},
   {"FR70", 1436},
   {"FR310XT_4T", 1446},
   {"AMX", 1461},
   {"FR10", 1482},
   {"EDGE800_KOREA", 1497},
   {"SWIM", 1499},
   {"FR910XT_CHINA", 1537},
   {"FENIX", 1551},
   {"EDGE200_TAIWAN", 1555},
   {"EDGE510", 1561},
   {"EDGE810", 1567},
   {"TEMPE", 1570},
   {"FR910XT_JAPAN", 1600},
   {"FR620", 1623},
   {"FR220", 1632},
   {"FR910XT_KOREA", 1664},
   {"FR10_JAPAN", 1688},
   {"EDGE810_JAPAN", 1721},
   {"VIRB_ELITE", 1735},
   {"EDGE_TOURING", 1736},
   {"EDGE510_JAPAN", 1742},
   {"HRM_RUN", 1752},
   {"FR920XT", 1765},
   {"EDGE510_ASIA", 1821},
   {"EDGE810_CHINA", 1822},
   {"EDGE810_TAIWAN", 1823},
   {"EDGE1000", 1836},
   {"VIVO_FIT", 1837},
   {"VIRB_REMOTE", 1853},
   {"VIVO_KI", 1885},
   {"FR15", 1903},
   {"VIVO_ACTIVE", 1907},
   {"EDGE510_KOREA", 1918},
   {"FR620_JAPAN", 1928},
   {"FR620_CHINA", 1929},
   {"FR220_JAPAN", 1930},
   {"FR220_CHINA", 1931},
   {"APPROACH_S6", 1936},
   {"VIVO_SMART", 1956},
   {"FENIX2", 1967},
   {"EPIX", 1988},
   {"FENIX3", 2050},
   {"EDGE1000_TAIWAN", 2052},
   {"EDGE1000_JAPAN", 2053},
   {"FR15_JAPAN", 2061},
   {"EDGE1000_CHINA", 2070},
   {"FR620_RUSSIA", 2072},
   {"FR220_RUSSIA", 2073},
   {"VECTOR_S", 2079},
   {"EDGE1000_KOREA", 2100},
   {"FR920XT_TAIWAN", 2130},
   {"FR920XT_CHINA", 2131},
   {"FR920XT_JAPAN", 2132},
   {"VIRBX", 2134},
   {"VIVO_SMART_APAC", 2135},
   {"VIVO_FIT2", 2150},
   {"FR225", 2153},
   {"VIVO_ACTIVE_APAC", 2160},
   {"VECTOR_2", 2161},
   {"VECTOR_2S", 2162},
   {"VIRBXE", 2172},
   {"FR620_TAIWAN", 2173},
   {"FENIX3_CHINA", 2188},
   {"FENIX3_TWN", 2189},
   {"SDM4", 10007},
   {"EDGE_REMOTE", 10014},
   {"TRAINING_CENTER", 20119},
   {"ANDROID_ANTPLUS_PLUGIN", 65532},
   {"CONNECT", 65534},
};
static const struct
{
   const char* device_type_name;
   const FIT_ANTPLUS_DEVICE_TYPE device_type_id;
} main_device_types[] =
{
   {"ANTFS", 1},
   {"BIKE POWER", 11},
   {"ENVIRONMENT SENSOR LEGACY", 12},
   {"MULTI SPORT SPEED DISTANCE", 15},
   {"CONTROL", 16},
   {"FITNESS EQUIPMENT", 17},
   {"BLOOD PRESSURE", 18},
   {"GEOCACHE NODE", 19},
   {"LIGHT ELECTRIC VEHICLE", 20},
   {"ENV SENSOR", 25},
   {"RACQUET", 26},
   {"WEIGHT SCALE", 119},
   {"HEART RATE", 120},
   {"BIKE SPEED CADENCE", 121},
   {"BIKE CADENCE", 122},
   {"BIKE SPEED", 123},
   {"STRIDE SPEED DISTANCE", 124},
};


static void main_usage(void);
static bool main_decode_file(FILE* in);


int main(int argc, char* argv[])
{
   FILE* in;

   if (argc != 2)
   {
      main_usage();
      return -1;
   }

   in = fopen(argv[1], "r+b");

   if (in == NULL)
   {
      fprintf(stderr, "Unable to open file.\n");
      main_usage();
      return -1;
   }

   main_decode_file(in);
   fclose(in);
   return 0;
}


static void main_usage(void)
{
   fprintf(stderr, "\nfitantid\nRevision %03u.%03u.%03u\n\n", REVISION_RELEASE, REVISION_MAJOR, REVISION_MINOR);
   fprintf(stderr, "FitSDK Revision %u.%02u\n\n", FIT_PROFILE_VERSION_MAJOR, FIT_PROFILE_VERSION_MINOR);
   fprintf(stderr, MAIN_USAGE);
}

static bool main_decode_file(FILE* in)
{
   FIT_UINT8 buf[32];
   FIT_CONVERT_RETURN convert_return;
   bool retval;
   uint32_t size_read;
   bool found_device_info_mesg = false;

   fseek(in, FIT_FILE_HDR_SIZE, SEEK_SET);                  // Skip the header.
   FitConvert_Init(FIT_FALSE);                              // Don't decode the header.

   do
   {
      size_read = fread(buf, sizeof(buf[0]), sizeof(buf), in);

      do
      {
         convert_return = FitConvert_Read(buf, size_read);  // if size_read == 0 this returns FIT_CONVERT_CONTINUE

         switch (convert_return)
         {
            case FIT_CONVERT_MESSAGE_AVAILABLE:
            {
               const FIT_UINT8 *mesg = FitConvert_GetMessageData();
               FIT_UINT16 mesg_num = FitConvert_GetMessageNumber();

               switch (mesg_num)
               {
                  case FIT_MESG_NUM_DEVICE_INFO:
                     {
                        FIT_DEVICE_INFO_MESG info_mesg;
                        int i;

                        printf("\n");
                        found_device_info_mesg = true;
                        memcpy(&info_mesg, mesg, FIT_DEVICE_INFO_MESG_SIZE);
                        printf("Manufacturer");

                        if (info_mesg.manufacturer != FIT_MANUFACTURER_INVALID)
                        {
                           printf("Number:  %u", (unsigned int) info_mesg.manufacturer);

                           for (i = 0; i < (sizeof(main_manufacturers) / sizeof(main_manufacturers[0])); i++)
                           {
                              if (main_manufacturers[i].manufacturer_id == info_mesg.manufacturer)
                              {
                                 printf(" (%s)", main_manufacturers[i].manufacturer_name);
                                 break;
                              }
                           }
                        }
                        else
                        {
                           printf(":  UNKNOWN");
                        }

                        printf("\n");

                        printf("Product");

                        if (info_mesg.product != FIT_UINT16_INVALID)
                        {
                           printf(" Number:  %u", (unsigned int) info_mesg.product);

                           if (info_mesg.manufacturer == FIT_MANUFACTURER_GARMIN)
                           {
                              for (i = 0; i < (sizeof(main_garmin_products) / sizeof(main_garmin_products[0])); i++)
                              {
                                 if (main_garmin_products[i].product_id == info_mesg.product)
                                 {
                                    printf(" (%s)", main_garmin_products[i].product_name);
                                    break;
                                 }
                              }
                           }
                        }
                        else
                        {
                           printf(":  UNKNOWN");
                        }

                        printf("\n");
                        printf("Device Type");

   
                        if (info_mesg.device_type != FIT_UINT8_INVALID)
                        {
                           printf(" Number:  %u", (unsigned int) info_mesg.device_type);

                           for (i = 0; i < (sizeof(main_device_types) / sizeof(main_device_types[0])); i++)
                           {
                              if (main_device_types[i].device_type_id == info_mesg.device_type)
                              {
                                 printf(" (%s)", main_device_types[i].device_type_name);
                                 break;
                              }
                           }
                        }
                        else
                        {
                           printf(":  UNKNOWN");
                        }

                        printf("\n");

                        if (info_mesg.serial_number != FIT_UINT32Z_INVALID)
                        {
                           printf("Device Serial Number:  %lu\n", (unsigned long int) info_mesg.serial_number);
                           printf("ANT ID:  %u\n", (int) (uint16_t) info_mesg.serial_number);
                        }
                        else
                        {
                           printf("Device Serial Number:  UNKNOWN\n");
                           printf("ANT ID:  UNKNOWN\n");
                        }
                     }
                     break;

                  default:
                    break;
               }
               break;
            }

            default:
               break;
         }
      } while (convert_return == FIT_CONVERT_MESSAGE_AVAILABLE);
   } while ((size_read != 0) && (convert_return == FIT_CONVERT_CONTINUE));

   if (convert_return == FIT_CONVERT_END_OF_FILE)
      retval = true;
   else
      retval = false;

   if (!found_device_info_mesg)
      printf("No device information messages found in file\n");

   return retval;
}

