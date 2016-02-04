#include "filter.h"
#include "addr_space.h"
#include "../cpu/common.h"

#include <iostream>
using namespace std;
#include "mc_scverify.h"
#include "../cpu/cpu.h"

// implementace v mcu; pouzijme reseni z cpu.c
extern int otsu(int *hist, int n);
extern void print_results(int frame, int threshold, int *hist, int n);

/***************************************************************************
 Hlavni program Testbench souboru. Zajistuje generovani vstupnich vektoru,
 volani puvodniho (referencniho) kodu a modifikovaneho kodu a porovnani
 jejich vystupu.
***************************************************************************/
CCS_MAIN(int argv, char *argc){
   t_pixel pix_in_hw, pix_out_hw = 0;
   t_pixel_sw pix_in_sw, pix_out_sw = 0;
   int pix_out_sw_vld;
   bool valid = true, non_valid = false;
   t_mcu_data mcu_data[MCU_SIZE];

   for(int i=0;i<MCU_SIZE;i++)
      mcu_data[i] = 0;

   for(int f=0;f<FRAMES;f++){
      for(int r=0;r<FRAME_ROWS;r++){
         for(int c=0;c<FRAME_COLS;c++){
            /* Generovani vstupniho pixelu */
            pix_in_sw = gen_pixel();
            pix_in_hw = (t_pixel)pix_in_sw;
            /* Volani puvodniho (referencniho) kod */
            pixel_processing(pix_in_sw, &pix_out_sw, &pix_out_sw_vld); 
            /* Volani modifikovaneho kodu */
            CCS_DESIGN(filter)(pix_in_hw, valid, pix_out_hw, mcu_data); 

            /* Porovnani vysledku referencniho vs. modifikovaneho kodu */
            if(pix_out_sw_vld && (f>=100) && (pix_out_hw != pix_out_sw)) {
               cout << endl << "ERROR: Chyba na pozici: (" << (f+1) << ":" << (r+1) << "," << (c+1) << "), ";
               cout << "In: out_sw/out_hw = " << pix_in_hw << ": " << (int)pix_out_sw << "/" << pix_out_hw <<  endl;
               CCS_RETURN(1);
            }
         }
      }

      int fpga_hist_wr_done = mcu_data[FPGA_ADDR_HISTOGRAM_WR_DONE];
      if (fpga_hist_wr_done == 1) {
          mcu_data[FPGA_ADDR_HISTOGRAM_WR_DONE] = 0;
          int histogram[PIXELS];
          for (int i = 0; i < PIXELS; ++i)
              histogram[i] = (&mcu_data[FPGA_ADDR_HISTOGRAM_BASE])[i];

          int new_threshold = otsu(histogram, PIXELS);
          mcu_data[FPGA_ADDR_THRESHOLD] = new_threshold;
          mcu_data[FPGA_ADDR_THRESHOLD_WR_DONE] = 1;

          if ((f % 100) == 0)
            print_results(f, new_threshold, histogram, PIXELS);
      }
   }
   cout << "INFO: ***********************************************************" << endl;
   cout << "INFO: Test referencniho kodu oproti upravenemu probehl v poradku." << endl;
   cout << "INFO: ***********************************************************" << endl;
   CCS_RETURN(0);
}
 
